from dataclasses import dataclass

from crypto.logging import log_error
from crypto.json   import json_dumps
from crypto.sha256 import sha256

@dataclass
class TxIn:
    txid: str             = None # TxInが参照しているTxのハッシュ値
    idx: int              = None # TxInが参照しているTx.voutのインデックス
    unlocking_script: str = None # デジタル署名チェック時に必要
    coinbase: str         = None # コインベースTxの場合は coinbase != None
    
    def __post_init__(self) -> None:
        if self.txid is None:
            self.txid = None
        if self.idx is None:
            self.idx = -1
        if self.unlocking_script is None:
            self.unlocking_script = ""
        if self.coinbase is None:
            self.coinbase = ""
        
    def __eq__(self,other) -> bool:
        return self.txid == other.txid and self.idx == other.idx and self.unlocking_script == other.unlocking_script and self.coinbase == coinbase
    
    def __ne__(self,other) -> bool:
        return self != other
    
    def to_json(self) -> str:
        return json_dumps(self)
    
@dataclass
class TxOut:
    amount: int         = None # 金額
    address: str        = None # アドレス
    locking_script: str = None # デジタル署名時に必要

    def __post_init__(self) -> None:
        if self.amount is None:
            self.amount = -1
        if self.address is None:
            self.address = ""
        if self.locking_script is None:
            self.locking_script = ""

    def __eq__(self,other) -> bool:
        return self.amount == other.amount and self.address == other.address and self.locking_script == other.locking_script
    
    def __ne__(self,other) -> bool:
        return self != other

    def to_json(self) -> str:
        return json_dumps(self)

    def is_mine(self,address) -> bool:
        return self.address == address
    
    def get_amount(self,address) -> int:
        if self.is_mine(address):
            return self.amount
        else:
            return 0
        
@dataclass
class Tx:    
    vin:  list[TxIn]   = None
    vout: list[TxOut]  = None
    
    def __post_init__(self) -> None:
        if self.vin is None:
            self.vin  = []
        if self.vout is None:
            self.vout = []

    def to_json(self) -> str:
        return json_dumps(self)    

    def to_string(self) -> str:
        """ txin.unlocking_scriptを含めていないことに注意(txidをunlocking_scriptに依存させたくないため) """
        
        s = "TX(vin=["
        for txin in self.vin:
            s += "TxIn(txid=%s,idx=%d,coinbase=%s),"%(txin.txid,txin.idx,txin.coinbase)
            
        s += "],vout=["
        for txout in self.vout:
            s += "TxOut(amount=%d,address=%s,locking_sciprt=%s),"%(txout.amount,txout.address,txout.locking_script)

        s += "])"
        return s
    
    def is_coinbase(self) -> bool:
        return self.get_size_of_vin() == 1 and self.vin[0].txid == None and self.vin[0].coinbase != None
        
    def is_valid(self) -> bool:
        """ Txの基本的なチェック """
        
        # vin,voutは共に非空
        if self.is_vin_empty() or self.is_vout_empty():
            log_error("Tx.check_tx(): Either vin or vout is empty")
            return False

        # すべてのtxoutが、txout.amout >= 0を満たす
        for txout in self.vout:
            if txout.amount < 0:
                log_error("Tx.check_tx(): The amount of some txout is negative")
                return False

        # コインベースではない場合、すべてのtxinがtxin.txid != Noneを満たす
        if not self.is_coinbase():
            for txin in self.vin:
                if txin.txid == None:
                    log_error("Tx.check_tx(): some txid is None")
                    return False

        return True
        
    def is_mine(self,address) -> bool:
        """ self.vout内にaddress宛のtxoutが含まれる場合Trueを返す """
        for txout in self.vout:
            if txout.is_mine(address):
                return True
        return False
    
    def get_amount(self,address) -> int:
        """ txout.address == addressを満たすtxoutの合計金額を返す"""
        credit = 0
        for txout in self.vout:
            credit += txout.get_amount(address)
        return credit
    
    def get_hash(self) -> str:
        """ Txのハッシュ値(txid)を返す """
        return sha256(self.to_string())

    def get_size_of_vin(self) -> int:
        return len(self.vin)

    def get_size_of_vout(self) -> int:
        return len(self.vout)

    def is_vin_idx_valid(self,idx) -> bool:
        return 0 <= idx < self.get_size_of_vin()

    def is_vout_idx_valid(self,idx) -> bool:
        return 0 <= idx < self.get_size_of_vout()

    def is_vin_empty(self) -> bool:
        self.get_size_of_vin() == 0

    def is_vout_empty(self) -> bool:
        self.get_size_of_vout() == 0

class WalletTx(Tx):
    """ Walletに格納されるTx。Txと唯一の違いはself.spentを持つか否か。 """
    def __init__(self,tx: Tx=Tx(),spent: bool=False) -> None:
        super().__init__(list(tx.vin),list(tx.vout))
        self.spent     = spent # すでに使用されているかチェック
        
