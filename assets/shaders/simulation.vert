#version 410

// layout(location = 0) 
in vec2 vPosition;
in vec2 vVelocity;

out vec2 velocity;

uniform mat4 Projection;
uniform float DeltaTime;

void main()
{
	velocity = vVelocity;
	
	gl_Position = Projection * vec4(vPosition + vVelocity * DeltaTime, -length(velocity), 1.0);
}