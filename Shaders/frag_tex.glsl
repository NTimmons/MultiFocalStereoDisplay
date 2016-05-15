#version 430

uniform sampler2D 	tex;
uniform mat4 		XYZtoRGB;

in 		vec2		finaltexcoord;

out 	vec4 		outColor;

void main()
{ 
	vec4 col = (texture(tex, vec2(finaltexcoord.x, 1.0 - finaltexcoord.y)));
	col.w = 1.f;
	col = XYZtoRGB * col;
	outColor = col;
}
