from ecc.params import _p

def exp(a: int,e: int,p: int = _p) -> int:
    """ (a**e) % p を高速に計算 """
    assert e >= 0
    assert p >= 0
    
    b = "{:b}".format(e)
    r = a
    for i in range(1,len(b)):
        r = r*r % p
        if(b[i] == '1'):
            r = r*a % p
    return r

def inv(a: int,p: int = _p) -> int:
    """ a**(-1) % _P (aの逆数)を計算  """
    # pが素数であるチェックを省いている(時間がかかるため)
    assert a%p != 0    
    return exp(a,p-2,p)
