# MultiFocalStereoDisplay

A tool for rendering a scene across four displays in a multi-focal stereo setup.

##Building
Use the premake file to generate the build files for your system using the provided .bat or .sh files.
If running on Linux run './LinuxBuild/makeRun.sh' to build and immediately run the program.


##Running

This codebase does not have scripting support, so unfortunately all experimental setups must be configured in the code.

In the file, main.cpp, there is a number of functions for contructing  series of experiments which build a stack fo scenes, with different types of input to allow testing.

For example:
  AddComparisonComfort( &TC,  eStereo_Both, eStereo_Both , eEye_Sep, eEye_Sep, eScene_Translation, eBlend_Projective , eBlend_Near			, eProj_Oblique, eProj_Oblique, 1000.f);
  
This function adds a Comfort Comparison test to the Test Controller 'TC'. Which is comparing the projective blend to the nearest blend and shows each scene in order for one second. Each type of comparison or test that can be added outputs its results to its own .csv when ran.

An example of a number of these tests being construct can be found in .src/TestSetup.Example to help guide you through the construction process.

The three inputs used in the default screen are ',', '.' and '/' but these can be changed easily in '·/src/main.cpp'

##Screen Setup
This code relies on the X11 Window controller being ran with multiple heads and the screen in positions 1-4 being the four screens of the multi-focal display.  

Left Near = 4
Left Far = 3
Right Near = 1
Right Far = 2

If there are not configured correctly you will see the error: 'no appropriate visual found' at start up and the program will end.

##Rendering Style
The fragment shader './Shaders/frag_persp_MRT.glsl' contains the code for lighting and rendering the scenes. It is very basic at the moment with most functionality disabled but can be extended however you like as long as the blending is maintained. Ideally the blending would be separated into their own shaders for each blend but we ran out of time on the project and this is functional and not limiting for the scenes we were rendering.

##Setup scenes.
Each scene is an index that is represented by the enum in src/TestController.h. They are implemented in './src/RenderScene.cpp' and are free to be modified. They use objects, materials and shaders stored which are loaded at startup. Certain scenes read information from the provided TestController setup. 

To add a new scene, add it to the list in 'src/testcontoller.h', implement its update behaviour like the others scenes in TestContoller::Update(...), add a function to store it on the list (such as AddComparisonComfort) and then implement the 3D objects in RenderScene.cpp.

##Calibration
There is a calibration file stored in Config which stores the current placement of each view on each screen. This is position and scale. To adjust these while running the program use the num keys to select each screen (1-4), or each side (Left 8, right 9, Both 0) and adjust using T,G,F,H for up, down, left and right. Then press 'P' to save the configuration. These vary from person to person, it is recommended to keep a backup for each for faster startup.



##TODO
The render code is very quickly written. Needs tidying and sorting to be not terribly inefficient.

