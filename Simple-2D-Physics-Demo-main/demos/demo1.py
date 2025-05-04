from mygame import *
from math2D.vector2 import *
from math2D.matrix3 import *

def main():        
    world = World()
        
    floor = make_rectangle(Vector2(320,50),600,50)
    upper_floor = make_rectangle(Vector2(220,350),300,50)
    right_wall = make_rectangle(Vector2(600,250),40,450)
    left_wall = make_rectangle(Vector2(40,250),40,450)
        
    convex = make_convex(Vector2(200,200),[Vector2(-50,-20),Vector2(-20,-50),Vector2(20,-70),Vector2(70,-10),Vector2(75,30),Vector2(-10,40)],50)
    convex.set_rotation(2.8)
    
    world.add(floor)
    world.add(upper_floor)
    world.add(right_wall)
    world.add(left_wall)
    world.add(convex)    
    stack_boxes(world,10)
    
    spring_force = SpringForce()
    spring_force.set_spring(upper_floor,4,10)
    world.add_force_generator(convex,spring_force)     
    
    g = MyGame(640,480,world)
    g.set_window_title("")
    g.run()
    
if __name__ == '__main__':
    main()
