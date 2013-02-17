//////////////////////////////////////////////
// DIFFUSE lighting calculation             //
//////////////////////////////////////////////
// Cdiff = max ( N * L , 0 ) * Cmat * Cli   //
//                                          //
// N = Vertex normal                        //
// L = Direction from vertex to light       //
// Cmat = material color                    //
// Cli = light color                        //
//////////////////////////////////////////////

//////////////////////////////////////////////
// SPECULAR lighting calculation            //
//////////////////////////////////////////////
// Cspec = max(N*H,0)^Sexp*Cmat*Cli         //
// N	= Vertex normal                     //
// H	= H is the unit vector representing //
//        the direction halfway between the //
//        light vector and the view vector. //
// Sexp = Specular exponent.                //
//////////////////////////////////////////////

#define textureWidth 128.0
#define textureHeight 128.0

//uniform float textureWidth;
//uniform float textureHeight;
#define texel_size_x 1.0 / textureWidth
#define texel_size_y 1.0 / textureHeight

vec4 texture2D_bilinear( sampler2D tex, vec2 uv )
{
	vec2 f;

	f.x	= fract( uv.x * textureWidth );
	f.y	= fract( uv.y * textureHeight );

	vec4 t00 = texture2D( tex, uv + vec2( 0.0, 0.0 ));
	vec4 t10 = texture2D( tex, uv + vec2( texel_size_x, 0.0 ));
	vec4 tA = mix( t00, t10, f.x);

	vec4 t01 = texture2D( tex, uv + vec2( 0.0, texel_size_y ) );
	vec4 t11 = texture2D( tex, uv + vec2( texel_size_x, texel_size_y ) );
	vec4 tB = mix( t01, t11, f.x );

	return mix( tA, tB, f.y );
}


uniform sampler2D colorMap;

varying vec3 N , L;

void main ( void )
{
	vec3 NN = normalize ( N );
	vec3 NL = normalize ( L );
	vec3 H = normalize ( -reflect ( NL , NN ) );

	// Calculate enviromental texture coordinates
	float m = 2.0 * sqrt( H.x*H.x + H.y*H.y + (H.z+1.0)*(H.z+1.0) );
	gl_TexCoord[0].s = H.x/m + 0.5;
	gl_TexCoord[0].t = H.y/m + 0.5;
	
	// Specular
//	float NdotH = max ( 0.0 , dot (NN,H) );
//	vec4 specular = vec4 ( pow (NdotH , gl_FrontMaterial.shininess) ) * gl_FrontLightProduct[0].specular;

	// Diffuse
	float NdotL = max ( 0.0 , dot (NN,NL) );
	vec4 diffuse = vec4 ( NdotL ) * gl_FrontLightProduct[0].diffuse;

	// Ambient
	vec4 ambient = gl_FrontLightProduct[0].ambient;

	//float gray = dot ( (diffuse+specular).rgb , vec3 ( 0.299 , 0.587 , 0.114 ) );
	
	// For black and white / gray
	//gl_FragColor = vec4 ( gray , gray , gray , 1.0 );
	
	// For sepia
	//gl_FragColor = vec4 ( gray * vec3 ( 1.2 , 1.0 , 0.8 ) , 1.0 );
	
	// For negative
	//gl_FragColor = 1.0 - (diffuse+specular);	




//	gl_FragColor = gl_FrontLightModelProduct.sceneColor + ambient + diffuse + specular;
	
//	gl_FragColor = texture2D( colorMap, gl_TexCoord[0].st )
//				+ gl_FrontLightModelProduct.sceneColor + ambient + diffuse;

	gl_FragColor = texture2D_bilinear ( colorMap, gl_TexCoord[0].st )
				+ gl_FrontLightModelProduct.sceneColor + ambient + diffuse;
}
