
#include <GL/glew.h>
#include "GL/freeglut.h"

#include <chrono>
#include <math.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

#include <iostream>
#include "RenderScene.h"



#ifndef _WIN32
//http://stackoverflow.com/questions/31579243/segmentation-fault-before-main-when-using-glut-and-stdstring
#include <pthread.h> 


void* simpleFunc(void*) { return NULL; } 
void forcePThreadLink() { pthread_t t1; pthread_create(&t1, NULL, &simpleFunc, NULL); }

#endif

GLint windowL = 0;
GLint windowR = 0;

GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};  // Red diffuse light.
GLfloat light_position[] = {100.0, 100.0, 1.0, 0.0};  // Infinite light location. 

GLfloat light_diffuse1[] = {1.0, 1.0, 0.0, 1.0};  // Red diffuse light. 
GLfloat light_position1[] = {-100.0, -100.0, -100.0, 0.0};  // Infinite light location.

GLfloat n[6][3] = {  // Normals for the 6 faces of a cube. 
  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
  {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
GLint faces[6][4] = {  // Vertex indices for the 6 faces of a cube.
  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
GLfloat v[8][3];  // Will be filled in with X,Y,Z vertices. 


RenderScene RS_L;
RenderScene RS_R;

void initAllignment()
{
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
}


void initBox(void)
{
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  // Setup cube vertex data. 
  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -10;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  10;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -10;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  10;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] =  10;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = -10;

  // Enable a single OpenGL light. 
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  // Use depth buffering for hidden surface elimination. 
  glEnable(GL_DEPTH_TEST);

  // Setup the view of the cube. 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective( 40.0,//field of view in degree 
    		  1.0, // aspect ratio 
		   0.1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(	0.0, 0.0, 15.0,  // eye is at (0,0,5) 
    			0.0, 0.0, 0.0,      // center is at (0,0,0) 
				0.0, 1.0, 0.);      // up is in positive Y direction 
}

void keyPressCallback(unsigned char key, int x, int y)
{

	(void)x;
	(void)y;

	RS_L.HandleInput(key);
	RS_R.HandleInput(key);

    std::cerr << "(" << key << ")";	    
    switch(key)
    {
		case 'q':
			//initAllignment();
		break;
		case 'e':
			//initBox();
		break;
    }	

	glutPostRedisplay ();
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

void drawLeft()
{
	glutSetWindow(windowL);
	//std::cout << "Drawing Left.\n";
	RS_L.Render();
	//glutPostRedisplay();
}

void drawRight()
{
	glutSetWindow(windowR);
	//std::cout << "Drawing Right.\n";
	RS_R.Render();
	//glutPostRedisplay();
}


void timerCB(int millisec)
{
	glutTimerFunc(millisec, timerCB, millisec);


	glutSetWindow(windowL);
	glutPostRedisplay();


	glutSetWindow(windowR);
	glutPostRedisplay();

}

int main(int argc, char **argv)
{
    std::cerr << "Main() Entry" << std::endl;

	RS_L.Initialise();
	RS_R.Initialise();

	putenv("DISPLAY=:0.0"); //display 1

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE);
    glutInitWindowSize(RS_L.m_SizeX, RS_L.m_SizeY);
    glutInitWindowPosition(RS_L.m_PosX, RS_L.m_PosY);



	windowL = glutCreateWindow("Multifocal Rendering L.");
    glutKeyboardFunc(keyPressCallback);
    glutDisplayFunc(drawLeft);

	glutTimerFunc(25, timerCB, 25); // draw every 50 ms

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  // Problem: glewInit failed, something is seriously wrong.
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    initAllignment();

    std::cerr << "About to load shaders..." << std::endl;

	std::string vert = "../Shaders/vert.glsl";
	std::string frag = "../Shaders/frag.glsl";
	std::string name = "TestShader";
    std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
	bool pass = RS_L.CreateShaderProgramObject( vert, frag, name );

	vert = "../Shaders/vert_tex.glsl";
	frag = "../Shaders/frag_tex.glsl";
	name = "TestShader_Tex";
    std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
	pass = RS_L.CreateShaderProgramObject( vert, frag, name );


	vert = "../Shaders/vert_MRT.glsl";
	frag = "../Shaders/frag_MRT.glsl";
	name = "TestShader_MRT";
    std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
	pass = RS_L.CreateShaderProgramObject( vert, frag, name );

	vert = "../Shaders/vert_persp_MRT.glsl";
	frag = "../Shaders/frag_persp_MRT.glsl";
	name = "TestShader_persp_MRT";
    std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
	pass = RS_L.CreateShaderProgramObject( vert, frag, name );
	(void)pass;


	RS_L.InitialiseRenderObjects();

	//--
	//--
	//--
	//--
	//--
	//--
	//Right

	putenv("DISPLAY=:0.1");
	windowR = glutCreateWindow("Multifocal Rendering R.");
    glutKeyboardFunc(keyPressCallback);
    glutDisplayFunc(drawRight);

    std::cerr << "About to load shaders..." << std::endl;

	vert = "../Shaders/vert.glsl";
	frag = "../Shaders/frag.glsl";
	name = "TestShader";
    std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
	pass = RS_R.CreateShaderProgramObject( vert, frag, name );

	vert = "../Shaders/vert_tex.glsl";
	frag = "../Shaders/frag_tex.glsl";
	name = "TestShader_Tex";
    std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
	pass = RS_R.CreateShaderProgramObject( vert, frag, name );


	vert = "../Shaders/vert_MRT.glsl";
	frag = "../Shaders/frag_MRT.glsl";
	name = "TestShader_MRT";
    std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
	pass = RS_R.CreateShaderProgramObject( vert, frag, name );

	vert = "../Shaders/vert_persp_MRT.glsl";
	frag = "../Shaders/frag_persp_MRT.glsl";
	name = "TestShader_persp_MRT";
    std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
	pass = RS_R.CreateShaderProgramObject( vert, frag, name );
	(void)pass;

	RS_R.InitialiseRenderObjects();


	//glutFullScreen(); 
    glutMainLoop();

	

    return 0;
}














