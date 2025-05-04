import hashlib

def sha256(s: str) -> str:
    assert isinstance(s,str)
    b = s.encode() # bytesクラスに変換
    return _sha256_bytes(b)

def _sha256_bytes(b: bytes) -> str:
    assert isinstance(b,bytes)
    h  = hashlib.sha256(b).hexdigest() 
    return h
