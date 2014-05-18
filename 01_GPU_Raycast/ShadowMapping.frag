#version 140

in  vec4 ex_Position;
out vec4 out_Color;

void main(void)
{
	float z = ex_Position.z / ex_Position.w;
	out_Color = vec4(z,z,z,1);
}
