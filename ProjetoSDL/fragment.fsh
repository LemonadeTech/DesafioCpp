// Fragment Shader
#version 400
out vec4 fragColor;
uniform vec4 color;

in vec2 inter_textureCoord;
in vec3 inter_vertex;
in vec3 inter_normal;

uniform vec3 light0Color;
uniform vec3 light0Position;
uniform sampler2D textUnit0;

void main() {
    vec3 L = normalize(light0Position - inter_vertex);
    float lightIntensity = max(dot(normalize(inter_normal), L), 0.0);

    vec3 viewDir = normalize(-inter_vertex);
    vec3 halfDir = normalize(L + viewDir);
    float specAngle = max(dot(halfDir, inter_normal), 0.0);
    float specular = pow(specAngle, 256.0);

    fragColor = texture(textUnit0, inter_textureCoord) + vec4(lightIntensity * light0Color, 1.0) + vec4(vec3(1.0, 1.0, 0.0) * specular, 1.0);
}
