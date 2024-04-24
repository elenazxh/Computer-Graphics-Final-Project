#version 330 core

/*default camera matrices. do not modify.*/
layout(std140) uniform camera
{
    mat4 projection;	/*camera's projection matrix*/
    mat4 view;			/*camera's view matrix*/
    mat4 pvm;			/*camera's projection*view*model matrix*/
    mat4 ortho;			/*camera's ortho projection matrix*/
    vec4 position;		/*camera's position in world space*/
};

/* set light ubo. do not modify.*/
struct light
{
	ivec4 att; 
	vec4 pos; // position
	vec4 dir;
	vec4 amb; // ambient intensity
	vec4 dif; // diffuse intensity
	vec4 spec; // specular intensity
	vec4 atten;
	vec4 r;
};
layout(std140) uniform lights
{
	vec4 amb;
	ivec4 lt_att; // lt_att[0] = number of lights
	light lt[4];
};

/*input variables*/
in vec3 vtx_normal; // vtx normal in world space
in vec3 vtx_position; // vtx position in world space
in vec3 vtx_model_position; // vtx position in model space
in vec4 vtx_color;
in vec2 vtx_uv;
in vec3 vtx_tangent;

uniform vec3 ka;            /* object material ambient */
uniform vec3 kd;            /* object material diffuse */
uniform vec3 ks;            /* object material specular */
uniform float shininess;    /* object material shininess */

uniform sampler2D tex_color;   /* texture sampler for color */
uniform sampler2D tex_normal;   /* texture sampler for normal vector */

/*output variables*/
out vec4 frag_color;

vec3 shading_texture_with_phong(light light, vec3 e, vec3 p, vec3 s, vec3 n)
{

    vec3 ambient = light.amb.rgb * ka;
    vec3 toLight = normalize(light.pos.xyz - p);
    vec3 toView = normalize(e - p);
    vec3 reflectDir = reflect(-toLight, n);

    float diff = max(dot(n, toLight), 0.0);
    vec3 diffuse = light.dif.rgb * kd * diff;

    float spec = pow(max(dot(toView, reflectDir), 0.0), shininess);
    vec3 specular = light.spec.rgb * ks * spec;

    vec3 phong = ambient + diffuse + specular;
    return phong;
}

vec3 read_normal_texture()
{
    vec3 normal = texture(tex_normal, vtx_uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    return normal;
}

void main()
{
    vec3 e = position.xyz;             
    vec3 p = vtx_position;              
    vec3 N = normalize(vtx_normal);     
    vec3 T = normalize(vtx_tangent);    

    vec3 texture_normal = read_normal_texture();
    mat3 TBN = mat3(T, cross(N, T), N); 
    vec3 N_mapped = normalize(TBN * texture_normal); 

    vec3 texture_color = texture(tex_color, vtx_uv).rgb; // Use texture color as diffuse color

    vec3 phong_lighting = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < lt_att[0]; i++)
    {
        phong_lighting += shading_texture_with_phong(lt[i], e, p, texture_color, N_mapped);
    }

    vec3 final_color = texture_color * phong_lighting; // Combine texture color with lighting
    frag_color = vec4(final_color, 1.0);
}