uniform vec2 dim;
uniform vec2 fudge;

void main(void)
{
	gl_Position = ftransform();
    gl_FrontColor = gl_Color;
    
	mat4 tTex = gl_TextureMatrix[0];
	vec4 texCd = tTex * gl_Vertex;
	texCd.xyz /= texCd.w;
	
	texCd.xy += 1.;
	texCd.xy /= 2.;
	texCd.xy += fudge;
    
	texCd.xy *= dim;
	gl_TexCoord[0] = texCd;
}
