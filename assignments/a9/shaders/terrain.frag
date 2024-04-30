#version 330 core

layout (std140) uniform camera
{
	mat4 projection;
	mat4 view;
	mat4 pvm;
	mat4 ortho;
	vec4 position;
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

uniform float iTime;
uniform mat4 model;		/*model matrix*/

in vec3 vtx_pos;

out vec4 frag_color;


uniform vec3 ka;            /* object material ambient */
uniform vec3 kd;            /* object material diffuse */
uniform vec3 ks;            /* object material specular */
uniform float shininess;    /* object material shininess */

vec2 hash2(vec2 v)
{
	vec2 rand = vec2(0,0);
	
	rand  = 50.0 * 1.05 * fract(v * 0.3183099 + vec2(0.71, 0.113));
    rand = -1.0 + 2 * 1.05 * fract(rand.x * rand.y * (rand.x + rand.y) * rand);
	return rand;
}

float perlin_noise(vec2 v) 
{
    float noise = 0;
	vec2 i = floor(v);
    vec2 f = fract(v);
    vec2 m = f*f*(3.0-2.0*f);
	
	noise = mix( mix( dot( hash2(i + vec2(0.0, 0.0)), f - vec2(0.0,0.0)),
					 dot( hash2(i + vec2(1.0, 0.0)), f - vec2(1.0,0.0)), m.x),
				mix( dot( hash2(i + vec2(0.0, 1.0)), f - vec2(0.0,1.0)),
					 dot( hash2(i + vec2(1.0, 1.0)), f - vec2(1.0,1.0)), m.x), m.y);
	return noise;
}

float noiseOctave(vec2 v, int num)
{
	float sum = 0;
	for(int i =0; i<num; i++){
		sum += pow(2,-1*i) * perlin_noise(pow(2,i) * v);
	}
	return sum;
}

float height(vec2 v){
    float h = 0;
	h = 0.75 * noiseOctave(v, 10);
	if(h<0) h *= .5;
	return h * 2.;
}

vec3 compute_normal(vec2 v, float d)
{	
	vec3 normal_vector = vec3(0,0,0);
	vec3 v1 = vec3(v.x + d, v.y, height(vec2(v.x + d, v.y)));
	vec3 v2 = vec3(v.x - d, v.y, height(vec2(v.x - d, v.y)));
	vec3 v3 = vec3(v.x, v.y + d, height(vec2(v.x, v.y + d)));
	vec3 v4 = vec3(v.x, v.y - d, height(vec2(v.x, v.y - d)));
	
	normal_vector = normalize(cross(v1-v2, v3-v4));
	return normal_vector;
}

vec4 shading_phong(light li, vec3 e, vec3 p, vec3 s, vec3 n) 
{
    vec3 v = normalize(e - p);
    vec3 l = normalize(s - p);
    vec3 r = normalize(reflect(-l, n));

    vec3 ambColor = ka * li.amb.rgb;
    vec3 difColor = kd * li.dif.rgb * max(0., dot(n, l));
    vec3 specColor = ks * li.spec.rgb * pow(max(dot(v, r), 0.), shininess);

    return vec4(ambColor + difColor + specColor, 1);
}

// Draw the terrain
vec3 shading_terrain(vec3 pos) {
	vec3 n = compute_normal(pos.xy, 0.01);
	vec3 e = position.xyz;
	vec3 p = pos.xyz;
	vec3 s = lt[0].pos.xyz;

    n = normalize((model * vec4(n, 0)).xyz);
    p = (model * vec4(p, 1)).xyz;

    vec3 color = shading_phong(lt[0], e, p, s, n).xyz;

	float h = pos.z + .8;
	h = clamp(h, 0.0, 1.0);
	vec3 emissiveColor = mix(vec3(.4,.6,.2), vec3(.4,.3,.2), h);

	return color * emissiveColor;
}


//  1 out, 2 in...
float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * .1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

//----------------------------------------------------------------------------------------
//  2 out, 1 in...
vec2 hash21(float p)
{
	vec3 p3 = fract(vec3(p) * vec3(.1031, .1030, .0973));
	p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.xx+p3.yz)*p3.zy);

}

float noise(vec2 p)
{
    vec2 ip = floor(p), fp = fract(p);
    fp = smoothstep(0.,1.,fp);
    return mix(
               mix(hash12(ip+vec2(0,0)), hash12(ip+vec2(1,0)), fp.x),
               mix(hash12(ip+vec2(0,1)), hash12(ip+vec2(1,1)), fp.x),
               fp.y);
}

