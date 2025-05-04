from collections import defaultdict

from crypto.params import GENESIS_BLOCKHASH,GENESIS_DIFFICULTY,GENESIS_NONCE
from crypto.map import Map
from crypto.multimap import MultiMap
from crypto.block import Block
from crypto.blockheader import Blockheader
from crypto.tx import Tx,TxIn,TxOut
from crypto.json import json_dumps
from crypto.logging import log_error,log_info

"""                              
参考図
[注意] 最も長いフォークがメインチェーンとなる

                                 ----|a3| <-- |a4| <-- |a5| <-- |a6| (Fork1,mainchain)
                                 |
 |genesis| <--|a1| <--- |a2| <---|
     ^                           |
     |                           ----|b3| <-- |b4|                   (Fork2)
     |
     ---------|c1| <--- |c2| <------ |c3| <-- |c4| <-- |c5|          (Fork3)


self._mainchain = [a1,a2,a3,a4,a5,a6]
self._blocks    = [a1,a2,a3,a4,a5,a6,b3,b4,c1,c2,c3,c4,c5]

(一部の例)
self._children_of_block[genesis] = [a1,c1]
self._children_of_block[a2]      = [a3,b3]
self._children_of_block[a1]      = [a2]
self._children_of_block[b4]      = []

"""

