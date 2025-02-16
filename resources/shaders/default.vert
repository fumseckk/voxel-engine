#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 vTex;

// uniform mat4 model;
uniform mat4 m, v, p;

void main() {
  gl_Position = p * v * m * vec4(aPos, 1.0);
  vTex = aTex;
}