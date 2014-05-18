#version 130

in vec3 ex_Normal;
in vec3 ex_worldPos;
out vec4 out_Color;

uniform vec3 eyePosition = vec3(0,20,20);

//light properties
uniform vec3 lightPosition = vec3(0,20,0);

vec4 ambientColor = vec4(0.4, 0.4, 0.7, 1);
vec4 ambientLight = vec4(0.2, 0.2, 0.2, 1);

vec4 diffuseColor = vec4(0.4, 0.4, 0.7, 1);
vec4 diffuseLight = vec4(0.8, 0.8, 0.8, 1);

vec4 specularColor = vec4(1, 1, 1, 1);
vec4 specularLight = vec4(0.2, 0.2, 0.2, 1);
float specularPower = 20;

void main(void)
{
	vec3 normal = normalize(ex_Normal);
	vec3 directionToLight = normalize(lightPosition - ex_worldPos);
	float diffuseIntensity = clamp( dot(directionToLight, normal), 0, 1);
	vec4 diffuse = diffuseColor * diffuseLight * diffuseIntensity;

	//calculate Phong components per-pixel
	vec3 reflectionVector = normalize(reflect(-directionToLight, normal));
	vec3 directionToEye = normalize(eyePosition - ex_worldPos);

	//calculate specular component
	vec4 specular = specularColor * specularLight * 
				   pow(clamp(dot(reflectionVector, directionToEye), 0, 1), 
					   specularPower);

	//out_Color = specular + diffuse + ambientColor*ambientLight;
	//out_Color.a = 1.0;
}
