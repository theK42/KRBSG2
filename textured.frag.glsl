#version @glsl_version_string@
precision lowp float;
uniform sampler2D Texture;
uniform float Alpha;

in lowp vec4 DestinationColor;
in lowp vec2 DestinationTexture;

out vec4 color;

void main(void) {
    color = texture(Texture, DestinationTexture) * DestinationColor * vec4(1.0, 1.0, 1.0, Alpha);
}

