#version 330 core
in vec3 frag_pos;
in vec3 vertex_normal;
in vec2 vertex_tex_coord;


struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform Material material;

struct Light {
	int type; // 0 = directional light, 1 = point light, 2 = spotlight (soft edges)

	vec3 position;
	vec3 direction;

	float cutoff;
	float outer_cutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define NR_LIGHTS 6
uniform Light lights[NR_LIGHTS];

uniform vec3 view_pos;


out vec4 frag_color;


vec4 Phong(Light light, vec3 normal, vec3 view_dir);
vec4 DirLight(Light light, vec3 normal, vec3 view_dir);
vec4 PointLight(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec4 Spotlight(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir);


void main()
{
	vec3 normal = normalize(vertex_normal);
	vec3 view_dir = normalize(view_pos - frag_pos);

	vec4 result = vec4(0.f);

	for (int i = 0; i < NR_LIGHTS; i++) {
		if (lights[i].type == 0)
			result += DirLight(lights[i], normal, view_dir);
		else if (lights[i].type == 1)
			result += PointLight(lights[i], normal, frag_pos, view_dir);
		else if (lights[i].type == 2)
			result += Spotlight(lights[i], normal, frag_pos, view_dir);
	}

	frag_color = result;
}

vec4 Phong(Light light, vec3 light_dir, vec3 normal, vec3 view_dir) {
	vec4 sampled_texture = texture(material.specular, vertex_tex_coord);

	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec_component = pow(max(dot(view_dir, reflect_dir), 0.f), material.shininess);
	vec4 specular = vec4(sampled_texture.xyz * spec_component * light.specular, sampled_texture.w);

	
	sampled_texture = texture(material.diffuse, vertex_tex_coord);

	float diffuse_strength = max(dot(normal, light_dir), 0.f);
	vec4 diffuse = vec4(sampled_texture.xyz * diffuse_strength * light.diffuse, sampled_texture.w);

	vec4 ambient = vec4(sampled_texture.xyz * light.ambient, sampled_texture.w);


	return ambient + diffuse + specular;
}

vec4 DirLight(Light light, vec3 normal, vec3 view_dir) {
	vec3 light_dir = normalize(-light.direction);

	return Phong(light, light_dir, normal, view_dir);
}

vec4 PointLight(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
	vec3 light_dir = normalize(light.position - frag_pos);

	float distance = length(light.position - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	return Phong(light, light_dir, normal, view_dir) * attenuation;
}

vec4 Spotlight(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir) {
	float theta = dot(normalize(light.position - frag_pos), normalize(-light.direction));
	float epsilon = light.cutoff - light.outer_cutoff;
	float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

	return PointLight(light, normal, frag_pos, view_dir) * intensity;
}
