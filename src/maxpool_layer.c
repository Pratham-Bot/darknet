#include "maxpool_layer.h"
#include "cuda.h"
#include <stdio.h>

image get_maxpool_image(maxpool_layer l)
{
    int h = l.out_h;
    int w = l.out_w;
    int c = l.c;
    return float_to_image(w,h,c,l.output);
}

image get_maxpool_delta(maxpool_layer l)
{
    int h = l.out_h;
    int w = l.out_w;
    int c = l.c;
    return float_to_image(w,h,c,l.delta);
}

maxpool_layer make_maxpool_layer(int batch, int h, int w, int c, int size, int stride, int padding)
{
    maxpool_layer l = {0};
    l.type = MAXPOOL;
    l.batch = batch;
    l.h = h;
    l.w = w;
    l.c = c;
    l.pad = padding;
    l.out_w = (w + padding - size)/stride + 1;
    l.out_h = (h + padding - size)/stride + 1;
    l.out_c = c;
    l.outputs = l.out_h * l.out_w * l.out_c;
    l.inputs = h*w*c;
    l.size = size;
    l.stride = stride;
    int output_size = l.out_h * l.out_w * l.out_c * batch;
    l.indexes = calloc(output_size, sizeof(int));
    l.output =  calloc(output_size, sizeof(float));
    l.delta =   calloc(output_size, sizeof(float));
    l.forward = forward_maxpool_layer;
    l.backward = backward_maxpool_layer;
    #ifdef GPU
    l.forward_gpu = forward_maxpool_layer_gpu;
    l.backward_gpu = backward_maxpool_layer_gpu;
    l.indexes_gpu = cuda_make_int_array(0, output_size);
    l.output_gpu  = cuda_make_array(l.output, output_size);
    l.delta_gpu   = cuda_make_array(l.delta, output_size);
    #endif
    fprintf(stderr, "max          %d x %d / %d  %4d x%4d x%4d   ->  %4d x%4d x%4d\n", size, size, stride, w, h, c, l.out_w, l.out_h, l.out_c);
    return l;
}

void resize_maxpool_layer(maxpool_layer *l, int w, int h)
{
    l->h = h;
    l->w = w;
    l->inputs = h*w*l->c;

    l->out_w = (w + l->pad - l->size)/l->stride + 1;
    l->out_h = (h + l->pad - l->size)/l->stride + 1;
    l->outputs = l->out_w * l->out_h * l->c;
    int output_size = l->outputs * l->batch;

    l->indexes = realloc(l->indexes, output_size * sizeof(int));
    l->output = realloc(l->output, output_size * sizeof(float));
    l->delta = realloc(l->delta, output_size * sizeof(float));

    #ifdef GPU
    cuda_free((float *)l->indexes_gpu);
    cuda_free(l->output_gpu);
    cuda_free(l->delta_gpu);
    l->indexes_gpu = cuda_make_int_array(0, output_size);
    l->output_gpu  = cuda_make_array(l->output, output_size);
    l->delta_gpu   = cuda_make_array(l->delta,  output_size);
    #endif
}

void forward_maxpool_layer(const maxpool_layer l, network net)
{
    int b,i,j,k,m,n;
    int w_offset = -l.pad/2;
    int h_offset = -l.pad/2;

    int h = l.out_h;
    int w = l.out_w;
    int c = l.c;

    for(b = 0; b < l.batch; ++b){
        for(k = 0; k < c; ++k){
            for(i = 0; i < h; ++i){
                for(j = 0; j < w; ++j){
                    int out_index = j + w*(i + h*(k + c*b));
                    float max = -FLT_MAX;
                    int max_i = -1;
                    for(n = 0; n < l.size; ++n){
                        for(m = 0; m < l.size; ++m){
                            int cur_h = h_offset + i*l.stride + n;
                            int cur_w = w_offset + j*l.stride + m;
                            int index = cur_w + l.w*(cur_h + l.h*(k + b*l.c));
                            int valid = (cur_h >= 0 && cur_h < l.h &&
                                         cur_w >= 0 && cur_w < l.w);
                            float val = (valid != 0) ? net.input[index] : -FLT_MAX;
                            max_i = (val > max) ? index : max_i;
                            max   = (val > max) ? val   : max;
                        }
                    }
                    l.output[out_index] = max;
                    l.indexes[out_index] = max_i;
                }
            }
        }
    }
}

void forward_maxpool_layer_opengl(const maxpool_layer l, network net) {  
    
    int poolSize = l.size;     
    int stride = l.stride;     
    int padding = l.pad;       

    GLuint inputBufferID;
    GLuint outputBufferID;

    glGenBuffers(1, &inputBufferID);
    glGenBuffers(1, &outputBufferID);

    char* vertexShaderSource = LoadShaderFromFile("shader/vertex_shader.glsl");
    char* fragmentShaderSource = LoadShaderFromFile("shader/fragment_shader.glsl");

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint compileStatus;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) {
        GLint infoLogLength;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* infoLog = (char*)malloc(infoLogLength);
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Vertex shader compilation error:\n%s\n", infoLog);
        free(infoLog);
    }


    glShaderSource(fragmentShader, 1, & fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE){
        GLint infoLogLength;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* infoLog = (char*)malloc(infoLogLength);
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Fragment shader compilation error:\n%s\n", infoLog);
        free(infoLog);

    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint linkStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        char* infoLog = (char*)malloc(infoLogLength);
        glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, infoLog);
        printf("Shader program linking error:\n%s\n", infoLog);
        free(infoLog);
        
        
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glBindBuffer(GL_ARRAY_BUFFER, inputBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, outputBufferID);
                    
    glBufferData(GL_ARRAY_BUFFER, inputBufferID, NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, outputBufferID, NULL, GL_STATIC_DRAW);

    glUseProgram(shaderProgram);


    char* computeShaderSource = LoadShaderFromFile("shader/maxpool_compute_shader.glsl");
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    glCompileShader(computeShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, computeShader);
    glLinkProgram(shaderProgram);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBufferID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBufferID);

    glUniform1i(glGetUniformLocation(shaderProgram, "size"), l.size);
    glUniform1i(glGetUniformLocation(shaderProgram, "stride"), l.stride);
    glUniform1i(glGetUniformLocation(shaderProgram, "padding"), l.pad);

    int workgroupSizeX = 32;  
    int workgroupSizeY = 1;
    int workgroupSizeZ = 1;
    int numWorkgroupsX = (l.out_w + workgroupSizeX - 1) / workgroupSizeX;
    int numWorkgroupsY = (l.out_h + workgroupSizeY - 1) / workgroupSizeY;
    int numWorkgroupsZ = l.c;

    glDispatchCompute(numWorkgroupsX, numWorkgroupsY, numWorkgroupsZ);
    glUseProgram(0); // Unbind the shader program

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    glUseProgram(0);
}


void backward_maxpool_layer(const maxpool_layer l, network net)
{
    int i;
    int h = l.out_h;
    int w = l.out_w;
    int c = l.c;
    for(i = 0; i < h*w*c*l.batch; ++i){
        int index = l.indexes[i];
        net.delta[index] += l.delta[i];
    }
}

