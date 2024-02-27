#version 450
//Vertex attributes
layout(location = 0) in vec3 vPos; //Vertex position in model space
layout(location = 1) in vec3 vNormal; //Vertex position in model space
layout(location = 2) in vec2 vTexCoord; //Vertex texture coordinate (UV)
uniform mat4 _Model; //Model->World Matrix
uniform mat4 _ViewProjection; //Combined View->Projection Matrix

out Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; //Vertex normal in world space
	vec2 TexCoord;
}vs_out;

uniform mat4 _LightViewProj;

out vec4 LightSpacePos;

uniform vec3 _LightPos;

void main(){
//Transform vertex position to World Space.
	LightSpacePos = _LightViewProj * _Model * vec4(_LightPos,1);

	vs_out.WorldPos = vec3(_Model * vec4(vPos,1.0));
	//Transform vertex normal to world space using Normal Matrix
	vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;
	vs_out.TexCoord = vTexCoord;
	gl_Position = _ViewProjection * _Model * vec4(vPos,1.0);

}
