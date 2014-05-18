#version 130

// VBO-ból érkezõ változók
in vec3 vs_in_pos;
in vec2 vs_in_tex;
// a pipeline-ban tovább adandó értékek
out vec3 vs_out_pos;
out vec2 vs_out_tex;
void main()
{
	gl_Position = vec4( vs_in_pos, 1 );
	vs_out_pos = vs_in_pos;
	vs_out_tex = vs_in_tex;
}