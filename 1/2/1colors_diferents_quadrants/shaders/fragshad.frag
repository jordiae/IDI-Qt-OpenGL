#version 330 core

out vec4 FragColor;

void main() {
    //FragColor = vec4(0, 0, 0, 1);
    if (gl_FragCoord.x <= 355 && gl_FragCoord.y <= 355)
        FragColor = vec4(1, 1, 0, 1);
    else if (gl_FragCoord.x <= 355 && gl_FragCoord.y > 355)
        FragColor = vec4(0, 1, 0, 1);
    else if (gl_FragCoord.x > 355 && gl_FragCoord.y <= 355)
        FragColor = vec4(0, 0, 1, 0);
    else if (gl_FragCoord.x > 355 && gl_FragCoord.y > 355)
        FragColor = vec4(1, 0, 0, 1);
}
