from math2D.vector2 import *

class ForceGenerator:
    def __init__(self):
        pass
    def update_force(self,obj,delta_time):
        raise NotImplementedError('Error(update_force):Override me!')

class SpringForce(ForceGenerator):
    def __init__(self):
        super().__init__()
        self.other = None
        self.spring_const = None
        self.rest_length = None

    def set_spring(self,other,spring_const,rest_length):
        assert(spring_const >= 0.0)
        assert(rest_length  >= 0.0)
        
        self.other = other
        self.spring_const = spring_const
        self.rest_length = rest_length
        
    def update_force(self,obj,delta_time):        
        force = obj.position - self.other.position
        norm_val = norm(force)
        if(norm_val == 0): return        
        force /= norm_val
        norm_val = abs(norm_val) - self.rest_length
        norm_val *= self.spring_const        
        force *= (-norm_val)        
        obj.add_force(force)
