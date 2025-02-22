#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 vTex;

// uniform mat4 model;
uniform mat4 v, p;

void main() {
  gl_Position = p * v * vec4(aPos, 1.0);
  vTex = aTex;
}