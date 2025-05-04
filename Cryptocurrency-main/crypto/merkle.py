from crypto.tx import Tx
from crypto.sha256 import sha256

def compute_merkleroot(txs: list[Tx]) -> str:
    """ Merkle rootを計算 """
    assert isinstance(txs,list)

    if len(txs) == 0:
        sha256("")
    else:
        return _recursive_merkleroot(txs)

def _recursive_merkleroot(txs: list[Tx]) -> str:
    """ 再帰的にMerkle rootを計算 """
    
    len_txs = len(txs)

     # if txs = [tx] => return Hash(Hash(tx) + Hash(tx))
    if len_txs == 1:
        return sha256(txs[0].get_hash() + txs[0].get_hash())

    # if txs = [tx0,tx1] => return Hash(Hash(tx0) + Hash(tx1))
    if len_txs == 2:
        return sha256(txs[0].get_hash() + txs[1].get_hash())

    # txs半分に分割。i.e. txs = low + high
    i = len_txs//2
    low  = txs[0:i]
    high = txs[i:]

    # 再帰的に計算。
    hash_val = sha256(_recursive_merkleroot(low) + _recursive_merkleroot(high))
    return hash_val
