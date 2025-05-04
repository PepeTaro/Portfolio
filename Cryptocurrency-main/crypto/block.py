from dataclasses import dataclass

from crypto.params import GENESIS_BLOCKHASH
from crypto.blockheader import Blockheader
from crypto.tx import Tx
from crypto.merkle import compute_merkleroot
from crypto.timestamp import get_timestamp
from crypto.json import json_dumps
from crypto.logging import log_error

@dataclass
class Block:
    blockheader: Blockheader = None # ブロックヘッダー
    txs:         list[Tx]    = None # Txを含んだリスト
    height:      int         = None # ブロックチェーン内での高さ(Genesisブロックが高さ0)
    
    def __post_init__(self) -> None:
        if self.blockheader is None:
            self.blockheader = Blockheader()
        if self.txs is None:
            self.txs = []
        if self.height is None:
            self.height = -1
            
    def to_json(self) -> str:
        return json_dumps(self)
    
    def mine(self) -> None:
        """ このブロックのマイニング開始 """
        while not self.blockheader.is_mined():
            self.blockheader.inc_nonce()
        
    def compute_merkleroot(self) -> str:
        """ ブロック内のTxからMerkle rootを計算して返す """
        return compute_merkleroot(self.txs)

    def get_num_txs(self) -> int:
        """ ブロック内に格納されたTxの数を返す """
        return len(self.txs)
    
    def get_height(self) -> int:
        return self.height
    
    def get_tx_by_txid(self,txid: str) -> Tx:
        """  ブロック内にtxidをもつTxが存在する場合そのTx、そうでない場合Noneを返す """
        for tx in self.txs:
            if tx.get_hash() == txid:
                return tx            
        return None

    def is_genesis(self) -> bool:
        """ Genesisブロックの場合True、そうでない場合Falseを返す"""
        if self.get_hash() == GENESIS_BLOCKHASH:
            return True
        else:
            return False
                
    def contains_txid(self,txid: str) -> bool:
        """ ブロック内にtxidを持つTxが存在する場合True、そうでない場合Falseを返す """
        for tx in self.txs:
            if tx.get_hash() == txid:
                return True
        return False
    
    def contains_tx(self,tx: Tx) -> bool:
        """ 引数で与えられたTxがブロック内に存在する場合True、そうでない場合Falseを返す """
        return self.contain_txid(tx.get_hash())
        
    def is_txs_empty(self) -> bool:
        """ ブロック内にTxが含まれない場合True、含まれる場合Falseを返す """
        return self.get_num_txs() == 0
    
    def is_merkleroot_valid(self) -> bool:
        """ ブロックヘッダーのMerkle Rootが正しいかチェック。"""        
        return self.compute_merkleroot() == self.blockheader.merkleroot

    def is_valid(self) -> bool:
        """ ブロックが正しいかチェック """

        # Txが含まれているか(少なくともコインベースTxが含まれているはず)
        if self.is_txs_empty():
            log_error("Block.check_block(): txsが空")
            return False

        # txsの先頭がコインベースTxであるかチェック
        if not self.txs[0].is_coinbase():
            log_error("Block.check_block(): txsの先頭がコインベースでない")
            return False

        # txsの先頭以外はコインベースTxでない
        for i in range(1,len(self.txs)):
            if self.txs[i].is_coinbase():
                log_error("Block.check_block(): コインベースがtxs内に複数ある")
                return False
        
        # txs内のすべてのTxが正しいかチェック
        for tx in self.txs:
            if not tx.is_valid():
                log_error("Block.check_block(): 正しくないtxが存在")
                return False

        # Proof-Of-Workチェック
        if not self.is_mined():
            log_error("Block.check_block(): PoWが正しくない")
            return False

        # Merkle rootチェック
        if not self.is_merkleroot_valid():
            log_error("Block.check_block(): Merkle rootが正しくない")
            return False

        return True
    
    ### 以下のコードは、単にBlockheaderのラッパー(使いやすいため)　####
    def get_hash(self) -> str:
        return self.blockheader.get_hash()
    
    def inc_nonce(self) -> None:
        return self.blockheader.inc_nonce()
    
    def is_mined(self) -> bool:
        return self.blockheader.is_mined()

    def get_prevhash(self) -> str:
        return self.blockheader.prevhash

    def get_merkleroot(self) -> str:
        return self.blockheader.merkleroot

    def get_timestamp(self) -> str:
        return self.blockheader.timestamp

    def get_nonce(self) -> str:
        return self.blockheader.nonce

    def get_difficulty(self) -> str:
        return self.blockheader.difficulty

        
