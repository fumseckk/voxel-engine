#version 460 core

uniform mat4 v, p;
uniform vec3 chunkOrigin;

out vec2 vsTex;
out vec3 vsFragPos;
out vec3 vsNormal;

layout(std430, binding = 0) readonly buffer vertexPullBuffer
{
  ivec4 packed_data[]; // coords then direction. TODO add texture here in [dir]
};

const vec3 normals[] = vec3[]
(
  vec3( 0,  0,  1),  // near
  vec3( 0,  0, -1), // far
  vec3(-1,  0,  0), // left
  vec3( 1,  0,  0),  // right
  vec3( 0, -1,  0), // down
  vec3( 0,  1,  0)   // up
);

// all 8 vertices of a cube
const vec3 vertex_positions[8] = vec3[8]
(
	vec3(0, 0,  0), // 0 near, bottom left
	vec3(0, 0, -1), // 1 far,  bottom left
  vec3(0, 1,  0), // 2 near, top left
  vec3(0, 1, -1), // 3 far,  top left
	vec3(1, 0,  0), // 4 near, bottom right
	vec3(1, 0, -1), // 5 far,  bottom right
	vec3(1, 1,  0), // 6 near, top right
	vec3(1, 1, -1)  // 7 far,  top right
);

// indices of a face in order: bottom left, bottom right, top right, top left
// seen from outside of the cube, while facing it.
const int indices[] = int[]
(
  0, 4, 6, 2, // near
  5, 1, 3, 7, // far
  1, 0, 2, 3, // left
  4, 5, 7, 6, // right
  1, 5, 4, 0, // down
  2, 6, 7, 3 // up
);

const int indices_order[] = int[]
(
  0, 1, 2, 2, 3, 0
);

const vec2 uv_order[] = vec2[]
(
  vec2(0, 0),
  vec2(1, 0),
  vec2(1, 1),
  vec2(0, 1)
);

void main() {
  // prepare indices & unpack data
  int face_index = gl_VertexID / 6; // TODO
  int vertex_index = gl_VertexID % 6;
  ivec4 data = packed_data[face_index];
  vec3 position = data.xyz + chunkOrigin;
  int dir = data.w & 7;
  int type = data.w >> 4;
  
  // prepare vertex data: uv and coords
  int index = indices_order[vertex_index];
  position += vertex_positions[indices[index + 4*dir]];
  vec2 uv = uv_order[index];

  // set out variables
  gl_Position = p * v * vec4(position, 1.0);
  vsFragPos = gl_Position.xyz;
  vsTex = uv;
  vsNormal = normals[dir];
}