#include <iostream>
#include <GLES2/gl2.h>
#include <benchmark/benchmark.h>

// Shader source code for addition
const char *vertexShaderSource = R"(
    attribute vec4 position;
    void main() {
        gl_Position = position;
    }
)";

const char *fragmentShaderSource = R"(
    precision mediump float;
    uniform vec2 matrixSize;
    uniform sampler2D matrixA;
    uniform sampler2D matrixB;
    void main() {
        vec2 texCoord = gl_FragCoord.xy / matrixSize;
        vec4 colorA = texture2D(matrixA, texCoord);
        vec4 colorB = texture2D(matrixB, texCoord);
        gl_FragColor = colorA + colorB;
    }
)";

// Function to compile and link shaders
GLuint createShaderProgram() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    return program;
}

// Benchmark function for matrix addition
static void BM_MatrixAddition(benchmark::State &state) {
    int matrixSize = state.range(0); // Size of the square matrix

    // Create two input matrices with random values
    std::vector<float> matrixDataA(matrixSize * matrixSize);
    std::vector<float> matrixDataB(matrixSize * matrixSize);
    for (int i = 0; i < matrixSize * matrixSize; i++) {
        matrixDataA[i] = static_cast<float>(rand()) / RAND_MAX;
        matrixDataB[i] = static_cast<float>(rand()) / RAND_MAX;
    }

    // Initialize OpenGL
    GLuint program = createShaderProgram();
    glUseProgram(program);

    GLuint matrixSizeLoc = glGetUniformLocation(program, "matrixSize");
    glUniform2f(matrixSizeLoc, static_cast<float>(matrixSize), static_cast<float>(matrixSize));

    // Create and bind textures for the input matrices
    GLuint textureA, textureB;
    glGenTextures(1, &textureA);
    glBindTexture(GL_TEXTURE_2D, textureA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, matrixSize, matrixSize, 0, GL_RGBA, GL_FLOAT, matrixDataA.data());
  
    glGenTextures(1, &textureB);
    glBindTexture(GL_TEXTURE_2D, textureB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, matrixSize, matrixSize, 0, GL_RGBA, GL_FLOAT, matrixDataB.data());

    GLuint matrixALoc = glGetUniformLocation(program, "matrixA");
    glUniform1i(matrixALoc, 0); // Use texture unit 0 for matrix A

    GLuint matrixBLoc = glGetUniformLocation(program, "matrixB");
    glUniform1i(matrixBLoc, 1); // Use texture unit 1 for matrix B

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureA);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureB);

    for (auto _ : state) {
        // Perform the matrix addition by drawing a full-screen quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // Clean up
    glDeleteTextures(1, &textureA);
    glDeleteTextures(1, &textureB);
    glDeleteProgram(program);
}

// Register the benchmark
BENCHMARK(BM_MatrixAddition)->Arg(256)->Arg(512)->Arg(1024);

int main(int argc, char **argv) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    return 0;
}
