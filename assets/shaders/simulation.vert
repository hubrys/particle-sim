#version 410

// layout(location = 0) 
in vec2 vPosition;
in vec2 vVelocity;

out vec2 velocity;

uniform mat4 Projection;

// uniform mat4 Projection;

void main()
{
	// Color = vColor;
	velocity = vVelocity;
	
	// gl_Position = Projection * vec4(position, 1.0);
	gl_Position = Projection * vec4(vPosition, -.5, 1.0);
}