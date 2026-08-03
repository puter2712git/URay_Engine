~/VulkanSDK/1.4.350.0/macOS/bin/dxc -spirv -T vs_6_0 -E VSMain shader.hlsl -Fo vert.spv
~/VulkanSDK/1.4.350.0/macOS/bin/dxc -spirv -T ps_6_0 -E PSMain shader.hlsl -Fo frag.spv
~/VulkanSDK/1.4.350.0/macOS/bin/dxc -spirv -T vs_6_0 -E VSMain line.hlsl -Fo line-vert.spv
~/VulkanSDK/1.4.350.0/macOS/bin/dxc -spirv -T ps_6_0 -E PSMain line.hlsl -Fo line-frag.spv
~/VulkanSDK/1.4.350.0/macOS/bin/dxc -spirv -T vs_6_0 -E VSMain base.hlsl -Fo base-vert.spv
~/VulkanSDK/1.4.350.0/macOS/bin/dxc -spirv -T ps_6_0 -E PSMain base.hlsl -Fo base-frag.spv