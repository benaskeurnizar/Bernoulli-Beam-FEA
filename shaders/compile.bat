@echo off
echo Compiling shaders...
"C:\VulkanSDK\1.4.350.0\Bin\glslc.exe" shader.vert -o vert.spv
"C:\VulkanSDK\1.4.350.0\Bin\glslc.exe" shader.frag -o frag.spv
"C:\VulkanSDK\1.4.350.0\Bin\glslc.exe" lineShader.vert -o lineVert.spv
"C:\VulkanSDK\1.4.350.0\Bin\glslc.exe" lineShader.frag -o lineFrag.spv

echo done!
