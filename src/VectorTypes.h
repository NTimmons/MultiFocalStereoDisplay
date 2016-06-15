#ifndef _INCL_VECTOR_TYPES
#define _INCL_VECTOR_TYPES


struct colour
{
	colour(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a){}
	colour(): r(0.f), g(0.f), b(0.f), a(0.f){}

	float R(){return r;} 
	float G(){return g;} 
	float B(){return b;} 
	float A(){return a;} 

	float r;
	float g;
	float b; 
	float a;
};

struct Position
{
	Position(float _x, float _y, float _z )
	{
		pos[0] = _x;
		pos[1] = _y;
		pos[2] = _z;
		pos[3] = 0.0f;
	}
	Position( ) 
	{
		pos[0] = 0.f;
		pos[1] = 0.f;
		pos[2] = 0.f;
		pos[3] = 0.f;
	}

	float* GetPtr(){return &pos[0];}
	float X(){return pos[0];}
	float Y(){return pos[1];}
	float Z(){return pos[2];}

	float pos[4];

};

struct size
{
	size(const float _w,const float _h): w(_w), h(_h){}
	size(): w(0.f), h(0.f){}

	float W(){return w;}
	float H(){return h;}

	float w;
	float h;
};

struct texCoord
{
	texCoord():u(0.f), v(0.f){}
	texCoord(float _u, float _v): u(_u), v(_v){}

	float u;
	float v;

};

#endif