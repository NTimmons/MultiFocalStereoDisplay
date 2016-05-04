#version 430

in vec4 finalcolour;
out vec4 outColor0;

void main()
{ 
   outColor0 = vec4(finalcolour.x, finalcolour.y, finalcolour.z, 1.0);	//vec4(1.0, 1.0, 0.0, 1.0);//	//(texture(tex, Texcoord) * vec4(Color, 1.0)) + vec4(0.1, 0.1, 0.1, 1.0);
}
