"""
https://en.wikibooks.org/wiki/Algorithm_Implementation/Geometry/Convex_hull/Monotone_chain#Python
"""
from math2D.vector2 import *

def make_convex_hull(points):    
    points = sorted(points)
    if len(points) <= 1:
        return points

    lower = []
    for p in points:
        while(len(lower) >= 2 and cross(lower[-1] - lower[-2],p - lower[-2]) <= 0):
            lower.pop()
        lower.append(p)

    upper = []
    for p in reversed(points):
        while(len(upper) >= 2 and cross(upper[-1] - upper[-2],p - upper[-2]) <= 0):
            upper.pop()
        upper.append(p)
    
    return lower[:-1] + upper[:-1]
