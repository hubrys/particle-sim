#version 410

in vec2 velocity;

out vec4 outColor;

void main() 
{ 
	vec2 nV = abs(normalize(velocity));	
	outColor = vec4(nV.x, 1 - nV.x, nV.y, 1.0);
} 