from math2D.vector2 import *
from math2D.matrix3 import *
from convex_hull.convex_hull import *

from physics.rigid  import *
from physics.aabb import *

class Convex(Rigid):
    """
    凸多角形を表すクラス
    [注意] self.vertsはBody Spaceにおける座標であることに注意。    
    """
    def __init__(self,points_list=[],static=False):
        """
        引数staticがTrueである場合,静的オブジェクトであることを表す,
        staticがTrue或いはFalseに応じて,AABBの形が異なる(静的オブジェクトの場合,AABBをタイトにする)
        """
        super().__init__()
        self.num   = 0  # 頂点数
        
        self.verts = [] # Body Space内の凸多角形の頂点
        self.verts_in_world = None # Body Space内の凸多角形の頂点
        
        self.aabb = None # Body Space内のAABB
        self.aabb_in_world = None # World Space内のAABB
                
        self._init(points_list,static)
        
    def _init(self,points_list,static):
        if(len(points_list) > 0):
            self._add_points(points_list)
            self.aabb = make_aabb(self.verts,static)

    def _add_points(self,points_list):
        verts = make_convex_hull(points_list)
        self.verts.extend(verts)
        self.num += len(verts)        
    
    def transform_aabb(self):
        """
        AABBをWorld座標に変換し,self.transformed_aabbに保存
        """        
        self.aabb_in_world = translate_aabb(self.aabb,self.position)
        
    def transform_verts(self):
        """
        頂点をWorld座標に変換し,self.transformed_vertsに保存
        """
        rotation_matrix    = Matrix3.Rotate(self.rotation)
        translation_matrix = Matrix3.Translate(self.position)
        matrix             = translation_matrix*rotation_matrix
        self.verts_in_world = []        
        for vert in self.verts:
            self.verts_in_world.append(matrix*vert)
