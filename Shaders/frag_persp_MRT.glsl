#version 430

//Focal Control
uniform float nearClip;
uniform float farClip;
uniform float blendClip;
uniform float near;

uniform vec3 cameraPos;
uniform vec3 cameraDir;

uniform vec4 lightPosArray[8];
uniform vec4 lightColScaleArray[8];

uniform vec4 ambientUniform;
uniform vec4 specularUniform;
uniform vec4 diffuseUniform;
uniform vec4 emissiveUniform;

uniform float specPowUniform;

uniform sampler2D tex_0;
uniform sampler2D tex_1;
uniform sampler2D tex_2;

in  vec4 finalColour;
in  vec4 finalNormal;
in  vec4 finalPosition;
in  vec4 finalUV;

out vec4 outColor0;

vec3 GetAmbient()
{
	return vec3(0.1, 0.1, 0.1);
}

void main()
{ 
	vec4 diffuseColour = diffuseUniform;

	vec4 diffuseTexColour = texture(tex_0, vec2(finalUV.x, finalUV.y) ) ;
	vec3 texDifCol = vec3(diffuseTexColour.x, diffuseTexColour.y, diffuseTexColour.z);

	vec4 specularTexColour = texture(tex_1, vec2(finalUV.x, finalUV.y) ) ;
	vec3 texSpecCol = specularTexColour.xyz;

	vec3 specCol = vec3(specularUniform.x, specularUniform.y, specularUniform.z);

	//Lighting
	vec3 light = vec3(0.0, 0.0, 0.0);
	vec3 ambient = GetAmbient();
	light = light + ambient;

	for(int i = 0; i < 8; i++)
	{
		vec3  pointCol 	= lightColScaleArray[i].xyz;
		float range    	= lightColScaleArray[i].w;
		vec3  pointPos 	= lightPosArray[i].xyz;

		vec3  gap  		= finalPosition.xyz - pointPos ;
		float dist 		= length(gap);
		float intensity = 1.0 - min(1.0, (dist/range) );

		vec3  l  		= normalize(gap);
		vec3  n 		= finalNormal.xyz;

		vec3 r = (reflect(l,n));  

		//Diffuse Term
		float ldotn 	= abs( dot(l, finalNormal.xyz) );


		// calculate Specular Term:
		float Ispec = pow( max( dot( r, cameraDir ), 0.0 ), specPowUniform);
		Ispec = texSpecCol.r * clamp(Ispec, 0.0, 1.0); 


		light = light + ( ldotn * intensity * texDifCol * diffuseColour.xyz) + (Ispec * intensity * specCol.xyz);
	}


	outColor0 = vec4(1.0, 1.0, 0.0, 1.0) ;

	float range  = length(cameraPos - finalPosition.xyz);
	if( near > 0.5 )
	{
		//Near Plane
		if ( range > blendClip || range < nearClip)
		{
			//Not coloured by this.
			outColor0 = vec4(0.0);
		}
		else if ( range > nearClip && range < farClip	)	
		{
			//Regular output
			outColor0 = vec4(light.xyz, 1.0);
		}
		else if ( range > farClip && range < blendClip )
		{
			float dist = blendClip - farClip;
			float blendstep = range - farClip;
			float norm = blendstep/dist;
			
			outColor0 = mix( vec4(light.xyz, 1.0) , vec4(0.0), norm);
		}		
	}
	else
	{
		// Far Plane
		if ( range < nearClip )
			outColor0 = vec4(0.0);
		else
			outColor0 =  vec4(light.xyz, 1.0) ;


		if ( range > farClip || range < nearClip)
		{
			//Not coloured by this.
			outColor0 = vec4(0.0);
		}
		else if ( range > blendClip && range < farClip	)	
		{
			//Regular output
			outColor0 = vec4(light.xyz, 1.0);
		}
		else if ( range > nearClip && range < blendClip )
		{
			float dist = blendClip - nearClip;
			float blendstep = range - nearClip;
			float norm = blendstep/dist;
			
			outColor0 = mix( vec4(0.0), vec4(light.xyz, 1.0) , norm);
		}	







	}



	//outColor0 =  vec4(light.xyz, 1.0) ;
}













