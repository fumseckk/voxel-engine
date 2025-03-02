#version 400 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 vsTex[];
in vec3 vsFragPos[];
out vec2 fsTex;
out vec3 fsNormal;
out vec3 fsFragPos;

vec3 get_normal() {
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}


void main() {
    // gl_Position = gl_in[0].gl_Position;
    // fsTex = vsTex[0];
    // EmitVertex();
    // gl_Position = gl_in[1].gl_Position;
    // fsTex = vsTex[1];
    // EmitVertex();
    // gl_Position = gl_in[2].gl_Position;
    // fsTex = vsTex[2];
    // EmitVertex();
    // EndPrimitive();

    vec3 normal = get_normal();
    gl_Position = gl_in[0].gl_Position;
    fsNormal = normal;
    fsFragPos = vsFragPos[0];
    fsTex = vsTex[0];
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    fsNormal = normal;
    fsFragPos = vsFragPos[1];
    fsTex = vsTex[1];
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    fsNormal = normal;
    fsFragPos = vsFragPos[2];
    fsTex = vsTex[2];
    EmitVertex();
    EndPrimitive();
}