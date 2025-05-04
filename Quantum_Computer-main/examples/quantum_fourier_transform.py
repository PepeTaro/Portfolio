""" https://en.wikipedia.org/wiki/Quantum_Fourier_transform """

from quantum.qubit import Qubit
from quantum.operator import *

def quantum_fourier_transform(x):
    """ 量子フーリエ変換 """    
    n = x.num_of_qubits
    
    for i in range(1,n+1):
        x.single_qubit_gate(hadamard(),i)
        for k,j in enumerate(range(i+1,n+1)):
            x.two_qubits_gate(controlled_phase(k+2),j,i)

    for i in range(1,n//2 + 1):
        j = n - (i - 1)
        x.two_qubits_gate(pauli_x(),i,j)
        x.two_qubits_gate(pauli_x(),j,i)
        x.two_qubits_gate(pauli_x(),i,j)

    return x

def inverse_quantum_fourier_transform(x):
    """ 逆量子フーリエ変換 """
    n = x.num_of_qubits

    for i in range(1,n//2 + 1):
        j = n - (i - 1)
        x.two_qubits_gate(pauli_x(),i,j)
        x.two_qubits_gate(pauli_x(),j,i)
        x.two_qubits_gate(pauli_x(),i,j)

    for i in range(n,0,-1):
        for k,j in enumerate(range(n,i,-1)):            
            z = (n - (i-1) -k)
            x.two_qubits_gate(inv_controlled_phase(z),j,i)
            
        x.single_qubit_gate(hadamard(),i)
        
    return x

""" 
 以下２つの関数は、わかりやすいように一般化しないでコーディングしている
"""

def qunatum_fourier_transform_four_qubits1000():
    x1 = Qubit.one()
    x2 = Qubit.zero()
    x3 = Qubit.zero()
    x4 = Qubit.zero()
    x = Qubit.entangle(x1,x2,x3,x4)
    
    x.single_qubit_gate(hadamard(),1)
    x.two_qubits_gate(controlled_phase(2),2,1)
    x.two_qubits_gate(controlled_phase(3),3,1)
    x.two_qubits_gate(controlled_phase(4),4,1)
    
    x.single_qubit_gate(hadamard(),2)
    x.two_qubits_gate(controlled_phase(2),3,2)    
    x.two_qubits_gate(controlled_phase(3),4,2)

    x.single_qubit_gate(hadamard(),3)
    x.two_qubits_gate(controlled_phase(2),4,3)

    x.single_qubit_gate(hadamard(),4)

    # SwapゲートをCnotで代用。
    x.two_qubits_gate(pauli_x(),1,4)
    x.two_qubits_gate(pauli_x(),4,1)
    x.two_qubits_gate(pauli_x(),1,4)

    x.two_qubits_gate(pauli_x(),2,3)
    x.two_qubits_gate(pauli_x(),3,2)
    x.two_qubits_gate(pauli_x(),2,3)
        
    print(x)
    
def qunatum_fourier_transform_three_qubits100():
    x1 = Qubit.one()
    x2 = Qubit.zero()
    x3 = Qubit.zero()
    x = Qubit.entangle(x1,x2,x3)
    
    x.single_qubit_gate(hadamard(),1)
    x.two_qubits_gate(controlled_phase(2),2,1)
    x.two_qubits_gate(controlled_phase(3),3,1)
    
    x.single_qubit_gate(hadamard(),2)
    x.two_qubits_gate(controlled_phase(2),3,2)
    
    x.single_qubit_gate(hadamard(),3)

    # SwapゲートをCnotゲートで代用。
    x.two_qubits_gate(pauli_x(),1,3)
    x.two_qubits_gate(pauli_x(),3,1)
    x.two_qubits_gate(pauli_x(),1,3)
        
    print(x)

def main():
    #qunatum_fourier_transform_four_qubits1000()
    qunatum_fourier_transform_three_qubits100()
    
if __name__=='__main__':
    main()
