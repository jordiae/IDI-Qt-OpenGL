#version 330 core

in vec3 vertex;
in vec3 col;
out vec4 col_out;

void main()  {
    gl_Position = vec4(vertex, 1.0);
    col_out = vec4(col, 1.0);
}
