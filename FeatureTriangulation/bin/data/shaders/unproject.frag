#version 110
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect tex0;
void main()
{
	vec4 col = texture2DRect(tex0, gl_TexCoord[0].st);
	gl_FragColor = col * gl_Color;

}