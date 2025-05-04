"""
ゲーム(アニメーション)用簡易pygameラッパー
"""
import pygame

from math2D.vector2 import *

QUIT = pygame.QUIT
KEYUP = pygame.KEYUP
KEYDOWN = pygame.KEYDOWN
MOUSEBUTTONDOWN = pygame.MOUSEBUTTONDOWN
MOUSEBUTTONUP = pygame.MOUSEBUTTONUP

K_RIGHT = pygame.K_RIGHT
K_LEFT = pygame.K_LEFT
K_DOWN = pygame.K_DOWN
K_UP = pygame.K_UP
K_q  = pygame.K_q
K_SPACE = pygame.K_SPACE
K_RETURN = pygame.K_RETURN
K_BACKSPACE = pygame.K_BACKSPACE
K_TAB = pygame.K_TAB

# (色参照) https://txwes.libguides.com/c.php?g=978475&p=7075536
WHITE   = (255,255,255)
BLACK   = (0,0,0)
RED     = (255,0,0)
GREEN   = (0,255,0)
BLUE    = (0,0,255)
YELLOW  = (255,255,0)
MAGENTA = (255,0,255)
CYAN    = (0,255,255)
SILVER  = (192,192,192)
GRAY    = (128,128,128)
MAROON  = (128,0,0)
OLIVE   = (128,128,0)
PURPLE  = (128,0,128)
TEAL    = (0,128,128)
NAVY    = (0,0,128)
#LIME   = (0,255,0)?
#GREEN   = (0,128,0)?

class Game:    
    def __init__(self,width,height):
        self.width = width
        self.height = height
        self.screen = pygame.display.set_mode([self.width,self.height])
        
        self.running = False        
        self.is_holding_key = False
        self.holding_which_key = None
        self.is_holding_mouse_button = False
                
        self.clock = pygame.time.Clock()
        self.dt = 0.01
                
    def _event_handler(self):        
        self._holding_key_handler()
        self._holding_mouse_handler()

        for event in pygame.event.get():
            self.event_handler(event)
            
    def _quit_handler(self):
        self.running = False
        self.quit_handler()
        
    def _keyboard_up_handler(self):
        self.is_holding_key = False
        self.holding_which_key = None
        self.keyboard_up_handler()
        
    def _keyboard_down_handler(self,key):
        self.is_holding_key = True
        self.holding_which_key = key
        self.keyboard_down_handler(key)
        
    def _mouse_button_down_handler(self,button):
        self.is_holding_mouse_button = True
        self.mouse_button_down_handler(button)
        
    def _mouse_button_up_handler(self):
        self.is_holding_mouse_button = False
        self.mouse_button_up_handler()
        
    def _holding_mouse_handler(self):
        if(self.is_holding_mouse_button):
            self.holding_mouse_handler()
        
    def _holding_key_handler(self):
        if(self.is_holding_key):
            self.holding_key_handler()

    def _calculate_cartesian_coordinate(self,vec):
        # 左下を原点に設定
        return (vec.x,self.height - vec.y)
            
    def _update(self):
        self.update()        
    def _draw(self):
        self.draw()        
    def _timer(self):
        self.timer()

    def set_window_title(self,title):
        pygame.display.set_caption(title)
        
    def init(self):
        pygame.init()

    def event_handler(self,event):
        if event.type == QUIT:
            self._quit_handler()            
        elif event.type == KEYUP:
            self._keyboard_up_handler()                
        elif event.type == KEYDOWN:
            self._keyboard_down_handler(event.key)                
        elif event.type == MOUSEBUTTONDOWN:
            self._mouse_button_down_handler(event.button)            
        elif event.type == MOUSEBUTTONUP:
            self._mouse_button_up_handler()

    def run(self):
        self.running = True
        while self.running:
            self.screen.fill(WHITE)
            self._event_handler()
            self._update()
            self._draw()
            self._timer()            
            pygame.display.flip()            
        pygame.quit()

    def quit_handler(self):
        pass
    def keyboard_up_handler(self):
        pass    
    def keyboard_down_handler(self,key):        
        if(key == K_q): self.running = False
    def mouse_button_down_handler(self,button):
        pass
    def mouse_button_up_handler(self):
        pass        
    def holding_mouse_handler(self):
        pass    
    def holding_key_handler(self):
        pass
                            
    def timer(self):                
        self.clock.tick()
        fps = self.clock.get_fps()
        if(fps != 0):
            self.dt= 1.0/self.clock.get_fps()
        else:
            self.dt = 0.01

    def draw_circle(self,color,vec,radius,width=0):
        (x,y) = self._calculate_cartesian_coordinate(vec)
        pygame.draw.circle(self.screen,color,(x,y),radius,width)
    def draw_line(self,color,vec1,vec2,width=1):
        (x1,y1) = self._calculate_cartesian_coordinate(vec1)
        (x2,y2) = self._calculate_cartesian_coordinate(vec2)        
        pygame.draw.line(self.screen,color,(x1,y1),(x2,y2),width)        
    def draw_triangle(self,color,vec1,vec2,vec3,width=1):
        (x1,y1) = self._calculate_cartesian_coordinate(vec1)
        (x2,y2) = self._calculate_cartesian_coordinate(vec2)
        (x3,y3) = self._calculate_cartesian_coordinate(vec3)        
        pygame.draw.line(self.screen,color,(x1,y1),(x2,y2),width)
        pygame.draw.line(self.screen,color,(x2,y2),(x3,y3),width)
        pygame.draw.line(self.screen,color,(x3,y3),(x1,y1),width)
    def draw_rect(self,color,vec1,vec2,width=1):
        (x1,y1) = self._calculate_cartesian_coordinate(vec1)
        (x2,y2) = self._calculate_cartesian_coordinate(vec2)        
        pygame.draw.rect(self.screen,color,(x1,y1,(x2-x1),(y2-y1)),width)
        
    def get_mouse_pos(self):
        """
        左下を原点とした座標成分に変換
        """
        pos = pygame.mouse.get_pos()
        pos_vec = Vector2(pos[0],self.height - pos[1])
        return pos_vec

    def draw(self):
        raise NotImplementedError('Error(draw):Override me!')
    def update(self):
        raise NotImplementedError('Error(update):Override me!')