class Blockchain:
    def __init__(self) -> None:
        
        self._mainchain          = []                # メインチェーンのハッシュを保存                 ,list[blockhash: str]
        self._blocks             = Map[Block]()      # 今までに受け取ったブロックすべてを格納         ,dict[blockhash:str,block:Block]
        self._children_of_block  = MultiMap[Block]() # ブロックハッシュから子のリストを得るために必要 ,dict[prevhash:str,blocks:list[Block]]
        
        self._best_height     = 0    # メインチェーンの高さ
        self._best_block      = None # メインチェーンの一番後ろにあるブロック
        self._best_blockhash  = None # メインチェーンの一番後ろにあるブロックのハッシュ値
        
        self.init()
            
    def init(self) -> None:
        """ Genesis Blockを追加 """
        blockheader  = Blockheader("GENESIS","GENESIS","GENESIS",GENESIS_DIFFICULTY,GENESIS_NONCE)
        tx           = Tx([TxIn("GENESIS",-1,"GENESIS")],[TxOut(-1,"GENESIS","GENESIS")])        
        self.genesis = Block(blockheader,[tx],0)
        
        self.genesis.mine()
        assert self.genesis.get_hash() == GENESIS_BLOCKHASH
        
        self._add_block(self.genesis)
        
    def to_json(self) -> str:
        return json_dumps(self)

    def get_mainchain(self) -> list[str]:
        return list(self._mainchain)
    
    def get_best_block(self) -> Block:
        return self._best_block
            
    def get_best_height(self) -> int:
        return self._best_height
        
    def get_best_blockhash(self) -> str:
        return self._best_blockhash
    
    def get_block_by_blockhash(self,blockhash: str) -> Block:
        """ ブロックハッシュからブロックを取得し返す、存在しない場合Noneを返す"""
        return self._blocks.get(blockhash)

    def get_blocks_by_prevhash(self,prevhash: str) -> list[Block]:
        """ prevhashをブロックハッシュとして持つブロックの子を返す """
        return self._children_of_block.get(prevhash)

    def get_height_of_block_from_parent(self,block: Block) -> int:
        """ 親ブロックを探してそこからブロック高さを計算し返す、親が存在しない場合Noneを返す """
        
        prevblock = self.get_block_by_blockhash(block.get_prevhash())
        if prevblock is None:
            log_error("Blockchain.get_height_of_block(): ブロックの親が存在しません")
            return None
        
        return prevblock.get_height() + 1 # 子の高さ = 親の高さ + 1
        
    def get_tx_by_txid(self,txid: str) -> Tx:
        """ txidをもつTxをメインチェーンから探して存在するなら返す """
        
        for blockhash in self._mainchain:
            block = self.get_block_by_blockhash(blockhash)
            tx = block.get_tx_by_txid(txid)
            if tx is not None:
                return tx
            
        return None
    
    def contains_txid(self,txid: str) -> bool:
        """ txidをもつTxがメインチェーンに存在する場合True、そうでない場合Falseを返す """
        
        for blockhash in self._mainchain:
            block = self.get_block_by_blockhash(blockhash)
            if block.contains_txid(txid):
                return True
        return False
            
    def contains_blockhash(self,blockhash) -> bool:
        """ blockhashをブロックハッシュとしてもつブロックが存在する場合True、そうでない場合False """
        return self._blocks.contains(blockhash)
    
    def is_tx_double_spent(self,tx) -> bool:
        """ Txがダブルスペンドされている場合True、そうでない場合Falseを返す """
        txid = tx.get_hash()
        for blockhash in self._mainchain:
            block = self.get_block_by_blockhash(blockhash)
            if block.contains_txid(txid):
                return True
        return False
    
    def is_block_in_mainchain(self,block) -> bool:
        """ ブロックがメインチェーン内に含まれている場合True、そうでない場合Falseを返す """
        blockhash = block.get_hash()
        if blockhash in self._mainchain:
            return True          
        return False

    def add_block_into_mainchain(self,block) -> bool:
        """ ブロックをメインチェーンに追加する """
        if block.get_prevhash() != self._mainchain[-1]:
            log_error("Blockchain.add_block_into_mainchain(): ブロックのブロックハッシュが正しくない")
            return False
        
        self._add_block(block)        
        return True
    
    def _add_block(self,block) -> None:
        """ ブロックをメインチェーンと他のプールに追加し情報を更新 """
        self._mainchain.append(block.get_hash())
        self.add_block_by_blockhash(block.get_hash(),block)
        self.add_block_by_prevhash(block.get_prevhash(),block)        
        self.update_best_info(block)
        
    def add_block_by_blockhash(self,blockhash,block) -> None:
        """ self._blocks[blockhash] = block　の形でブロックを追加 """
        self._blocks.add(blockhash,block)
        
    def add_block_by_prevhash(self,prevhash,block) -> None:
        """ self._children_of_block[prevhash].append(block)をしている """
        self._children_of_block.add(prevhash,block)
        
    def detach_mainchain(self,detach_blockhash: str) -> bool:
        """ detach_blockhashから一番後ろのブロックまでに存在するブロックを、メインチェーンから削除する """
        
        for i in range(len(self._mainchain)):
            if self._mainchain[i] == detach_blockhash:
                self._mainchain = self._mainchain[:i+1]
                return True
        return False

    def switch_mainchain(self,fork_blockhash: str,blocks: list[Block]) -> bool:
        """ fork_blockhashから一番後ろのブロックまでに存在するブロックを削除し、blocks内のブロックで置き換える """
        
        if not self.detach_mainchain(fork_blockhash):
            log_error("Blockchain.switch_mainchain(): ブロックチェーンの切り離しに失敗")
            return False
        
        # メインチェーンを切り替え
        for block in blocks:
            if not self.add_block_into_mainchain(block):
                log_error("Blockchain.switch_mainchain(): ブロックの追加に失敗")
                return False

        log_info("Blockchain.switch_mainchain(): メインチェーンを切り替えました")
        return True

    def accept_block_on_blockchain(self,block: Block) -> bool:        
        """ 
        ブロック追加に必要な最低限の条件を満たしている場合True、そうでない場合Falseを返す
        1) ブロックがメインチェーンにすでに存在しない
        2) ブロックの親がself._blocks内に存在する(つまり親ブロックをすでに受け取っている)
        """
        
        if self.is_block_in_mainchain(block):
            log_error("Blockchain.is_block_valid_on_blockchain(): すでにブロックがメインチェーン上に存在します")
            return False

        if not self.contains_blockhash(block.get_prevhash()):
            log_error("Blockchain.is_block_valid_on_blockchain(): ブロックの親が存在しません")
            return False      
        
        return True    
    
    def update_best_info(self,block: Block) -> None:        
        """ blockがブロックチェーンの最も後ろのブロックになると仮定して情報更新 """
        self._best_block     = block
        self._best_height    = block.get_height()
        self._best_blockhash = block.get_hash()
        log_info("Blockchain.update_best_info(): ブロックチェーンの情報を更新しました")
