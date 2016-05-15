
#include <GL/glew.h>
//#include "GL/freeglut.h"

#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/glx.h>

#include <chrono>
#include <math.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

#include <iostream>
#include "RenderScene.h"

#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

struct termios orig_termios;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}



#ifndef _WIN32
//http://stackoverflow.com/questions/31579243/segmentation-fault-before-main-when-using-glut-and-stdstring
#include <pthread.h> 


void* simpleFunc(void*) { return NULL; } 
void forcePThreadLink() { pthread_t t1; pthread_create(&t1, NULL, &simpleFunc, NULL); }

#endif

GLint windowL = 0;
GLint windowR = 0;

struct WindowData
{
	WindowData()
	{
		att[0]= GLX_RGBA;
		att[1]= GLX_DEPTH_SIZE;
		att[2]= 24;
		att[3]= GLX_DOUBLEBUFFER;
		att[4]= None;
	}

	Display                 *dpy;
	Window                  root;
	GLint                   att[5] ;
	XVisualInfo             *vi;
	Colormap                cmap;
	XSetWindowAttributes    swa;
	Window                  win;
	GLXContext              glc;
	XWindowAttributes       gwa;
};


RenderScene RS_R;


void keyPressCallback(int _key, RenderScene* _rs)
{
	_rs->HandleInput(_key);


	std::cerr << "(" << _key << ")";	    
	switch(_key)
	{
		case 'q':
		break;
	}	
}



#define KEYCODE XK_Down

// Function to create a keyboard event
XKeyEvent createKeyEvent(Display *display, Window &win,
                           Window &winRoot, bool press,
                           int keycode, int modifiers)
{
   XKeyEvent event;

   event.display     = display;
   event.window      = win;
   event.root        = winRoot;
   event.subwindow   = None;
   event.time        = CurrentTime;
   event.x           = 1;
   event.y           = 1;
   event.x_root      = 1;
   event.y_root      = 1;
   event.same_screen = True;
   event.keycode     = XKeysymToKeycode(display, keycode);
   event.state       = modifiers;

   if(press)
      event.type = KeyPress;
   else
      event.type = KeyRelease;

   return event;
}

/*
//I miss windows.
unsigned long GetTickCount()
{
	struct timeval tv;
	if( gettimeofday(&tv, NULL) != 0 )
    	return 0;

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
*/

void draw(RenderScene* _rs)
{
	
	//glutSetWindow(windowL);
	//std::cout << "Drawing Left.\n";
	_rs->Render();
	//glutPostRedisplay();
}


void InitialiseWindow( WindowData* _win, int _window, RenderScene* _RS)
{
	_win->dpy = XOpenDisplay(NULL); //
 
 	if(_win->dpy == NULL) 
	{
 		printf("\n\tcannot connect to X server\n\n");
        exit(0);
	}
       
 	_win->root = XRootWindow(_win->dpy, _window);
 	_win->vi = glXChooseVisual(_win->dpy, _window, _win->att);
 	if(_win->vi == NULL) 
	{
		printf("\n\tno appropriate visual found\n\n");
        exit(0);
 	} 

 	_win->cmap = XCreateColormap(_win->dpy, _win->root, _win->vi->visual, AllocNone);
 	_win->swa.colormap = _win->cmap;
 	_win->swa.event_mask = ExposureMask | KeyPressMask;
 	_win->win = XCreateWindow(_win->dpy, _win->root, 0, 0, _RS->m_SizeX, _RS->m_SizeY, 0, _win->vi->depth, InputOutput, _win->vi->visual, CWColormap | CWEventMask, &(_win->swa));
 	XMapWindow(_win->dpy, _win->win);
 	XStoreName(_win->dpy, _win->win, "APPLICATION");

 	_win->glc = glXCreateContext(_win->dpy, _win->vi, NULL, GL_TRUE);
 	glXMakeCurrent(_win->dpy, _win->win, _win->glc);
}


void SetLight( glm::vec3 _pos, glm::vec3 _colour, float _scale, int _index, RenderScene* _pRS)
{
	for(int i = 0; i < 4; i++)
	{
		_pRS[i].SetLight(_pos, _colour, _scale, _index);
	}
}

