from math2D.vector2 import *
from math2D.matrix3 import *
from collision import struct,epa

from physics.params import SLEEP_ENERGY_EPSILON,MOTION_AVG_BIAS,SOLVER_ITER,BETA,SLOP
from physics.rigid  import *
from physics.convex import *
from physics.objects import *
from physics.aabb import *

class Contact:
    def __init__(self,a=None,b=None,manifold=None,t=None):
        """
        AがBに衝突していると仮定。つまり、nはAの法線ベクトル。
        """
        self.a = a # オブジェクトA
        self.b = b # オブジェクトB
        self.manifold = manifold # Contact Manifold
        self.t = t # 接ベクトル

        self.bias = 0.0 # Bias Velocity
        self.friction = 0.0 # 動摩擦係数
        
        self.accum_normal_impulse = 0.0  # Normal Impulseを蓄積するための変数
        self.accum_tangent_impulse = 0.0 # Tangent Impulseを蓄積するための変数
        self.mass_normal = 0.0           # Normal Impuseの計算時に使用         
        self.mass_tangent = 0.0          # Tangent Impuseの計算時に使用
        self.is_mass_normal_stored = False
        self.is_mass_tangent_stored = False

def clamp(val,min_val,max_val):
    return min(max(val,min_val),max_val)

def compute_frictional_impulse(contact,p,normal_impulse):
    t = contact.t # Contact Tangent
    
    r1 = p - contact.a.position
    r2 = p - contact.b.position
    delta_v = contact.b.velocity + cross(contact.b.angular_velocity,r2) - (contact.a.velocity + cross(contact.a.angular_velocity,r1))
    vrel = dot(contact.t,delta_v)    
    numerator = -vrel
    
    if(not contact.is_mass_tangent_stored):
        rt1 = dot(r1,t)
        rt2 = dot(r2,t)

        denom1 = contact.a.inv_mass + contact.b.inv_mass
        denom2 = contact.a.inv_inertia*(dot(r1,r1) - rt1*rt1)
        denom3 = contact.b.inv_inertia*(dot(r2,r2) - rt2*rt2)
        k_t = denom1 + denom2 + denom3
        contact.mass_tangent = 1.0/k_t
        contact.friction = sqrt(contact.a.friction*contact.b.friction)
        contact.is_mass_tangent_stored = True
        
    max_frictional_impulse = contact.friction*normal_impulse
    tangent_impulse = numerator*contact.mass_tangent
    old_accum_tangent_impulse = contact.accum_tangent_impulse
    contact.accum_tangent_impulse = clamp(old_accum_tangent_impulse + tangent_impulse,-max_frictional_impulse,max_frictional_impulse)
    tangent_impulse = contact.accum_tangent_impulse - old_accum_tangent_impulse
    return tangent_impulse

def compute_normal_impulse(contact,p,dt):
    n = contact.manifold.n   # Contact Normal
    d = contact.manifold.d   # Depth
    
    r1 = p - contact.a.position
    r2 = p - contact.b.position
    delta_v = contact.b.velocity + cross(contact.b.angular_velocity,r2) - (contact.a.velocity + cross(contact.a.angular_velocity,r1))
    vrel = dot(n,delta_v)    
    numerator = -vrel
    
    if(not contact.is_mass_normal_stored):            
        # Bias Velocity((振動するため)完全にめり込みを直さないために必要)
        contact.bias = BETA*max(0,d - SLOP)/dt
        
        rn1 = dot(r1,n)
        rn2 = dot(r2,n)
        
        denom1 = contact.a.inv_mass + contact.b.inv_mass
        denom2 = contact.a.inv_inertia*(dot(r1,r1) - rn1*rn1)
        denom3 = contact.b.inv_inertia*(dot(r2,r2) - rn2*rn2)        
        k_n = denom1 + denom2 + denom3
        contact.mass_normal = 1.0/k_n
        contact.is_mass_normal_stored = True

    normal_impulse = (numerator + contact.bias)*contact.mass_normal    
    old_accum_normal_impulse = contact.accum_normal_impulse
    contact.accum_normal_impulse = max(old_accum_normal_impulse + normal_impulse,0.0)
    normal_impulse = contact.accum_normal_impulse - old_accum_normal_impulse
    return normal_impulse

