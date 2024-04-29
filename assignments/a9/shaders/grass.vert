#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 windDirection;
uniform float maxWindStrength;
uniform float time; // Time variable for animation

out vec2 TexCoord;
out vec3 Normal;

void main()
{
    // Calculate wind strength based on time (e.g., wind starts and stops periodically)
    float windStrength = maxWindStrength * (0.5 * (1.0 + sin(time))); // Modulate wind strength based on time

    // Calculate displacement based on wind direction and strength
    vec3 displacement = normalize(windDirection) * windStrength;

    // Apply displacement to vertex position
    vec3 displacedPosition = position + displacement;

    // Transform vertex position to clip space
    gl_Position = projection * view * model * vec4(displacedPosition, 1.0);

    // Pass texture coordinates and normal to fragment shader
    TexCoord = texCoord;
    Normal = normal;
}
