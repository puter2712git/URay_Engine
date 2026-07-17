#version 450

layout(push_constant) uniform ObjectConstants {
    mat4 model;
} obj;

layout(binding = 0) uniform FrameConstants {
    mat4 view;
    mat4 proj;
} frame;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = frame.proj * frame.view * obj.model * vec4(inPosition, 1.0);
    fragColor = inColor;
}
