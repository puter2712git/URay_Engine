~/VulkanSDK/1.4.350.0/macOS/bin/dxc -spirv -T vs_6_0 -E VSMain shader.hlsl -Fo vert.spv
~/VulkanSDK/1.4.350.0/macOS/bin/dxc -spirv -T ps_6_0 -E PSMain shader.hlsl -Fo frag.spv