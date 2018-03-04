#version 330 core

in vec3 vertex;
in vec3 col;
out vec4 col_out;
uniform float val;
uniform mat4 TG;

void main()  {
    gl_Position = TG*vec4 (vertex*val, 1.0);
    col_out = vec4(col, 1.0);
}
