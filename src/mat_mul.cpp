#include <stdlib.h>
#include <iostream>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <benchmark/benchmark.h>

using namespace std;

void matrix_multiplication(float **A, float **B, float **C, int m, int n, int p) {
  // Perform matrix multiplication calculations
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < p; j++) {
      C[i][j] = 0;
      for (int k = 0; k < n; k++) {
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}

void initializeShader(GLuint program) {
  const char *vertexShaderSource =
      "#version 300 es\n"
      "in vec2 position;\n"
      "void main() {\n"
      "  gl_Position = vec4(position, 0.0, 1.0);\n"
      "}\n";

  glShaderSource(program, 1, &vertexShaderSource, NULL);
  glCompileShader(program);

  const char *fragmentShaderSource =
      "#version 300 es\n"
      "precision mediump float;\n"
      "uniform mat4 matrixA;\n"
      "uniform mat4 matrixB;\n"
      "out vec4 color;\n"
      "void main() {\n"
      "  mat4 result = matrixA * matrixB;\n"
      "  color = vec4(result[0][0], result[1][1], result[2][2], result[3][3]);\n"
      "}\n";
  glShaderSource(program, 1, &fragmentShaderSource, NULL);
  glCompileShader(program);

  glLinkProgram(program);
}

static void BM_MatrixMultiplication(benchmark::State &state) {
  int m = state.range(0);
  int n = state.range(1);
  int p = state.range(2);

  float **A = (float **)malloc(sizeof(float *) * m);
  for (int i = 0; i < m; i++) {
    A[i] = (float *)malloc(sizeof(float) * n);
  }

  float **B = (float **)malloc(sizeof(float *) * n);
  for (int i = 0; i < n; i++) {
    B[i] = (float *)malloc(sizeof(float) * p);
  }

  float **C = (float **)malloc(sizeof(float *) * m);
  for (int i = 0; i < m; i++) {
    C[i] = (float *)malloc(sizeof(float) * p);
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < p; j++) {
      A[i][j] = i * j;
    }
  }

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < p; j++) {
      B[i][j] = i * j;
    }
  }

 // Create and initialize the shader program
  GLuint program = glCreateProgram();
  initializeShader(program);

  // Get the uniform locations for the matrices
  GLint matrixALocation = glGetUniformLocation(program, "matrixA");
  GLint matrixBLocation = glGetUniformLocation(program, "matrixB");

  // Run the benchmark
  for (auto _ : state) {
    // Perform OpenGL ES matrix multiplication using the shader program
    glUseProgram(program);
    glUniformMatrix4fv(matrixALocation, 1, GL_FALSE, &A[0][0]);
    glUniformMatrix4fv(matrixBLocation, 1, GL_FALSE, &B[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  // Free the allocated memory
  for (int i = 0; i < m; i++) {
    free(A[i]);
    free(C[i]);
  }
  free(A);
  free(C);
}

BENCHMARK(BM_MatrixMultiplication)
    ->Args({100, 100, 100})  // Arguments for matrix size
    ->Args({200, 200, 200})
    ->Args({300, 300, 300});

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  return 0;
}
