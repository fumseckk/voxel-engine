#version 460 core
out vec4 FragColor;
in vec2 vsTex;
in vec3 vsFragPos;
in vec3 vsNormal;

uniform sampler2D tex;


vec3 lightDir = vec3(-0.2, -1.0, -0.3);
vec3 lightColor = vec3(1.0);

uniform vec3 viewPos = vec3(100.0); // TODO pass this data

void main() {
  vec3 objectColor = texture(tex, vsTex).xyz;

  float ambientStrength = 0.4;
  vec3 ambient = ambientStrength * lightColor;
  
  float diff = max(dot(vsNormal, lightDir), 0.0);
  vec3 diffuse = diff * objectColor;
  
  float specularStrength = 0.1;
  vec3 viewDir = normalize(viewPos - vsFragPos);
  vec3 reflectDir = reflect(-lightDir, vsNormal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
  vec3 specular = specularStrength * spec * lightColor;  
      
  // vec3 result = (ambient + diffuse) * objectColor;
  vec3 result = (ambient + diffuse + specular) * objectColor;

  FragColor = vec4(result, 1.0);

  // FragColor = texture(tex, vsTex);
}