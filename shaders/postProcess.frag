#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float gamma;

void main()
{
    vec3 color = texture(screenTexture, TexCoords).rgb; // ignore alpha channel
    // grayscale
    //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //FragColor = vec4(average, average, average, 1.0);

    // gamma correction
    if (gamma > 0) // gamma == 0 is off
        color = pow(color, vec3(1.0 / gamma));
    FragColor = vec4(color, 1.0);
} 