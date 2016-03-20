#version 430

in vec4 finalcolour;

//in vec2 Texcoord;

//uniform sampler2D tex;

out vec4 outColor;

void main()
{ 
   outColor = vec4(finalcolour.x, finalcolour.y, 0.0, 1.0);		//(texture(tex, Texcoord) * vec4(Color, 1.0)) + vec4(0.1, 0.1, 0.1, 1.0);

}
