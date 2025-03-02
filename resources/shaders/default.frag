#version 330 core
out vec4 FragColor;
in vec2 fsTex;
in vec3 fsNormal;
in vec3 fsFragPos;

uniform sampler2D tex;

vec3 lightDir = vec3(-0.2, -1.0, -0.3);
vec3 lightColor = vec3(1.0);
uniform vec3 viewPos = vec3(100.0); 


void main() {
  vec3 objectColor = texture(tex, fsTex).xyz;

  float ambientStrength = 0.5;
  vec3 ambient = ambientStrength * lightColor;
  
  float diff = max(dot(fsNormal, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;
  
  float specularStrength = 0.1;
  vec3 viewDir = normalize(viewPos - fsFragPos);
  vec3 reflectDir = reflect(-lightDir, fsNormal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;  
      
  vec3 result = (ambient + diffuse + specular) * objectColor;
  FragColor = vec4(result, 1.0);

  // FragColor = texture(tex, fsTex);
}