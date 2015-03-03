#version 120

varying vec2 velocity;

// out vec4 fColor;

void main() 
{ 
	float scale = length(velocity) / 400;
	vec2 nV = abs(normalize(velocity));	
	// gl_FragColor = vec4(nV.x, 1 - nV.y, nV.y, 1.0);
	gl_FragColor = vec4(nV.x, scale, nV.y, 1.0);
} 