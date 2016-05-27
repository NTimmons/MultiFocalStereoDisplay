
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

#include <glm/gtx/string_cast.hpp> 

#include <math.h>

#include <random>

#include "TestController.h"

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
    if ((r = read(0, &c, sizeof(c))) < 0)
	{
        return r;
    }
	else 
	{
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

void SetOffAxisProjection(RenderScene* _RS, float _eyeOffSet, float _aspect, float _fov, float _near, float _far)
{
	//Reset Camera
	glm::vec3 lefteye  		= glm::vec3( -_eyeOffSet	, 0.f, 0.f	);
	glm::vec3 righteye 		= glm::vec3(  _eyeOffSet	, 0.f, 0.f	);
	//Parallel
	glm::vec3 convergePoint = glm::vec3(  0.f		, 0.f, 100000.f);

	Camera camera_left;
	Camera camera_right;

	camera_left.Init( lefteye, convergePoint, glm::vec3(0.f, 1.f, 0.f),		_fov, _aspect, _near, _far); 
	camera_right.Init( righteye, convergePoint, glm::vec3(0.f, 1.f, 0.f),	_fov, _aspect, _near, _far); 

	float wd2       = 1.0f * tan(_fov/4);
//	float ndfl      = _near / _far;

	float bottom    = - wd2 - (_eyeOffSet * 0.5f);
	float top   	=   wd2 + (_eyeOffSet * 0.5f);

	//right eye
	float righteye_left 	= -(wd2 * _aspect) - _eyeOffSet;// *  _aspect;
	float righteye_right 	=  (wd2 * _aspect);// * _aspect;

/*
	float width 			= righteye_right - righteye_left;
	float height 			= top - bottom;


	std::cerr << "left: " << righteye_left << "\n";
	std::cerr << "right: " << righteye_right << "\n";
	std::cerr << "top: " << top << "\n";
	std::cerr << "bottom: " << top << "\n";
	std::cerr << "width: " << width << "\n";
	std::cerr << "height: " << height << "\n";
	std::cerr << "w/h: " << width/height << "\n";
	std::cerr << "aspect: " << _aspect << "\n";
	std::cerr << "eyeOffset: " << _eyeOffSet << "\n";
*/

	float lefteye_left  = -(wd2 * _aspect);
	float lefteye_right =  (wd2 * _aspect) + _eyeOffSet * _aspect;

	camera_left.InitOffAxisProj ( lefteye_left , lefteye_right, 	bottom, top, 1.f, 8.1f);
	camera_right.InitOffAxisProj( righteye_left, righteye_right,	bottom, top, 1.f, 8.1f);

	_RS[0].SetCamera(camera_right);
	_RS[1].SetCamera(camera_right);
	_RS[2].SetCamera(camera_left);
	_RS[3].SetCamera(camera_left);
}

void SetToeInProjection(RenderScene* _RS, float _eyeOffSet, float _aspect, float _fov, float _near, float _far)
{
	//Toe-in
	//Reset Camera
	glm::vec3 lefteye  		= glm::vec3( -_eyeOffSet, 0.f, 0.f	);
	glm::vec3 righteye 		= glm::vec3(  _eyeOffSet, 0.f, 0.f	);

	//Converging
	glm::vec3 convergePoint = glm::vec3(  0.f		, 0.f, 5.4f);

	Camera camera_left;
	Camera camera_right;

	camera_left.Init		( lefteye	, convergePoint, glm::vec3(0.f, 1.f, 0.f),	_fov, _aspect, _near, _far); 
	camera_right.Init		( righteye	, convergePoint, glm::vec3(0.f, 1.f, 0.f),	_fov, _aspect, _near, _far); 
	camera_left.InitProj	( _fov, _aspect, _near, _far);
	camera_right.InitProj	( _fov, _aspect, _near, _far);

	_RS[0].SetCamera(camera_right);
	_RS[1].SetCamera(camera_right);
	_RS[2].SetCamera(camera_left);
	_RS[3].SetCamera(camera_left);
}


void SetObliqueProjection( RenderScene* _RS, float _eyeOffSet, float _screenAspect, float _fov )
{
//Oblique
	//Reset Camera
	glm::vec3 lefteye  		= glm::vec3(  0.0f	, 0.f, 0.f	);
	glm::vec3 righteye 		= glm::vec3(  0.0f	, 0.f, 0.f	);
	glm::vec3 convergePoint = glm::vec3(  0.f	, 0.f, -1.0f	);

	float sz 				= 5.4f * tan(_fov/4);
	float sx				= _screenAspect * sz;				

	glm::mat4 				mlt(1.0f);
	mlt[0][0] 				= 1.f/sx;
	mlt[1][1] 				= 1.f/sz;
	mlt[3][3] 				= 1.f;

	float eyeDepth 			= -5.4f;

	float near_ 			= 8.1f;
	float far_  			= 5.4f;

	glm::vec3 screenNormal 	= glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 screenPos 	= glm::vec3(0.f, 0.f, 0.f);

	float pr22 				= -1.f/(far_-near_);
	float pr23 				= near_/(far_-near_);

	//Set left eye
	{
		glm::vec3 eye	  = glm::vec3(-_eyeOffSet, 0.f, eyeDepth);
		glm::vec4 nn 	  = glm::vec4( screenNormal, -glm::dot(screenPos, screenNormal) );
		glm::vec4 eyePosW = glm::vec4(eye, 1.0f);

		glm::mat4 PR      = glm::outerProduct(eyePosW, nn  ) + ( glm::mat4(1.0f) * glm::dot(screenNormal, screenPos - eye) );
	
		PR[2][2] 		  = pr22;
		PR[3][2] 		  = pr23;
		PR 				  = PR * mlt;

		std::cerr << "PR" << glm::to_string(PR) << "\n";

		Camera camera_left;
		camera_left.Init( lefteye, convergePoint, glm::vec3(0.f, 1.f, 0.f),	_fov, _screenAspect, near_, far_);
		camera_left.SetProjectionMatrix(PR);
		_RS[2].SetCamera(camera_left);
		_RS[3].SetCamera(camera_left); 
	}

	//Set right eye
	{
		glm::vec3 eye	  = glm::vec3(_eyeOffSet, 0.f, eyeDepth); //
		glm::vec4 nn 	  = glm::vec4( screenNormal, -glm::dot(screenPos, screenNormal) );
		glm::vec4 eyePosW = glm::vec4(eye, 1.0f);

		glm::mat4 PR 	  = glm::outerProduct(eyePosW, nn  ) + ( glm::mat4(1.0f) * glm::dot(screenNormal, screenPos - eye) );
		
		PR[2][2] 		  = pr22;
		PR[3][2] 		  = pr23;
		PR 				  = PR * mlt;
	
		Camera camera_right;
		camera_right.Init( righteye, convergePoint, glm::vec3(0.f, 1.f, 0.f),	_fov, _screenAspect, near_, far_); 
		camera_right.SetProjectionMatrix(PR);
		_RS[0].SetCamera(camera_right);
		_RS[1].SetCamera(camera_right);
	}
}


void SetLight( glm::vec3 _pos, glm::vec3 _colour, float _scale, int _index, RenderScene* _pRS)
{
	for(int i = 0; i < 4; i++)
	{
		_pRS[i].SetLight(_pos, _colour, _scale, _index);
	}
}


void AddComparison ( TestController* _TC, StereoModes _stereoA, StereoModes _stereoB, EyeSeperationModes _eyeSepA, EyeSeperationModes _eyeSepB, 
					 SceneModes _scene, 
					 BlendModes _blendA, BlendModes _blendB, ProjectionModes _projA, ProjectionModes _projB, float _displayTime)
{
	ContrEntry calib;
	calib.InitCalibration();

	ContrEntry twoSecondTimerA;
	twoSecondTimerA.InitTimer	(_scene, _stereoA, _eyeSepA, _blendA, _projA, _displayTime);

	ContrEntry twoSecondTimerB;
	twoSecondTimerB.InitTimer	(_scene, _stereoB, _eyeSepB, _blendB, _projB, _displayTime);

	ContrEntry input;
	input.InitInput				(_scene, _stereoA, _eyeSepA, _blendA, _projA, _scene, _stereoB, _eyeSepB, _blendB, _projB);

	_TC->Add(calib);
	_TC->Add(twoSecondTimerA);
	_TC->Add(calib);
	_TC->Add(twoSecondTimerB);
	_TC->Add(input);
}

void AddDepthTest(TestController* _TC, BlendModes _blendA, StereoModes _stereoA, EyeSeperationModes _eyeSepA, float _depth, float _minOffset, float _maxOffset, float _minScale, float _maxScale)
{
	std::vector<int> ranges;

	static unsigned int seed = 123;
	seed++;

    std::random_device rd;
    std::mt19937 gen(seed);
    std::uniform_real_distribution<> dis_scale(_minScale, _maxScale);
    std::uniform_real_distribution<> dis_depthOffset(_minOffset/2.f, _maxOffset/2.f);
	std::uniform_int_distribution<> dis_signbit(0,1);
     
	float scaleA 	= dis_scale(gen);
	float scaleB 	= dis_scale(gen);
	float scaleC 	= dis_scale(gen);

	float offsetA 	= _depth + (dis_depthOffset(gen) * ( 1 ));
	float offsetB 	= _depth + (dis_depthOffset(gen) * ( -1));
	float offsetC 	= _depth + (dis_depthOffset(gen) * ( 1 ));

	offsetA = glm::max(2.5f, offsetA);
	offsetB = glm::max(2.5f, offsetB);
	offsetC = glm::max(2.5f, offsetC);

    std::uniform_int_distribution<> dis_order(0, 2);
	int order = dis_order(gen);
	int a,b,c;
	float depth[3];

	std::cerr << "Order:" << order << "\n";

	if(order == 0)
	{
		a = 0;
		b = 1;
		c = 2;
	}	
	else if( order == 1)
	{
		a = 2;
		b = 0;
		c = 1;
	}
	else if( order == 2)
	{
		a = 1;
		b = 2;
		c = 0;
	}
	else
	{
		std::cerr << "invalid depth order \n";
	}

	float smallest 	= glm::min( offsetA, glm::min(offsetB, offsetC) );
	float largest 	= glm::max( offsetA, glm::max(offsetB, offsetC) );
	float mid 		= (offsetA + offsetB + offsetC) - (largest + smallest);

	depth[a] = smallest;
	depth[b] = mid;
	depth[c] = largest;

	ContrEntry dist;
	dist.InitDistanceInput( eScene_Distance, _blendA, _stereoA, _eyeSepA, depth[0], depth[1], depth[2], scaleA, scaleB, scaleB, a);
	
	ContrEntry calib;
	calib.InitCalibration();

	_TC->Add(calib);
	_TC->Add(dist);
}

int main(int argc, char **argv)
{
	(void)argc; (void)argv;
    std::cerr << "Main() Entry" << std::endl;

	//Render Scene Controllers (One per context)
	RenderScene RS[4];
	float fov 				= 0.7f;
	float interocularDist 	= 0.6f;
	float eyePos 			= interocularDist / 2.0f;

	float near 				= 0.1f;//2.5f;
	float far  				= 15.0f;

	float sizeX 		= 2048.f;
	float sizeY 		= 1536.f;
	float screenAspect 	= sizeX/sizeY;
	float aspect 		= sizeX/sizeY;

	glm::vec3 lefteye  		= glm::vec3(  eyePos	, 0.0f, 0.f	);
	glm::vec3 righteye 		= glm::vec3( -eyePos	, 0.0f, 0.f	);

	//Parallel
	glm::vec3 convergePoint = glm::vec3(  0.f		, 0.f, 10000.f); //6.75f	);

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
		SetLight(glm::vec3( (i-4) * 20.0, 0.0, 0.0), glm::vec3(40.0), 15.f, i, &RS[0]);
	}	

	//SetLight(glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(40.0), 15.f, 0, &RS[0]);
	SetLight(glm::vec3( -0.0f,  1.f, 1.0f), glm::vec3(20.f, 20.f, 20.f), 15.f, 0, &RS[0]);
	SetLight(glm::vec3( -0.0f,  0.f, 1.0f), glm::vec3(0.f, 20.f, 20.f), 15.f, 0, &RS[0]);

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

		vert = "../Shaders/vert_tex.glsl";
		frag = "../Shaders/frag_tex_calibration.glsl";
		name = "TestShader_Tex_Calibration";
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
		RS[i].LoadMaterial("../Materials/Red.Material", "Red");
		RS[i].LoadMaterial("../Materials/Green.Material", "Green");
		RS[i].LoadMaterial("../Materials/Blue.Material", "Blue");

		RS[i].InitialiseRenderObjects();
	}

	TestController TC;
	
	int numRepeatsForComfort = 5;
	int order = 0;

	AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static	, eBlend_Projective	, eBlend_Near	, eProj_Oblique, eProj_Oblique, 500000.f);
	
	float timeRot = 6000.f;
	float timeTra = 6000.f;
	float timeSta = 10000.f;

	for(int i = 0; i < numRepeatsForComfort; i++)
	{
		if(order == 0)
		{
			//Testing the blend modes for depth.
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Projective , eBlend_Near			, eProj_Oblique, eProj_Oblique, timeTra);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Near	    , eBlend_Far			, eProj_Oblique, eProj_Oblique, timeTra);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Far		, eBlend_Projective		, eProj_Oblique, eProj_Oblique, timeTra);

			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Rotation	, eBlend_Projective	, eBlend_Near			, eProj_Oblique, eProj_Oblique, timeRot);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Rotation	, eBlend_Near	    , eBlend_Far			, eProj_Oblique, eProj_Oblique, timeRot);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Rotation	, eBlend_Far		, eBlend_Projective		, eProj_Oblique, eProj_Oblique, timeRot);

			//AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static		, eBlend_Projective , eBlend_Near			, eProj_Oblique, eProj_Oblique, timeSta);
			//AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static		, eBlend_Near	    , eBlend_Far			, eProj_Oblique, eProj_Oblique, timeSta);
			//AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static		, eBlend_Far		, eBlend_Projective		, eProj_Oblique, eProj_Oblique, timeSta);
		}
		else if (order == 1)
		{
			//AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static		, eBlend_Projective , eBlend_Near			, eProj_Oblique, eProj_Oblique, timeSta);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Rotation	, eBlend_Projective	, eBlend_Near			, eProj_Oblique, eProj_Oblique, timeRot);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Far		, eBlend_Projective		, eProj_Oblique, eProj_Oblique, timeTra);

			//AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static		, eBlend_Near	    , eBlend_Far			, eProj_Oblique, eProj_Oblique, timeSta);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Rotation	, eBlend_Far		, eBlend_Projective		, eProj_Oblique, eProj_Oblique, timeRot);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Near	    , eBlend_Far			, eProj_Oblique, eProj_Oblique, timeTra);		

			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Projective , eBlend_Near			, eProj_Oblique, eProj_Oblique, timeTra);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Rotation	, eBlend_Near	    , eBlend_Far			, eProj_Oblique, eProj_Oblique, timeRot);
			//AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static		, eBlend_Far		, eBlend_Projective		, eProj_Oblique, eProj_Oblique, timeSta);
		}
		else if (order == 2)
		{
			//AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static		, eBlend_Near	    , eBlend_Far			, eProj_Oblique, eProj_Oblique, timeSta);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Rotation	, eBlend_Far		, eBlend_Projective		, eProj_Oblique, eProj_Oblique, timeRot);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Near	    , eBlend_Far			, eProj_Oblique, eProj_Oblique, timeTra);	

			//AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static		, eBlend_Projective , eBlend_Near			, eProj_Oblique, eProj_Oblique, timeSta);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Rotation	, eBlend_Projective	, eBlend_Near			, eProj_Oblique, eProj_Oblique, timeRot);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Far		, eBlend_Projective		, eProj_Oblique, eProj_Oblique, timeTra);

			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Projective , eBlend_Near			, eProj_Oblique, eProj_Oblique, timeTra);
			AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Rotation	, eBlend_Near	    , eBlend_Far			, eProj_Oblique, eProj_Oblique, timeRot);
			//AddComparison( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Static		, eBlend_Far		, eBlend_Projective		, eProj_Oblique, eProj_Oblique, timeSta);
		}
		
		order = (order++) % 2;

	}

	float  depths[5] 	= { 3.6, 5.4	, 6.48	, 8.1	, 9.257 	};
	float  minOffset[6] = { 5.0f , 2.5f, 1.25f	, 0.625f, 0.325f, 0.15125f	};
	float  maxOffset[6] = { 8.1f , 5.0f, 2.5f	, 1.25f	, 0.625f, 0.325f	};


	//Stereo depth testing
	for(int i = 0; i < 5; i++)
	{
		for(int os = 0; os < 6; os++)
		{
				// float _depth, float _minOffset, float _maxOffset, float _minScale, float _maxScale)
				AddDepthTest ( &TC, eBlend_Near			, eStereo_Both, eEye_Sep, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Projective	, eStereo_Both, eEye_Sep, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Near			, eStereo_Both, eEye_Sep, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Far			, eStereo_Both, eEye_Sep, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Near			, eStereo_Both, eEye_Sep, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Far			, eStereo_Both, eEye_Sep, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Projective	, eStereo_Both, eEye_Sep, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Projective	, eStereo_Both, eEye_Sep, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Far			, eStereo_Both, eEye_Sep, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);

		}
	}

	// Mono depth testing
	for(int i = 0; i < 5; i++)
	{
		for(int os = 0; os < 6; os++)
		{
				AddDepthTest ( &TC, eBlend_Far			, eStereo_Both, eEye_None, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Projective	, eStereo_Both, eEye_None, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Far			, eStereo_Both, eEye_None, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Near			, eStereo_Both, eEye_None, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Projective	, eStereo_Both, eEye_None, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Projective	, eStereo_Both, eEye_None, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Far			, eStereo_Both, eEye_None, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Near			, eStereo_Both, eEye_None, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
				AddDepthTest ( &TC, eBlend_Near			, eStereo_Both, eEye_None, depths[i], minOffset[os], maxOffset[os], 0.7f, 1.3f);
		}
	}

	std::string name = "";
	std::cout << "Enter Name: ";
	std::cin >> name;
	std::cout << "Name is " << name << "\n";

	TC.Start(name);

	SetObliqueProjection( &RS[0],  eyePos, screenAspect, fov );

	std::cerr << " Starting main loop\n";
	int activeInput = -1;

    typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;    	
	auto t0 = Time::now();

 	while(1) 
	{
	    set_conio_terminal_mode();

		int ch = -1;

		//Check for input
		if(kbhit())
		{
			reset_terminal_mode();

			ch = getch();
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
				//Off-axis
				SetOffAxisProjection(&RS[0],  eyePos, aspect, fov, near, far);
			}
			else if (ch == 'c')
			{
				SetToeInProjection(&RS[0],  eyePos, aspect, fov, near, far);
			}
			else if (ch == 'v')
			{
				SetObliqueProjection( &RS[0],  eyePos, screenAspect, fov );
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

		int tcInput = -1;

		if(ch == ',')
			tcInput = 0;
		else if(ch == '.')
			tcInput = 1;
		else if(ch == '/')
			tcInput = 2;

		ms nw = std::chrono::duration_cast<ms>(Time::now() - t0);
		float timenow = nw.count();

		TC.Update(timenow,tcInput); 

		ContrEntry curActive = TC.Get();
		for(int i = 0; i < 4; i++)
		{
			RS[i].SetBlendMode	(curActive.blendModeA);
			RS[i].SetStereoMode	(curActive.stereoModeA);

			if(curActive.inputMode == eMode_INPUT)
			{
				RS[i].SetScene		( 5	);	
				RS[i].SetStereoMode		(eStereo_Both	);
			}
			else if(curActive.inputMode == eMode_CALIBRATE)
			{
				RS[i].SetScene		( 4	);	
				RS[i].SetStereoMode		(eStereo_Both	);
			}
			else if(curActive.inputMode == eMode_INPUT_DEPTH)
			{
				RS[i].SetScene		( 1	);	
				RS[i].SetDepthComparison( curActive.depthA, curActive.depthB, curActive.depthC, curActive.scaleA, curActive.scaleB, curActive.scaleC);				
			}
			else
				RS[i].SetScene		(curActive.renderModeA	);	

			
			float seperation = curActive.eyeSeperationA == eEye_Sep ? eyePos : 0.f;


			if(curActive.projModeA == 0)
				SetOffAxisProjection(&RS[0],  seperation, aspect, fov, near, far);
			else if(curActive.projModeA == 1)
				SetToeInProjection(&RS[0],  seperation, aspect, fov, near, far);
			else if(curActive.projModeA == 2)
				SetObliqueProjection( &RS[0],  seperation, screenAspect, fov );




		}
			
		//////////////////////////

		for(unsigned int i = 0; i < 4; i++)
		{
	   	    glXMakeCurrent( win[i].dpy, win[i].win, win[i].glc );

			//Draw
			draw(&RS[i]);
		    glXSwapBuffers(win[i].dpy, win[i].win);
		}

		usleep(10000);
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














