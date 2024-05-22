:: ..\..\..\downloads\VulkanSDK\1.3.280.0\Bin\glslc.exe simple_shader.vert -o simple_shader.vert.spv
:: ..\..\..\downloads\VulkanSDK\1.3.280.0\Bin\glslc.exe simple_shader.frag -o simple_shader.frag.spv

echo Shader compilation
set currentDir=%0\..
glslc.exe %currentDir%\simple_shader.vert -o %currentDir%\simple_shader.vert.spv
glslc.exe %currentDir%\simple_shader.frag -o %currentDir%\simple_shader.frag.spv

if [%1] == [] (
	pause 
	exit
)
set shaderBuildDir=%1\shaders
if not exist %shaderBuildDir% mkdir %shaderBuildDir%
xcopy /f /y %currentDir%\simple_shader.vert.spv %shaderBuildDir%
xcopy /f /y %currentDir%\simple_shader.frag.spv %shaderBuildDir%