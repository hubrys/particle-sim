#version 410

in vec2 velocity;

out vec4 outColor;

void main() 
{ 
	float scale = length(velocity);
	outColor = vec4(scale, 1 - scale, 0, 1.0);
	// fColor = vec4(Color, 1.0);
    //gl_FragColor = vec4(0, .5, 0, 1.0);
} 