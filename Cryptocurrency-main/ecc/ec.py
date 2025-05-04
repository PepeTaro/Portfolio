"""
EC(Elliptic Curve)関連

参照: Introduction to Mathematical Cryptography p322.

Jacobian座標での計算は以下参照:
 [https://en.wikibooks.org/wiki/Cryptography/Prime_Curve/Jacobian_Coordinates]
"""

from ecc.ntheory import exp,inv
from ecc.random  import secure_randbelow
from ecc.params  import _p,_a

Affine   = tuple[int,int]     # Affine座標
Jacobian = tuple[int,int,int] # Jacobian座標

A_INF = None    # Affine座標での無限遠点
J_INF = (1,1,0) # Jacobian座標での無限遠点

def to_affine(q: Jacobian) -> Affine:
    """ Jacobin座標からAffine座標に変換 """
    
    assert q == J_INF or len(q) == 3
    
    if q == J_INF:
        return A_INF
    
    inv_z = inv(q[2])
    inv_z2 = inv_z*inv_z
    x = q[0]*inv_z2 % _p
    y = q[1]*inv_z2*inv_z % _p
    return (x,y)
    
def to_jacobian(q: Affine) -> Jacobian:
    """ Affine座標からJacobian座標に変換 """
    
    assert q == A_INF or len(q) == 2
    if q == A_INF:
        return J_INF
    
    return (q[0],q[1],1)

def add(q1: Jacobian,q2: Jacobian) -> Jacobian:
    """ q1 + q2 を計算 """
    
    assert q1 == J_INF or len(q1) == 3 and "[!] Error(add): Jacobian座標に変換してください"
    assert q2 == J_INF or len(q2) == 3 and "[!] Error(add): Jacobian座標に変換してください"

    
    if q1 == J_INF:
        return q2
    if q2 == J_INF:
        return q1
    
    u1 = q1[0]*q2[2]**2 % _p
    u2 = q2[0]*q1[2]**2 % _p
    s1 = q1[1]*q2[2]**3 % _p
    s2 = q2[1]*q1[2]**3 % _p
    
    if u1 == u2:
        if s1 != s2:
            return J_INF
        else:
            return double(q1)

    h = (u2 - u1) % _p
    r = (s2 - s1) % _p
    x3 = (r**2 - h**3 - 2*u1*h**2) % _p
    y3 = (r*(u1*h**2 - x3) - s1*h**3) % _p
    z3 = h*q1[2]*q2[2] % _p
    return (x3,y3,z3)

def double(q: Jacobian) -> Jacobian:
    """ q + q を計算 """
    
    assert q == J_INF or len(q) == 3 and "[!] Error(double): Jacobian座標に変換してください"
    
    if q[1] == 0:
        return J_INF
    
    s = (4*q[0]*q[1]**2) % _p
    m = (3*q[0]**2 + _a*q[2]**4) % _p
    x = (m**2 - 2*s) % _p
    y = (m*(s - x) - 8*q[1]**4) % _p
    z = 2*q[1]*q[2] % _p
    return (x,y,z)


def mul(d: int,q: Jacobian) -> Jacobian:
    """ [d]*q を計算 """
    
    assert d >= 0
    assert q == J_INF or len(q) == 3 and "[!] Error(mul): Jacobian座標に変換してください"
    
    if d == 0 or q == J_INF:
        return J_INF
            
    b = "{:b}".format(d)
    t = q
    for i in range(1,len(b)):
        t = double(t)
        if(b[i] == '1'):
            t = add(t,q)    
    
    return t

# Jacobianを比較したい場合は、Affineに変換してから比較。
def a_eq(q1: Affine,q2: Affine) -> bool:
    """ q1 == q2 を評価。 """
    
    assert q1 == A_INF or len(q1) == 2
    assert q2 == A_INF or len(q2) == 2

    # 両方とも無限遠点
    if q1 == A_INF and q2 == A_INF:
        return True

    # 一方のみが無限遠点
    if q1 == A_INF or q2 == A_INF:
        return False
    
    if q1[0] == q2[0] and q1[1] == q2[1]:
        return True
    else:
        return False
    
