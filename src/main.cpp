
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
	float fov = 2.09f;
	float interocularDist = 0.60f;
	float eyePos = interocularDist / 2.0f;

	float near = 0.1f;
	float far  = 15.0f;
	float aspect = 1.0f;

	glm::vec3 lefteye  		= glm::vec3(  eyePos	, 1.6f, 0.f	);
	glm::vec3 righteye 		= glm::vec3( -eyePos	, 1.6f, 0.f	);

	//Parallel
	glm::vec3 convergePoint = glm::vec3(  0.f		, 1.6f, 10000.f); //6.75f	);

	Camera camera_left;
	camera_left.Init( lefteye, convergePoint, glm::vec3(0.f, 1.f, 0.f),		fov, aspect, near, far); 
	
	Camera camera_right;
	camera_right.Init( righteye, convergePoint, glm::vec3(0.f, 1.f, 0.f),	fov, aspect, near, far); 
	
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

	glm::mat4 screen1Mat ( 0.011892436210176f,  -0.003075955081056f,   0.000118814145655f, 0.f,
						  -0.005536392048931f,   0.005868097848507f,  -0.000463192564228f, 0.f,
						  -0.001974460228384f,   0.000249230286101f,   0.002781496777127f, 0.f,
						   0.f	   			 ,   0.f 			   ,   0.f       		 , 1.f  );
	glm::vec3 screen1Gamma (2.2368f, 2.2064f, 2.2247f);
	glm::vec3 screen1Black (0.5074f, 0.4216f, 1.1063f);


	glm::mat4 screen2Mat ( 0.034475622628084f,  -0.007862267519549f,   0.000280997295198f, 0.f,
						  -0.015857954131203f,   0.014923870331527f,  -0.001109405678344f, 0.f,
						  -0.005787481462444f,   0.000723034313692f,   0.006189001240789f, 0.f,
						   0.f	   			 ,   0.f 			   ,   0.f       		 , 1.f  );
	glm::vec3 screen2Gamma (2.231899881371509f, 2.166339420405890f, 2.193620521358579f);
	glm::vec3 screen2Black (0.504462381467474f, 0.423266666666667f, 1.086920591710570f);


	glm::mat4 screen3Mat ( 0.032444761295140f,  -0.009547688833762f,   0.000595203468668f, 0.f,
						  -0.014657770181074f,   0.018025429443314f,  -0.001692196521873f, 0.f,
						  -0.005050388050673f,   0.000554559502155f,   0.008060495469209f, 0.f,
						   0.f	   			 ,   0.f 			   ,   0.f       		 , 1.f  );
	glm::vec3 screen3Gamma (2.443844772990262f, 2.447539148753688f, 2.443793612251334f);
	glm::vec3 screen3Black (0.442039104728199f, 0.432133333333333f, 0.811464976466370f);


	glm::mat4 screen4Mat ( 0.011542204338726f,  -0.003764915349369f,   0.000364183349641f, 0.f,
						  -0.005303723637745f,   0.007036682349081f,  -0.000946132633313f, 0.f,
						  -0.001761148442849f,   0.000161634970344f,   0.003448831543285f, 0.f,
						   0.f	   			 ,   0.f 			   ,   0.f       		 , 1.f  );

	glm::vec3 screen4Gamma (2.240571482171093f, 2.195605584878330f, 2.255393264342192f);
	glm::vec3 screen4Black (0.437321230188809f, 0.432466666666667f, 0.800857106179404f);


	//screen1Mat = glm::transpose(screen1Mat);
	//screen2Mat = glm::transpose(screen2Mat);
	//screen3Mat = glm::transpose(screen3Mat);
	//screen4Mat = glm::transpose(screen4Mat);

	RS[0].SetXYZtoRGBMat(screen1Mat);
	RS[1].SetXYZtoRGBMat(screen2Mat);
	RS[2].SetXYZtoRGBMat(screen3Mat);
	RS[3].SetXYZtoRGBMat(screen4Mat);

	RS[0].SetBlackLevel(screen1Black);
	RS[1].SetBlackLevel(screen2Black);
	RS[2].SetBlackLevel(screen3Black);
	RS[3].SetBlackLevel(screen4Black);

	RS[0].SetGamma(screen1Gamma);
	RS[1].SetGamma(screen2Gamma);
	RS[2].SetGamma(screen3Gamma);
	RS[3].SetGamma(screen4Gamma);

	
	for(int i = 0; i < 8; i++)
	{
		SetLight(glm::vec3( (i-4) * 25.0, 0.0, 0.0), glm::vec3(1.5f) * glm::vec3(150.0), 35.f, i, &RS[0]);
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
			else if (ch == 'x')
			{
				float sx = 2.f;
				float wd2     = near * tan(fov);
			    float ndfl    = near / far;
			    float left    = - aspect * wd2 - 0.5 * interocularDist * ndfl;
			    float right   =   aspect * wd2 - 0.5 * interocularDist * ndfl;


				camera_left.InitObliqueProj ( -sx, sx, -sx, sx, 1.f, 150.f);
				camera_right.InitObliqueProj( -sx, sx, -sx, sx, 1.f, 150.f);
				RS[0].SetCamera(camera_right);
				RS[1].SetCamera(camera_right);
				RS[2].SetCamera(camera_left);
				RS[3].SetCamera(camera_left);
			}
			else if (ch == 'c')
			{
				camera_left.InitProj(fov, aspect, near, far);
				camera_right.InitProj(fov, aspect, near, far);
				RS[0].SetCamera(camera_right);
				RS[1].SetCamera(camera_right);
				RS[2].SetCamera(camera_left);
				RS[3].SetCamera(camera_left);
			}
			
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














