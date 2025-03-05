#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 vsTex;
out vec3 vsFragPos;
out vec3 vsViewPos;

// uniform mat4 model;
uniform mat4 v, p;

void main() {
  vec4 viewPos = v * vec4(aPos, 1.0);
  gl_Position = p * viewPos;
  vsViewPos = viewPos.xyz;
  vsFragPos = gl_Position.xyz;
  vsTex = aTex;
}