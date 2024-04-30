#version 330 core

/*default camera matrices. do not modify.*/
layout(std140) uniform camera {
    mat4 projection;    /*camera's projection matrix*/
    mat4 view;          /*camera's view matrix*/
    mat4 pvm;           /*camera's projection*view*model matrix*/
    mat4 ortho;         /*camera's ortho projection matrix*/
    vec4 position;      /*camera's position in world space*/
};

uniform mat4 model;       /*model matrix*/
uniform float iTime;      /* time */

/*input variables*/
layout(location = 0) in vec4 pos;            /*vertex position*/
layout(location = 1) in vec4 v_color;        /*vertex color*/
layout(location = 2) in vec4 normal;         /*vertex normal*/
layout(location = 3) in vec4 uv;             /*vertex uv*/
layout(location = 4) in vec4 tangent;        /*vertex tangent*/

/*output variables*/
out vec4 vtx_color;
out vec3 vtx_normal; // world space normal
out vec3 vtx_position; // world space position
out vec3 vtx_model_position; // model space position
out vec2 vtx_uv;
out vec3 vtx_tangent;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

#define OCTAVES 6
float fbm(vec2 st) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 0.;
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= 0.5;
    }
    return value;
}

void main() {

    vec4 worldPos = model * vec4(pos.xyz, 1.);
    vec4 worldNormal = model * vec4(normal.xyz, 0.);
    vec4 worldTangent = model * vec4(tangent.xyz, 0.);

    vtx_normal = normalize(worldNormal.xyz);
    vtx_model_position = pos.xyz;
    vtx_position = worldPos.xyz;
    vtx_color = vec4(v_color.rgb, 1.);
    vtx_uv = uv.xy;
    vtx_tangent = worldTangent.xyz;

    // Apply height-dependent sway
    float heightFactor = vtx_model_position.y;
    float swayFactor = sin(iTime * 0.5 + fbm(vtx_position.xz) * 10.0 + heightFactor * 5.0) * 0.1 * heightFactor;

    // Adjust the world position by swayFactor
    worldPos.x += swayFactor;

    // Compute the final position of the vertex
    gl_Position = projection * view * worldPos;
}
