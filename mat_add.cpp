#include <iostream>
#include <GLES2/gl2.h>

using namespace std;

void matrix_addition(float **A, float **B, float **C, int m, int n) {

  #ifdef HAVE_OPEN_GLES

  // Create a GLSL program.
  GLuint program = glCreateProgram();

  // Load the vertex shader.
  const char *vertexShaderSource =
    "#version 300 es\n"
    "in vec2 position;\n"
    "void main() {\n"
    "  gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

  glShaderSource(program, 1, &vertexShaderSource, NULL);
  glCompileShader(program);

  // Load the fragment shader.
  const char *fragmentShaderSource =
      "precision mediump float;\n"
      "uniform mat4 matrixA;\n"
      "uniform mat4 matrixB;\n"
      "out vec4 color;\n"
      "void main() {\n"
      "  color = matrixA + matrixB;\n"
      "}\n";
  glShaderSource(program, 1, &fragmentShaderSource, NULL);
  glCompileShader(program);

  // Link the program.
  glLinkProgram(program);

  // Get the location of the "matrixA" uniform.
  GLint matrixALocation = glGetUniformLocation(program, "matrixA");

  // Get the location of the "matrixB" uniform.
  GLint matrixBLocation = glGetUniformLocation(program, "matrixB");

  // Set the values of the uniforms.
  glUniformMatrix4fv(matrixALocation, 1, GL_FALSE, A[0]);
  glUniformMatrix4fv(matrixBLocation, 1, GL_FALSE, B[0]);

  // Draw a quad.
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Read back the result from the framebuffer
  GLfloat pixelData[m * n];
  glReadPixels(0, 0, m, n, GL_RGBA, GL_FLOAT, pixelData);

  // Store the result in matrix C
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      C[i][j] = pixelData[i * n + j];
    }
  }

  // Clean up OpenGL resources
  glDeleteProgram(program);
  #endif
}

int main() {
  int m = 3, n = 3;

  float **A = new float *[m];
  float **B = new float *[m];
  float **C = new float *[m];

  for (int i = 0; i < m; i++) {
    A[i] = new float[n];
    B[i] = new float[n];
    C[i] = new float[n];
  }

  // Initialize matrices A and B
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      A[i][j] = i * j;
      B[i][j] = i + j;
    }
  }

  matrix_addition(A, B, C, m, n);

  // Print matrix C
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < n; j++) {
      cout << C[i][j] << " ";
    }
    cout << "\n";
  }
}
