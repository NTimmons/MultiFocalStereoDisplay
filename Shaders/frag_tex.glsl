#version 430

in vec2 finaltexcoord;

uniform sampler2D tex;

out vec4 outColor;

void main()
{ 
   outColor = (texture(tex, finaltexcoord)) + vec4(0.1, 0.1, 0.1, 1.0);
}
