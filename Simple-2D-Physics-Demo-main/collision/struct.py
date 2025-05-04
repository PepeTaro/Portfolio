from math2D.vector2 import *
from collision.utils import *

def add_vertex(head,p):
    if(head != None):
        p.next = head
        p.prev = head.prev
        head.prev = p
        p.prev.next = p
    else:
        head = p
        head.next = p
        head.prev = p

def head_vertex():
    head = Vertex()
    head.prev = head
    head.next = head
    return head

class Vertex:
    def __init__(self,vector=None,prev=None,next=None):
        self.vector = vector
        self.prev   = prev
        self.next   = next

class Polygon:
    """
    self.vertsは連結リストの頂点であることに注意。
    """
    def __init__(self,vectors_list=[]):
        self.verts = None
        self.num   = 0

        if(len(vectors_list) > 0):
            self.add_vectors_list(vectors_list)
            
    def add_vectors_list(self,vectors_list):
        for vector in vectors_list:
            self.add_vector(vector)

    def add_vector(self,vec):
        vert = Vertex(vec)        
        self.add_vertex(vert)

    def add_vertex(self,vert):
        if(self.num == 0):
            self.verts = vert
            self.verts.prev = vert
            self.verts.next = vert            
            self.num += 1
        else:
            add_vertex(self.verts,vert)
            self.num += 1

    def find_vertex(self,vert):
        """
        引数vがself.vertsに含まれるならTrue,そうでないならFalseを返す。
        """
        head = self.verts
        for i in range(self.num):
            if(head is vert): return True            
            head = head.next
        return False

class DiffVert:
    """
    ２頂点の差である vert1 - vert2 を表すデータ構造。
    """
    def __init__(self,vert1,vert2):
        self.vert1 = vert1
        self.vert2 = vert2
        self.vector  = self.vert1.vector - self.vert2.vector
        
class Minkowski_Simplex:
    """
    Minkowski Differenceを表すデータ構造。
    
    [注意] self.vertsは,クラスDiffVertのリストであることに注意。
    """
    def __init__(self):
        self.verts = []
        self.num   = 0
        self.extra_idx = None
        
    def add_vertex(self,vert):
        self.verts.append(vert)
        self.num += 1

    def insert_vertex(self,idx,vert):
        self.verts.insert(idx,vert)
        self.num += 1

    def remove_extra_vertex(self):
        if(self.extra_idx == None): return

        del self.verts[self.extra_idx]
        self.num -= 1
        self.extra_idx = None

    def does_contain_origin(self):
        assert(self.num > 0  and self.num <= 3)
        
        if(self.num == 0):
            return False
        elif(self.num == 1):
            if(self.verts[0].vector.is_zero()):
                return True
            else:
                return False
        elif(self.num == 2):
            if(is_origin_on_segment(self.verts[0].vector,self.verts[1].vector)):
                return True
            else:
                return False
        else:
            if(is_origin_inside_triangle(self.verts[0].vector,self.verts[1].vector,self.verts[2].vector)):
                return True
            else:
                return False
            
