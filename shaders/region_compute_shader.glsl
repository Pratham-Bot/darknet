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

// constants
uniform int inputWidth;
uniform int inputHeight;
uniform int inputChannels;
uniform int numBoxes;  // Number of region boxes
uniform int coords;    // Number of coordinates per box (usually 4)
uniform int classes;   // Number of classes

void main() {
    ivec2 outputIndex = ivec2(gl_GlobalInvocationID.xy);

    int boxIndex = outputIndex.x; // Each work item processes one box

    // Calculate the starting index for this box in the input buffer
    int startIndex = boxIndex * (coords + 1 + classes);

    // Calculate the starting index for this box in the output buffer
    int outputStartIndex = boxIndex * (coords + 1 + classes);

    // Copy the coordinates and objectness score directly from input to output
    for (int i = 0; i < coords + 1; ++i) {
        outputBuffer.output[outputStartIndex + i] = inputBuffer.input[startIndex + i];
    }

    // Apply softmax activation function to class probabilities
    float maxClassProb = inputBuffer.input[startIndex + coords + 1];
    for (int i = 1; i < classes; ++i) {
        int classIndex = startIndex + coords + 1 + i;
        maxClassProb = max(maxClassProb, inputBuffer.input[classIndex]);
    }

    float sumExp = 0.0;
    for (int i = 0; i < classes; ++i) {
        int classIndex = startIndex + coords + 1 + i;
        float expProb = exp(inputBuffer.input[classIndex] - maxClassProb);
        sumExp += expProb;
        outputBuffer.output[outputStartIndex + coords + 1 + i] = expProb;
    }

    // Normalize using the sum of exponentials
    for (int i = 0; i < classes; ++i) {
        outputBuffer.output[outputStartIndex + coords + 1 + i] /= sumExp;
    }

}
