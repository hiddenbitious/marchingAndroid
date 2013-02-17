
//varying vec3 N , L;

void main (void)
{
//	float intensity = max ( dot ( normalize(N) , normalize(L) ) , 0.0 );

//	gl_FragColor = texture2D ( colorMap , gl_TexCoord[0].st );
	gl_FragColor = gl_Color;// * intensity;
}
