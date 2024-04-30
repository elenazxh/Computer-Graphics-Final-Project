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
//uniform sampler2D tex_sdf;   /* texture sampler for normal vector */
uniform sampler2D tex_normal;
uniform sampler2D tex_rs;
//uniform sampler2D tex_M;

uniform bool outline;
/*output variables*/
out vec4 frag_color;

vec3 shading_texture_with_phong(light li, vec3 e, vec3 p, vec3 s, vec3 n, vec2 uv)
{

    vec3 ambient = li.amb.rgb * ka;
    vec3 toLight = normalize(li.pos.xyz - p);
    vec3 toView = normalize(e - p);
    vec3 reflectDir = reflect(-toLight, n);

    vec3 st = texture(tex_st, uv).rgb; // Use vec2 for the ST map's two components
    float specularIntensity = st.b; // Specular intensity from S channel
    float glossiness = st.r; // Glossiness from T channel

    float diff = max(dot(n, toLight), 0.0);
    vec3 diffuse = li.dif.rgb * kd * diff;

    // Use glossiness in the exponent for shininess to modify the specular highlight size
    float spec = pow(max(dot(toView, reflectDir), 0.0), shininess * glossiness);
    vec3 specular = li.spec.rgb * ks * specularIntensity * spec;

    vec3 phong = ambient + diffuse + specular;
    return phong;
}

vec3 toon_shading(light li, vec3 e, vec3 p, vec3 n)
{
    vec3 toLight = normalize(li.pos.xyz - p);

    // Calculate diffuse intensity and clamp to [0, 0.]
    float diff = clamp(dot(n, toLight), 0.60, 0.99);

    // Remap the diffuse range [0, 1] to [0, 1] and use it as the texture coordinate to sample from the color ramp
    float rampPosition = diff; // No need to remap as dot product is already [0, 1]
    vec4 rampColor = texture(tex_rs, vec2(rampPosition, 0.5));

    // The color from the ramp replaces the simple step function to create smooth transitions
    vec3 diffuse = rampColor.rgb * kd; // Multiply by material's diffuse to incorporate the object's color

    // Toon shading typically does not include a specular component, but you can add it back if desired
    // For this example, the specular is removed to simplify the toon effect

    // Combine ambient and diffuse components
    vec3 toon = li.amb.rgb * ka + diffuse;
    return toon;
}

vec3 getNormalFromMap()
{
    vec3 packedNormal = texture(tex_normal, vtx_uv).rgb;
    packedNormal = normalize(packedNormal * 2.0 - 1.0);
    return packedNormal;
}

void main()
{
    if (outline) {
        frag_color = vec4(0.0, 0.0, 0.0, 1.0);  // Black color for the outline
    } else {
        vec3 e = position.xyz;             
        vec3 p = vtx_position;              
        vec3 N = getNormalFromMap();     
        vec3 T = normalize(vtx_tangent);    

        vec3 texture_color = texture(tex_color, vtx_uv).rgb; // Use texture color as diffuse color

        vec3 toon_lighting = vec3(0.0, 0.0, 0.0);
        toon_lighting += toon_shading(lt[0], e, p, N);
        //toon_lighting += shading_texture_with_phong(lt[0], e, p, texture_color, N, vtx_uv);


        // Final color is a combination of texture color and toon lighting
        vec3 final_color = texture_color*toon_lighting;
        frag_color = vec4(final_color, 1.0);
    }
}