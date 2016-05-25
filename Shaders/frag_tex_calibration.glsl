#version 430

uniform sampler2D 	tex;
uniform mat4 		XYZtoRGB;
uniform vec3        blacklevel;
uniform vec3		gamma;

in 		vec2		finaltexcoord;

out 	vec4 		outColor;

void main()
{ 
	vec4 col 	= (texture(tex, vec2(finaltexcoord.x, 1.0 - finaltexcoord.y)));
	col.w 		= 1.f;
	
	outColor = col * 400.f;
}
