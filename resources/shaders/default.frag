#version 460 core
out vec4 FragColor;
in vec2 vsTex;
in vec3 vsFragPos;
in vec3 vsNormal;
in float vsType;


uniform sampler2DArray texArray;

vec3 lightDir = vec3(-0.2, -1.0, -0.3);
vec3 lightColor = vec3(1.0);

uniform vec3 viewPos; // TODO pass this data

void main() {
  vec4 tex = texture(texArray, vec3(vsTex, vsType));
  if (tex.a < 0.01) discard;
  vec3 objectColor = texture(texArray, vec3(vsTex, vsType)).xyz;

  float ambientStrength = 0.2;
  vec3 ambient = ambientStrength * lightColor;
  
  float diff = abs(dot(vsNormal, lightDir));
  vec3 diffuse = diff * lightColor;
  
  float specularStrength = 0.2;
  vec3 viewDir = normalize(viewPos - vsFragPos);
  vec3 reflectDir = reflect(-lightDir, vsNormal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
  vec3 specular = specularStrength * spec * lightColor;  
      
  // vec3 result = (ambient + diffuse) * objectColor;
  vec3 result = (ambient + diffuse + specular) * objectColor;

  FragColor = vec4(result, 1.0);

  // FragColor = texture(tex, vsTex);
}