#version 330 core

layout(std140) uniform camera
{
    mat4 projection;
    mat4 view;
    mat4 pvm;
    mat4 ortho;
    vec4 position;
};

struct light
{
    ivec4 att;
    vec4 pos;
    vec4 dir;
    vec4 amb;
    vec4 dif;
    vec4 spec;
    vec4 atten;
    vec4 r;
};
layout(std140) uniform lights
{
    vec4 amb;
    ivec4 lt_att;
    light lt[4];
};

in vec3 vtx_normal;
in vec3 vtx_position;
in vec2 vtx_uv;
in vec3 vtx_tangent;

uniform vec3 ka;            // Ambient reflection coefficient
uniform vec3 kd;            // Diffuse reflection coefficient
uniform vec3 ks;            // Specular reflection coefficient
uniform float shininess;    // Specular shininess factor

uniform sampler2D tex_color;
uniform sampler2D tex_normal;
uniform sampler2D tex_metal;
uniform sampler2D tex_emission;

out vec4 frag_color;

vec3 getNormalFromMap()
{
    vec3 packedNormal = texture(tex_normal, vtx_uv).rgb;
    packedNormal = normalize(packedNormal * 2.0 - 1.0);
    return packedNormal;
}

void main()
{
    vec3 N = normalize(vtx_normal);
    vec3 T = normalize(vtx_tangent);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);

    vec3 normal = normalize(TBN * getNormalFromMap());
    vec3 albedo = texture(tex_color, vtx_uv).rgb;
    vec3 emission = texture(tex_emission, vtx_uv).rgb;
    float metalness = texture(tex_metal, vtx_uv).r;

    vec3 e = position.xyz;
    vec3 p = vtx_position;
    vec3 ambient = amb.rgb * ka;

    vec3 color = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < lt_att[0]; i++)
    {
        vec3 lightDir = normalize(lt[i].pos.xyz - p);
        float NdotL = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = lt[i].dif.rgb * albedo * NdotL;

        vec3 H = normalize(lightDir + normalize(e - p));
        float NdotH = max(dot(normal, H), 0.0);
        vec3 specular = lt[i].spec.rgb * pow(NdotH, shininess) * ks;

        // Linear blend between metalness and non-metalness
        vec3 lighting = mix(diffuse + ambient, specular, metalness);
        color += lighting;
    }

    color += emission; // Add emission component directly
    frag_color = vec4(color, 1.0);
}