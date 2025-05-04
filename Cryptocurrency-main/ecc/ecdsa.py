"""
ECDSA(Elliptic Curve Digital Signature Algorithm)関連
[https://en.wikipedia.org/wiki/Elliptic_Curve_Digital_Signature_Algorithm]
"""

from hashlib import sha256

from ecc.ntheory import inv
from ecc.der     import der_encode,der_decode
from ecc.random  import secure_randbelow
from ecc.params  import _n,_j_g
from ecc.ec      import add,mul,to_jacobian,to_affine,A_INF
    
def gen_privkey() -> int:
    """
    秘密鍵を生成
    secure_randbelow(n)は [0,n-1]の範囲にある乱数を返すことに注意。
    """
    s = secure_randbelow(_n - 3) # 0 <= s <= _n - 4
    privkey = s + 2              # 2 <= privkey <= _n - 2
    return privkey

def gen_pubkey(privkey: int) -> str:
    """
    秘密鍵から公開鍵を生成
    """
    j_m    = mul(privkey,_j_g)
    a_m    = to_affine(j_m)
    pubkey = der_encode(a_m[0],a_m[1])
    return pubkey

def hash_message(m: str) -> int:
    """
    mをハッシュした後に、そのハッシュ値の上位256ビットを取り出し、整数で返す。
    """
    bit_len   = 256 # _nのビット長
    hex_val   = sha256(m.encode()).hexdigest()
    int_val   = int(hex_val,16)
    bin_val   = "{:b}".format(int_val)
    left_most = bin_val[0:bit_len]
    hash_val  = int(left_most,2) 
    return hash_val

def ec_sign(m: str,privkey: int) -> str:
    """
    秘密鍵privkeyを使用して、メッセージmにデジタル署名。
    """    
    d = hash_message(m)
    
    while True:
        e    = secure_randbelow(_n - 1) + 1 # 1 <= e <= _n - 1
        j_eG = mul(e,_j_g)
        a_eG = to_affine(j_eG)
        
        s1 = a_eG[0] % _n
        if s1 == 0:
            continue
        
        s2 = (d + privkey*s1)*inv(e,_n) % _n
        if s2 == 0:
            continue

        return der_encode(s1,s2)
    
def ec_verify(m: str,signature: str,pubkey: str) -> bool:
    """
    公開鍵pubkeyを使用して、デジタル署名チェック。
    """

    d = hash_message(m)
    
    s1,s2 = der_decode(signature)
    
    if s1 < 1 or s1 > (_n - 1):
        return False
    if s2 < 1 or s2 > (_n - 1):
        return False

    inv_s2 = inv(s2,_n) % _n
    v1     = d*inv_s2 % _n
    v2     = s1*inv_s2 % _n

    v   = der_decode(pubkey)
    j_v = to_jacobian(v)
    
    p1 = mul(v1,_j_g)
    p2 = mul(v2,j_v)
    p  = add(p1,p2)
    q  = to_affine(p)

    if q == A_INF:
        return False
    
    x = q[0] % _n     
    return x == s1
