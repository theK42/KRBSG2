#version @glsl_version_string@
precision lowp float;
in vec4 Position;
in vec4 VertexColor;
in vec2 TextureCoordinates;

out lowp vec4 DestinationColor;
out lowp vec2 DestinationTexture;

uniform mat4 Projection;
uniform mat4 Model;


void main(void) {
    DestinationColor = VertexColor;
    DestinationTexture = TextureCoordinates;
    gl_Position = Projection * Model * Position;
}

