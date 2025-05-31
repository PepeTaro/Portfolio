from .ntheory import isprime,randprime,primitive_root,sq_and_mul,rand_between
from hashlib import sha256

def are_p_q_valid(p,q):
    """
    引数p,qがある条件を満たすか否かチェック。
    満たす場合True,そうでないFalseを返す。
    条件:
     p,qはそれぞれ素数で、q = (p - 1)/2を満たす。

    @param p:int
    @param q:int
    @return:bool
    """
    
    test1 = isprime(p)
    test2 = isprime(q)
    test3 = ((p - 1)&1 == 0) # (p-1)は偶数か?
    test4 = (p == 2*q + 1)
    
    test = test1 & test2 & test3 & test4
    return test

def init_primes(security_bit):
    """
    qをsecurity_bit-ビットであるような素数、
    pを p = 2*q + 1を満たすような素数としてランダムに選ぶ。
    この２つを返す。

    @param security_bit:int
    @return: int,int
    """
    low = 1 << (security_bit - 1)
    high = (1 << security_bit) - 1

    while True:
        q = randprime(low,high)
        p = 2*q + 1
        if isprime(p):
            break

    return p,q

def init_primitive_root(p):
    """
    gをZp上の"原始根の二乗"とし、g1,g2を
    g1 = (g**r1) % mod p,g2 = (g**r2) % mod p
    をとして定義する、ここでr1,r2は乱数。
    
    @param p:int
    @return: int,int,int
    """
    assert isprime(p)
    a = primitive_root(p)

    r1 = rand_between(1,p-1)
    r2 = rand_between(1,p-1)

    g  = a*a
    g1 = sq_and_mul(g,r1,p)
    g2 = sq_and_mul(g,r2,p)

    return g,g1,g2

def H(x1,x2,x3,x4,x5,q):
    """
    スキーム内で使用するハッシュ関数。

    @param x1:int
    @param x2:int
    @param x3:int
    @param x4:int
    @param x5:int
    @param q:int
    @return: int
    """
    
    assert isinstance(x1,int)
    assert isinstance(x2,int)
    assert isinstance(x3,int)
    assert isinstance(x4,int)
    assert isinstance(x5,int)
    assert isinstance(q,int)
    
    x = str(x1) + str(x2) + str(x3) + str(x4) + str(x5) # type(x) = str
    b = x.encode() # type(b) = bytes
    h = int(b,16)  # type(h) = int
    output = h%q
    return  output

def H0(x1,x2,x3,x4,q):
    """
    スキーム内で使用するハッシュ関数。

    @param x1:int
    @param x2:int
    @param x3:int
    @param x4:int
    @param q:int
    @return: int
    """

    assert isinstance(x1,int)
    assert isinstance(x2,int)
    assert isinstance(x3,int)
    assert isinstance(x4,int)
    assert isinstance(q,int)
    
    x = str(x1) + str(x2) + str(x3) + str(x4) # type(x) = str
    b = x.encode() # type(b) = bytes
    h = int(b,16)  # type(h) = int
    output = h%q
    return  output


