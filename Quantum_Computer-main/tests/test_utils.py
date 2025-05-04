import numpy as np
from quantum.qubit import Qubit

# 以下のqubitはすべて、np.ndarrayに注意。
qubit_0 = np.array([[1,0]],dtype=complex).reshape(2,1)# |0> Qubit
qubit_1 = np.array([[0,1]],dtype=complex).reshape(2,1)# |1> Qubit

qubit_00 = np.array([[1,0,0,0]],dtype=complex).reshape(4,1)# |00> Qubit
qubit_01 = np.array([[0,1,0,0]],dtype=complex).reshape(4,1)# |01> Qubit
qubit_10 = np.array([[0,0,1,0]],dtype=complex).reshape(4,1)# |10> Qubit
qubit_11 = np.array([[0,0,0,1]],dtype=complex).reshape(4,1)# |11> Qubit
    
qubit_000 = np.array([[1,0,0,0,0,0,0,0]],dtype=complex).reshape(8,1)# |000> Qubit
qubit_001 = np.array([[0,1,0,0,0,0,0,0]],dtype=complex).reshape(8,1)# |001> Qubit
qubit_010 = np.array([[0,0,1,0,0,0,0,0]],dtype=complex).reshape(8,1)# |010> Qubit
qubit_011 = np.array([[0,0,0,1,0,0,0,0]],dtype=complex).reshape(8,1)# |011> Qubit
qubit_100 = np.array([[0,0,0,0,1,0,0,0]],dtype=complex).reshape(8,1)# |100> Qubit
qubit_101 = np.array([[0,0,0,0,0,1,0,0]],dtype=complex).reshape(8,1)# |101> Qubit
qubit_110 = np.array([[0,0,0,0,0,0,1,0]],dtype=complex).reshape(8,1)# |110> Qubit
qubit_111 = np.array([[0,0,0,0,0,0,0,1]],dtype=complex).reshape(8,1)# |111> Qubit

def get_two_qubit(state):
    """
    stateで指定された量子状態をつくり、返す。
    Ex) state = "01" => returns |01>

    Args:
     state:str
    
    Returns:
     Qubit class
    """
    assert(isinstance(state,str) and len(state) == 2)
    
    if(state[0] == "0"):
        x1 = Qubit.zero()
    elif(state[0] == "1"):
        x1 = Qubit.one()
    else:
        print("[!] Error: 引数の値が正しくない。")
        exit(-1)

    if(state[1] == "0"):
        x2 = Qubit.zero()
    elif(state[1] == "1"):
        x2 = Qubit.one()
    else:
        print("[!] Error: 引数の値が正しくない。")
        exit(-1)
    
    x = Qubit.entangle(x1,x2)
    return x

def get_three_qubit(state):
    """
    stateで指定された量子状態をつくり、返す。
    Ex) state = "010" => returns |010>

    Args:
     state:str
    
    Returns:
     Qubit class
    """
    assert(isinstance(state,str) and len(state) == 3)
    
    if(state[0] == "0"):
        x1 = Qubit.zero()
    elif(state[0] == "1"):
        x1 = Qubit.one()
    else:
        print("[!] Error: 引数の値が正しくない。")
        exit(-1)

    if(state[1] == "0"):
        x2 = Qubit.zero()
    elif(state[1] == "1"):
        x2 = Qubit.one()
    else:
        print("[!] Error: 引数の値が正しくない。")
        exit(-1)

    if(state[2] == "0"):
        x3 = Qubit.zero()
    elif(state[2] == "1"):
        x3 = Qubit.one()
    else:
        print("[!] Error: 引数の値が正しくない。")
        exit(-1)
    
    x = Qubit.entangle(x1,x2,x3)
    return x


def const_zero(i):
    return 0

def const_one(i):
    return 1

def balanced_func1(i):
    if(i == 0):
        return 0
    else:
        return 1
    
def balanced_func2(i):
    if(i == 0):
        return 1
    else:
        return 0