float fbm(vec2 p, int lv)
{
    float a = 1.0;
    float t = 0.0;
    for( int i=0; i<lv; i++ )
    {
        p += vec2(13.102,1.535);
        t += a*noise(p);
        p *= mat2(3,4,-4,3) * 0.4;
        a *= 0.5;
    }
    return 0.5*t;
}

void main()
{
    // frag_color = vec4(shading_terrain(vtx_pos), 1.0);

	vec2 uv = vtx_pos.xy;
    
    float mtHeight = fbm(uv.xx+0.6, 8);
    float mtHeightSm = fbm(uv.xx+0.6, 3);
    vec3 col = vec3(0);
    vec2 sunPos = vec2(0.8,-0.8);
    vec3 skyCol = vec3(0.075,0.310,0.518);
    float q = uv.y-sunPos.y;
    float q2 = uv.x-sunPos.x;
    skyCol = mix(skyCol, vec3(0.482,0.580,0.902), exp(-0.5*q*q-0.2*q2*q2));
    vec3 cloudCol = mix(skyCol, vec3(0.5), 0.2);
    skyCol = mix(skyCol, vec3(0.706,0.851,0.953), exp(-q*q*3.-0.5*q2*q2));
    skyCol = mix(skyCol, vec3(0.980,0.5,0.3), exp(-q*q*10.-0.5*q2*q2));
    skyCol = mix(skyCol, vec3(1.0,1.0,0.7), exp(-3.*length(uv-sunPos)));
    vec3 cloudCol2 = mix(skyCol, vec3(0.5), 0.2);
    vec3 cloudCol3 = mix(vec3(0.980,0.5,0.3),vec3(1.0,1.0,0.7), exp(-length(uv-sunPos)));
    cloudCol3 = mix(cloudCol3, cloudCol, smoothstep(0.,-1.,uv.y+0.3*uv.x));
    
    float w = 1.5*length(fwidth(uv));
    float isSky = smoothstep(0.,w,uv.y+0.3*uv.x+0.2*max(uv.x,0.)+1.-mtHeight);
    col = mix(col, skyCol, isSky);
    col = mix(col, skyCol, 0.5*smoothstep(-0.5,0.1,uv.y+0.3*uv.x+0.2*max(uv.x,0.)+1.-mtHeightSm));
    
    vec2 fuv = fract(0.1*uv);
    vec2 uvv = 20.*fuv*(1.-fuv)*(0.5-fuv);
    uvv = vec2(1,-1)*uvv.yx;
    vec2 uv2 = uv + uvv*cos(0.1*iTime);// twisting
    
    float silver = fbm(30.*uv - 0.06*iTime, 8) + 30.*(uv.y + 0.8);
    silver = smoothstep(0.,1.,silver)*smoothstep(2.,1.,silver) * 1./(1.+500.*q2*q2) * isSky;
    col += silver * vec3(0.9,0.6,0.3) * 100.;
    
    
    float lowClouds = fbm(5.*uv + 0.1*iTime, 8);
    float midClouds = fbm(3.*uv + vec2(0.06,-0.03)*iTime, 8);
    float hiClouds = fbm(uv2 + vec2(0.1,0.01)*iTime, 8) - 0.5;
    float hiClouds2 = fbm(uv + 10. + vec2(0.062,-0.03)*iTime, 8) - 0.5;
    col = mix(col, cloudCol3, 0.5*smoothstep(0.,1., -uv.y+3.*hiClouds));
    col = mix(col, cloudCol3, 0.5*smoothstep(0.,1., -uv.y+3.*hiClouds2));
    col = mix(col, vec3(0.9,0.6,0.3) * 100., 1./(1.+2000.*(q*q+q2*q2)));
    col += vec3(0.9,0.6,0.3) * 2./(1.+10.*sqrt(q*q+0.3*q2*q2));
    col = mix(col, 0.8*cloudCol, 0.8*smoothstep(0.,1., -2.*(uv.y+0.5)+midClouds));
    col = mix(col, 0.5*cloudCol, smoothstep(0.,1., -3.*(uv.y+0.8)+lowClouds));
    
    
    col = mix(col, col*pow(col/(col.r+col.g+col.b), vec3(dot(uv,uv)*0.3)), 0.2); // vignette
    col = pow(col, vec3(2.2));
    col = (col*(2.51*col+0.03))/(col*(2.43*col+0.59)+0.14); // tonemapping
    col = pow(col, vec3(1./2.2));
    
    col += 0.03 * (hash12(uv)-0.5) * sqrt(vtx_pos.y/400.);

    // Output to screen
    frag_color = vec4(col,1.0);
}
