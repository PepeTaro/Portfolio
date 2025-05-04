import unittest
import time
import math
import random

from crypto.block import Block
from crypto.blockheader import Blockheader
from crypto.blockchain import Blockchain
from crypto.tx    import Tx,WalletTx,TxIn,TxOut
from crypto.wallet import Wallet
from crypto.miner import Miner
from crypto.node import Node
from crypto.timestamp import get_timestamp
from crypto.address import gen_address
from crypto.params import COINBASE_REWARD,NUM_TX_IN_BLOCK

def equals_mainchain(miner1,miner2):
    
    if miner1.get_best_height() != miner2.get_best_height():
        return False
    
    if miner1.get_best_blockhash() != miner2.get_best_blockhash():
        return False

    mainchain1 = miner1.get_mainchain()
    mainchain2 = miner1.get_mainchain()

    if len(mainchain1) -1 != miner1.get_best_height():
        return False
    
    if len(mainchain2) -1 != miner2.get_best_height():
        return False
    
    if mainchain1[-1]  != miner1.get_best_blockhash():
        return False
    
    if mainchain2[-1]  != miner2.get_best_blockhash():
        return False
    
    for blockhash1,blockhash2 in zip(mainchain1,mainchain2):
        if blockhash1 != blockhash2:
            return False

    return True

def assert_mainchain(miner1,miner2):
    """ miner1とminer2のメインチェーンが等しいかチェック """
    return equals_mainchain(miner1,miner2)

def assert_same_height(miners):
    """ すべてのマイナーのメインチェーンの高さが同じ """
    n = len(miners)
    for i in range(n):
        for j in range(i + 1,n):
            if miners[i].get_best_height() != miners[j].get_best_height():
                return False
    return True

def assert_consensus(miners):
    """ すべてのマイナーのメインチェーンが一致 """
    n = len(miners)
    for i in range(n):
        for j in range(i + 1,n):
            if not assert_mainchain(miners[i],miners[j]):
                return False
    return True

