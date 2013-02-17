//varying vec3 N, L;
attribute vec4 a_vertices;
attribute vec4 a_normals;

uniform mat4 u_modelviewMatrix;
uniform mat4 u_projectionMatrix;
//uniform mat4 u_mvpMatrix;

void main ( void )
{
	// Multiply object-space position by MVP matrix
//	gl_Position = a_vertices + a_normals;
	mat4 mvpMatrix = u_projectionMatrix * u_modelviewMatrix;
	gl_Position = mvpMatrix * a_vertices;
//	gl_Position = gl_ModelViewProjectionMatrix * a_vertices;
//	gl_Position = ftransform ();


	// Calculate various vectors
//	N = gl_NormalMatrix * gl_Normal;
//	vec4 V = gl_ModelViewMatrix * gl_Vertex;
//	L = gl_LightSource[0].position.xyz - V.xyz;

	// Calculate the diffuse color
//	float NdotL = max ( 0.0 , dot (N,L) );
//	vec4 diffuse = gl_FrontMaterial.diffuse * vec4 ( NdotL ) * 2.5;

	// Sum the diffuse and specular components and pass them ahead
	gl_FrontColor = a_normals;
}
