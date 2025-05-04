from crypto.tx import Tx,WalletTx
from crypto.sha256 import sha256
from ecc.ecdsa import ec_verify,ec_sign

def sign_tx(tx: Tx,pubkey: str,privkey: int) -> None:
    """ Txに署名した後に、Tx内のunlocking_scriptを更新 """

    # デジタル署名
    sig_hash   = tx.get_hash()
    sig = ec_sign(sig_hash,privkey)
    
    # 署名した後にunlocking_scriptを上書き    
    unlocking_script = "%s %s"%(sig,pubkey)
    for txin in tx.vin:
        txin.unlocking_script = unlocking_script

def verify_tx(tx: Tx,sig: str,pubkey: str) -> bool:
    """ デジタル署名チェック """
    sig_hash   = tx.get_hash()
    return ec_verify(sig_hash,sig,pubkey)
