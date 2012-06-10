#version 120
#extension GL_EXT_geometry_shader4 : enable

float normalz(float zed){
    return (zed - 400.)/5000.;
}

void main() {
    float nz0 = normalz(gl_PositionIn[0].z);
    float nz1 = normalz(gl_PositionIn[1].z);
    float nz2 = normalz(gl_PositionIn[2].z);
    
    float dif = abs(nz0-nz1) + abs(nz1-nz2) + abs(nz1-nz2);
    if( dif < .01){
        for(int i = 0; i < gl_VerticesIn; ++i) {
            gl_Position = gl_PositionIn[i];
            gl_FrontColor = gl_FrontColorIn[i];
            //gl_FrontColor.rgb = vec3((normalz(gl_PositionIn[i].z));            
            //gl_FrontColor.rgb = vec3(dif);
            //gl_FrontColor.a = 1.0;
            
            gl_TexCoord[0] = gl_TexCoordIn[i][0];
            EmitVertex();
        }
    }
}