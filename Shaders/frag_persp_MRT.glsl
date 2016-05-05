#version 430

//uniform vec3 cameraPos;
//uniform vec3 cameraDir;

uniform vec4 lightPosArray[8];
uniform vec4 lightColScaleArray[8];

//GLfloat v[10] = {...};
//glUniform1fv(glGetUniformLocation(program, "lightArray"), 8, v);

in  vec4 finalColour;
in  vec4 finalNormal;
in  vec4 finalPosition;

out vec4 outColor0;


vec3 GetAmbient()
{
	return vec3(0.1, 0.1, 0.1);
}

void main()
{ 
   outColor0 = vec4(0.0);//lightPosArray[0] * finalColour * abs(vec4(finalNormal.x, finalNormal.y, finalNormal.z, max(1.0, 1.0 + finalPosition.x)    ));

	//Lighting
	vec3 light = vec3(0.0, 0.0, 0.0);
	vec3 ambient = GetAmbient();
	light = light + ambient;

	for(int i = 0; i < 1; i++)
	{
		vec3  pointCol 	= lightColScaleArray[i].xyz;
		float range    	= lightColScaleArray[i].w;
		vec3  pointPos 	= lightPosArray[i].xyz;

		vec3   gap  	= finalPosition.xyz - pointPos ;
		vec3   l  		= normalize(gap);
        float  dist 	= length(gap);

		float ldotn 	= abs( dot(l, finalNormal.xyz) );

		float intensity =  1.0 - min(1.0, (dist/range) );
	
		light = light + ( ldotn * vec3 (intensity) ) ;//(intensity * pointCol);
	}

	outColor0 = outColor0 + vec4(light, 1.0) ;
}
