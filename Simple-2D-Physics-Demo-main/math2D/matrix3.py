import math

from math2D.vector2 import *

def _Translate(x,y):                
    mat = Matrix3.Identity()
    mat[2] = x
    mat[5] = y
    return mat

def _mul_num(data,num):
    """
    行列を定数(num)倍する
    """
    mat = Matrix3.Zero()
    for i in range(9):
        mat[i] = data[i]*num
    return mat

def _mul_matrix(data,rhs):
    """
    行列と行列の掛け算
    """        
    mat = Matrix3.Zero()
    for i in range(3):
        for j in range(3):
            for k in range(3):
                mat[j + 3*i] += data[k + 3*i]*rhs.data[j + 3*k]
                
    return mat

def _mul_vec(data,vec):
    """
    行列とベクトルの掛け算
    """
    x = vec.x*data[0] + vec.y*data[1] + data[2] 
    y = vec.x*data[3] + vec.y*data[4] + data[5]        
    return Vector2(x,y)

class Matrix3:
    def __init__(self,data=[0]*9):
        self.data = data # 3x3ゼロ行列を生成

    def __str__(self):
        s = "["
        s += "[{0},{1},{2}]\n".format(self.data[0],self.data[1],self.data[2])
        s += "[{0},{1},{2}]\n".format(self.data[3],self.data[4],self.data[5])
        s += "[{0},{1},{2}]".format(self.data[6],self.data[7],self.data[8])
        s += "]"
        return s
        
    def __add__(self,rhs):
        mat = Matrix3.Zero()
        for i in range(9):
            mat[i] = self.data[i] + rhs.data[i]
            
        return mat

    def __sub__(self,rhs):
        mat = Matrix3.Zero()
        for i in range(9):
            mat[i] = self.data[i] - rhs.data[i]
            
        return mat
    
    def __truediv__(self,rhs):
        assert(rhs != 0.0)

        mat = Matrix3.Zero()
        for i in range(9):
            mat[i] = self.data[i]/rhs
            
        return mat

    def __mul__(self,rhs):
        if(isinstance(rhs,Matrix3)):
            return _mul_matrix(self.data,rhs)
        elif(isinstance(rhs,Vector2)):
            return _mul_vec(self.data,rhs)        
        else:
            return _mul_num(self.data,rhs)        
        
    def __rmul__(self,lhs):
        mat = Matrix3.Zero()
        for i in range(9):
            mat[i] = self.data[i]*lhs
            
        return mat    

    def __getitem__(self,i):
        assert(i >= 0 and i < 9)
        return self.data[i]

    def __setitem__(self,i,value):
        assert(i >= 0 and i < 9)
        self.data[i] = value

    def __eq__(self,mat):
        for i in range(9):
            if(self.data[i] != mat[i]): return False

        return True
    
    def inverse(self):
        det = self.data[0]*self.data[4]*self.data[8]
        det += self.data[3]*self.data[7]*self.data[2]
        det += self.data[6]*self.data[1]*self.data[5]
        det -= self.data[0]*self.data[7]*self.data[5]
        det -= self.data[6]*self.data[4]*self.data[2]
        det -= self.data[3]*self.data[1]*self.data[8]

        if(det == 0): return None # 逆行列が存在しない場合
        
        mat = Matrix3.Zero()
        mat[0] = self.data[4]*self.data[8] - self.data[5]*self.data[7]
        mat[1] = self.data[2]*self.data[7] - self.data[1]*self.data[8]
        mat[2] = self.data[1]*self.data[5] - self.data[2]*self.data[4]
        
        mat[3] = self.data[5]*self.data[6] - self.data[3]*self.data[8]
        mat[4] = self.data[0]*self.data[8] - self.data[2]*self.data[6]
        mat[5] = self.data[2]*self.data[3] - self.data[0]*self.data[5]
        
        mat[6] = self.data[3]*self.data[7] - self.data[4]*self.data[6]
        mat[7] = self.data[1]*self.data[6] - self.data[0]*self.data[7]
        mat[8] = self.data[0]*self.data[4] - self.data[1]*self.data[3]

        mat /= det
        return mat

    def transpose(self):
        mat = Matrix3.Zero()
        mat[0] = self.data[0]
        mat[1] = self.data[3]
        mat[2] = self.data[6]
        mat[3] = self.data[1]
        mat[4] = self.data[4]
        mat[5] = self.data[7]
        mat[6] = self.data[2]
        mat[7] = self.data[5]
        mat[8] = self.data[8]

        return mat
    
    @staticmethod
    def Zero():
        m=[0]*9
        return Matrix3(m)

    @staticmethod
    def Identity():
        mat = Matrix3.Zero()
        mat[0] = 1.0
        mat[4] = 1.0
        mat[8] = 1.0
        return mat

    @staticmethod
    def Translate(*args):
        if(len(args) == 1): # args[0] == Vector2と仮定
            return _Translate(args[0].x,args[0].y)
        else:               # args[0] == x,args[1] == yと仮定
            return _Translate(x,y)
    
    @staticmethod
    def Rotate(angle):
        mat = Matrix3.Identity()
        c = math.cos(angle)
        s = math.sin(angle)
        mat[0] = c
        mat[1] = -s
        mat[3] = s
        mat[4] = c
        return mat
