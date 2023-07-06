#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

const char *vertexShaderSource =
      "#version 300 es\n"
      "in vec4 position;\n"
      "void main() {\n"
      "  gl_Position = position;\n"
      "}\n";

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

// Function to compile a shader
GLuint compileShader(GLenum shaderType, const char* sourceCode) {
    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &sourceCode, NULL);
    glCompileShader(shaderID);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        printf("Shader compilation failed: %s\n", infoLog);
        exit(1);
    }

    return shaderID;
}

GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Shader program linking failed: %s\n", infoLog);
        exit(1);
    }
 // Clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}



