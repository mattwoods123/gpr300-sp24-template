#version 450
//out vec4 FragColor;
//
//in vec3 FragPos;
//int vec3 Normal;
//in vec2 TexCoords;
//in vec4 FragPosLightSpace;
//
//uniform sampler2D diffuseTexture;
//uniform sampler2D shadowMap;
//
//uniform vec3 lightPos;
//uniform vec3 viewPos;
//
//float shadCalc(){
//	vec3 pos = FragPosLightSpace.xyz * 0.5 + 0.5;
//	float depth = texture(shadowMap, pos.xy).r;
//	return depth < pos.z ? 0.0 : 1.0;
//}
//
//void main()
//{             
//   vec3 color = texture(diffuseTexture, TexCoords).rgb;
//   vec3 normal = normalize(Normal);
//   vec3 lightColor = vec3(0.3);
//
//   vec3 ambient = 0.3 * color;
//
//   vec3 lightDir = normalize(lightPos - FragPos);
//   float dotLightNormal = dot(lightDir, normal);
//   float diff = max(dotLightNormal, normal);
//   vec3 diffuse = dif*lightColor;
//
//   vec3 viewDir = normalize(viewPos - FragPos);
//   halfwayDir = normalize(light + viewDir);
//   float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
//   vec3 specular = spec * lightColor;
//
//   float shadow = shadCalc();
//   vec3 lighting = (shadow * (diffuse + specular) + ambient) * color;
//
//   FragColor = vec4(lighting, 1.0);
//}  