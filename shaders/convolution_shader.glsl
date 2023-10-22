#version 460

layout(local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout(set = 0, binding = 0) buffer InputBuffer {
    float input[]; // Input data format
} inputBufferID;

layout(set = 0, binding = 1) buffer WeightsBuffer {
    float weights[]; // Weights data format (consider grouping)
} weightBufferID;

layout(set = 0, binding = 2) buffer OutputBuffer {
    float output[]; // Output data format
} outputBufferID;

uniform int inputWidth; // Width of the input image
uniform int inputHeight; // Height of the input image
uniform int kernelSize; // Size of the convolution kernel (e.g., 3x3)
uniform int outputWidth; // Width of the output feature map
uniform int outputHeight; // Height of the output feature map
uniform int batchSize; // Batch size

void main() {   
    // Calculate the global index within the output feature map
    uvec3 outputIndex = gl_GlobalInvocationID;

    // Calculate the corresponding batch, input region, and grouping
    int batchIdx = int(outputIndex.z);
    int startX = int(outputIndex.x) * int(outputWidth);
    int startY = int(outputIndex.y) * int(outputHeight);

    float sum = 0.0;

    // Perform the convolution
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            int inputX = startX + i;
            int inputY = startY + j;

            // Perform boundary checks to ensure we're within the input image
            if (inputX >= 0 && inputX < inputWidth && inputY >= 0 && inputY < inputHeight) {
                // Access input and weights, perform multiplication, and accumulate
                float inputValue = inputBuffer.input[batchIdx * inputWidth * inputHeight + inputY * inputWidth + inputX];
                float weightValue = weightsBuffer.weights[batchIdx * (kernelSize * kernelSize) * (l.c / l.groups) + i * kernelSize + j];
                sum += inputValue * weightValue;
            }
        }
    }

    // Write the result to the output buffer
    outputBuffer.output[batchIdx * outputWidth * outputHeight + outputIndex.y * outputWidth + outputIndex.x] = sum;
}
