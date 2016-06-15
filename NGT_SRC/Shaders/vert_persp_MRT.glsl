#version 430

uniform mat4 mvp;
uniform mat4 imv;
uniform mat4 m;
uniform mat4 r;
//uniform mat4 p;

in vec4 pos;
in vec4 nor;
in vec2 uv;
in vec2 uv1;
in vec4 tan;

out vec4 finalColour;
out vec4 finalNormal;
out vec4 finalTangent;
out vec4 finalPosition;
out vec4 finalUV;

void main() 
{ 
	gl_Position =  mvp * vec4(pos.xyz, 1.0);

	finalColour   = vec4(1.0);
	finalNormal   = normalize( r * (  vec4(nor.xyz, 0.0) ) );
	finalTangent  = normalize( r * (  vec4(tan.xyz, 0.0) ) );
	finalPosition = m * (  vec4(pos.xyz, 1.0) );
	finalUV		  = vec4(uv,uv1);

	gl_Position.z = (gl_Position.z/2.0);
}
