#version 300 es
precision mediump float;
precision lowp int;
precision mediump sampler2D;
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

#ifdef HAVE_OPEN_GLES

void im2col_get_pixel(int inputTexture,int height, int width,int channel, int row, int col,  int pad) {
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
