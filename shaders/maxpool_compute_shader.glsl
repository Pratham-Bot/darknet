#version 310 es
precision highp float;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// Buffer bindings and uniforms ...
layout(binding = 0) buffer InputBuffer {
    float input[];
};

layout(binding = 1) buffer OutputBuffer {
    float output[];
};

uniform vec2 poolSize;
uniform int inputWidth;
uniform int inputHeight;
uniform int outputWidth;
uniform int outputHeight;

void apply_activation(inout float value) {
    // Apply your activation function here
     value = max(0.0, value);
}

void main() {
    int globalIdx = int(gl_GlobalInvocationID.x);
    int globalIdy = int(gl_GlobalInvocationID.y);
    
    // Calculate output index
    int outputIdx = globalIdy * outputWidth + globalIdx;
    
    // Calculate input indices based on maxpooling pool size
    int startX = globalIdx * int(poolSize.x);
    int startY = globalIdy * int(poolSize.y);
    
    // Initialize the max value
    float maxValue = -INFINITY;
    
    // Iterate over the pool area and find the max value
    for (int y = startY; y < startY + int(poolSize.y); ++y) {
        for (int x = startX; x < startX + int(poolSize.x); ++x) {
            int inputIdx = y * inputWidth + x;
            maxValue = max(maxValue, input[inputIdx]);
        }
    }
    
    // Apply bias and activation function
    maxValue = maxValue + biasValue; 
    apply_activation(maxValue);       
    
    // Store the result in the output buffer
    output[outputIdx] = maxValue;
}
