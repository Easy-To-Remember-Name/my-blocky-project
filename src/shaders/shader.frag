#version 330 core
in float height;
in vec2 TexCoord;
out vec4 FragColor;

uniform bool isOutline;
uniform sampler2D grassTexture;
uniform sampler2D rockTexture;
uniform sampler2D snowTexture;
uniform sampler2D soilTexture;

void main()
{
    if(isOutline) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        vec4 texColor;

        // Choose texture based on height (no blending)
        if(height < 2.0) {
            texColor = texture(soilTexture, TexCoord);
        } else if(height < 5.0) {
            texColor = texture(grassTexture, TexCoord);
        } else if(height < 8.0) {
            texColor = texture(rockTexture, TexCoord);
        } else {
            texColor = texture(snowTexture, TexCoord);
        }

        FragColor = texColor;
    }
}
