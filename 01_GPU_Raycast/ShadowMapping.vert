#version 140

in  vec3 vs_in_pos;
out vec4 ex_Position;

uniform mat4x4 ModelLightViewProjection;

void main(void)
{
	gl_Position = ex_Position = ModelLightViewProjection * vec4(vs_in_pos, 1.0);
}
