from quantum.qubit import Qubit
from quantum.operator import *
import numpy as np

def const_zero(i):
    """
    constant関数
    """
    return 0

def const_one(i):
    """
    constant関数
    """
    return 1

def balanced_func1(i):
    """
    balanced関数
    """
    if(i == 0):
        return 0
    else:
        return 1
    
def balanced_func2(i):
    """
    balanced関数
    """
    if(i == 0):
        return 1
    else:
        return 0


def deutsch_algorithm_setup(qubit,function):
    """
    Args:
     qubit:Qubit class,オラクルに入るQubit。
     function:ブール関数、オラクルが使用するブール関数。

    Returns:
     Qubit class
    """
    qubit.single_qubit_gate(hadamard(),1)
    qubit.single_qubit_gate(hadamard(),2)
    qubit.oracle(function)
    qubit.single_qubit_gate(hadamard(),1)
    return qubit

def deutsch_algorithm(function):
    """
    Deutschのアルゴリズムを使って、functionがconstantかあるいはbalancedかを判定。
    
    Args:
     function:ブール関数。
    """
    
    x1 = Qubit.zero()
    x2 = Qubit.one()
    x = Qubit.entangle(x1,x2)    
    x = deutsch_algorithm_setup(x,function)
    outcome = x.observe_with_index(1)
    
    if(outcome == 0):
        print("関数はconstantです。")
    elif(outcome == 1):
        print("関数はbalancedです。")
    else:
        print("[!] Error: 観測結果が適切でない。")
        exit(-1)
     
def main():
    deutsch_algorithm(balanced_func1)

if __name__=='__main__':
    main()
