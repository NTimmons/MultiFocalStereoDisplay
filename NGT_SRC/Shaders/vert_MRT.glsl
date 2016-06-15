#version 430


uniform float testVal;
uniform vec4 positionOffset;

in vec4 pos;
in vec4 col;
in vec2 size;
in vec2 texcoord;

out vec4 finalcolour;

void main() 
{ 
	//Texcoord = texcoord;
	
	gl_Position =  vec4(testVal * pos.x, pos.y, 0.0, 1.0);

	finalcolour = col;
}