class TestCrypto(unittest.TestCase):
    def test_txid(self):
        tx = Tx([TxIn()],[TxOut(10)])
        wtx = WalletTx(tx)
        
        self.assertTrue(tx.to_string() == wtx.to_string())
        self.assertTrue(tx.get_hash() == wtx.get_hash())
    
        old_txid = tx.get_hash()
        tx.vin[0].unlocking_script = "Changed the Unlocking Script"
        
        # unlocking_scriptを変えてもtxidが変わらないかチェック
        self.assertTrue(tx.get_hash() == old_txid)
                        
    def test_basic1(self):
        blockchain = Blockchain()
        miner = Miner()    
        wallet = Wallet(miner._address)
        
        tx = Tx([TxIn()],[TxOut(10,miner._address)])
        self.assertTrue(tx.is_coinbase())
        self.assertTrue(tx.is_valid())
        self.assertTrue(wallet.add_tx_if_mine(tx))
        self.assertTrue(wallet.get_balance() == 10)
        
        tx = wallet.create_tx(miner._pubkey,miner._privkey,"<DUMMY ADDR>",5)
        self.assertTrue(wallet.get_balance() == 5)
        
        tx = wallet.create_tx(miner._pubkey,miner._privkey,"<DUMMY ADDR>",5)
        self.assertTrue(wallet.get_balance() == 0)
    
    def test_basic2(self):

        miner = Miner()
        tx = miner.create_coinbase_tx()
        
        block = miner.create_new_block()
        block.mine()
        
        self.assertTrue(block.is_valid())
        self.assertTrue(miner.receive_block(block))
        
        # すでにblockがブロックチェーンに存在しているため弾かれる  
        self.assertTrue(not miner.receive_block(block))
        
        block = miner.create_new_block()    
        block.blockheader.prevhash = "This prevhash doesn't exist in the blockchain"
        block.mine()
    
        # prevhashを変えたためblockの親がブロックチェーン上にない、よって弾かれる
        self.assertTrue(not miner._blockchain.accept_block_on_blockchain(block))
        self.assertTrue(not miner.accept_block(block))

    def test_basic3(self):
        miner = Miner()
        tx = miner.create_coinbase_tx()
        
        block1 = miner.create_new_block()
        block1.mine()
        self.assertTrue(miner._blockchain.accept_block_on_blockchain(block1))
        self.assertTrue(miner.accept_block(block1))
        
        # ブロックのタイムスタンプが同じ場合blockhashが同じになってしまうため
        time.sleep(1)
        
        block2 = miner.create_new_block()
        block2.mine()
        self.assertTrue(miner._blockchain.accept_block_on_blockchain(block2))
        self.assertTrue(miner.accept_block(block2))
    
        self.assertTrue(block1.get_hash() != block2.get_hash())

    def test_fork1(self):
        miner1 = Miner()
        miner2 = Miner()
        miner3 = Miner()
        
        block11 = miner1.mine()
        time.sleep(1)
        block12 = miner1.mine()
        time.sleep(1)
        block13 = miner1.mine()
    
        self.assertTrue(miner1.get_best_height() == 3)
        self.assertTrue(block11 is not None and block12 is not None and block13 is not None)
        self.assertTrue(miner1.get_balance() == 3*COINBASE_REWARD)
        
        block21 = miner2.mine()
        time.sleep(1)
        block22 = miner2.mine()    
        
        self.assertTrue(miner2._blockchain.get_best_height() == 2)
        self.assertTrue(block21 is not None and block22 is not None)
        self.assertTrue(miner2.get_balance() == 2*COINBASE_REWARD)
        
        block31 = miner3.mine()
        
        self.assertTrue(miner3._blockchain.get_best_height() == 1)
        self.assertTrue(block31 is not None)
        self.assertTrue(miner3.get_balance() == 1*COINBASE_REWARD)
        
        miner2.receive_block(block11)
        miner2.receive_block(block12)
        miner2.receive_block(block13)
        
        miner3.receive_block(block13)
        miner3.receive_block(block12)    
        miner3.receive_block(block11)
        
        # Miner1のメインチェーンが切り替わっていないかチェック
        blockhashes = [block11.get_hash(),block12.get_hash(),block13.get_hash()]
        mainchain = miner1.get_mainchain()
        for i in range(1,len(mainchain)):
            # Genesisを除いているためi+1
            self.assertTrue(blockhashes[i-1] == mainchain[i])
            
        # Miner2のメインチェーンが切り替わったかチェック
        blockhashes = [block11.get_hash(),block12.get_hash(),block13.get_hash()]
        mainchain = miner2.get_mainchain()
        for i in range(1,len(mainchain)):
            # Genesisを除いているためi+1
            self.assertTrue(blockhashes[i-1] == mainchain[i])
            
        # Miner3のメインチェーンが切り替わったかチェック
        blockhashes = [block11.get_hash(),block12.get_hash(),block13.get_hash()]
        mainchain = miner2.get_mainchain()
        for i in range(1,len(mainchain)):
            # Genesisを除いているためi+1
            self.assertTrue(blockhashes[i-1] == mainchain[i])
            
        self.assertTrue(miner1.get_balance() == 3*COINBASE_REWARD)
        self.assertTrue(miner2.get_balance() == 0)
        self.assertTrue(miner3.get_balance() == 0)
        
        self.assertTrue(miner1._blockchain.get_best_height() == 3)
        self.assertTrue(miner1.get_num_of_orphan_blocks() == 0)
        self.assertTrue(miner1.get_size_of_children_of_orphan_block() == 0)
        
        self.assertTrue(miner2._blockchain.get_best_height() == 3)
        self.assertTrue(miner2.get_num_of_orphan_blocks() == 0)
        self.assertTrue(miner2.get_size_of_children_of_orphan_block() == 0)
        
        self.assertTrue(miner3._blockchain.get_best_height() == 3)
        self.assertTrue(miner3.get_num_of_orphan_blocks() == 0)
        self.assertTrue(miner3.get_size_of_children_of_orphan_block() == 0)
        
        self.assertTrue(assert_mainchain(miner1,miner2))
        self.assertTrue(assert_mainchain(miner1,miner3))
    
    def test_fork2(self):
        """
                       --|31|<-|41|<-|51|<-|61|       miner1
                       |
        |g|<-|1|<-|2|<---|32|<-|42|<-|52|             miner2
                            ^
                            |
                            -----|53|<-|63|<-|73|     miner3
    
        最終的にminer1,miner2,miner3のメインチェーンを、最も長いminer3のメインチェーンに収束させる
        """
        # フォークマニュアルテスト
        
        miner1 = Miner()
        miner2 = Miner()
        miner3 = Miner()
        
        block1  = miner1.mine()
        block2  = miner1.mine()
        
        miner2.receive_block(block1)
        miner2.receive_block(block2)
        
        miner3.receive_block(block1)
        miner3.receive_block(block2)
        
        # ここまではメインチェーンは同じ
        self.assertTrue(assert_mainchain(miner1,miner2))
        self.assertTrue(assert_mainchain(miner1,miner3))
        self.assertTrue(miner1.get_balance() == 2*COINBASE_REWARD)
        self.assertTrue(miner2.get_balance() == 0)
        self.assertTrue(miner3.get_balance() == 0)
        
        # ここからフォーク
        block31 = miner1.mine()
        block41 = miner1.mine()
        block51 = miner1.mine()
        block61 = miner1.mine()
        
        block32 = miner2.mine()
        block42 = miner2.mine()
        block52 = miner2.mine()
        
        miner3.receive_block(block32)
        miner3.receive_block(block42)
        
        block53 = miner3.mine()
        block63 = miner3.mine()
        block73 = miner3.mine()
        
        self.assertTrue(miner1.get_best_height() == 6)
        self.assertTrue(miner2.get_best_height() == 5)
        self.assertTrue(miner3.get_best_height() == 7)
        self.assertTrue(miner1.get_balance() == 6*COINBASE_REWARD)
        self.assertTrue(miner2.get_balance() == 3*COINBASE_REWARD)
        self.assertTrue(miner3.get_balance() == 3*COINBASE_REWARD)
        
        # miner3がminer1とminer2のブロックを受け取る、しかしメインチェーンは変わらない(長いから)
        miner3.receive_block(block31)
        miner3.receive_block(block41)
        miner3.receive_block(block51)
        miner3.receive_block(block61)
        self.assertTrue(miner3.get_best_height() == 7)
        self.assertTrue(miner3.get_balance() == 3*COINBASE_REWARD)
        
        miner3.receive_block(block32)
        miner3.receive_block(block42)
        miner3.receive_block(block52)
        self.assertTrue(miner3.get_best_height() == 7)
        self.assertTrue(miner3.get_balance() == 3*COINBASE_REWARD)
        
        # miner2がminer1とminer3のブロックを受け取る、
        # まずメインチェーンがminer1のそれに変わり、次にminer3のそれに変わる
        miner2.receive_block(block31)
        miner2.receive_block(block41)
        miner2.receive_block(block51)
        miner2.receive_block(block61)
        self.assertTrue(miner2.get_best_height() == 6)
        self.assertTrue(miner2.get_balance() == 0)
        
        miner2.receive_block(block53)
        miner2.receive_block(block63)
        miner2.receive_block(block73)
        self.assertTrue(miner2.get_best_height() == 7)
        self.assertTrue(miner2.get_balance() == 2*COINBASE_REWARD)
        self.assertTrue(assert_mainchain(miner2,miner3)) # miner2.mainchain == miner3.mainchain?
        
        # miner1がminer3とminer2のブロックを受け取る、メインチェーンがminer3のそれに変わる    
        miner1.receive_block(block53)
        miner1.receive_block(block63)
        miner1.receive_block(block73)
        self.assertTrue(miner1.get_best_height() == 6) # miner3のブロックはOrphanとなるため、まだメインチェーンは切り替わらない
        self.assertTrue(miner1.get_balance() == 6*COINBASE_REWARD)

        miner1.receive_block(block32)
        miner1.receive_block(block42)
        miner1.receive_block(block52)        
        self.assertTrue(miner1.get_best_height() == 7)
        self.assertTrue(miner1.get_balance() == 2*COINBASE_REWARD)
        self.assertTrue(assert_mainchain(miner1,miner3)) # miner1.mainchain == miner3.mainchain?
        
        
    def test_orphan_tx1(self):
        
        miner1 = Miner()
        miner2 = Miner()    
        block1  = miner1.mine()
        self.assertTrue(miner1.get_balance() == COINBASE_REWARD)
        self.assertTrue(miner2.get_balance() == 0)
        self.assertTrue(miner1.get_best_height() == 1)
        self.assertTrue(miner2.get_best_height() == 0)
        
        tx = miner1.create_tx(miner2.get_address(),10)
        self.assertTrue(miner1.get_balance() == COINBASE_REWARD - 10)
        
        miner2.receive_tx(tx)
        self.assertTrue(miner1.get_num_of_txs_in_mempool() == 1)
        self.assertTrue(miner2.get_num_of_txs_in_mempool() == 0) # block1を受け取っていないため、txはOrphan
        
        block2 = miner1.create_new_block()
        
        block2.mine()
        miner1.receive_block(block2)
        
        self.assertTrue(miner1.get_balance() == 2*COINBASE_REWARD - 10)
        self.assertTrue(miner2.get_balance() == 0)
        self.assertTrue(miner1.get_best_height() == 2)
        self.assertTrue(miner2.get_best_height() == 0)
        
        miner2.receive_block(block2)
        miner2.receive_block(block1)
        
        self.assertTrue(miner2.get_balance() == 10)
        self.assertTrue(miner2.get_best_height() == 2)
        self.assertTrue(assert_mainchain(miner1,miner2))
    
    def test_orphan_tx2(self):
        # Orphan Txマニュアルテスト
        
        """    テストの説明
        
        Blockchain <= txs[-1] <= txs[-2] <= txs[-3] <=  ... <= tx[1] <= txs[0]
                               ^          ^                 ^         ^
                               |          |                 |         |
                             Orphan     Orphan            Orphan    Orphan    

        "<="はTxの依存を表している、例えば"txs[1] <= txs[0]"は、txs[0]のあるtxinがtxs[1]のお釣りを使用している。
        miner2がtxs[-2]からtxs[0]しか受け取らなかった場合、そのTxはすべてOrphanとなる。
        なぜなら、txs[-2]の親がブロックチェーンにもMempoolにも存在しないから、同じ理由でtxs[-3] ... txs[0]もOrphanとなる。
        miner2がtxs[-1]を受け取ると、txs[-2]の親がMempool内に見つかり、txs[-2]がMempoolに追加、同様にtxs[-3]もMempoolに追加...
        最後にすべてがOrphanではなくなる。
        """
        
        miner1 = Miner()
        miner2 = Miner()
        
        # まずプリマインしてお金集め
        for _ in range(2):
            block = miner1.mine()
            miner2.receive_block(block)
            
        self.assertTrue(miner1.get_best_height() == 2)
        self.assertTrue(miner1.get_balance() == 2*COINBASE_REWARD)
        
        self.assertTrue(COINBASE_REWARD >= 4 and COINBASE_REWARD%2 == 0)
        amount = COINBASE_REWARD//2 - 1
        
        txs = []
        num_txs = 0
        # tx2がtx1のお釣りを使用するようにしている
        tx1 = miner1.create_tx(miner2.get_address(),amount) 
        tx2 = miner1.create_tx(miner2.get_address(),COINBASE_REWARD + 1)  
        txs.append(tx1)
        txs.append(tx2)
        num_txs = 2
        
        # 現在miner1の所持金はCOINBASE_REWARD//2
        for i in range(COINBASE_REWARD//2):
            tx = miner1.create_tx(miner2.get_address(),1) 
            txs.append(tx)
            num_txs += 1
            
        # 敢えて逆向きに受け取る。txs[-1]は受け取らない(txs[-1]以外はすべてOrphanとなる)
        txs.reverse()
        for i in range(len(txs)-1):
            miner2.receive_tx(txs[i])
            
        # まだtxs[-1]以外はすべてOrphan
        self.assertTrue(miner2.get_num_of_txs_in_mempool()  == 0)
        self.assertTrue(miner2.get_num_of_orphan_txs() == num_txs - 1)
        
        # txs[-1]を受け取った後に、今までOrphanだったTxがすべてOprhanでなくなる
        miner2.receive_tx(txs[-1])
        self.assertTrue(miner2.get_num_of_txs_in_mempool()  == num_txs)
        self.assertTrue(miner2.get_num_of_orphan_txs() == 0)

    def test_tx1(self):
        # Txマニュアルテスト
        
        miner1 = Miner()
        miner2 = Miner()
        
        # まずプリマインしてお金集め
        for _ in range(2):
            block = miner1.mine()
            miner2.receive_block(block)
            
        for _ in range(2):
            block = miner2.mine()
            miner1.receive_block(block)
            
        self.assertTrue(miner1.get_best_height() == 4)
        self.assertTrue(miner2.get_best_height() == 4)
        self.assertTrue(miner1.get_balance() == 2*COINBASE_REWARD)
        self.assertTrue(miner2.get_balance() == 2*COINBASE_REWARD)
        
        tx1 = miner1.create_tx(miner2.get_address(),10) # [$10| miner1 => miner2]
        tx2 = miner1.create_tx(miner2.get_address(),1)  # [$1 | miner1 => miner2]    
        tx3 = miner2.create_tx(miner1.get_address(),10) # [$10| miner2 => miner1]
        
        self.assertTrue(miner1.get_balance() == 2*COINBASE_REWARD - 11)
        self.assertTrue(miner2.get_balance() == 2*COINBASE_REWARD - 10)
        self.assertTrue(miner1.get_num_of_txs_in_mempool() == 2)
        self.assertTrue(miner2.get_num_of_txs_in_mempool() == 1)
        
        miner1.receive_tx(tx3)
        miner2.receive_tx(tx1)
        miner2.receive_tx(tx2)
        
        self.assertTrue(miner1.get_num_of_txs_in_mempool() == 3)
        self.assertTrue(miner2.get_num_of_txs_in_mempool() == 3)
        
        block = miner1.mine()
        miner2.receive_block(block)
        
        self.assertTrue(len(block.txs) == 4) # コインベース + 上記のTx３つ    
        self.assertTrue(miner1.get_num_of_txs_in_mempool() == 0) # NUM_TX_IN_BLOCK = 10
        self.assertTrue(miner2.get_num_of_txs_in_mempool() == 0) # NUM_TX_IN_BLOCK = 10
        self.assertTrue(miner1.get_balance() == 3*COINBASE_REWARD - 11 + 10)
        self.assertTrue(miner2.get_balance() == 2*COINBASE_REWARD - 10 + 11)
        
    def test_tx2(self):
        # Tx自動テスト
        """
        miner0とminer1間でTxを送り合う自動テスト
        
        [メモ]
        assertで引っかる場合、NUM_MININGの値を上げてみて。
        Txをランダムに受け取っているため、OrphanであるTxがブロックに入れない場合が多々発生する、
        よって、決定的に計算した値num_blocksでは、Txを処理しきれない可能性がある。
        """
        
        miner0 = Miner()
        miner1 = Miner()
        miners = [miner0,miner1]
        
        NUM_MINING     = 9 # マイニング回数(確率的にassert回避用)
        AMOUNT         = 7 # マイナーに送る金額
        NUM_LOOP       = 3 # テストの繰り返し回数
        NUM_PREMINING_MAX = 2 # プリマイン回数(乱数)の上限値
        num_mining0 = random.randint(0,NUM_PREMINING_MAX) 
        num_mining1 = random.randint(0,NUM_PREMINING_MAX)
        
        # まずプリマインしてお金集め
        for _ in range(num_mining0):
            block = miner0.mine()
            miner1.receive_block(block)
            
        for _ in range(num_mining1):
            block = miner1.mine()
            miner0.receive_block(block)
            
        self.assertTrue(miner0.get_balance() == num_mining0*COINBASE_REWARD)
        self.assertTrue(miner1.get_balance() == num_mining1*COINBASE_REWARD)
        self.assertTrue(miner0.get_best_height() == num_mining0 + num_mining1)
        self.assertTrue(miner1.get_best_height() == num_mining0 + num_mining1)
        
        # 現在時点での所持金を取得、後にassert時に使用
        miner0_amount = miner0.get_balance()
        miner1_amount = miner1.get_balance()
        miners_amount = [miner0_amount,miner1_amount]
        
        for _ in range(NUM_LOOP):
            # 所持金に応じて、マイナーの生成するTx数を決定
            num_txs0 = random.randint(0,miner0.get_balance()//AMOUNT)
            num_txs1 = random.randint(0,miner1.get_balance()//AMOUNT)
            txs = []
            
            # ランダムに送信先を決定(自分自身あるいは他方のマイナー)
            for _ in range(num_txs0):
                recipient_idx = random.randint(0,1)
                rand_tx = miner0.create_tx(miners[recipient_idx].get_address(),AMOUNT)
                self.assertTrue(rand_tx is not None)
                txs.append((0,recipient_idx,rand_tx))
                
            for _ in range(num_txs1):
                recipient_idx = random.randint(0,1)
                rand_tx = miner1.create_tx(miners[recipient_idx].get_address(),AMOUNT)
                self.assertTrue(rand_tx is not None)
                txs.append((1,recipient_idx,rand_tx))
                
            # Mempool内のTx数チェック(miner.create_tx時にminerは、生成したTxをMempoolに入れる)
            self.assertTrue(miner0.get_num_of_txs_in_mempool() == num_txs0)
            self.assertTrue(miner1.get_num_of_txs_in_mempool() == num_txs1)
            
            # Txの受け取り方がランダムとなるようにシャッフル
            random.shuffle(txs)
            
            for sidx,ridx,tx in txs:            
                if sidx == 0: # txを作ったマイナーがminer0の場合、miner1がtxを受け取る
                    miner1.receive_tx(tx)
                else: # sidx == 1
                    miner0.receive_tx(tx)
            
                miners_amount[sidx] -= AMOUNT # 送信側
                miners_amount[ridx] += AMOUNT # 受信側
                
            # Txを受け取ったあとに、Mempoolのサイズをチェック
            self.assertTrue(miner0.get_num_of_txs_in_mempool() == miner1.get_num_of_txs_in_mempool() == len(txs))
            
            # txsのサイズとパラメータ(ブロック内に格納できる最大のTx数)から何回マイニングする必要があるか計算
            num_blocks = math.ceil(len(txs)/NUM_TX_IN_BLOCK)
            
            # マイニングした後に、そのブロックを他方のマイナーに送信
            for _ in range(num_blocks + NUM_MINING):
                size_mempool = miner0.get_num_of_txs_in_mempool()
                miner_idx = random.randint(0,1)              # ランダムにマイナーを選ぶ
                miners_amount[miner_idx] += COINBASE_REWARD  # リワード金            
                block = miners[miner_idx].mine()             # マイニング開始
                miners[(miner_idx+1)%2].receive_block(block) # ブロック受信
                
                self.assertTrue(miner0.get_num_of_txs_in_mempool() == miner1.get_num_of_txs_in_mempool()) # Mempoolのサイズが同じかチェック
                
        self.assertTrue(miner0.get_num_of_txs_in_mempool() == miner1.get_num_of_txs_in_mempool() == 0)
        self.assertTrue(miner0.get_num_of_orphan_txs() == miner1.get_num_of_orphan_txs() == 0)

        # 最後に所持金が一致するかチェック
        self.assertTrue(miners[0].get_balance() == miners_amount[0])
        self.assertTrue(miners[1].get_balance() == miners_amount[1])                      

    def test_tx3(self):
        # 自分自身にお金を送るテスト
        
        miner1 = Miner()
        miner2 = Miner()
        block = miner1.mine()
        miner2.receive_block(block)
        
        self.assertTrue(miner1.get_balance() == COINBASE_REWARD)
        self.assertTrue(miner2.get_balance() == 0)
        
        tx = miner1.create_tx(miner1.get_address(),COINBASE_REWARD)
    
        #miner1.receive_tx(tx)
        miner2.receive_tx(tx)

        self.assertTrue(miner1.get_num_of_txs_in_mempool() == 1)
        self.assertTrue(miner2.get_num_of_txs_in_mempool() == 1)
        
        block = miner2.mine()
        miner1.receive_block(block)
        self.assertTrue(miner1.get_num_of_txs_in_mempool() == 0)
        self.assertTrue(miner2.get_num_of_txs_in_mempool() == 0)
    
        self.assertTrue(miner1.get_balance() == COINBASE_REWARD) # 自身にお金を送ったため不変
        self.assertTrue(miner2.get_balance() == COINBASE_REWARD)

    def test_tx4(self):
        # お釣りテスト
        
        miner1 = Miner()
        miner2 = Miner()
        
        # まずプリマインしてお金集め
        for _ in range(2):
            block = miner1.mine()
            miner2.receive_block(block)
            
        self.assertTrue(miner1.get_best_height() == 2)
        self.assertTrue(miner1.get_balance() == 2*COINBASE_REWARD)

        self.assertTrue(COINBASE_REWARD >= 4 and COINBASE_REWARD%2 == 0)
        amount = COINBASE_REWARD//2 - 1
        
        # tx2がtx1のお釣りを使用するようにしている
        tx1 = miner1.create_tx(miner2.get_address(),amount) 
        tx2 = miner1.create_tx(miner2.get_address(),COINBASE_REWARD + 1)  
        
        # tx2がtx1のお釣りを使用しているかチェック
        use_tx1 = False
        for txin in tx2.vin:
            if txin.txid == tx1.get_hash():
                use_tx1 = True
                break        
        self.assertTrue(use_tx1)
        
        # 敢えて逆向きに受け取る
        miner2.receive_tx(tx2)
        miner2.receive_tx(tx1)
        
        block = miner2.mine()
        self.assertTrue(len(block.txs) == 3) # coinbase + 2

    def test_double_spending(self):
        # ダブルスペンド テスト 
        
        miner1 = Miner()    
        block = miner1.mine()
        
        tx = miner1.create_tx("DUMMY ADDRESS",COINBASE_REWARD)
        miner1.mine() # txをブロックチェーンに追加
        
        tx.vout[0].address = miner1.get_address() # 送信先を自分に変えて、お金を取り戻そうとする。
        self.assertTrue(not miner1.receive_tx(tx)) # すでにtxがブロックチェーンに存在するためエラー
        
    def test_consensus(self):
        # コンセンサス テスト
    
        NUM_TXS = 20
        NUM_BLOCKS = 10
        
        # 以下の３つのマイナーはマイニングのみする
        miner1 = Miner()
        miner2 = Miner()
        miner3 = Miner()
        
        # 以下の３つのマイナーはTxのみを生成する
        alice   = Miner()
        bob     = Miner()
        charlie = Miner()
        
        nodes = [miner1,miner2,miner3,alice,bob,charlie]
        miners = [miner1,miner2,miner3]
        users  = [alice,bob,charlie]
        
        # 1) まずTxを作るためのお金を稼ぐ
        for user in users:
            block1 = user.mine()
            block2 = user.mine()
        
            self.assertTrue(block1 is not None and block2 is not None)
            
            # すべてのマイナーがブロックを受け取る
            for node in nodes:
                node.receive_block(block1)
                node.receive_block(block2)

        self.assertTrue(assert_consensus(nodes)) # コンセンサスが得られているかチェック
        
        # 2) ランダムにTxを生成
        txs = []
        for i in range(NUM_TXS):        
            sender    = users[random.randint(0,2)]
            recipient = users[random.randint(0,2)]
            tx = sender.create_tx(recipient.get_address(),random.randint(1,10))
            
            if tx is not None:        
                txs.append(tx)
                
        for miner in miners:
            random.shuffle(txs) # Txの受け取り方をマイナーごとに異なるようにする
            for tx in txs:
                miner.receive_tx(tx)
                
        # 3) ノードはマイニング直後にブロックを受け取る(フォークは発生しない)
        for i in range(NUM_BLOCKS):
            miner = miners[random.randint(0,2)]
            block = miner.mine()
            
            # すべてのマイナーがブロックを受け取る
            for node in nodes:
                node.receive_block(block)    
                
        self.assertTrue(assert_consensus(nodes)) # コンセンサスが得られているかチェック
        
        # 4) 先にマイナーがマイニングをし、その後にすべてのノードがブロックを受け取る
        # 　 よってフォークが発生する
        blocks = []
        for i in range(NUM_BLOCKS):
            miner = miners[random.randint(0,2)]
            block = miner.mine()
            blocks.append(block)
            
        # すべてのマイナーがブロックを受け取る    
        for node in nodes:
            for block in blocks:
                node.receive_block(block)
                
        # メインチェーンの高さが同じがチェック
        # (確率的にコンセンサスが得られないため、メインチェーンの高さのみチェック)
        self.assertTrue(assert_same_height(nodes))

    def test_node(self):
        # ノード同期 テスト
        
        node1 = Node()
        node2 = Node()
        
        node1.mine()
        node1.mine()
        self.assertTrue(node1.get_best_height() == 2)

        node1.sync_with(node2)
        self.assertTrue(node1.get_best_height() == 2)
        self.assertTrue(node2.get_best_height() == 2)

if __name__ == '__main__':
    unittest.main()
