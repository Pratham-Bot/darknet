#version 300 es
precision highp float;
    
layout(std430) buffer;
uniform int batchSize;
uniform int numChannels;
uniform int size;
uniform float biases[numChannels];

layout(location = 0) in vec4 in_color;
layout(location = 0) out vec4 out_color;

void main() {
    int b = int(gl_FragCoord.x); // Batch index
    int c = int(gl_FragCoord.y); // Channel index
    int i = int(gl_FragCoord.z); // Element index

    int outputIndex = (b * numChannels + c) * size + i;
    out_color = in_color + biases[c];
}