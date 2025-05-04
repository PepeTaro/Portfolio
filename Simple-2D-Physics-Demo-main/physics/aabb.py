from math2D.vector2 import *
from math2D.matrix3 import *
from convex_hull.convex_hull import *

from math import sqrt

# AABBをキツキツにすると数値誤差の影響で,Broad Phaseで実際に衝突しているオブジェクトが衝突していないと判定されるのを回避するため
# すこしAABBを"太らせる"ための定数。
MARGIN_EPSILON = 2.0
MARGIN_LOWER_LEFT = Vector2(-MARGIN_EPSILON,-MARGIN_EPSILON)
MARGIN_UPPER_RIGHT = Vector2(MARGIN_EPSILON,MARGIN_EPSILON)

class AABB:
    def __init__(self,lower_left,upper_right):
        self.lower_left  = lower_left + MARGIN_LOWER_LEFT
        self.upper_right = upper_right + MARGIN_UPPER_RIGHT

def transform_aabb(aabb,matrix):
    return AABB(matrix*aabb.lower_left,matrix*aabb.upper_right)

def translate_aabb(aabb,position):
    translation = Matrix3.Translate(position)    
    return transform_aabb(aabb,translation)

def does_aabb_intersect(a,b):
    """
    ２つのAABBであるa,bが衝突しているは否か判定。
    """
    if(a.upper_right.x < b.lower_left.x or a.lower_left.x > b.upper_right.x): return False
    if(a.upper_right.y < b.lower_left.y or a.lower_left.y > b.upper_right.y): return False    
    return True

def make_aabb(verts_list,is_static):
    """
    verts_listを囲むAABBを生成。
    
    オブジェクトが回転するたびに新たにAABBは作りたくないので,"最悪"状態のAABBを生成する(つまりタイトなAABBでない)
    例えば細長い三角形などに対しては,大きいAABBとなる。
    
    is_static == Trueの場合(つまり,オブジェクトが動かない場合),AABBをタイトにする。
    """
    verts = make_convex_hull(verts_list)
    xs = []
    ys = []
    for vert in verts:
        xs.append(vert.x)
        ys.append(vert.y)

    min_x = min(xs)
    max_x = max(xs)

    min_y = min(ys)
    max_y = max(ys)

    if(is_static):
        aabb = AABB(Vector2(min_x,min_y),Vector2(max_x,max_y))
    else:            
        min_val = min(min_x,min_y)
        max_val = max(max_x,max_y)
        val = sqrt(2*(max_val**2))
        aabb = AABB(Vector2(-val,-val),Vector2(val,val))
        
    return aabb