class World:
    def __init__(self):        
        self.gravity = Vector2(0,-10)
        
        self.objs = [] # World Space内のオブジェクトを保持するリスト
        self.contacts = [] # すべてのContactクラスを保持するリスト
        self.colliding_aabb_index_pair = [] # AABB衝突しているオブジェクトのインデックスペアを保存するリスト
        self.force_generators = [] # Force Generator
        
    def clear(self):
        self.objs.clear()
        self.contacts.clear()

    def add(self,obj):
        self.objs.append(obj)

    def add_force_generator(self,obj,force_generator):
        self.force_generators.append((obj,force_generator))
        
    def broad_phase(self):
        n = len(self.objs)
        self.colliding_aabb_index_pair.clear()

        # 前もってすべてのオブジェクトのAABBを計算し,メンバ変数aabb_in_worldに保存。
        # 後に重複計算を防ぐために,ここでverts_in_worldをNoneに設定
        # (ここでNoneに設定しなくてもいいが,全オブジェクトをループしているため都合がいい)
        for i in range(n):
            self.objs[i].transform_aabb()
            self.objs[i].verts_in_world = None
        
        for i in range(n):            
            for j in range(i+1,n):
                if(self.objs[i].inv_mass == 0 and self.objs[j].inv_mass == 0): continue # 両オブジェクトが動かない場合
                if(self.objs[i].is_sleep and self.objs[j].is_sleep): continue           # 両オブジェクトが寝ている場合
                
                if(does_aabb_intersect(self.objs[i].aabb_in_world,self.objs[j].aabb_in_world)): # AABBが衝突している場合
                    self.colliding_aabb_index_pair.append((i,j)) # 衝突しているオブジェクトのインデックスを保存(Narrow Phase時に使用)
                    
    def narrow_phase(self,dt):
        self.collision_detection(dt)
        self.collision_response(dt)

    def apply_forces_by_generators(self,dt):
        for (obj,force_gen) in self.force_generators:
            force_gen.update_force(obj,dt)

    def update_velocity(self,dt):        
        for obj in self.objs:
            if(obj.inv_mass == 0): continue # 静的オブジェクトの場合
            if(obj.is_sleep): continue      # 寝ているオブジェクトの場合
            
            obj.add_force(obj.mass*self.gravity)
            obj.integrate_velocity(dt)
            
    def update_position(self,dt):
        for obj in self.objs:
            obj.integrate_position(dt)

    def update(self,dt):
        if(dt == 0.0): dt = 0.0

        self.try2sleep() # 寝てみる
        
        self.apply_forces_by_generators(dt)
        
        self.broad_phase()
        self.update_velocity(dt)
        self.narrow_phase(dt)
        self.update_position(dt)        

    def get_contact_manifold(self,obj1,obj2):

        # 頂点をWorld Space座標に変換(重複計算を防ぐためにNoneか否かチェックしている)
        if(obj1.verts_in_world is None):
            obj1.transform_verts()
        if(obj2.verts_in_world is None):
            obj2.transform_verts()

        # EPAアルゴリズムに備えてデータ構造を初期化
        p1 = struct.Polygon(obj1.verts_in_world)
        p2 = struct.Polygon(obj2.verts_in_world)
        
        manifold = epa.contact_manifold(p1,p2) # EPAを使用してContact Manifoldを取得
        
        if(manifold is None): # 衝突していない場合
            return None
        
        manifold.points.append(manifold.pa) # manifold.paをContact Pointとする(manifold.pbでもOK)
        return manifold

    def update_contacts(self,obj1,obj2,manifold):
        if(manifold is None):
            return
        
        tangent = cross(manifold.n,1.0)
        contact = Contact(obj1,obj2,manifold,tangent)
        self.contacts.append(contact)

        # オブジェクトが衝突している場合,両方のオブジェクトを起こす
        obj1.set_awake()
        obj2.set_awake()

    def collision_detection(self,dt):
        self.contacts.clear()
                
        for (i,j) in self.colliding_aabb_index_pair:            
            manifold = self.get_contact_manifold(self.objs[i],self.objs[j])                
            self.update_contacts(self.objs[i],self.objs[j],manifold)                
            
    def collision_response(self,dt):                        
        for k in range(SOLVER_ITER):
            for contact in self.contacts:
                for p in contact.manifold.points:

                    # HACK:
                    # Normal Impulseを計算して速度を更新し,Frictional Impulseを計算して速度を更新するより
                    # 最初にいっぺんにNormal Impulse,Frictional Impulseを計算してあとで速度をいっきに更新したほうが"比較的"安定する。
                    impulse_coeff = compute_normal_impulse(contact,p,dt)
                    impulse = impulse_coeff*contact.manifold.n
                    
                    frictional_impulse_coeff = compute_frictional_impulse(contact,p,contact.accum_normal_impulse)
                    frictional_impulse = frictional_impulse_coeff*contact.t

                    contact.a.add_impulse_on_point(p,-impulse)
                    contact.b.add_impulse_on_point(p,impulse)
                                        
                    contact.a.add_impulse_on_point(p,-frictional_impulse)
                    contact.b.add_impulse_on_point(p,frictional_impulse)
                    
    def try2sleep(self):        
        for obj in self.objs:
            if(obj.is_sleep): continue # すでに"寝ている"場合
            if(obj.inv_mass == 0.0):   # 静的オブジェクトの場合はすぐに"寝かせる"
                obj.set_sleep()
                continue

            current_energy = norm2(obj.velocity) + obj.inv_inertia*obj.rotation**2 # HACK: 運動エネルギーを若干改良している
            obj.energy = MOTION_AVG_BIAS*obj.energy + (1.0 - MOTION_AVG_BIAS)*current_energy # 移動平均を使用してenergy変数を更新
            
            if(obj.energy > 3*SLEEP_ENERGY_EPSILON): obj.energy = 3*SLEEP_ENERGY_EPSILON # エネルギーが大きすぎる場合は"抑える"            
            if(obj.energy < SLEEP_ENERGY_EPSILON): # sleep状態の条件を満たしている場合
                obj.set_sleep() # sleep状態にする
