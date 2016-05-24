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

	vec4 xyz 	= col;

	vec4 xyzBlk = xyz - vec4(blacklevel,1.0);
	vec4 rgb 	= XYZtoRGB * xyzBlk;
	rgb 		= pow(rgb, vec4(1/gamma, 1.0));

	outColor = rgb;
}
