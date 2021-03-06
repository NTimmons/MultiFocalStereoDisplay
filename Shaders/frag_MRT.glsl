#version 430

in vec4 finalcolour;

out vec4 outColor0;
out vec4 outColor1;
out vec4 outColor2;
out vec4 outColor3;

void main()
{ 
   outColor0 = vec4(finalcolour.x, finalcolour.y, 1.0, 1.0);	//vec4(1.0, 1.0, 0.0, 1.0);//	//(texture(tex, Texcoord) * vec4(Color, 1.0)) + vec4(0.1, 0.1, 0.1, 1.0);
   outColor1 = vec4(1.0, 1.0, 1.0, 0.0) - outColor0;
   outColor2 = vec4(1.0, 0.0, 0.0, 1.0);
   outColor3 = vec4(0.0, 0.0, 1.0, 1.0);
}
