#version 110

uniform sampler2DRect tex0;
uniform float white; 

void main()
{
	vec4 sample = texture2DRect(tex0, gl_TexCoord[0].st);
	gl_FragColor = sample * (1.0-white) + vec4(white);
}