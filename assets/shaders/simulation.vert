#version 120

// layout(location = 0) 
attribute vec2 vPosition;

uniform float scale;

// uniform mat4 Projection;

void main()
{
	// Color = vColor;
	
	// gl_Position = Projection * vec4(position, 1.0);
	gl_Position = vec4(vPosition * scale, -.5, 1.0);
}