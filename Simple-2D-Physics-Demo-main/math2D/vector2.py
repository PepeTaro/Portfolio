import math
    
def dot(a,b):
    return (a.x*b.x + a.y*b.y)
        
def norm(a):
    return math.sqrt(dot(a,a))

def norm2(a):
    return dot(a,a)

def _cross_vector_vector(a,b):
    return (a.x*b.y - a.y*b.x)

def _cross_vector_scalar(a,b):
    return b*Vector2(a.y,-a.x)

def _cross_scalar_vector(a,b):
    return a*Vector2(-b.y,b.x)

def cross(a,b): 
    if(isinstance(a,Vector2) and isinstance(b,Vector2)):
        return _cross_vector_vector(a,b)
    elif(isinstance(a,Vector2) and isinstance(b,(int,float))):
         return _cross_vector_scalar(a,b)
    elif(isinstance(a,(int,float)) and isinstance(b,Vector2)):
        return _cross_scalar_vector(a,b)
    else:
        print("[!] cross function Error")
        exit(-1)
        
def normalize(a):
    norm_val = norm(a)
    x = a.x
    y = a.y
    if(norm_val > 0.0):
        x /= norm_val
        y /= norm_val
    else:
        x = y = 0.0
    return Vector2(x,y)

def dist(a,b):
    c = a - b
    return math.sqrt(dot(c,c))

class Vector2:
    def __init__(self,x=0.0,y=0.0):
        self.x = x
        self.y = y

    def __str__(self):
        return "[{0},{1}]".format(self.x,self.y)

    def __hash__(self):
        return hash((self.x,self.y))
    
    def __add__(self,rhs):
        x = self.x + rhs.x
        y = self.y + rhs.y        
        return Vector2(x,y)

    def __iadd__(self,rhs):
        self.x += rhs.x
        self.y += rhs.y
        return self
    
    def __sub__(self,rhs):
        x = self.x - rhs.x
        y = self.y - rhs.y        
        return Vector2(x,y)

    def __isub__(self,rhs):
        self.x -= rhs.x
        self.y -= rhs.y
        return self

    def __truediv__(self,rhs):
        assert(rhs != 0.0)
        x = self.x/rhs
        y = self.y/rhs        
        return Vector2(x,y)

    def __mul__(self,rhs):
        x = self.x*rhs
        y = self.y*rhs
        return Vector2(x,y)

    def __rmul__(self,lhs):
        x = lhs*self.x
        y = lhs*self.y        
        return Vector2(x,y)
    
    def __neg__(self):
        return Vector2(-self.x,-self.y)
    
    def __eq__(self,vec):
        if(vec == None): return False
        elif(self.x == vec.x or self.y == vec.y): return True
        return False

    def __ne__(self,other):
        return (not (self == other))

    def __lt__(self,other):
        """
        Lexicographical order
        """
        if(self.x < other.x):
            return True
        elif(self.x > other.x):
            return False
        else:            # self.vec[0] == other.vec[0]
            if(self.y < other.y):
                return True
            else:
                return False

    def set(self,other):
        self.x = other.x
        self.y = other.y
        
    def invert(self):
        self.x = -self.x
        self.y = -self.y

    def clear(self):
        self.x = self.y = 0.0
    
    def normalize(self):
        norm_val = norm(self)        
        if(norm_val > 0.0):
            self.x /= norm_val
            self.y /= norm_val
    
    def is_zero(self):
        if(self.x == 0 and self.y == 0):
            return True
        else:
            return False
