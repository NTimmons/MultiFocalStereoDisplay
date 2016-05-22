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

uniform float blendMode;

uniform sampler2D tex_0;
uniform sampler2D tex_1;
uniform sampler2D tex_2;

in  vec4 finalColour;
in  vec4 finalNormal;
in  vec4 finalTangent;
in  vec4 finalPosition;
in  vec4 finalUV;

out vec4 outColor0;

vec3 GetAmbient()
{
	return vec3(0.1, 0.1, 0.1);
}

vec4 BlendMethod( vec4 _col0, vec4 _col1, float _a)
{
	//0 - Linear
	//1 - Box
	//2 - Non-linear
	//3 - Near
	//4 - Far

	if (blendMode < 0.5) 		// 0
		return mix( _col0, _col1, _a);
	else if (blendMode < 1.5) 	//1
		return _a < 0.5 ? _col0 : _col1;
	else if (blendMode < 2.5) 	//2
	{
		float nonLinA = (1.0 / (1.0 + exp(  (( -(_a*2) + 1)*6)     )     ));	//1.0 / (1.0 + exp(-10.0 * (_a - 0.5) ) );
		return mix( _col0, _col1, nonLinA);
	}

}

void main()
{ 
	vec4 diffuseColour = diffuseUniform;

	vec4 diffuseTexColour 	= texture(tex_0, vec2(finalUV.x, finalUV.y) ) ;
	vec3 texDifCol 			= vec3(diffuseTexColour.x, diffuseTexColour.y, diffuseTexColour.z);

	float avg = (texDifCol.x + texDifCol.y + texDifCol.z) / 3.f;
	texDifCol = vec3(avg);
	texDifCol = normalize(texDifCol);


	vec4 specularTexColour 	= texture(tex_1, vec2(finalUV.x, finalUV.y) ) ;
	vec3 texSpecCol 		= specularTexColour.xyz;

	vec3 specCol 			= vec3(specularUniform.x, specularUniform.y, specularUniform.z);

	//Lighting
	vec3 light 				= vec3(0.0, 0.0, 0.0);
	vec3 ambient 			= GetAmbient();
	light 					= light + ambient;

	//Normal
	vec3 normal 			= normalize (texture(tex_2, vec2(finalUV.x, finalUV.y) ).xyz * 2.0 - 1.0);

	//Normal Mapping
	vec3 binormal 			= cross( finalNormal.xyz, finalTangent.xyz );	
	mat3 TBN 				= mat3 (finalTangent.xyz, binormal, finalNormal.xyz);

	for(int i = 0; i < 8; i++)
	{
		vec3  pointCol 	= lightColScaleArray[i].xyz;
		float range    	= lightColScaleArray[i].w;
		vec3  pointPos 	= lightPosArray[i].xyz;

		vec3  gap  		= finalPosition.xyz - pointPos ;
		float dist 		= length(gap);
		float intensity = 1.0 - min(1.0, (dist/range) );

		vec3  l  		= normalize ( TBN * normalize(gap) );
		vec3  n 		= finalNormal.xyz;
		vec3  r			= reflect(l,n);  

		//Diffuse Term
		float ldotn 	= 1.0 - max(0.0, ( dot(l, normal.xyz) ) );

		// calculate Specular Term:
		float Ispec = pow( max( dot( r, cameraDir ), 0.0 ), specPowUniform);
		Ispec 		= texSpecCol.r * clamp(Ispec, 0.0, 1.0); 

		light = light + ( ldotn * intensity * texDifCol * diffuseColour.xyz * pointCol) + (Ispec * intensity * specCol.xyz);
	}

	outColor0 = vec4(1.0, 1.0, 0.0, 1.0) ;

	float range  = length(cameraPos - finalPosition.xyz);
	if( near > 0.5 )
	{
		if(blendMode > 2.5 && blendMode < 3.5)
		{
			outColor0 = vec4(light.xyz, 1.0);
		}
		else if(blendMode > 3.5 && blendMode < 4.5)
		{
			outColor0 = vec4(0.0, 0.0, 0.0, 1.0);
		}
		else
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

				if ( blendMode < 5.5 && blendMode > 4.5) 	//5
				{
					//n2  - n1
					float dist 		= (1/blendClip) - (1/farClip);
					//ni - n1
					float blendstep = (1/range) - (1/farClip);
					float norm 		= clamp(blendstep/dist, 0.0, 1.0);

					outColor0 = mix( vec4(light.xyz, 1.0), vec4(0.0, 0.0, 0.0, 1.0), norm);
				}
				else
				{
					float dist = blendClip - farClip;
					float blendstep = range - farClip;
					float norm = clamp(blendstep/dist, 0.0, 1.0);
			
					outColor0 = BlendMethod(vec4(light.xyz, 1.0) , vec4(0.0), norm);
				}
			}	
		}	
	}
	else
	{
		if(blendMode > 3.5 && blendMode < 4.5)
		{
			outColor0 = vec4(light.xyz, 1.0);
		}
		else if(blendMode > 2.5 && blendMode < 3.5)
		{
			outColor0 = vec4(0.0, 0.0, 0.0, 1.0);
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
				if ( blendMode < 5.5 && blendMode > 4.5) 	//5
				{
					//n2  - n1
					float dist 		= (1/blendClip) - (1/nearClip);
					//ni - n1
					float blendstep = (1/range) - (1/nearClip);
					float norm 		= clamp(blendstep/dist, 0.0, 1.0);

					outColor0 = mix( vec4(0.0, 0.0, 0.0, 1.0), vec4(light.xyz, 1.0), norm);
				}
				else
				{
					float dist 		= blendClip - nearClip;
					float blendstep = range - nearClip;
					float norm 		= clamp(blendstep/dist, 0.0, 1.0);
			
					outColor0 = BlendMethod(vec4(0.0, 0.0, 0.0, 1.0), vec4(light.xyz, 1.0) , norm);
				}
			}
		}	
	}
}













