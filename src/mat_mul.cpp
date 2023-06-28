#include <stdlib.h>
#include <iostream>
#include <GLES2/gl2.h>

using namespace std; 

void matrix_multiplication(float **A, float **B, float **C, int m, int n, int p) {
  
  #ifdef HAVE_OPEN_GLES

  GLuint program = glCreateProgram();

  const char *vertexShaderSource =
    "#version 300 es\n"
    "in vec2 position;\n"
    "void main() {\n"
    "  gl_Position = vec4(position, 0.0, 1.0);\n"
    "}\n";

  glShaderSource(program, 1, &vertexShaderSource, NULL);
  glCompileShader(program);

  const char *fragmentShaderSource =
      "precision mediump float;\n"
      "uniform mat4 matrixA;\n"
      "uniform mat4 matrixB;\n"
      "out vec4 color;\n"
      "void main() {\n"
      "  color = matrixA * matrixB;\n"
      "}\n";
  glShaderSource(program, 1, &fragmentShaderSource, NULL);
  glCompileShader(program);

  glLinkProgram(program);

  GLint matrixALocation = glGetUniformLocation(program, "matrixA");
  GLint matrixBLocation = glGetUniformLocation(program, "matrixB");

  glUniformMatrix4fv(matrixALocation, 1, GL_FALSE, A[0]);
  glUniformMatrix4fv(matrixBLocation, 1, GL_FALSE, B[0]);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  #endif

}

int main() {
   int m = 3, n = 3, p = 3;

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

  for (int i = 0; i < n; i++) 
  {
    for (int j = 0; j < p; j++) 
    {
      A[i][j] = i * j;
    }
  }

  for (int i = 0; i < n; i++) 
  {
    for (int j = 0; j < p; j++) 
    {
      B[i][j] = i * j;
    }
  }

  for (int i = 0; i < m; i++) 
  {
  for (int j = 0; j < p; j++) 
    {
    C[i][j] = 0;
    for (int k = 0; k < n; k++) 
    {
      C[i][j] += A[i][k] * B[k][j];
    }
  }
}

  matrix_multiplication(A, B, C, m, n, p);

  for (int i = 0; i < m; i++) 
  {
    for (int j = 0; j < p; j++) 
    {
      cout << C[i][j] << " ";
    }
    cout << "\n";
  }

  return 0;
}