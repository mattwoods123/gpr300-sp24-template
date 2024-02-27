#version 450
out vec4 FragColor; //The color of this fragment
in Surface{
	vec3 WorldPos; //Vertex position in world space
	vec3 WorldNormal; //Vertex normal in world space
	vec2 TexCoord;
}fs_in;

uniform sampler2D _MainTex; 
uniform vec3 _EyePos;
uniform vec3 _LightDirection = vec3(0.0,-1.0,0.0);
uniform vec3 _LightColor = vec3(1.0); 
uniform vec3 _AmbientColor = vec3(0.3,0.4,0.46);

struct Material{
	float Ka;
	float Kd; 
	float Ks; 
	float Shininess;
};

uniform Material _Material;


in vec4 LightSpacePos;

uniform sampler2D _ShadowMap; 



float shadCalc(sampler2D shadowMap, vec4 Lpos){
	vec3 pos = Lpos.xyz * Lpos.w;
	pos = pos * 0.5 + 0.5;
	float myDepth = pos.z;
	float shadowMapDepth = texture(shadowMap, pos.xy).r;
	return step(shadowMapDepth, myDepth);
}


void main(){



	
	//_LightDirection = vec3(0,0,0) - _LightPos;
	//Make sure fragment normal is still length 1 after interpolation.
	vec3 normal = normalize(fs_in.WorldNormal);
	//Light pointing straight down
	vec3 toLight = -_LightDirection;
	float diffuseFactor = max(dot(normal,toLight),0.0);
	//Direction towards eye
	vec3 toEye = normalize(_EyePos - fs_in.WorldPos);
	//Blinn-phong uses half angle
	vec3 h = normalize(toLight + toEye);
	float specularFactor = pow(max(dot(normal,h),0.0),_Material.Shininess);
	// Shadow Calc
	float shadow = shadCalc(_ShadowMap, LightSpacePos);
	

	//Combination of specular and diffuse reflection
	vec3 lightColor = (_Material.Kd * diffuseFactor + _Material.Ks * specularFactor) * _LightColor * (1.0 - shadow);
	lightColor += _AmbientColor * _Material.Ka;
	vec3 objectColor = texture(_MainTex,fs_in.TexCoord).rgb;
	FragColor = vec4(objectColor * lightColor,1.0);
}
