
uniform sampler2D colorMap;

varying vec3 N , L;

void main (void)
{
	float intensity = max ( dot ( normalize(N) , normalize(L) ) , 0.0 );

	gl_FragColor = gl_Color * texture2D ( colorMap , gl_TexCoord[0].st );
	gl_FragColor.rgb *= intensity;
}