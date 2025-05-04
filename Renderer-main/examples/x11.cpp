#include "x11.h"

static int width  = 640;
static int height = 480;

static Display *display;
static XImage *image;
static XShmSegmentInfo shminfo;
static GC gc;
static Window win;

static void abort(const std::string& message){
  std::cerr << "[!]Error:" << message << std::endl;
  exit(-1);
}

void GetMouseCoordXY(int& x,int& y){
  static Window root_window;
  static int root_x,root_y;
  static unsigned int mask;

  XQueryPointer(display,win,&root_window,&root_window,&root_x,&root_y,&x,&y,&mask);
}

static void FlushX11(){
  XShmPutImage(display,win,gc,image,0,0,0,0,width,height,True);  
  XFlush(display);
}

static void Draw(){
  DrawMain(image->data);
  FlushX11();
}

static void InitX11(){
  //https://www.x.org/releases/X11R7.7/doc/xextproto/shm.html 参照
  
  int screen;
  int major,minor;
  Bool pixmaps;
  XVisualInfo vinfo;  
  XWindowAttributes attributes;
  Status error;
  
  display = XOpenDisplay(NULL);
  if(display == NULL) abort("ディスプレイを開けませんでした");    
  
  screen = DefaultScreen(display);    
  win = XCreateSimpleWindow(display,RootWindow(display,screen),
			    0,0,
			    width,height,1,0,0);
    
  gc = XCreateGC(display,win,0,0);    
  error = XShmQueryVersion(display, &major, &minor, &pixmaps);
  if(not error) abort("Xshmがサポートされていません");
  
  XMatchVisualInfo(display,
		   screen,
		   24,
		   TrueColor,
		   &vinfo);

  XGetWindowAttributes(display,
		       DefaultRootWindow(display),
		       &attributes);
    
  image = XShmCreateImage(display,
			  vinfo.visual,24, ZPixmap, NULL,
			  &shminfo,
			  width,height);
    
  shminfo.shmid = shmget(IPC_PRIVATE,
			 image->bytes_per_line * image->height,
			 IPC_CREAT | 0777);
    
  shminfo.shmaddr = image->data = (char*)shmat(shminfo.shmid,0,0);
  shminfo.readOnly = False;
          
  error = XShmAttach(display, &shminfo);
  if(error == 0) abort("共有メモリセグメントにアタッチできませんでした");

  XSelectInput(display,win,ExposureMask | ButtonPressMask | KeyPressMask);
  XMapWindow(display,win);  
}

static void FreeX11(){      
  XShmDetach(display,&shminfo);
  XDestroyImage(image);
  shmdt(shminfo.shmaddr);
  shmctl(shminfo.shmid, IPC_RMID, 0);
  XCloseDisplay(display);
}

static void Run(){
  XEvent event;
  KeySym key;
  
  while(1){
    if(XPending(display) > 0) {
      XNextEvent(display,&event);
      
      if(event.type == KeyPress){	
	if(not KeyboardPress(event.xkey.keycode)) return;
      }

      if(event.type == ButtonPress){
        MouseButton();	
      }
      
      MouseMove();                
    }else{      
      Draw();
    }
  }
  
}

int main(int argc, char *argv[]){
  InitX11();
  
  Init(width,height);
  Run();
  Free();
  
  FreeX11();
  
  return 0;
}
