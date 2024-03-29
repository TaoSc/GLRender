#version 330 core
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_tex_coord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec3 frag_pos;
out vec3 vertex_normal;
out vec2 vertex_tex_coord;


void main()
{
	frag_pos = vec3(model * vec4(a_pos, 1.f));
	vertex_normal = mat3(transpose(inverse(model))) * a_normal;
	vertex_tex_coord = a_tex_coord;

	gl_Position = projection * view * model * vec4(a_pos, 1.f);
}
