//////////////////////////////////////////////
// DIFFUSE lighting calculation				//
//////////////////////////////////////////////
// Cdiff = max ( N * L , 0 ) * Cmat * Cli	//
//											//
// N = Vertex normal						//
// L = Direction from vertex to light		//
// Cmat = material color					//
// Cli = light color						//
//////////////////////////////////////////////


//////////////////////////////////////////////
// SPECULAR lighting calculation			//
//////////////////////////////////////////////
// Cspec = max(N*H,0)^Sexp*Cmat*Cli			//
// N	= Vertex normal						//
// H	= H is the unit vector representing	//
//		  the direction halfway between the	//
//		  light vector and the view vector.	//
// Sexp = Specular exponent.				//
//////////////////////////////////////////////


void main ( void )
{
	// Multiply object-space position by MVP matrix
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = ftransform ();


	// Calculate various vectors
	vec3 N = normalize ( gl_NormalMatrix * gl_Normal );
	vec4 V = gl_ModelViewMatrix * gl_Vertex;
	vec3 L = normalize ( gl_LightSource[0].position - V.xyz );
	vec3 H = normalize(L + vec3(0.0, 0.0, 1.0));
	const float specularExp = 100.0;


	// Calculate the diffuse color
	float NdotL = max ( 0.0 , dot (N,L) );
	vec4 diffuse = gl_FrontMaterial.diffuse * vec4 ( NdotL ) * 2.5;


	// Calculate the specular color
	float NdotH = max ( 0.0 , dot (N,H) );
	vec4 specular = vec4(0.0);
	if ( NdotL > 0.0 )
		specular = vec4 ( pow (NdotH,specularExp) ) * gl_FrontMaterial.specular;

	// Sum the diffuse and specular components and pass them ahead
	gl_FrontColor = diffuse + specular;

	// Copy texture coordinates to fragment shader
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
