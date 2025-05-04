import sys
sys.path.insert(1, '../')

from game2D.game import *
from physics.objects import *
from physics.world import *
from physics.force import *
        
def stack_boxes(world,num):
    width = 40
    height = 40
    mass = 50
    x = 420
    y = 300
    gap = 50    
    for i in range(num):
        box = make_rectangle(Vector2(x,y + i*gap),width,height,mass)        
        world.add(box)        

class MyGame(Game):
    def __init__(self,w,h,world):
        super().__init__(w,h)
        self.world = world
        self.debug = False
        self.mouse_pos = Vector2(0.0)
        self.mouse_drag_force = 30
        self.moving_idx = 0

        self.show_messages()

    def show_messages(self):
        print("[*]マウスをドラッグすることでオブジェクトを動かせる")
        print("[*]Spaceキーを押下することでデバッグ情報を表示")
        print("[*]Enterキーを押下することでデバッグ情報を非表示")
        
    def holding_mouse_handler(self):
        mouse_pos = self.get_mouse_pos()
        delta_mouse_pos = mouse_pos - self.mouse_pos
        self.mouse_pos = mouse_pos

        if(self.world.objs[self.moving_idx].inv_mass == 0.0): return
        self.world.objs[self.moving_idx].set_awake()
        self.world.objs[self.moving_idx].add_impulse_on_center_of_mass(self.mouse_drag_force*delta_mouse_pos)
        
    def mouse_button_down_handler(self,button):                    
        self.mouse_pos = self.get_mouse_pos()
        distances = [dist(self.mouse_pos,obj.position) for obj in self.world.objs]
        self.moving_idx = min(range(len(distances)),key=distances.__getitem__)

    def keyboard_down_handler(self,key):        
        if(key == K_q): self.running = False
        elif(key == K_SPACE): self.debug = True
        elif(key == K_RETURN): self.debug = False

    def draw(self):            
        for obj in self.world.objs:
            self.draw_obj(obj)

        for obj,gen in self.world.force_generators:
            if(not isinstance(gen,SpringForce)): continue
            super().draw_line(BLACK,obj.position,gen.other.position,gen.spring_const)
        
    def draw_obj(self,obj,color=BLACK,width=1):
        if(obj.verts_in_world is None):
            obj.transform_verts()
            
        verts = obj.verts_in_world
        for i in range(obj.num):
            super().draw_line(color,verts[i],verts[(i+1)%obj.num],width)

    def draw_debug(self):
        if(not self.debug): return
        
        for obj in self.world.objs:
            rotation = Matrix3.Rotate(obj.rotation)
            translation = Matrix3.Translate(obj.position)
            mat = translation
            lower = mat*obj.aabb.lower_left
            upper = mat*obj.aabb.upper_right
            super().draw_rect(MAROON,lower,upper,1)
            
            if(obj.is_sleep):
                self.draw_obj(obj,GREEN,5)

            if(self.world.objs[self.moving_idx] == obj):
                super().draw_circle(TEAL,obj.position,3)
                
        for contact in self.world.contacts:
            scale = 10
            for p in contact.manifold.points:
                normal = contact.manifold.n
                mtv = normal*scale
                super().draw_line(RED,p,p+mtv,1)
                super().draw_line(BLUE,p,p-mtv,1)            
                super().draw_circle(MAGENTA,p,3)
                
    def update(self):
        dt = 1.0/60
        self.world.update(dt)
        self.draw_debug()
