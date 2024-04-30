#version 330 core

in vec2 TexCoord;
in vec3 Normal;

uniform sampler2D tex_color;

out vec4 FragColor;

void main()
{
    // Sample the grass texture
    vec4 textureColor = texture(tex_color, TexCoord);

    // Output the final color
    FragColor = textureColor;
}
