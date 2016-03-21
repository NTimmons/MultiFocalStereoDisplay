#version 430


uniform float testVal;
uniform vec4 positionOffset;
uniform vec2 scale;

in vec4 pos;
in vec4 col;
in vec2 size;
in vec2 texcoord;

out vec2 finaltexcoord;

void main() 
{ 
	finaltexcoord = texcoord;
	
	gl_Position = vec4(testVal * pos.x, pos.y, 0.0, 1.0);
	gl_Position.x *= scale.x;
	gl_Position.y *= scale.y;
	gl_Position += positionOffset;
}
