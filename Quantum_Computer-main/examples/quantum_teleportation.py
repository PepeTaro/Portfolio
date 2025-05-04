from quantum.qubit import Qubit
from quantum.operator import *
import numpy as np

def quantum_teleportation_setup(sending_qubit):
    """
    Quantum teleporationのセットアップ作業。
    
    Args:
     sending_qubit: Qubit class,テレポートさせたいQubit
    
    Returns:
     Qubit class,int
     観測後のQubitの状態と、アリスが観測した結果。
    """
    q = sending_qubit
    r = Qubit.zero() # |0>
    s = Qubit.zero() # |0>

    x = Qubit.entangle(q,r,s)
        
    # rとsから、Bell基底1/(\sqrt{2})(|00> + |11>)をつくる。
    x.single_qubit_gate(hadamard(),2)
    x.two_qubits_gate(pauli_x(),2,3)
        
    x.two_qubits_gate(pauli_x(),1,2)
    x.single_qubit_gate(hadamard(),1)
        
    # アリスの２つのQubitを観測。
    result = x.observe_with_indices(1,2)
    alice_result = (result[1] << 1) + (result[2] << 0)
    
    return x,alice_result

def act_gate_on_his_qubit(qubit,alice_result):
    """
    Quantum teleportationのスキームに従い、アリスの観測結果をもとに、ボブのQubitにゲートを作用させる。
    Args:
     qubit: Qubit class
     alice_result: 0,1,2,あるいは３(つまり2bit)。
    
    Returns:
     量子状態が更新されたQubit
    
    """

    if(alice_result == 0):
        pass
    elif(alice_result == 1):
        qubit.single_qubit_gate(pauli_x(),1)
    elif(alice_result == 2):
        qubit.single_qubit_gate(pauli_z(),1)
    elif(alice_result == 3):
        qubit.single_qubit_gate(pauli_x(),1)
        qubit.single_qubit_gate(pauli_z(),1)
    else:            
        print("[!]観測結果が正しくない。")
        exit(-1)

    return qubit

def qunatum_teleportation(q):
    """
    [注意]テレポートした結果の状態が、テレポート前の状態と僅かに異なることがあるが、
    それは数値計算の誤差。
    
    Args:
     q: テレポートさせたいQubit class。
    """
    print("テレポート前の状態: ",q)    
    x,alice_result = quantum_teleportation_setup(q)    
    x = act_gate_on_his_qubit(x,alice_result)
    print("テレポート後の状態: ",x)

def main():    
    q = Qubit.zero()
    q.single_qubit_gate(hadamard(),1)
    qunatum_teleportation(q)
    
if __name__=='__main__':
    main()
