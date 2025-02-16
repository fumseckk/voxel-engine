#version 330 core
out vec4 FragColor;
in vec2 vTex;

uniform sampler2D tex;

void main() {
  // FragColor = vec4(mix(1, 0, vPos.x), mix(1, 0, vPos.y), mix(1, 0, vPos.z), 1.0);
  FragColor = texture(tex, vTex);
  // FragColor = vec4(vTex, 0.0, 1.0);
}