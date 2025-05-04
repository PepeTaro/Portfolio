import unittest
import numpy as np

from quantum.qubit import Qubit
from quantum.operator import *
from .test_utils import *

class TestQubit(unittest.TestCase):
    def test_binary_to_ket(self):
        x = get_three_qubit("000")
        self.assertTrue(x.decimal_to_ket(0) == "|000>")
        self.assertTrue(x.decimal_to_ket(1) == "|001>")
        self.assertTrue(x.decimal_to_ket(2) == "|010>")
        self.assertTrue(x.decimal_to_ket(3) == "|011>")
        self.assertTrue(x.decimal_to_ket(4) == "|100>")
        self.assertTrue(x.decimal_to_ket(5) == "|101>")
        self.assertTrue(x.decimal_to_ket(6) == "|110>")
        self.assertTrue(x.decimal_to_ket(7) == "|111>")

    def test_observe(self):
        x1 = Qubit.zero() # 1*|0> + 0*|1>
        x2 = Qubit.one() # 0*|0> + 1*|1>
        self.assertTrue(x1.observe() == 0)
        self.assertTrue(x2.observe() == 1)
        
        x = get_two_qubit("00")
        observation_outcome = x.observe()
        self.assertTrue(isinstance(observation_outcome,int) and 0 <= observation_outcome and observation_outcome <= 3) 

        x = get_two_qubit("01")
        observation_outcome = x.observe()
        self.assertTrue(isinstance(observation_outcome,int) and 0 <= observation_outcome and observation_outcome <= 3) 

        x = get_two_qubit("10")
        observation_outcome = x.observe()
        self.assertTrue(isinstance(observation_outcome,int) and 0 <= observation_outcome and observation_outcome <= 3) 

        x = get_two_qubit("11")
        observation_outcome = x.observe()
        self.assertTrue(isinstance(observation_outcome,int) and 0 <= observation_outcome and observation_outcome <= 3) 

        
    def test_observe_multiple_times(self):                
        x = get_two_qubit("00") # |00>
        self.assertTrue(x.observe_multiple_times(100) == {0: 100, 1: 0, 2: 0, 3: 0})

        x = get_two_qubit("01") # |01>
        self.assertTrue(x.observe_multiple_times(100) == {0: 0, 1: 100, 2: 0, 3: 0})

        x = get_two_qubit("10") # |10>        
        self.assertTrue(x.observe_multiple_times(100) == {0: 0, 1: 0, 2: 100, 3: 0})

        x = get_two_qubit("11") # |11>
        self.assertTrue(x.observe_multiple_times(100) == {0: 0, 1: 0, 2: 0, 3: 100})

    def test_get_state_after_observing(self):
        x = get_two_qubit("00")
        self.assertTrue(x.get_state_after_observing(1)[1] == 0)
        
        x = get_two_qubit("01")
        self.assertTrue(x.get_state_after_observing(2)[1] == 1)
        
        x = get_two_qubit("10")
        self.assertTrue(x.get_state_after_observing(2)[1] == 0)
        
        x = get_two_qubit("11")
        self.assertTrue(x.get_state_after_observing(1)[1] == 1)

    def test_observe_with_index(self):
        x1 = Qubit.zero() # |0>
        x2 = Qubit.one() # |1>
        self.assertEqual(x1.observe_with_index(1),0)
        self.assertEqual(x2.observe_with_index(1),1)

        x = get_two_qubit("01") # |01>
        x.single_qubit_gate(hadamard(),1) # (1/sqrt(2))( |01> + |11> )
        first_bit = x.observe_with_index(1) 
        second_bit = x.observe_with_index(1)
        self.assertTrue((first_bit == 0 and second_bit == 1) or (first_bit == 1 and second_bit == 1))# 01 or 11

        x = get_two_qubit("11") # |11>
        x.single_qubit_gate(hadamard(),1) # (1/sqrt(2))( |01> - |11> )
        first_bit = x.observe_with_index(1) 
        second_bit = x.observe_with_index(1)
        self.assertTrue((first_bit == 0 and second_bit == 1) or (first_bit == 1 and second_bit == 1))# 01 or 11

    def test_observe_with_indices(self):        
        x = get_three_qubit("000") # |000>
        self.assertTrue(x.observe_with_indices(1,2,3) == {1: 0, 2: 0, 3: 0})
        
        x = get_three_qubit("001") # |001>
        self.assertTrue(x.observe_with_indices(1,2,3) == {1: 0, 2: 0, 3: 1})
        
        x = get_three_qubit("011") # |011>
        self.assertTrue(x.observe_with_indices(2,3) == {2: 1, 3: 1})
        
        x = get_three_qubit("111") # |111>
        self.assertTrue(x.observe_with_indices(1) == {1: 1})

    def test_oracle(self):        
        x = get_two_qubit("01") # |01>
        x.oracle(const_zero) # x = |01>
        self.assertTrue(x.observe_with_index(2) == 1)
        
        x = get_two_qubit("11") # |11>
        x.oracle(const_one) # x = |10>
        self.assertTrue(x.observe_with_index(2) == 0)
        
        x = get_two_qubit("10") # |10>
        x.oracle(balanced_func1) # x = |11>
        self.assertTrue(x.observe_with_index(2) == 1)
        
        x = get_two_qubit("00") # |00>
        x.oracle(balanced_func2) # x = |01>
        self.assertTrue(x.observe_with_index(2) == 1)


if __name__=='__main__':
    unittest.main()
