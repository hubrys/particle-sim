#version 120

varying vec2 velocity;

// out vec4 fColor;

void main() 
{ 
	vec2 nV = abs(normalize(velocity));	
	gl_FragColor = vec4(nV.x, 1 - nV.x, nV.y, 1.0);
} 