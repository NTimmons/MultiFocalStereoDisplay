-- A solution contains projects, and defines the available configurations
solution "MultiFocalDisplay"
   configurations { "Debug", "Release" }
 
   -- A project defines one build target
   project "MultiFocalDisplay"

      kind "ConsoleApp"
      language "C++"
      files { "../src/**.h", "../src/**.cpp" }
	  libdirs { "../lib/GLEW/lib", "../lib/assimp-3.1.1/lib","../lib/glfw-3.1.2/lib", "../lib" }
	  flags{ "ExtraWarnings"}
	  buildoptions {"-std=c++11 -ggdb -pthread"}
	  targetdir ("../output")
	  objdir ("../output/obj")
	  includedirs { "../lib/glfw-3.1.2/include",  "../include/", "../lib/glfw-3.1.2/deps"}
	  
	  configuration "windows"
	         location "../WindowsBuild"
		 links { "glut", "glew32", "opengl32"}
 
	  configuration "linux"
	         location "../LinuxBuild"
		 links { "GLEW", "pthread", "assimp", "glut", "GL", "GLU"}
 
      configuration "Debug"
		 targetsuffix "d"
         defines { "DEBUG" }
         flags { "Symbols" }
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }  
