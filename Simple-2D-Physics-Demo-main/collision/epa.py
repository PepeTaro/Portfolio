from math2D.vector2 import *

from collision.gjk import *
from collision.utils import *
from collision.struct import *

EPA_EPSILON = 1e-8

class ContactManifold:
    def __init__(self,a,b,pa,pb,n,d):
        self.a = a # オブジェクト A
        self.b = b # オブジェクト B
        self.pa = pa # A上のContact Point
        self.pb = pb # B上のContact Point
        self.n  = n  # Contact Normal,Bをn方向に動かすとめり込みが直るように向いている。
        self.d  = d  # Depth(めり込みの深さ)

        self.points = [] # Contact Points
        
def get_point_of_polytope_closest_to_origin(polytope):
    """
    ポリトープ上の点の内,最も原点に近い点を求める。
    戻り値として,その点とその点がある線分上のインデックスを返す。
    """
    num = polytope.num
    min_val = float("inf")
    min_vert = None
    min_edge_idx = None
    
    for i in range(num):
        v = closest_point_on_segment_to_origin(polytope.verts[i].vector,polytope.verts[(i+1)%num].vector)
        norm2_v = norm2(v)
        if(norm2_v < min_val):
            min_vert = v
            min_val = norm2_v
            min_edge_idx = i
            
    return (min_vert,min_edge_idx)

def epa(a,b,polytope):
    max_iter = a.num + b.num
    for _ in range(max_iter): # HACK:無限ループにならないようにmax_iterを設定。
        (v,edge_idx) = get_point_of_polytope_closest_to_origin(polytope)
        normal = normalize(v)
        
        w = support_minkowski(a,b,normal)
        d = dot(w.vector,normal)

        if( (d - norm(v)) < EPA_EPSILON):
            return (normal,d,edge_idx)
        else:
            polytope.insert_vertex(edge_idx+1,w)
        
    return None

def contact_manifold(a,b):
    assert(a.num >= 3 and b.num >= 3) # TODO: 点,線分にも対応

    # GJKアルゴリズムを使用して,aとbが衝突している場合,aとbのMinkowski Differenceの内原点を含む最小のシンプレックスを求める
    simplex = gjk(a,b)
    
    if(simplex is None): # 衝突していない場合
        return None
    
    # EPAアルゴリズムを使用して,最も原点に近い点point_closest_to_origin,めり込みの深さdepth,
    # 点point_closest_to_originが"乗っている"線分のインデックスを得る
    epa_info = epa(a,b,simplex)
    if(epa_info is None): return None
    (point_closest_to_origin,depth,edge_idx) = epa_info
    
    # segment_beginとsegment_endを結ぶ線分上にv*dが存在する
    segment_begin = simplex.verts[edge_idx]
    segment_end = simplex.verts[(edge_idx+1)%simplex.num]

    # v0とv1を結ぶ線分上にあるv*dのパラメータを求める
    t = get_parameter_of_point_on_segment(point_closest_to_origin*depth,segment_begin.vector,segment_end.vector)
    
    if(t is None): # "理論上" tがNoneであることはありえない(つまり数値誤差),なぜならv*dはsegment_beginとsegment_end上に存在するはずだから。
        print("[!] ERROR(contact_manifold): 数値誤差!")
        return None

    # aとbのContact Pointを計算
    pa = (1 - t)*segment_begin.vert1.vector + t*segment_end.vert1.vector
    pb = (1 - t)*segment_begin.vert2.vector + t*segment_end.vert2.vector
    
    return ContactManifold(a,b,pa,pb,point_closest_to_origin,depth)
