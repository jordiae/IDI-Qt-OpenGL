#version 330 core

in vec3 vertex;
in vec3 vertex2;
uniform mat4 TG;
uniform mat4 TG2;
void main()  {
    //gl_Position = TG*vec4 (vertex, 1.0);
    gl_Position = TG2*vec4 (vertex2, 1.0);
}
