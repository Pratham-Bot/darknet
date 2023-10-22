#version 460 es

// Define the workgroup size (adjust as needed)
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// Input buffer (3D)
layout(set = 0, binding = 0) buffer InputBuffer {
    float input[]; // Input data format
} inputBuffer;

// Output buffer (3D)
layout(set = 0, binding = 1) buffer OutputBuffer {
    float output[]; // Output data format
} outputBuffer;

// Uniforms (constants)
uniform int inputWidth;
uniform int inputHeight;
uniform int inputChannels;
uniform int poolSize;  // Pooling size (e.g., 2 for 2x2 pooling)
uniform int stride;

void main() {
    ivec2 outputIndex = ivec2(gl_GlobalInvocationID.xy);

    int inputX = outputIndex.x * stride;
    int inputY = outputIndex.y * stride;

    float maxVal = -3.402823466e+38; // Minimum representable negative value in IEEE 

    for (int i = 0; i < poolSize; ++i) {
        for (int j = 0; j < poolSize; ++j) {
            int inputPosX = inputX + i;
            int inputPosY = inputY + j;

            if (inputPosX >= 0 && inputPosX < inputWidth && inputPosY >= 0 && inputPosY < inputHeight) {
                for (int c = 0; c < inputChannels; ++c) {
                    int inputIndex = c + inputChannels * (inputPosX + inputWidth * inputPosY);
                    maxVal = max(maxVal, inputBuffer.input[inputIndex]);
                }
            }
        }
    }

    int outputIndexFlat = outputIndex.x + outputIndex.y * (inputWidth / stride);
    outputBuffer.output[outputIndexFlat] = maxVal;
}
