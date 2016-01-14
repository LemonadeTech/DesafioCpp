// Vertex Shader
#version 400

in vec3 vertex;
in vec3 normal;
in vec2 textureCoord;

out vec2 inter_textureCoord;
out float lightIntensity;

out vec3 inter_vertex;
out vec3 inter_normal;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

void main() {
    inter_textureCoord = textureCoord;

    inter_vertex = (modelViewMatrix * vec4(vertex, 1.0)).xyz;
    inter_normal = normalize(transpose(inverse(mat3(modelViewMatrix))) * normal);

    gl_Position = projectionMatrix * modelViewMatrix * vec4(vertex, 1.0);
}
