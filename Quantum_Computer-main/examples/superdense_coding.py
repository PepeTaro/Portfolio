from quantum.qubit import Qubit
from quantum.operator import *
import numpy as np

def superdense_coding_setup(sending_bit):
    """
    Superdense codingのセットアップ作業。
    Args:
     sending_bit:int, アリスがボブに送りたい古典ビット。
    
    Returns:
     Qubit class
    """
    x1 = Qubit.zero() # アリス用
    x2 = Qubit.zero() # ボブ用
    x = Qubit.entangle(x1,x2) # |00>
        
    x.single_qubit_gate(hadamard(),1)
    x.two_qubits_gate(pauli_x(),1,2) # xは今、Bell基底の状態。

    x = act_gate_on_her_qubit(x,sending_bit)
    
    # ボブ側で観測。
    x.two_qubits_gate(pauli_x(),1,2) 
    x.single_qubit_gate(hadamard(),1)
        
    return x

def act_gate_on_her_qubit(qubit,sending_bit):
    """
    Superdense codingのスキームに従い、sending_bitの値に応じで、アリスのQubitにゲートを作用させる。
    
    Args:
     qubit: Qubit class
     sending_bit: 0,1,2,あるいは３(つまり2bit)。
    
    Returns:
     量子状態が更新されたQubit

    """
    if(sending_bit == 0):
        qubit.single_qubit_gate(identity(),1)
        return qubit
    elif(sending_bit == 1):
        qubit.single_qubit_gate(pauli_x(),1)
        return qubit
    elif(sending_bit == 2):
        qubit.single_qubit_gate(pauli_z(),1)
        return qubit
    elif(sending_bit == 3):
        qubit.single_qubit_gate(pauli_x(),1) 
        qubit.single_qubit_gate(pauli_z(),1)
        return qubit
    else:
        print("[!] Error: sending_bitの値が適切ではない。")
        exit(-1)

def superdense_coding(sending_bit):
    """
    暗号学の慣習に従い、送信者をアリス、受信者をボブと呼ぶ。
    sending_bitは、アリスがボブに送りたい古典ビット。
    
    Args:
     sending_bit: 0,1,2,あるいは3(つまり2bit)。
    """
    
    print("アリスが送りたい値: ",sending_bit)
    
    x = superdense_coding_setup(sending_bit)    
    outcome = x.observe_with_indices(1,2) # ボブが観測。
    outcome_val = (outcome[1] << 1) + (outcome[2] << 0)
    
    print("ボブが観測した値: ",outcome_val)
    
def main():
    sending_bit = 2
    superdense_coding(sending_bit)
    
if __name__=='__main__':
    main()
