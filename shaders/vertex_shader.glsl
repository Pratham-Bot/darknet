    #version 460 es
    precision highp float;
    
    // Vertex attributes
    layout(location = 0) in vec3 inPosition;
    
    void main() {
        // Simply pass through the position
        gl_Position = vec4(inPosition, 1.0);
    }