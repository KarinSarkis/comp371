#version 330 core
out vec4 FragColor;

in vec3 vColor;
in vec2 vTexCoord;

uniform sampler2D textureSampler;

void main()
{
    // Sample the texture and multiply by the vertex color
    FragColor = texture(textureSampler, vTexCoord) * vec4(vColor, 1.0);
}