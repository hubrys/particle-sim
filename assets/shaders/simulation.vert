#version 120

// layout(location = 0) 
attribute vec2 vPosition;
attribute vec2 vVelocity;

varying vec2 velocity;

uniform mat4 Projection;

// uniform mat4 Projection;

void main()
{
	// Color = vColor;
	velocity = vVelocity;
	
	// gl_Position = Projection * vec4(position, 1.0);
	gl_Position = Projection * vec4(vPosition, -.5, 1.0);
}