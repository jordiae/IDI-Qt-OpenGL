#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;



out vec3 NormSCO;
out vec4 vertexSCO;



out vec3 matamb_fs;
out vec3 matdiff_fs;
out vec3 matspec_fs;
out float matshin_fs;


void main()
{
    mat3 nMatrix = inverse(transpose(mat3(view * TG)));
    NormSCO = normalize(nMatrix * normal);
    vertexSCO = view * TG * vec4(vertex, 1.0);
   
    matamb_fs = matamb;
    matdiff_fs = matdiff;
    matspec_fs = matspec;
    matshin_fs = matshin;

    gl_Position = proj * view * TG * vec4 (vertex, 1.0);
}
