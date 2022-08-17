#version 330
uniform sampler2D Texture;

in lowp vec4 DestinationColor;
in lowp vec2 DestinationTexture;

out vec4 color;

void main(void) {
    color = texture2D(Texture, DestinationTexture) * DestinationColor;
}

