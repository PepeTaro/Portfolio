import unittest
import numpy as np

from quantum.qubit import Qubit
from quantum.operator import *
from .test_utils import *

class TestOperator(unittest.TestCase):
    def test_identity_matrix_for_two_qubit(self):
        q0 = Qubit.zero()
        q1 = Qubit.one()
        
        q0.single_qubit_gate(identity(),1)
        q1.single_qubit_gate(identity(),1)
        
        self.assertTrue(np.array_equal(q0.state,qubit_0))
        self.assertTrue(np.array_equal(q1.state,qubit_1))

    def test_pauli_x_matrix_for_two_qubit(self):
        q0 = Qubit.zero()
        q1 = Qubit.one()
        
        q0.single_qubit_gate(pauli_x(),1)
        q1.single_qubit_gate(pauli_x(),1)
        
        self.assertTrue(np.array_equal(q0.state,qubit_1))
        self.assertTrue(np.array_equal(q1.state,qubit_0))    

    def test_pauli_y_matrix_for_two_qubit(self):
        q0 = Qubit.zero()
        q1 = Qubit.one()
        
        q0.single_qubit_gate(pauli_y(),1)
        q1.single_qubit_gate(pauli_y(),1)
        
        self.assertTrue(np.array_equal(q0.state,1j*qubit_1))
        self.assertTrue(np.array_equal(q1.state,-1j*qubit_0))    

    def test_pauli_z_matrix_for_two_qubit(self):
        q0 = Qubit.zero()
        q1 = Qubit.one()
        
        q0.single_qubit_gate(pauli_z(),1)
        q1.single_qubit_gate(pauli_z(),1)
        
        self.assertTrue(np.array_equal(q0.state,qubit_0))
        self.assertTrue(np.array_equal(q1.state,-1*qubit_1))    

    def test_hadamard_matrix_for_two_qubit(self):
        q0_after_hadamard = np.sqrt(1/2)*qubit_0 + np.sqrt(1/2)*qubit_1
        q1_after_hadamard = np.sqrt(1/2)*qubit_0 - np.sqrt(1/2)*qubit_1
        
        q0 = Qubit.zero()
        q1 = Qubit.one()
        
        q0.single_qubit_gate(hadamard(),1)
        q1.single_qubit_gate(hadamard(),1)
        
        self.assertTrue(np.array_equal(q0.state,q0_after_hadamard))
        self.assertTrue(np.array_equal(q1.state,q1_after_hadamard))

    def test_phase_matrix_for_two_qubit(self):
        q0 = Qubit.zero()
        q1 = Qubit.one()
        
        q0.single_qubit_gate(phase(),1)
        q1.single_qubit_gate(phase(),1)
        
        self.assertTrue(np.array_equal(q0.state,qubit_0))
        self.assertTrue(np.array_equal(q1.state,1j*qubit_1))    

    def test_pi_over_eight_matrix_for_two_qubit(self):
        q0 = Qubit.zero()
        q1 = Qubit.one()
        
        q0.single_qubit_gate(pi_over_eight(),1)
        q1.single_qubit_gate(pi_over_eight(),1)
        
        self.assertTrue(np.array_equal(q0.state,qubit_0))
        self.assertTrue(np.array_equal(q1.state,(np.exp(np.pi*1j/4))*qubit_1))    

    def test_pauli_x_matrix_for_three_qubit(self):
        """
        Controled pauli_xは、Cnotゲートと同値。
        """
        x = get_three_qubit("100") # |100>
        x.two_qubits_gate(pauli_x(),1,3) # => |101>
        self.assertTrue(np.array_equal(x.state,qubit_101))
        
        x = get_three_qubit("011") # |011>
        x.two_qubits_gate(pauli_x(),1,3) # => |011>
        self.assertTrue(np.array_equal(x.state,qubit_011))
        
        x = get_three_qubit("011") # |011>
        x.two_qubits_gate(pauli_x(),3,1) # => |111>
        self.assertTrue(np.array_equal(x.state,qubit_111))
        
        x = get_three_qubit("111") # |111>
        x.two_qubits_gate(pauli_x(),2,1) # => |011>        
        self.assertTrue(np.array_equal(x.state,qubit_011))        

    def test_make_bell_state1(self):
        """
        Bell基底１。
        """
        bell_state1 = np.sqrt(1/2)*qubit_00 + np.sqrt(1/2)*qubit_11        
        x = get_two_qubit("00") # |00>
        x.single_qubit_gate(hadamard(),1)
        x.two_qubits_gate(pauli_x(),1,2)
        self.assertTrue(np.array_equal(x.state,bell_state1))
    
    def test_make_bell_state2(self):
        """
        Bell基底2。
        """        
        bell_state2 = np.sqrt(1/2)*qubit_01 + np.sqrt(1/2)*qubit_10        
        x = get_two_qubit("01") # |01>
        x.single_qubit_gate(hadamard(),1)
        x.two_qubits_gate(pauli_x(),1,2)        
        self.assertTrue(np.array_equal(x.state,bell_state2))

    def test_make_bell_state3(self):
        """
        Bell基底3。
        """
        bell_state3 = np.sqrt(1/2)*qubit_00 - np.sqrt(1/2)*qubit_11        
        x = get_two_qubit("10") # |10>
        x.single_qubit_gate(hadamard(),1)
        x.two_qubits_gate(pauli_x(),1,2)
        self.assertTrue(np.array_equal(x.state,bell_state3))
    
    def test_make_bell_state4(self):
        """
        Bell基底4。
        """
        bell_state4 = np.sqrt(1/2)*qubit_01 - np.sqrt(1/2)*qubit_10        
        x = get_two_qubit("11") # |11>
        x.single_qubit_gate(hadamard(),1)
        x.two_qubits_gate(pauli_x(),1,2)        
        self.assertTrue(np.array_equal(x.state,bell_state4))
        
if __name__=='__main__':
    unittest.main()
