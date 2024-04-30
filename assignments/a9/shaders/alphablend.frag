#version 330 core

/*default camera matrices. do not modify.*/
layout(std140) uniform camera {
    mat4 projection;
    mat4 view;
    mat4 pvm;
    mat4 ortho;
    vec4 position;
};

/* set light ubo. do not modify.*/
struct light {
    ivec4 att;
    vec4 pos;
    vec4 dir;
    vec4 amb;
    vec4 dif;
    vec4 spec;
    vec4 atten;
    vec4 r;
};
layout(std140) uniform lights {
    vec4 amb;
    ivec4 lt_att;
    light lt[4];
};

/*input variables*/
in vec3 vtx_normal; // vtx normal in world space
in vec3 vtx_position; // vtx position in world space
in vec3 vtx_model_position; // vtx position in model space
in vec4 vtx_color;
in vec2 vtx_uv;
in vec3 vtx_tangent;

uniform sampler2D tex_color;   /* texture sampler for color */
uniform float iTime; 

/*output variables*/
out vec4 frag_color;

void main() {
    vec4 tex_color = texture(tex_color, vtx_uv);

    if (vtx_uv.y < 0.3) {
        discard;  
    }

    if (tex_color.a < 0.1) {
        discard;
    }

    frag_color = tex_color;
}
