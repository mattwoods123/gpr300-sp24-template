#version 450
	
uniform mat4 _ViewProjection;

uniform mat4 _Model;

void main()
{
	gl_Position = _ViewProjection * _Model* vec4()
}  