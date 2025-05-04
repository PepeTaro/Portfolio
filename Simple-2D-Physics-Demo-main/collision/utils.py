from math2D.vector2 import *

EPSILON = 1e-8 # 数値誤差用

def is_point_on_left_side_of_segment(p,v0,v1):
    """
    点pがv0とv1を結ぶ線分に対して"左側"にある場合True,そうでない場合Falseを返す。
    [注意] 点pが線分上にある場合Falseを返すことに注意。
    """
    d1 = v1 - v0
    d2 = p - v0
    if(cross(d1,d2) > 0):
        return True
    else:
        return False

def get_parameter_of_point_on_segment(p,v0,v1):
    """
    点pがv0を始点,v1を終点とする線分上にある場合,対応するパラメータを返す。(p = (1-t)*v0 + t*v1)
    点pが線分上にない場合Noneを返す。
    """
    v0_p = v0 - p
    v0_v1 = v0 - v1
    norm2_val = norm2(v0_v1)
    if(norm2_val == 0.0): return v0
    t = dot(v0_p,v0_v1)/norm2_val
    if(t < 0 or t > 1.0 - EPSILON): return None
    return t

def is_point_inside_triangle(p,v0,v1,v2):
    """
    点pが頂点v0,v1,v2から成る三角形の内側にある場合True,そうでない場合Falseを返す。
    """    
    q = p - v0
    v10 = v1 - v0
    v20 = v2 - v0
    delta = (v10.x*v20.y - v20.x*v10.y)
    if(delta == 0): return False

    alpha = (q.x*v20.y - q.y*v20.x)/delta
    if(alpha < 0 or alpha > 1.0): return False
    
    beta  = (-q.x*v10.y + q.y*v10.x)/delta
    if(beta < 0 or beta > 1.0): return False

    if(alpha + beta - 1 > 0): return False
    
    return True

def is_origin_inside_triangle(v0,v1,v2):
    origin = Vector2(0,0)
    return is_point_inside_triangle(origin,v0,v1,v2)

def is_point_on_segment(p,v0,v1):
    """
    点pがv0とv1を結ぶ線分上にある場合True,そうでない場合Falseを返す。
    """
    p_v0 = p - v0
    v1_v0 = v1 - v0
    if(abs(cross(v1_v0,p_v0)) < EPSILON): # cross(v1_v0,p_v0) == 0
        return True
    else:
        return False
    
def is_origin_on_segment(v0,v1):        
    origin = Vector2(0,0)
    return is_point_on_segment(origin,v0,v1)

def closest_point_on_segment_to_origin(begin,end):
    """
    beginとendを結ぶ線分上の点の内,最も原点に近い点を返す。
    """
    
    delta = begin - end
    norm_val = norm2(delta)
    if(norm_val == 0): return begin # beginとendが重なっている場合
    
    interp = dot(begin,delta)/norm_val

    if(interp <= 0.0):
        return begin
    elif(interp >= 1.0):
        return end
    else:
        return begin + interp*(end - begin)
