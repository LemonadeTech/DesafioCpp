#version 120

uniform sampler2D diffuse;

varying vec2 textCoord0;
varying vec3 normal0;

void main()
{
	gl_FragColor = texture2D(diffuse, textCoord0) * clamp(dot(-vec3(0,-1,0), normal0), 0.0, 1.0);
}