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


// These vectors are interpolands which means that when you write per vertex information these
// values will be automatically interpolated across the fragments and passed to the fragment shader.
// So I calculate here the information needed for specular (N and L) and diffuse lighting
// and pass it to the fragment shader so i can perform per-fragment lighting.
varying vec3 N , L;

void main ( void )
{
	// Multiply object-space position by MVP matrix
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = ftransform ();


	// Calculate various vectors
	N = gl_NormalMatrix * gl_Normal;
	vec4 V = gl_ModelViewMatrix * gl_Vertex;
	L = gl_LightSource[0].position.xyz - V.xyz;

	// Calculate the diffuse color
	float NdotL = max ( 0.0 , dot (N,L) );
	vec4 diffuse = gl_FrontMaterial.diffuse * vec4 ( NdotL ) * 2.5;

	// Sum the diffuse and specular components and pass them ahead
	gl_FrontColor = diffuse;
//	gl_FrontColor = gl_Color;

	// Copy texture coordinates to fragment shader
//	gl_TexCoord[0] = gl_MultiTexCoord0;
}
