
uniform sampler2D colorMap;

void main (void)
{
	// Grey scale
//	float grey = dot ( texture2D ( colorMap , gl_TexCoord[0].st ).rgb , vec3 ( 0.299 , 0.587 , 0.114 ) );
//	gl_FragColor = vec4 ( grey , grey , grey , 1.0 );

	// Inverse colors
//	gl_FragColor.rgb = 1.0 - gl_Color.rgb * texture2D ( colorMap , gl_TexCoord[0].st ).rgb;
//	gl_FragColor.a = 1.0;

	gl_FragColor = gl_Color * texture2D ( colorMap , gl_TexCoord[0].st );
}