#include "im2col.h"
#include <stdio.h>
float im2col_get_pixel(float *im, int height, int width, int channels,
                        int row, int col, int channel, int pad)
{
    row -= pad;
    col -= pad;

    if (row < 0 || col < 0 ||
        row >= height || col >= width) return 0;
    return im[col + width*(row + height*channel)];
}

//From Berkeley Vision's Caffe!
//https://github.com/BVLC/caffe/blob/master/LICENSE
void im2col_cpu(float* data_im,
     int channels,  int height,  int width,
     int ksize,  int stride, int pad, float* data_col) 
{
    int c,h,w;
    int height_col = (height + 2*pad - ksize) / stride + 1;
    int width_col = (width + 2*pad - ksize) / stride + 1;

    int channels_col = channels * ksize * ksize;
    for (c = 0; c < channels_col; ++c) {
        int w_offset = c % ksize;
        int h_offset = (c / ksize) % ksize;
        int c_im = c / ksize / ksize;
        for (h = 0; h < height_col; ++h) {
            for (w = 0; w < width_col; ++w) {
                int im_row = h_offset + h * stride;
                int im_col = w_offset + w * stride;
                int col_index = (c * height_col + h) * width_col + w;
                data_col[col_index] = im2col_get_pixel(data_im, height, width, channels,
                        im_row, im_col, c_im, pad);
            }
        }
    }
}

#ifdef HAVE_OPEN_GLES

#version 300 es

precision highp float;

// Input variables
in vec2 texCoord;

// Output variable
out float result;

// Uniforms
uniform sampler2D inputTexture;
uniform int channels;
uniform int height;
uniform int width;
uniform int ksize;
uniform int stride;
uniform int pad;

void im2col_get_pixel(sampler2D inputTex, int row, int col, int channel) {
    row -= pad;
    col -= pad;

    if (row < 0 || col < 0 || row >= height || col >= width) {
        return 0.0;
    }

    float texCoordX = (float(col) + 0.5) / float(width);
    float texCoordY = (float(row) + 0.5) / float(height);
    vec2 texCoord = vec2(texCoordX, texCoordY);

    // Access the input texture and retrieve the pixel value
    vec4 pixel = texture(inputTex, texCoord);

    // Return the value of the specified channel
    return pixel[channel];
}

void main() {
    int heightCol = (height + 2 * pad - ksize) / stride + 1;
    int widthCol = (width + 2 * pad - ksize) / stride + 1;

    int channelsCol = channels * ksize * ksize;
    int cIm = int(gl_FragCoord.x) / (ksize * ksize);
    int hOffset = (int(gl_FragCoord.x) / ksize) % ksize;
    int wOffset = int(gl_FragCoord.x) % ksize;

    float sum = 0.0;
    for (int h = 0; h < heightCol; ++h) {
        for (int w = 0; w < widthCol; ++w) {
            int imRow = hOffset + h * stride;
            int imCol = wOffset + w * stride;
            int colIndex = (int(gl_FragCoord.x) * heightCol + h) * widthCol + w;

            // Retrieve the pixel value from the input texture
            float pixelValue = im2col_get_pixel(inputTexture, imRow, imCol, cIm);
            sum += pixelValue;

            // Store the result in the output buffer
            result = sum;
        }
    }
}
#endif
