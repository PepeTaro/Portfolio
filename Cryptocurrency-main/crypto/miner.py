from crypto.params import COINBASE_REWARD,NUM_TX_IN_BLOCK
from crypto.tx import Tx,TxIn,TxOut
from crypto.block import Block
from crypto.wallet import Wallet
from crypto.blockchain import Blockchain
from crypto.map import Map
from crypto.multimap import MultiMap

from crypto.script import check_signature
from crypto.address import gen_address
from crypto.timestamp import get_timestamp
from crypto.logging import log_error,log_warning,log_info,log_debug

from ecc.ecdsa import gen_privkey,gen_pubkey

class Miner:
    def __init__(self):        
        self._privkey = gen_privkey()             # 秘密鍵
        self._pubkey  = gen_pubkey(self._privkey) # 公開鍵
        self._address = gen_address(self._pubkey) # アドレス
        self._locking_script = "DUP HASH %s EQUALVERIFY CHECKSIG"%(self._address) # Locking Script(固定なため、ここで保存)
        
        self._mempool      = Map[Tx]()             # 現在までに受け取っていて、かつブロックチェーン上に存在しないTx用
        self._wallet       = Wallet(self._address) # 財布の様なもの(使用可能なTxを格納)
        
        self._orphan_blocks = Map[Block]()                 # Orphanブロックを格納
        self._children_of_orphan_block = MultiMap[Block]() # Orphanブロックのblockhashから子のリストを返す
        
        self._orphan_txs = Map[Tx]()                  # OrphanTxを格納
        self._children_of_orphan_tx = MultiMap[Tx]()  # Orphan Txのtxidから子のリストを返す
        
        self._blockchain   = Blockchain()             # ブロックチェーン
        
        log_info("Miner(): 鍵を生成[アドレス:%s]"%(self._address))
        
    def get_address(self) -> str:
        return self._address
    
    def get_balance(self) -> int:
        """ マイナーの所持金を返す """        
        return self._wallet.get_balance()

    def get_best_height(self) -> int:
        """ メインチェーンの高さを返す """
        return self._blockchain.get_best_height()

    def get_best_blockhash(self) -> str:
        """ メインチェーンの一番後方にあるブロックのハッシュ値を返す """
        return self._blockchain.get_best_blockhash()
    
    def get_mainchain(self) -> list[str]:
        """ メインチェーンの返す、ブロックハッシュのリストであることに注意 """
        return self._blockchain.get_mainchain()

    def get_genesis_block(self) -> Block:
        """ Genesisブロックを返す """
        return self._blockchain.genesis

    def get_block(self,blockhash: str) -> Block:
        """ ブロックハッシュからブロックを返す """
        return self._blockchain.get_block_by_blockhash(blockhash)
    
    def get_num_of_txs_in_mempool(self) -> int:
        return self._mempool.size()

    def get_num_of_txs_in_wallet(self) -> int:
        return self._wallet.size()
    
    def get_num_of_orphan_blocks(self) -> int:
        return self._orphan_blocks.size()
    
    def get_num_of_orphan_txs(self) -> int:
        return self._orphan_txs.size()

    def get_size_of_children_of_orphan_block(self) -> int:
        return self._children_of_orphan_block.size()

    def get_size_of_children_of_orphan_tx(self) -> int:
        return self._children_of_orphan_tx.size()
    
    def can_accept_txs_in_block(self,block: Block) -> bool:                            
        """ ブロック内にあるすべてのTxが受け入れられるかチェック """
        
        for tx in block.txs:
            if not self.accept_tx(tx,block):
                log_error("Miner.can_accept_txs_in_block(): ブロック内に受け入れられないTxが存在します")
                return False
        return True

    def add_block_tx_to_wallet_if_mine(self,block: Block) -> None:        
        """ ブロック内のTxをWalletに追加 """
        for tx in block.txs:
            self._wallet.add_tx_if_mine(tx)

    def store_block(self,block: Block) -> bool:
        """ ブロックを保存 """
        height = self._blockchain.get_height_of_block_from_parent(block)
        
        if height is None:
            log_error("Miner.store_block(): ブロックの親が存在しません")
            return False

        block.height = height # 高さを更新
        self._blockchain.add_block_by_blockhash(block.get_hash(),block)
        self._blockchain.add_block_by_prevhash(block.get_prevhash(),block)
        return True
    
    def store_ophan_tx(self,orphan_tx) -> None:
        """ Orphan Txを保存 """
        self._orphan_txs.add(orphan_tx)
        for txin in orphan_tx.vin:
            self._children_of_orphan_tx.add(txin.txid,orphan_tx)
    
    def get_tx_from_blockchain_or_mempool_by_txid(self,txid) -> Tx:
        """ txidを持つTxを、ブロックチェーン或いはMempool内から取り出し返す、存在しない場合Noneを返す """
        
        tx = self._mempool.get(txid)        
        if tx is not None: # Mempool内に存在
            return tx
        
        tx = self._blockchain.get_tx_by_txid(txid)
        if tx is not None: # ブロックチェーン内に存在
            return tx
    
        return None
    
    def get_blocks_between_blockhashes(self,from_blockhash,to_blockhash) -> list[Block]:
        """ from_blockからto_blockまでのブロックを格納したリストを返す """
        
        block  = self._blockchain.get_block_by_blockhash(to_blockhash)
        blocks = [block]
        while True:
            block = self._blockchain.get_block_by_blockhash(block.get_prevhash())
            
            if block is None:
                log_error("Miner.get_blocks_between_blockhashes(): ブロックが見つかりませんでした、正しい引数を渡してください")
                return None
            
            if block.get_hash() == from_blockhash:
                break
            
            blocks.append(block)
            
        blocks.reverse() # ブロック高さの昇順に並べ替え
        return blocks
    
    def locate_blockhash_of_fork(self,longer_block) -> str:
        """ 
        フォーク(分岐)が発生した位置(ブロックハッシュ)を返す、特定できない場合Noneを返す
        

                         |--|b3|<-|best_block|         <-- mainchain
        |g|<-|a1|<-|a2|<-|
                    ^    |--|c3|<-|c4|<-|longer_block| <-- longer chain
                    |
                    | fork_blockhash(戻り値はこのブロックのハッシュ値)
        """
        
        fork_blockhash   = self._blockchain.get_best_blockhash()
        longer_blockhash = longer_block.get_hash()

        # 以下のwhile条件が成り立たなくなる時、その位置がフォーク発生位置
        while fork_blockhash != longer_blockhash:

            # fork_blockhash = fork_blockの親のハッシュ値、fork_block = fork_blockの親
            fork_blockhash = self._blockchain.get_block_by_blockhash(fork_blockhash).get_prevhash()
            fork_block     = self._blockchain.get_block_by_blockhash(fork_blockhash)
            
            if fork_block is None:
                log_error("Miner.process_fork(): fork_blockが存在しません")
                return None
            
            longer_block = self._blockchain.get_block_by_blockhash(longer_blockhash)
            
            if longer_block is None:
                log_error("Miner.process_fork(): longer_blockが存在しません")
                return None
            
            # longer_blockの高さとfork_blockの高さが同じになるまで、longer_blockを"下げていく"
            while longer_block.get_height() > fork_block.get_height():
                
                # longer_blockhash = longer_blockの親のハッシュ値、longer_block = longer_blockの親
                longer_blockhash = self._blockchain.get_block_by_blockhash(longer_blockhash).get_prevhash()
                longer_block     = self._blockchain.get_block_by_blockhash(longer_blockhash)
                
                if longer_block is None:
                    log_warning("Miner.process_fork(): longer_blockが存在しません")
                    return None

        return fork_blockhash

    def process_fork(self,longer_block) -> None:
        """ メインチェーンをより長いフォークに切り替える """
        
        # フォークの発生した位置(厳密にはブロックハッシュ)を特定
        fork_blockhash = self.locate_blockhash_of_fork(longer_block)
        if fork_blockhash is None:
            log_error("Miner.process_fork(): フォークの位置を特定できませんでした")
            return False

        # メインチェーンから切り離すブロックのリストをdisconnectに、新たにメインチェーンに追加するブロックのリストをconnectに保存
        disconnect = self.get_blocks_between_blockhashes(fork_blockhash,self._blockchain.get_best_blockhash())
        connect    = self.get_blocks_between_blockhashes(fork_blockhash,longer_block.get_hash())

        if disconnect is None or connect is None:
            log_error("Miner.process_fork(): メインチェーン切り替えのために必要なブロック取得に失敗")
            return False
        
        resurrect = [] # 切り離すブロック内のすべてのtxを格納
        delete    = [] # 追加するブロック内のすべてのtxを格納 

        # 切り離すブロックからTxを集める(後にそのTxをMempoolに戻し、WalletMapから削除)
        for block in disconnect:
            for tx in block.txs:
                resurrect.append(tx)

        # 新たに追加するブロックからTxを集める(後にそのTxをMempoolから削除)
        for block in connect:
            for tx in block.txs:
                delete.append(tx)
                                
        # メインチェーンを切り替え
        self._blockchain.switch_mainchain(fork_blockhash,connect)

        # 切り離すブロックからTxを取り戻す(Txの使用をキャンセルしたため)
        for tx in resurrect:      
            self.receive_tx(tx)     # "古い"TxをMempoolに戻す
            self._wallet.remove(tx) # "古い"TxをWalletから削除
            
        # 追加するブロック内のTxをMempoolから削除(そのTxは使用したため)
        for tx in delete:            
            self._mempool.remove(tx)
            self._wallet.add_tx_if_mine(tx) # "新しい"TxをWalletに追加

    def process_orphan_tx(self,tx) -> None:
        """ OrphanでなくなったTxを削除したり、Mempoolに移したりする(process_orphan_blockと似ている) """
        
        parents = [tx]
        while len(parents) != 0:
            parent  = parents.pop()
            parent_txid = parent.get_hash()
            children = self._children_of_orphan_tx.get(parent_txid)
            
            for child in children:
                self._mempool.add(child)
                parents.append(child)
                self._orphan_txs.remove(child.get_hash())
                log_info("Miner.process_orphan_tx(): TxをOrphanから開放しました")
                
            self._children_of_orphan_tx.remove(parent_txid) # 親をOrphanPrevマップ削除
        
    def process_orphan_block(self,block: Block) -> None:
        """ 
           blockを親としてもつOrphanブロックを見つけ、self._orphan_block,self._children_of_orphan_blockから必要でなくなったブロックを削除。
        
           [説明] 新しいブロックを受け取ったことで、今までにOrphanであったがそうではなくなるブロックが出てくる。(つまり新たに受け取ったブロックがOrphanの親となる場合)
                  そうした場合に、OrphanではなくなったブロックをOrphanマップから削除。
                  そのOrphanでなくなったブロックが、他のOrphanの親となる可能性があるので再帰的に実行。
        """
        
        parents = [block]
        while len(parents) != 0:
            parent = parents.pop()
            parent_blockhash = parent.get_hash()
            children  = self._children_of_orphan_block.get(parent_blockhash)
            
            for child in children:
                child_hash = child.get_hash()
                if self.process_block(child):
                    parents.append(child)
                self._orphan_blocks.remove(child_hash)    # 子をorphan_blocksから削除 
                log_info("Miner.process_orphan_block(): BlockをOrphanから開放しました")
                
            self._children_of_orphan_block.remove(parent_blockhash) # 親をchildren_of_orphan_block削除

    def accept_tx(self,tx: Tx,block: Block = None) -> bool:
        """ 
        Txがブロックチェーンに受け入れられる場合True、そうでない場合Falseを返す
        [条件]
        1) Txが使用しているすべてのUTXOがブロックチェーン或いはMempoolに存在する
        2) Tx.vin内のすべてのTxIn.idxが正しいインデックス
        3) Txのデジタル署名が正しい
        4) ダブルスペンドされていない        
        """
        
        # コインベースTxの場合は無条件にValid
        if tx.is_coinbase():
            return True
        
        for txin in tx.vin:

            parent_tx_found = False
            parent_tx       = None
            
            # ブロックチェーン或いはMempoolからTxを取り出す
            parent_tx = self.get_tx_from_blockchain_or_mempool_by_txid(txin.txid)            
            if parent_tx is not None: 
                parent_tx_found = True
                
            # parent_txをブロック内から探す
            if not parent_tx_found and block is not None:
                parent_tx = block.get_tx_by_txid(txin.txid)
                if parent_tx is not None:
                    parent_tx_found = True

            # 親Txが見つからない場合
            if not parent_tx_found:
                log_info("Miner.accept_tx(): Orphan Txを発見")
                log_debug("Orphan Tx txid: %s"%(tx.get_hash()))
                self.store_ophan_tx(tx)                    
                return False

            # txin.idxが正しいかチェック
            if not parent_tx.is_vout_idx_valid(txin.idx):
                log_error("Miner.accept_tx(): tx.idxのインデックスが正しくない")
                return False

            # txinが参照しているtxoutを取り出す
            txout = parent_tx.vout[txin.idx]
            
            # デジタル署名チェック
            if not check_signature(tx,txin,txout):
                log_error("Miner.accept_tx(): txのデジタル署名チェックに失敗")
                return False
            
            # ダブルスペンドチェック
            if self._blockchain.is_tx_double_spent(tx):
                log_error("Miner.accept_tx(): txはすでに使用されています")
                return False
        
        return True

    def receive_tx(self,tx: Tx) -> bool:
        """ 
        マイナーがTxを受け取った場合True、そうでない場合Falseを返す。
        
        マイナーがTxを受け取るときにこの関数を呼び出す
        """
        
        if tx is None:
            log_error("Miner.receive_tx(): txがNone")
            return False
        
        # コインベースTxは受け付けない
        if tx.is_coinbase():
            log_error("Miner.receive_tx(): コインベースは受け付けない")
            return False
        
        # Txチェック
        if not tx.is_valid():
            log_error("Miner.receive_tx(): Txチェックに失敗")
            return False

        # すでにMempoolに存在
        if self._mempool.contains(tx):
            log_error("Miner.receive_tx(): すでにTxがMempoolに存在")
            return False

        # ブロックチェーン、Mempoolに依存したTxチェック
        if not self.accept_tx(tx):
            log_error("Miner.receive_tx(): Txが受け入れられなかった")
            return False

        # TxをMempoolに追加
        self._mempool.add(tx)
         
        # Orphan Txを処理
        self.process_orphan_tx(tx)

        return True

    def is_forked(self,block: Block) -> bool:
        """ フォークが発生した場合Trueを返す """
        
        height  = self._blockchain.get_height_of_block_from_parent(block)
        
        if height is None:
            log_error("Miner.is_forked(): ブロックの親がブロックチェーン上に存在しないため、高さを取得できませんでした")
            return False

        prevhash       = block.get_prevhash()
        best_height    = self._blockchain.get_best_height()
        best_blockhash = self._blockchain.get_best_blockhash()
        
        if height > best_height and prevhash != best_blockhash:
            return True
        else:
            return False

    def process_block(self,block):
        """ ブロックが受け入れられた場合、あるいはフォークが発生した場合Trueを返す """
        
        success = True
                        
        # ブロックを保存
        self.store_block(block)

        # ブロックチェック
        if self.accept_block(block):
        
            # ブロックをブロックチェーンに追加
            self._blockchain.add_block_into_mainchain(block)
            
            # ブロック内のTxをWalletに追加
            self.add_block_tx_to_wallet_if_mine(block)
            
            # ブロック内のTxをMempoolから削除
            self._mempool.remove(block.txs)

        else:
            log_warning("Miner.process_block(): ブロックを受け入れられなかった")
            success = False
        
        # フォークが発生した場合
        if self.is_forked(block):        
            log_info("Miner.process_block(): フォーク発生")
            self.process_fork(block)
            success = True
            
        # Orphanブロックを処理
        self.process_orphan_block(block)

        return success
    
    def accept_block(self,block: Block) -> bool:
        """ 
        ブロックがブロックチェーンに受け入れられる場合True、そうでない場合Falseを返す。

        [条件]
        1) Genesisブロックではない
        2) ブロックチェック(PoW,Merkle root,etc.)
        3) ブロックチェーンに依存したブロックチェック(ブロックがすでにメインチェーンに存在しない、かつブロックの親が存在する)
        4) メインチェーンに追加しようとする場合、すべてのTxが正しいかチェック
        """

        # Genesisブロックの場合
        if block.is_genesis():
            log_error("Miner.accept_block(): Genesisブロックは追加できません")
            return False

        # ブロックチェック
        if not block.is_valid():
            log_error("Miner.accept_block(): ブロックチェックに失敗")
            return False

        # ブロックチェーンに依存したブロックチェック
        if not self._blockchain.accept_block_on_blockchain(block):
            log_error("Miner.accept_block(): ブロックチェーンに依存したブロックチェックに失敗")
            return False

        # ブロックチェーンの最新情報を取得
        best_blockhash = self._blockchain.get_best_blockhash()
        best_height    = self._blockchain.get_best_height()

        # ブロックの高さを親から計算
        block_height = self._blockchain.get_height_of_block_from_parent(block)        
        
        # ブロックの高さが、メインチェーンより高い場合
        if block_height > best_height:

            # ブロックがメインチェーンの後ろに追加される場合
            if block.get_prevhash() == best_blockhash:
                
                # ブロック内のすべてのTxが正しいかチェック
                if not self.can_accept_txs_in_block(block):
                    log_info("Miner.accept_block(): ブロック内に正しくないTxが存在")
                    return False
                
            else:
                log_info("Miner.accept_block(): フォーク発生")
                return False
            
        else:
            log_info("Miner.accept_block(): ブロック高さが低い[ブロック高さ:%d,ブロックチェーン高さ:%d]"%(block.height,best_height))
            
        return True

    def receive_block(self,block: Block) -> bool:
        """ 
        マイナーがブロックを受け取った場合True、そうでない場合Falseを返す。

        マイナーがブロックを受け取るときにこの関数を呼ぶ 
        """
        
        blockhash = block.get_hash()

        # ブロックがすでにブロックチェーンに含まれている
        if self._blockchain.contains_blockhash(blockhash):
            log_error("Miner.receive_block(): ブロックがすでにブロックチェーンに含まれています")
            return False

        # ブロックがすでにOrpmanブロックマップに含まれている
        if self._orphan_blocks.contains(blockhash):
            log_error("Miner.receive_block(): ブロックがすでにOrphanマップに含まれています")
            return False
        
        # ブロックが正しいかチェック
        if not block.is_valid():
            log_error("Miner.receive_block(): ブロックが正しくない")
            return False

        # ブロックの親が存在しない場合、Orphanブロックマップに保存
        prevhash = block.get_prevhash()        
        if not self._blockchain.contains_blockhash(prevhash):
            log_info("Miner.receive_block(): ブロックをOrphanマップに追加しました")
            self._orphan_blocks.add(block)
            self._children_of_orphan_block.add(prevhash,block)
            
            return True

        # ブロック処理
        if not self.process_block(block):
            log_error("Miner.receive_block(): ブロック処理に失敗しました")
            return False
        
        return True            
        
    def select_txs_from_mempool(self) -> list[Tx]:
        """        
        MempoolからTxを集めて、リストとして返す
        
        1) Phase-1で、親がブロックチェーン上に存在するTxを集める
        2) Phase-2で、親がtxs内に存在するTxを集める
        
        このようにする理由は、1)でOrphan Tx(親がブロックチェーン上に無く、Mempool上にあるTx)が
         txsに追加されるのを防ぐため。(最悪の場合、txs内のすべてのTxがOrphan、この場合ブロックが弾かれる)
        そして2)で、txsに格納されたTxの子を回収する。
        """
        
        txs      = []
        txs_txid = []        
        is_valid = True
        
        # Phase-1 (Txの親がブロックチェーン上に存在する場合にのみ、txsに追加)
        for tx in self._mempool.values():
            is_valid = True
            
            for txin in tx.vin:                
                # txが使用しているUTXOがブロックチェーン上に存在するかチェック
                if not self._blockchain.contains_txid(txin.txid):
                    is_valid = False
                    break

            if is_valid:
                txs.append(tx)
                txs_txid.append(tx.get_hash())
                
            # ブロック内に詰め込めるTx数の上限に達した場合
            if len(txs) == NUM_TX_IN_BLOCK:
                return txs
            
        # Phase-2 (txsに格納されたTxの子を回収)
        for tx in self._mempool.values():
            if tx in txs:
                continue

            is_valid = True
            for txin in tx.vin:
                # txの親がブロックチェーン上にも、txs上にも存在しない
                if not self._blockchain.contains_txid(txin.txid) and not txin.txid in txs_txid:
                    is_valid = False
                    break

            if is_valid:
                txs.append(tx)
                txs_txid.append(tx.get_hash())
                
            # ブロック内に詰め込めるTx数の上限に達した場合
            if len(txs) == NUM_TX_IN_BLOCK:
                return txs

        return txs

    def create_coinbase_tx(self) -> Tx:
        """ コインベースTxを返す """
        txin = TxIn()
        txout = TxOut(COINBASE_REWARD,self._address,self._locking_script)
        
        # コインベースTxのtxidをブロックごとに異なるようにするための処置
        # (タイムスタンプだけでも十分だと思うが、一秒以内に連続してコインベースTxを生成した場合、全く同じtxidとなる)        
        txin.coinbase = get_timestamp() + "," + self._blockchain.get_best_blockhash()
        
        tx = Tx([txin],[txout])
        return tx

    def create_tx(self,address,target_amount) -> Tx:
        """ address宛にtarget_amount分のお金を送るためのTxを生成し返す、生成に失敗した場合Noneを返す """
        tx = self._wallet.create_tx(self._pubkey,self._privkey,address,target_amount)
        
        if tx is None:
            log_error("Miner.create_tx(): Txを生成できませんでした")
            return None
        else:
            self.receive_tx(tx)

        return tx
    
    def create_new_block(self) -> Block:
        """ 新たなブロックを作り、返す """
        
        block = Block()

        # コインベースTxをブロックに追加
        coinbase_tx = self.create_coinbase_tx()
        block.txs.append(coinbase_tx)

        # MempoolからTxを取り出し、ブロックに追加
        txs = self.select_txs_from_mempool()
        block.txs.extend(txs)
                
        block.blockheader.timestamp  = get_timestamp()
        block.blockheader.merkleroot = block.compute_merkleroot()
        block.blockheader.prevhash   = self._blockchain.get_best_blockhash()        
        
        return block

    def mine(self) -> Block:
        """  新たにブロックを作り、そのブロックをマイニングした後に返す """
        
        log_info("Miner.mine(): マイニングを開始")
        block = self.create_new_block()
        block.mine()
        log_info("Miner.mine(): マイニング終了")
        
        # 一連のブロックチェックをした後に、ブロックチェーンに追加
        if not self.receive_block(block):
            return None # 失敗した場合
        else:
            return block
        
