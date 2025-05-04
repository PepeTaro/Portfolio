from convex_hull.convex_hull import *
from physics.rigid  import *
from physics.convex import *
from physics.params import INF

from math import sqrt

def is_mass_infinite(mass):
    if(mass == INF): return True
    else: return False

def make_rectangle(center,width,height,mass=INF):
    """
    centerを中心に横幅width,縦幅heightをもつ長方形を返す。
    massは長方形の質量。
    """
    static = is_mass_infinite(mass)
    
    #inertia = mass * width * height * (width**2 + height**2) / 12.0
    #inertia = mass * (width**2 + height**2) / 12.0 # Box2D
    inertia = mass * (width**2 + height**2) / 5 # HACK        
    rectangle = Convex([Vector2(-width/2,-height/2),Vector2(width/2,-height/2),Vector2(width/2,height/2),Vector2(-width/2,height/2)],static)
    rectangle.set_mass(mass)
    rectangle.set_inertia(inertia)
    rectangle.set_position(center.x,center.y)

    return rectangle

def make_triangle(center,v1,v2,v3,mass=INF):
    """
    "半時計回り"にv1,v2,v3を頂点としてもつ三角形を返す。
    massは三角形の質量。
    """
    static = is_mass_infinite(mass)
    
    width = max(v1.x,v2.x,v3.x) - min(v1.x,v2.x,v3.x)
    height = max(v1.y,v2.y,v3.y) - min(v1.y,v2.y,v3.y)
    #inertia = mass * width * (height**3) * 1/36 # 
    inertia = mass * width * (height**3) * 1e-5 # HACK

    triangle = Convex([v1,v2,v3],static)
    triangle.set_mass(mass)
    triangle.set_inertia(inertia)
    triangle.set_position(center.x,center.y)

    return triangle

def make_convex(center,verts_list,mass=INF):
    """
    "半時計回り"にリストverts_listを頂点としてもつ凸集合を返す。
    massは凸集合の質量。
    """
    static = is_mass_infinite(mass)
    
    verts = make_convex_hull(verts_list)
    xs = []
    ys = []
    for vert in verts:
        xs.append(vert.x)
        ys.append(vert.y)
        
    width = max(xs) - min(xs)
    height = max(ys) - min(ys)
    inertia = mass * (width**2 + height**2) / 36 # HACK

    convex = Convex(verts,static)
    convex.set_mass(mass)
    convex.set_inertia(inertia)
    convex.set_position(center.x,center.y)

    return convex

        
