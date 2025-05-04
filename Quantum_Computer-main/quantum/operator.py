"""
Two Qubit量子ゲートまで扱う。
ToffoliなどThree Qubit以上の量子ゲートは扱わない。

https://en.wikipedia.org/wiki/Quantum_logic_gate
"""

import numpy as np

from quantum.qubit import Qubit

identity_matrix = np.array([[1,0],
                           [0,1]],dtype=complex)

pauli_x_matrix = np.array([[0,1],
                           [1,0]],dtype=complex)
    
pauli_y_matrix = np.array([[0,-1j],
                           [1j,0]],dtype=complex)

pauli_z_matrix = np.array([[1,0],
                           [0,-1]],dtype=complex)

hadamard_matrix = np.sqrt(1/2)*np.array([[1,1],
                                         [1,-1]],dtype=complex)

phase_matrix = np.array([[1,0],
                         [0,1j]],dtype=complex)

pi_over_eight_matrix = np.array([[1,0],
                                 [0,np.exp(np.pi*1j/4)]],dtype=complex)

def identity():
    return identity_matrix

def pauli_x():
    return pauli_x_matrix

def pauli_y():
    return pauli_y_matrix

def pauli_z():
    return pauli_z_matrix

def hadamard():
    return hadamard_matrix

def phase():
    return phase_matrix

def pi_over_eight():
    return pi_over_eight_matrix

def controlled_phase(m):    
    exponent = (2*np.pi*1j)/(2**m)
    controlled_phase_matrix = np.array([[1,0],
                                        [0,np.exp(exponent)]],dtype=complex)
    return controlled_phase_matrix

def inv_controlled_phase(m):    
    exponent = -(2*np.pi*1j)/(2**m)
    controlled_phase_matrix = np.array([[1,0],
                                        [0,np.exp(exponent)]],dtype=complex)
    return controlled_phase_matrix

def rotate_x(theta):
    angle = theta/2
    rotation_x_matrix = np.array([[np.cos(angle),-1j*np.sin(angle)],
                                   [-1j*np.sin(angle),np.cos(angle)]],dtype=complex)
    return rotation_x_matrix

def rotate_y(theta):
    angle = theta/2
    rotation_y_matrix = np.array([[np.cos(angle),-np.sin(angle)],
                                   [np.sin(angle),np.cos(angle)]],dtype=complex)
    return rotation_y_matrix

def rotate_z(theta):
    angle = theta/2
    rotation_z_matrix = np.array([[np.exp(-1j*angle),0],
                                  [0,np.exp(1j*angle)]],dtype=complex)
    return rotation_z_matrix
