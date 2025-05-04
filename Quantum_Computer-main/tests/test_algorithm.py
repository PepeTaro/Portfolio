import unittest
import numpy as np

from quantum.qubit import Qubit
from quantum.operator import *
from examples.quantum_teleportation import *
from examples.superdense_coding import *
from examples.deutsch_algorithm import *
from examples.quantum_fourier_transform import *
from .test_utils import *

class TestAlgorithm(unittest.TestCase):
    # ヘルパー
    def are_vectors_almost_same(self,vec1,vec2):
        """
        np.ndarrayであるvec1とvec2の各成分が"ほぼ"同じ値ならばTrueを返す。
        """
        for val1,val2 in zip(vec1,vec2):
            if(not np.isclose(val1,val2)):
                return False
        return True
    
    def test_qunatum_teleportation(self):
        """
        量子teleportaionテスト。
        """
        q = Qubit.zero() # |0>
        q.single_qubit_gate(hadamard(),1) # 1/(sqrt(2))(|0> + |1>) このQubitをテレポートさせる。
        x,alice_result = quantum_teleportation_setup(q)
        x = act_gate_on_his_qubit(x,alice_result)
        self.assertTrue(self.are_vectors_almost_same(x.state,q.state))

        q = Qubit.one() # |1>
        q.single_qubit_gate(hadamard(),1) # 1/(sqrt(2))(|0> - |1>) このQubitをテレポートさせる。
        x,alice_result = quantum_teleportation_setup(q)
        x = act_gate_on_his_qubit(x,alice_result)
        self.assertTrue(self.are_vectors_almost_same(x.state,q.state))
        
    def test_superdense_coding(self):
        """
        Superdense codingテスト。
        """
        sending_bit = 0 # (00) アリスがボブに送りたい古典ビット。        
        x = superdense_coding_setup(sending_bit)        
        self.assertTrue(x.observe_with_indices(1,2) == {1: 0, 2: 0})

        sending_bit = 1 # (01) アリスがボブに送りたい古典ビット。        
        x = superdense_coding_setup(sending_bit)
        self.assertTrue(x.observe_with_indices(1,2) == {1: 0, 2: 1})
        
        sending_bit = 2 # (10) アリスがボブに送りたい古典ビット。        
        x = superdense_coding_setup(sending_bit)
        self.assertTrue(x.observe_with_indices(1,2) == {1: 1, 2: 0})
        
        sending_bit = 3 # (11) アリスがボブに送りたい古典ビット。        
        x = superdense_coding_setup(sending_bit)
        self.assertTrue(x.observe_with_indices(1,2) == {1: 1, 2: 1})
        
    def test_deutsch_algorithm(self):
        """
        Deutschのアルゴリズムをテスト。
        """                
        x = get_two_qubit("01") # |01>                
        x = deutsch_algorithm_setup(x,const_zero)
        self.assertTrue(x.observe_with_index(1) == 0)
        
        x = get_two_qubit("01") # |01>        
        x = deutsch_algorithm_setup(x,const_one)
        self.assertTrue(x.observe_with_index(1) == 0)
        
        x = get_two_qubit("01") # |01>        
        x = deutsch_algorithm_setup(x,balanced_func1)
        self.assertTrue(x.observe_with_index(1) == 1)
        
        x = get_two_qubit("01") # |01>        
        x = deutsch_algorithm_setup(x,balanced_func2)
        self.assertTrue(x.observe_with_index(1) == 1)
            
    def test_quantum_fourier_transform(self):
        """
        量子フーリエ変換テスト
        """
        for i in range(1,100):
            b = "{:b}".format(i)
            
            n = 2**len(b)
            x = int(b,2)
            k = np.arange(0,n)
            
            exponent = 1j*(2*np.pi*x*k)/n
            result = np.e**(exponent)/np.sqrt(n)
            result = result.reshape(n,1)
            
            x = quantum_fourier_transform(Qubit.binary(b))
            self.assertTrue(x == result)

    def test_inv_quantum_fourier_transform(self):
        """
        逆量子フーリエ変換テスト
        """
        for i in range(100):
            b = "{:b}".format(i)
            
            n = int(b,2)
            z = Qubit.binary(b)
            
            x = quantum_fourier_transform(z)    
            y = inverse_quantum_fourier_transform(x)
            self.assertTrue(n == y.observe()) # 元の状態(zのこと)を観測できるかチェック
    
if __name__=='__main__':
    unittest.main()
