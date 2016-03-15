// Programmer: Mihalis Tsoukalos
// Date: Wednesday 04 June 2014
//
// A simple OpenGL program that draws a triangle.


#include <GL/glew.h>
#include "GL/freeglut.h"

#include <iostream>

#include <chrono>
#include <math.h>

#include <sys/time.h>

#include "RenderScene.h"

GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {100.0, 100.0, 1.0, 0.0};  /* Infinite light location. */

GLfloat light_diffuse1[] = {1.0, 1.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_position1[] = {-100.0, -100.0, -100.0, 0.0};  /* Infinite light location. */

GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
  {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
  {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
  {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
  {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertices. */

RenderScene RS;

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

  /* Setup cube vertex data. */
  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -10;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  10;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -10;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  10;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] =  10;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = -10;

  /* Enable a single OpenGL light. */
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  /* Use depth buffering for hidden surface elimination. */
  glEnable(GL_DEPTH_TEST);

  /* Setup the view of the cube. */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective( /* field of view in degree */ 40.0,
    		  /* aspect ratio */ 1.0,
		  /* Z near */ 0.1, /* Z far */ 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(	0.0, 0.0, 15.0,  /* eye is at (0,0,5) */
    			0.0, 0.0, 0.0,      /* center is at (0,0,0) */
				0.0, 1.0, 0.);      /* up is in positive Y direction */
}

void keyPressCallback(unsigned char key, int x, int y)
{
    std::cerr << "SCREEN KEYPRESS: " << key << std::endl;	    
    switch(key)
    {
		case 'q':
			initAllignment();
		break;
		case 'e':
			initBox();
		break;
    }	

	//float ratio = (float)gSizeX/(float)gSizeY;

    //glutPositionWindow(gPosX, gPosY);
    //glutReshapeWindow((int)((float)gSizeY*ratio),gSizeY);
	//glutPostRedisplay ();
}

//I miss windows.
unsigned long GetTickCount()
{
	struct timeval tv;
	if( gettimeofday(&tv, NULL) != 0 )
    	return 0;

	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

void SpinningBoxMode()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static float rot = 60.f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	0.0, 0.0, 15.0,  	/* eye is at (0,0,5) */
    			0.0, 0.0, 0.0,      /* center is at (0,0,0) */
				0.0, 1.0, 0.);      /* up is in positive Y direction */
	/* Adjust cube position to be asthetic angle. */
	glTranslatef	( 0.0, 0.0, -30.0);
	glRotatef	( 60.f * sin((double)GetTickCount()/600.0) , 1.0,  0.0, 0.0);
	glRotatef	(-20 , 0.0,  0.0, 1.0);

	rot+= 0.01f;

	for (int i = 0; i < 6; i++) 
	{
		glBegin(GL_QUADS);
    	glNormal3fv(&n[i][0]);
    	glVertex3fv(&v[faces[i][0]][0]);
    	glVertex3fv(&v[faces[i][1]][0]);
    	glVertex3fv(&v[faces[i][2]][0]);
    	glVertex3fv(&v[faces[i][3]][0]);
    	glEnd();
  	}

	glFlush();
	glutSwapBuffers();  
	glutPostRedisplay ();
}



void AlignmentMode()
{
	RS.Render();
}

void draw()
{
	AlignmentMode();
}

int main(int argc, char **argv)
{
    std::cerr << "Starting" << std::endl;



	RS.Initialise();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(RS.m_SizeX, RS.m_SizeY);

	glutInitWindowSize(RS.m_SizeX, RS.m_SizeY);
    glutInitWindowPosition(RS.m_PosX, RS.m_PosY);
	glutCreateWindow("Multifocal Rendering.");

    glutKeyboardFunc(keyPressCallback);
    glutDisplayFunc(draw);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	
    initAllignment();

    std::cerr << "About to load shaders..." << std::endl;

	std::string vert = "../Shaders/vert.glsl";
	std::string frag = "../Shaders/frag.glsl";
    std::cerr << "Calling CreateShaderProgramObject..." << std::endl;
	ShaderProgram* shad = RS.CreateShaderProgramObject( vert, frag );

	printf("Shader ID: %d", shad->programID);













    glutMainLoop();

    return 0;
}














