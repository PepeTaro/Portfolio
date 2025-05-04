from quantum.qubit import Qubit
from quantum.operator import *

def main():
    x1 = Qubit.zero() # |0>
    print(x1) # ((1+0j))|0> + (0j)|1>
    
    x2 = Qubit.zero() # |0>
    print(x2) # ((1+0j))|0> + (0j)|1>
    
    x = Qubit.entangle(x1,x2) # |00>
    print(x) # ((1+0j))|00> + (0j)|01> + (0j)|10> + (0j)|11>
    
    x.single_qubit_gate(hadamard(),1) # x1にアダマールゲートを作用。
    x.two_qubits_gate(pauli_x(),1,2) # x1,x2にCnot(pauli_x)ゲートを作用。
    
    print(x) #((0.7071067811865476+0j))|00> + (0j)|01> + (0j)|10> + ((0.7071067811865476+0j))|11>
    
if __name__=='__main__':
    main()

