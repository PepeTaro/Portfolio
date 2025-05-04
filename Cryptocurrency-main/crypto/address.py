import hashlib
from crypto.sha256 import sha256

def _ripemd160(s: str) -> str:
    assert isinstance(s,str)
    b = s.encode()
    return _ripemd160_bytes(b)

def _ripemd160_bytes(b: bytes) -> str:
    assert isinstance(b,bytes)    
    ripemd160 = hashlib.new("ripemd160")
    ripemd160.update(b)
    ret = ripemd160.hexdigest()
    return ret

def gen_address(pubkey: str) -> str:
    """ 公開鍵pubkeyからアドレスを生成。"""
    
    assert isinstance(pubkey,str)
    return _ripemd160(sha256(pubkey))
