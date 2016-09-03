solution "Shapes"

configurations { "Release", "Debug" }

project "Shapes"

	language "C++"
	kind "consoleApp"
	
	files "src/*"
	flags "StaticRuntime"
	
	defines { "ASSETSDIR=\"assets/\"" }

	includedirs { "C:/SDL/include", "C:/SDLU/include" }
	links { "C:/SDL/lib/SDL2.lib", "C:/SDL/lib/SDL2main.lib", "C:/SDLU/lib/SDLU.lib"}
