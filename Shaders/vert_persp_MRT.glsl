#version 430

uniform mat4 mvp;
uniform mat4 m;
//uniform mat4 v;
//uniform mat4 p;

in vec4 pos;
in vec4 nor;
in vec2 uv;
in vec2 uv1;

out vec4 finalColour;
out vec4 finalNormal;
out vec4 finalPosition;

void main() 
{ 
	gl_Position =  mvp * vec4(pos.x, pos.y, pos.z, 1.0);

	finalColour   = vec4(1.0);
	finalNormal   = normalize(nor);
	finalPosition = vec4(pos.x, pos.y, pos.z, 1.0);
}
