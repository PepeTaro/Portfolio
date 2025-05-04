import unittest

from ecc.ec import *
from ecc.ecdsa import *
from ecc.params import _j_g,_n
from ecc.der import der_encode,der_decode
from random import randint

class TestECC(unittest.TestCase):    
    def test_add_mul(self):    
        p1  = mul(2,_j_g)
        p2  = add(_j_g,_j_g)
        
        q1  = to_affine(p1)
        q2  = to_affine(p2)
        
        self.assertTrue(a_eq(q1,q2))
    
        q = mul(3,_j_g)
        _j_g2 = add(_j_g,_j_g)
        _j_g3 = add(_j_g2,_j_g)
        
        p1  = to_affine(q)
        p2  = to_affine(_j_g3)
        
        self.assertTrue(a_eq(p1,p2))
    
        q = mul(4,_j_g)
        _j_g2 = add(_j_g,_j_g)
        _j_g3 = add(_j_g2,_j_g)
        _j_g4 = add(_j_g3,_j_g)
        
        p1 = to_affine(q)
        p2 = to_affine(_j_g4)
        
        self.assertTrue(a_eq(p1,p2))

    def test_auto_add_mul(self):
        for i in range(1):
            d = randint(0,1000)
            
            q1 = mul(d,_j_g)
            q2 = J_INF
            
            for i in range(d):
                q2 = add(q2,_j_g)
            
            a_q1 = to_affine(q1)
            a_q2 = to_affine(q2)        
            self.assertTrue(a_eq(a_q1,a_q2))
            
    def test_order(self):
            
        for d in range(100):
            d = randint(0,_n)
            
            j_gd = mul(d,_j_g)
            a_gd = to_affine(j_gd)
            self.assertTrue(j_gd is not J_INF)
            self.assertTrue(a_gd is not A_INF)

        # d = 11をテストしているのは、以前にバグがあったから。
        d = 11
        j_gd = mul(d,_j_g)
        a_gd = to_affine(j_gd)
        self.assertTrue(j_gd is not J_INF)
        self.assertTrue(a_gd is not A_INF)
        
        _j_g_n = mul(_n,_j_g)
        a_g_n = to_affine(_j_g_n)
        self.assertTrue(_j_g_n is J_INF)
        self.assertTrue(a_g_n is A_INF)

    def test_sign_verify(self):
        s = """  
        !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~
        """
        
        test_messages = [s,"Hello World!","","A"*(2**25)]
        
        for m in test_messages:
            privkey = gen_privkey()
            pubkey  = gen_pubkey(privkey)    
            s = ec_sign(m,privkey)
            b = ec_verify(m,s,pubkey)
            self.assertTrue(b == True)

    def test_der(self):
        r = int("1ea1fdff81b3a271659df4aad19bc4ef83def389131a36358fe64b245632e777",16)
        s = int("29e164658be9ce810921bf81d6b86694785a79ea1e52dbfa5105148d1f0bc1",16)
        e = der_encode(r,s)
        _r,_s = der_decode(e)
        self.assertTrue(r == _r and s == _s)
        
        a = 1 << 256
        for i in range(10000):
            r = randint(0,a)
            s = randint(0,a)
            e = der_encode(r,s)
            _r,_s = der_decode(e)
            self.assertTrue(r == _r and s == _s)    

if __name__ == '__main__':
    unittest.main()
