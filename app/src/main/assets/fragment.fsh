#version 300 es

// Interpolated values from the vertex shaders
in highp vec2 UV;

// Output data
out highp vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D textureSampler;

void main(){
    // Output color = color of the texture at the specified UV
    color = texture(textureSampler, UV).rgba;
}
