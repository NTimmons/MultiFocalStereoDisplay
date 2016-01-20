-- A solution contains projects, and defines the available configurations
solution "MultiFocalDisplay"
   configurations { "Debug", "Release" }
 
   -- A project defines one build target
   project "MultiFocalDisplay"
      kind "ConsoleApp"
      language "C++"
      files { "../src/**.h", "../src/**.cpp" }
	  includedirs { "../lib/glfw-3.1.2/include", "../include/", "../lib/glfw-3.1.2/deps"}
	  libdirs { "../lib/GLEW/lib", "../lib/glfw-3.1.2/lib" }
	  targetdir ("../output")
	  objdir ("../obj")
	  
	  configuration "windows"
		 links { "glfw3", "glew32", "opengl32"}
 
	  configuration "linux"
		 links { "glfw3", "glew32", "GL"}
 
      configuration "Debug"
		 targetsuffix "-d"
         defines { "DEBUG" }
         flags { "Symbols" }
 
      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }  