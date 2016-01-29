// Programmer: Mihalis Tsoukalos
// Date: Wednesday 04 June 2014
//
// A simple OpenGL program that draws a triangle.

#include "GL/freeglut.h"
#include "GL/gl.h"
#include <iostream>

#include <chrono>
#include <math.h>

#include <sys/time.h>


int gPosX = 4493;
int gPosY = 0;

int gSizeX = 1224;
int gSizeY = 918;
float ratio = (float)gSizeX/(float)gSizeY;


enum renderModes
{
  Alignment_E,
  Box_E
};

bool g_Mode = Alignment_E;
bool g_top = true;

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
GLfloat v[8][3];  /* Will be filled in with X,Y,Z vertexes. */



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

if(g_top)
{
  gluPerspective( /* field of view in degree */ 40.0,
    		  /* aspect ratio */ 1.0,
		  /* Z near */ 35.0, /* Z far */ 100.0);
}
else
{
  gluPerspective( /* field of view in degree */ 40.0,
    		  /* aspect ratio */ 1.0,
		  /* Z near */ 1.0, /* Z far */ 36.5);
}
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(	0.0, 0.0, 15.0,  /* eye is at (0,0,5) */
    		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
		0.0, 1.0, 0.);      /* up is in positive Y direction */

  /* Adjust cube position to be asthetic angle. */
  //glTranslatef	( 0.0, 0.0, -30.0);
  //glRotatef	( 60 , 1.0,  0.0, 0.0);
  //glRotatef	(-20 , 0.0,  0.0, 1.0);
}



void keyPressCallback(unsigned char key, int x, int y)
{
	//if(!g_top)
	{
	    std::cerr << "SCREEN KEYPRESS: " << key << std::endl;	    switch(key)
	    {

		case 'w':
			gPosY -= 5;
		break;
		case 's':
			gPosY += 5;
		break;
		case 'a':
			gPosX -= 5;
		break;
		case 'd':
			gPosX += 5;
		break;

		case 't':
			gSizeY += 5;
		break;
		case 'r':
			gSizeY -= 5;
		break;


		case 'q':
			g_Mode = Alignment_E;
			initAllignment();
		break;
		case 'e':
			g_Mode = Box_E;
			initBox();
		break;
	    }	

	    std::cerr << "Pos = X:" << gPosX << " Y:" << gPosY << ".\n";
	    std::cerr << "Size = X:" << gSizeY*ratio << " Y:" << gSizeY << ".\n";


      	    glutPositionWindow(gPosX, gPosY);

            glutReshapeWindow((int)((float)gSizeY*ratio),gSizeY);
	    glutPostRedisplay ();
	}
}


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
  int i;

  static float rot = 60.f;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(	0.0, 0.0, 15.0,  /* eye is at (0,0,5) */
    		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
		0.0, 1.0, 0.);      /* up is in positive Y direction */
  /* Adjust cube position to be asthetic angle. */
  glTranslatef	( 0.0, 0.0, -30.0);
  glRotatef	( 60.f * sin((double)GetTickCount()/600.0) , 1.0,  0.0, 0.0);
  glRotatef	(-20 , 0.0,  0.0, 1.0);

  rot+= 0.01f;

  for (i = 0; i < 6; i++) 
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
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); 
    glViewport(0, 0, gSizeX, gSizeY);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glCullFace(GL_NONE);

    glColor3f(1.0, 1.0, 1.0);
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

   if(g_top)
    {
        glBegin(GL_TRIANGLES);
		glColor3f(0.0, 0.5, 1.0);
                glVertex3f(-1.0, -1.0, 0);
                glVertex3f(-1.0,  1.0, 0);
                glVertex3f( 1.0,  1.0, 0);

		glColor3f(1.0, 0.50, 0.0);
                glVertex3f(-1.0,  -1.0, 0);
                glVertex3f( 1.0,  1.0, 0);
                glVertex3f(1.0, -1.0, 0);
        glEnd();
    }
    else
    {
        glBegin(GL_TRIANGLES);
		glColor3f(1.0, 0.5, 0.0);
                glVertex3f(-1.0, -1.0, 0);
                glVertex3f(-1.0,  1.0, 0);
                glVertex3f( 1.0,  1.0, 0);

		glColor3f(0.0, 0.5, 1.0);
                glVertex3f(-1.0,  -1.0, 0);
                glVertex3f( 1.0,  1.0, 0);
                glVertex3f(1.0, -1.0, 0);
        glEnd();
    }


    glFlush();
    glutSwapBuffers() ;
}



void draw()
{
	if(g_Mode == Alignment_E)
	{
		AlignmentMode();
std::cerr << "outputting alignment\n";
	}
	if(g_Mode == Box_E)
	{
		SpinningBoxMode();
	}
}

int main(int argc, char **argv)
{
    std::cerr << "Starting" << std::endl;


    if(argc > 1)
    { g_top = false;}

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(2048, 1536);
    if(g_top)
    {
	gSizeX = 2048;
	gSizeY = 1536;

	gPosX = 1920;
	gPosY = 0;

	glutInitWindowSize(gSizeX, gSizeY);
    	glutInitWindowPosition(gPosX, gPosY);
	glutCreateWindow("Alignment TOP");
    }
    else
    {
	gPosX = 4493;
	gPosY = 0;

	gSizeX = 1224;
	gSizeY = 918;

	glutInitWindowSize(gSizeX, gSizeY);
    	glutInitWindowPosition(gPosX, gPosY);
	glutCreateWindow("Alignment BOTTOM");
    }

    glutKeyboardFunc(keyPressCallback);
    glutDisplayFunc(draw);
	
    initAllignment();


    glutMainLoop();

    return 0;
}
