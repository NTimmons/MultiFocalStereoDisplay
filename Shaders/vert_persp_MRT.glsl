#version 430

uniform mat4 mvp;

in vec4 pos;
in vec4 nor;
in vec2 uv;
in vec2 uv1;

out vec4 finalcolour;

void main() 
{ 
	gl_Position =  mvp * vec4(pos.x, pos.y, pos.z, 1.0);

	finalcolour = abs(nor);
}
