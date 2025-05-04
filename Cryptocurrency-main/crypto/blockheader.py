from dataclasses import dataclass

from crypto.params import MAX_DIFFICULTY,DEFAULT_DIFFICULTY
from crypto.tx import Tx
from crypto.merkle import compute_merkleroot
from crypto.sha256 import sha256
from crypto.timestamp import get_timestamp
from crypto.json import json_dumps

@dataclass
class Blockheader:
    prevhash:   str = None # 親ブロックのハッシュ
    merkleroot: str = None # Merkle root
    timestamp:  str = None # タイムスタンプ
    difficulty: int = None # PoWの難易度
    nonce:      int = None # PoWに使用するNonce
    
    def __post_init__(self) -> None:
        if self.prevhash is None:
            self.prevhash = ""
        if self.merkleroot is None:
            self.merkleroot = ""
        if self.timestamp is None:
            self.timestamp = ""
        if self.difficulty is None:
            self.difficulty = DEFAULT_DIFFICULTY
        if self.nonce is None:
            self.nonce = 0
            
        assert 0 <= self.difficulty <= MAX_DIFFICULTY
        
    def to_json(self) -> str:
        return json_dumps(self)

    def get_hash(self) -> str:
        return sha256(str(self))

    def inc_nonce(self) -> None:
        self.nonce += 1
        
    def is_mined(self) -> bool:
        """ このブロックがマイニングされている場合Trueを返す """
        
        n = int(self.get_hash(),16)  # ブロックハッシュを整数に変換
        
        if n < self.difficulty:
            return True
        else:
            return False
