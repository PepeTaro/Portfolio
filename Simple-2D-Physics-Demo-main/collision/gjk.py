from math2D.vector2 import *

from collision.struct import *
from collision.utils import *

def support_minkowski(a,b,d):
    """
    aとbのMinkowski Differenceに対して,ベクトルd方向のSupport関数を計算。
    [注意] 戻り値はVector2クラスでなくて,DiffVertクラスであることに注意。
    """
    a_vert = support_polygon(a,d)
    b_vert = support_polygon(b,-d)

    return DiffVert(a_vert,b_vert)

def support_polygon(polygon,d,start=None):
    """
    Hill Climbing(山登り法)を使用して,最大のdot(v,d)をもつ頂点vを求める。
    引数startはHill Climbingのスタート地点(シード)。
    """
    assert(polygon.num >= 1)
    
    if(start is None): # シードを使用しない場合は適当に頂点を選択。
        vert = polygon.verts
    else:
        assert(polygon.find_vertex(start)) # シードがポリゴンpの頂点であるか否かチェック。
        vert = init        
    
    val = dot(vert.vector,d)        
    while(True):            
        neighbor_val1 = dot(vert.next.vector,d)
        neighbor_val2 = dot(vert.prev.vector,d)
        
        if(neighbor_val1 > val):
            vert = vert.next
            val  = neighbor_val1
        elif(neighbor_val2 > val):
            vert = vert.prev
            val  = neighbor_val2
        else:
            return vert

def get_idx_of_minimum3(val0,val1,val2):
    """
    val0,val1,val2の内,最小の値を持つインデックスを返す。

    例えば,val1がval0,val1,val2の内最小ならば,1を返す。
    """
    if(val0 < val1):
        if(val1 < val2):
            return 0 # val0 < val1 < val2
        else: # val2 <= val1
            if(val0 < val2):
                return 0 # val0 < val2 <= val1
            else: # val2 <= val0
                return 2 # val2 <= val0 <= val1

    else: # val1 <= val0
        if(val0 < val2):
            return 1 # val1 <= val0 < val2
        else: # val2 <= val0
            if(val1 < val2):
                return 1 # val1 < val2 <= val0
            else: # val2 <= val1
                return 2 # val2 <= val1 <= val0

def minimum_norm_vertex(simplex):
    """
    シンプレックス上の点の内,最も最小のノルムをもつ点を返す。
    [注意]この点はシンプレックスの頂点である必要はない。
    
    """
    assert(simplex.num > 0  and simplex.num <= 3)
    
    if(simplex.num == 1): # シンプレックスが1点のみから成る場合
        return simplex.verts[0].vector
    elif(simplex.num == 2):  # シンプレックスが2点のみから成る場合
        p = closest_point_on_segment_to_origin(simplex.verts[0].vector
                                               ,simplex.verts[1].vector)
        return p    
    else:        # シンプレックスが3点から成る場合
        p01 = closest_point_on_segment_to_origin(simplex.verts[0].vector
                                             ,simplex.verts[1].vector)
        p12 = closest_point_on_segment_to_origin(simplex.verts[1].vector
                                             ,simplex.verts[2].vector)
        p20 = closest_point_on_segment_to_origin(simplex.verts[2].vector
                                             ,simplex.verts[0].vector)
        
        norm_p01 = norm2(p01)
        norm_p12 = norm2(p12)
        norm_p20 = norm2(p20)
        min_idx = get_idx_of_minimum3(norm_p01,norm_p12,norm_p20)

        # simplex.extra_idxは"余分"なシンプレックスの頂点を表すインデックス
        # 余分である理由は,GJKのアルゴリズムの仕様上,シンプレックスを最小にしたいため。
        if(min_idx == 0):
            simplex.extra_idx = 2
            return p01
        elif(min_idx == 1):
            simplex.extra_idx = 0
            return p12        
        else:
            simplex.extra_idx = 1
            return p20
        

def gjk(a,b,max_iter=50,init_dir=None):    
    simplex = Minkowski_Simplex()    
    if(init_dir is None):
        v = DiffVert(a.verts,b.verts)
    else:
        v = init_dir

    v = v.vector
    w = support_minkowski(a,b,-v)
    counter = 0 # 無限ループ回避用
    
    # While条件は以下のWikipediaの疑似コードを参照。
    # https://en.wikipedia.org/wiki/Gilbert%E2%80%93Johnson%E2%80%93Keerthi_distance_algorithm
    while(dot(w.vector,v) < 0 and counter < max_iter):
        simplex.add_vertex(w)
        
        if(simplex.does_contain_origin()):
            return simplex
        
        v = minimum_norm_vertex(simplex)
        simplex.remove_extra_vertex()
        w = support_minkowski(a,b,-v)

        counter += 1
    return None
    #return simplex
