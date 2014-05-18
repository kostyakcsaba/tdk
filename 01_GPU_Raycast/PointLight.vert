#version 130

in  vec3 vs_in_pos;
in  vec3 vs_in_normal;
out vec3 ex_Normal;
out vec3 ex_worldPos;

uniform mat4 ModelViewProjection;
uniform mat4 Model;
uniform mat4 InvModel;

void main(void)
{
	vec4 pos = vec4(vs_in_pos, 1.0f);
	gl_Position = ModelViewProjection * pos;
	ex_Normal = (vec4(vs_in_normal, 0.0f) * InvModel).xyz;
	ex_worldPos = (Model * pos).xyz;
}
