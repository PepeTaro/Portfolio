from math2D.vector2 import *
from physics.params import LINEAR_DAMPING,ANGULAR_DAMPING,SLEEP_ENERGY_EPSILON,INF

class Rigid:
    """
    動的オブジェクトの基底となるクラス
    """
    def __init__(self):
        # 剛体の状態を決定する変数
        self.position = Vector2(0,0)       # 質量中心
        self.velocity = Vector2(0,0)       # 速度
        self.rotation = 0.0                # 角度
        self.angular_velocity = 0.0        # 角速度

        # 定数
        self.mass = INF        # 質量            
        self.inv_mass = 0.0    # 質量の逆数        
        self.inertia = INF     # 慣性モーメント
        self.inv_inertia = 0.0 # 慣性モーメントの逆数
        
        self.force = Vector2(0,0)   # 合力
        self.torque = 0.0           # トルク
        
        self.friction = 2.0         # 動摩擦係数
        self.is_sleep = False       # オブジェクトがスリープ状態にある場合True
        self.energy = 100.0         # 運動エネルギー(スリープ状態にするのに必要,初期値を0にすると,重力によって落ちてこない)
        
    def set_position(self,x,y):
        self.position = Vector2(x,y)
    def set_velocity(self,x,y):
        self.velocity = Vector2(x,y)
    def set_rotation(self,rotation):
        self.rotation = rotation
    def set_angular_velocity(self,angular_velocity):
        self.angular_velocity = angular_velocity
    def set_mass(self,mass):
        assert(mass > 0.0)
        self.mass = mass
        self.inv_mass = 1.0/self.mass        
    def set_inertia(self,inertia):
        assert(inertia > 0.0)
        self.inertia = inertia
        self.inv_inertia = 1.0/self.inertia
    def set_restitution(self,restitution):
        self.restitution = restitution
    def set_friction(self,friction):
        self.friction = friction
    
    def clear_force(self):
        self.force.clear()
    def clear_torque(self):
        self.torque = 0

    """
    以下すべてのメソッドにおいて,下のコードはオブジェクトが静的オブジェクトである場合即座にreturnすることを意味する。
     if(self.inv_mass == 0.0): return
    """
    
    def add_force(self,force):
        if(self.inv_mass == 0.0): return
        
        self.force += force
    def add_torque(self,torque):
        if(self.inv_mass == 0.0): return
        
        self.torque += torque
        
    def add_impulse_on_center_of_mass(self,impulse):
        if(self.inv_mass == 0.0): return
        
        self.velocity += self.inv_mass*impulse
        
    def add_impulse_on_point(self,point,impulse):
        if(self.inv_mass == 0.0): return
        
        r = point - self.position
        self.velocity         += self.inv_mass*impulse
        self.angular_velocity += self.inv_inertia*cross(r,impulse)
                                
    def integrate_velocity(self,delta_time):
        if(self.inv_mass == 0.0): return

        # 簡易化した減衰項
        self.force  += -LINEAR_DAMPING*self.velocity
        self.torque += -ANGULAR_DAMPING*self.angular_velocity
        
        self.velocity         += self.force*self.inv_mass*delta_time
        self.angular_velocity += self.torque*self.inv_inertia*delta_time
        
        self.clear_force()  # 合力を0に初期化
        self.clear_torque() # トルクを0に初期化        
        
    def integrate_position(self,delta_time):
        if(self.inv_mass == 0.0): return
        
        self.position += self.velocity*delta_time
        self.rotation += self.angular_velocity*delta_time
        
    def set_sleep(self):        
        self.is_sleep = True
        # 速度を0に設定
        self.velocity.clear() 
        self.angular_velocity = 0.0
        
    def set_awake(self):
        if(not self.is_sleep): return # すでに"起きている"場合
        self.is_sleep = False
        self.energy = SLEEP_ENERGY_EPSILON*2.0 # sleep状態からawake状態に遷移した"直後"に再度sleepしないようにする処置
        
