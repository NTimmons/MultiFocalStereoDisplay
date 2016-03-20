#version 430

in vec4 pos;
in vec4 col;
in vec2 size;
in vec2 texcoord;

out vec4 finalcolour;

void main() 
{ 
	//Texcoord = texcoord;
	
	gl_Position = vec4(col.x, col.y, 0.0, 1.0) * 1000.0;

	finalcolour = col;
}
