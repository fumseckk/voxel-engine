#ifndef CUBE_H
#define CUBE_H

#define VERTEX_SIZE 5
#define FACE_SIZE 30

// Texture coordinates for faces in the same consistent order
float faceUVs[] = {
    0.0f, 0.0f, // Bottom left
    1.0f, 0.0f, // Bottom right
    1.0f, 1.0f, // Top right
    0.0f, 1.0f  // Top left
};

// Indices to create two triangles for a face (using counter-clockwise winding)
unsigned int faceIndices[] = {
    0, 1, 2, // First triangle
    0, 2, 3  // Second triangle
};


// Vertex format: X, Y, Z, U, V
// float vertices[] = {
//     // Front face (Z = 0)
//     0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
//     1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Bottom-right
//     1.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top-right
//     0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
//     1.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top-right
//     0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top-left

//     // Back face (Z = 1)
//     1.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Bottom-right
//     0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom-left
//     0.0f, 1.0f, 1.0f,  0.0f, 1.0f, // Top-left
//     1.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Bottom-right
//     0.0f, 1.0f, 1.0f,  0.0f, 1.0f, // Top-left
//     1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Top-right

//     // Left face (X = 0)
//     0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom-back
//     0.0f, 1.0f, 1.0f,  0.0f, 1.0f, // Top-back
//     0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top-front
//     0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom-back
//     0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // Top-front
//     0.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Bottom-front

//     // Right face (X = 1)
//     1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom-front
//     1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top-front
//     1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Top-back
//     1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom-front
//     1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Top-back
//     1.0f, 0.0f, 1.0f,  1.0f, 0.0f, // Bottom-back

//     // Bottom face (Y = 0)
//     0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Front-left
//     0.0f, 0.0f, 1.0f,  0.0f, 1.0f, // Back-left
//     1.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Back-right
//     0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Front-left
//     1.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Back-right
//     1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // Front-right

//     // Top face (Y = 1)
//     0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Front-left
//     1.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Front-right
//     1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Back-right
//     0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // Front-left
//     1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Back-right
//     0.0f, 1.0f, 1.0f,  0.0f, 1.0f  // Back-left
// };



float vertices[] = {
    0.0f, 0.0f, 0.0f,  1.0f, 0.0f, // FORWARD
    0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
    1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,  1.0f, 0.0f,

    0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // BACKWARD
    1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
    0.0f, 1.0f, 1.0f,  0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,  0.0f, 0.0f,

    0.0f, 1.0f, 1.0f,  1.0f, 1.0f, // LEFT
    0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    0.0f, 1.0f, 1.0f,  1.0f, 1.0f,

    1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // RIGHT
    1.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 0.0f,

    0.0f, 0.0f, 0.0f,  0.0f, 1.0f, // DOWN
    1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
    1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,  0.0f, 1.0f,

    0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // UP
    0.0f, 1.0f, 1.0f,  1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,  0.0f, 1.0f
};

#endif