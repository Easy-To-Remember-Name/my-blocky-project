#version 330 core

layout(location = 0) in vec3 aPos;       // position
layout(location = 1) in float aHeight;   // height for terrain blending
layout(location = 2) in vec2 aTexCoord;  // UVs

out float height;       // pass to fragment shader
out vec2 TexCoord;      // pass to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    height = aHeight;
    TexCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