int main(int argc, char **argv)
{
	(void)argc; (void)argv;

    std::cerr << "Main() Entry" << std::endl;

	//Render Scene Controllers (One per context)
	RenderScene RS[4];

	glm::vec3 lefteye  		= glm::vec3( 3.25f	, 0.f, 0.f	);
	glm::vec3 righteye 		= glm::vec3( -3.25f	, 0.f, 0.f	);
	glm::vec3 convergePoint = glm::vec3( 0.f	, 0.f, 25.f	);

	Camera camera_left;
	camera_left.Init( lefteye, convergePoint, glm::vec3(0.f, 1.f, 0.f),	1.04, 1.0f, 0.01f, 200.f); 
	
	Camera camera_right;
	camera_right.Init( righteye, convergePoint, glm::vec3(0.f, 1.f, 0.f),	1.04, 1.0f, 0.01f, 200.f); 
	
	RS[0].Initialise();
	RS[1].Initialise();
	RS[2].Initialise();
	RS[3].Initialise();

	RS[0].SetID(1);
	RS[1].SetID(2);
	RS[2].SetID(3);
	RS[3].SetID(4);

	RS[0].SetCamera(camera_right);
	RS[1].SetCamera(camera_right);
	RS[2].SetCamera(camera_left);
	RS[3].SetCamera(camera_left);

	RS[0].SetLeftRight(false);
	RS[1].SetLeftRight(false);
	RS[2].SetLeftRight(true);
	RS[3].SetLeftRight(true);

	RS[0].SetNearFar(true);
	RS[1].SetNearFar(false);
	RS[2].SetNearFar(false);
	RS[3].SetNearFar(true);


	glm::mat4 ident(1.0f);
	RS[0].SetXYZtoRGBMat(ident);
	RS[1].SetXYZtoRGBMat(ident);
	RS[2].SetXYZtoRGBMat(ident);
	RS[3].SetXYZtoRGBMat(ident);
	
	for(int i = 0; i < 8; i++)
	{
		SetLight(glm::vec3( (i-4) * 45.0, 0.0, 0.0), glm::vec3(0.8, 0.8, 0.7),55.f, i, &RS[0]);
	}	

	//Windows
	WindowData win[4];

	InitialiseWindow(&win[0],1, &RS[0]);
	InitialiseWindow(&win[1],2, &RS[1]);
	InitialiseWindow(&win[2],3, &RS[2]);
	InitialiseWindow(&win[3],4, &RS[3]);

	//Glew initialisation
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  // Problem: glewInit failed, something is seriously wrong.
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));


	//Data loading for Scenes

	for(unsigned int i = 0; i < 4; i++)
	{
   	    glXMakeCurrent( win[i].dpy, win[i].win, win[i].glc );

		std::cerr << "About to load shaders..." << std::endl;

		std::string vert = "../Shaders/vert.glsl";
		std::string frag = "../Shaders/frag.glsl";
		std::string name = "TestShader";
		std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
		bool pass = RS[i].CreateShaderProgramObject( vert, frag, name );
		if(!pass) return -1;

		vert = "../Shaders/vert_tex.glsl";
		frag = "../Shaders/frag_tex.glsl";
		name = "TestShader_Tex";
		std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
		pass = RS[i].CreateShaderProgramObject( vert, frag, name );
		if(!pass) return -1;

		vert = "../Shaders/vert_MRT.glsl";
		frag = "../Shaders/frag_MRT.glsl";
		name = "TestShader_MRT";
		std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
		pass = RS[i].CreateShaderProgramObject( vert, frag, name );
		if(!pass) return -1;

		vert = "../Shaders/vert_persp_MRT.glsl";
		frag = "../Shaders/frag_persp_MRT.glsl";
		name = "TestShader_persp_MRT_near";
		std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
		pass = RS[i].CreateShaderProgramObject( vert, frag, name );
		if(!pass) return -1;

		vert = "../Shaders/vert_persp_MRT.glsl";
		frag = "../Shaders/frag_persp_MRT.glsl";
		name = "TestShader_persp_MRT_far";
		std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
		pass = RS[i].CreateShaderProgramObject( vert, frag, name );
		if(!pass) return -1;

		RS[i].LoadMaterial("../Materials/Test.Material", "Test");
		RS[i].LoadMaterial("../Materials/Box.Material", "Box");


		RS[i].InitialiseRenderObjects();
	}

	std::cerr << " Starting main loop\n";
	int activeInput = 0;

 	while(1) 
	{
	    set_conio_terminal_mode();
		//Check for input
		if(kbhit())
		{
			reset_terminal_mode();

			int ch = getch();
			//std::cerr << "Input " <<ch << "\n";
			if(ch == 3)
				return -1;
			else if (ch == 'p')
			{
				std::string configFile = "../Config/layout.data";
				ScreenWriteToFile(configFile, &RS[0]);
			}
			else if (ch == '0')
				activeInput = -1;
			else if (ch == '9')
				activeInput = -2;
			else if (ch == '8')
				activeInput = -3;
			else if (ch == '1')
				activeInput = 0;
			else if (ch == '2')
				activeInput = 1;
			else if (ch == '3')
				activeInput = 2;
			else if (ch == '4')
				activeInput = 3;
			
			if (activeInput >= 0)
			{
				keyPressCallback(ch, &RS[activeInput]);
			}
			else if(activeInput == -1)
			{
				keyPressCallback(ch, &RS[0]);
				keyPressCallback(ch, &RS[1]);
				keyPressCallback(ch, &RS[2]);
				keyPressCallback(ch, &RS[3]);
			}
			else if(activeInput == -2)
			{
				keyPressCallback(ch, &RS[0]);
				keyPressCallback(ch, &RS[1]);
			}
			else if(activeInput == -3)
			{
				keyPressCallback(ch, &RS[2]);
				keyPressCallback(ch, &RS[3]);
			}
		}	

		reset_terminal_mode();
		//////////////////////////

		for(unsigned int i = 0; i < 4; i++)
		{
	   	    glXMakeCurrent( win[i].dpy, win[i].win, win[i].glc );

			//Draw
			draw(&RS[i]);
		    glXSwapBuffers(win[i].dpy, win[i].win);
		}

		usleep(40000);
    }


	for(unsigned int i = 0; i < 4; i++)
	{
		//Shutdown
		glXMakeCurrent(win[i].dpy, None, NULL);
		glXDestroyContext(win[i].dpy, win[i].glc);
		XDestroyWindow(win[i].dpy, win[i].win);
		XCloseDisplay(win[i].dpy);
	}		

    return 0;
}














