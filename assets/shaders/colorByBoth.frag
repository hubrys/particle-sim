#version 410

in vec2 velocity;

out vec4 outColor;

void main() 
{ 
	float scale = length(velocity) / 1;
	vec2 nV = abs(normalize(velocity));	
	// gl_FragColor = vec4(nV.x, 1 - nV.y, nV.y, 1.0);
	outColor = vec4(nV.x, scale, nV.y, 1.0);
} 