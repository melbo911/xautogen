
# Xautogen

Xautogen modifies the X-Plane default autogen housess to hide the ground textures. This might be usefull if you use Ortho SAT images as ground textures while still using the default autogen sceneries instead senery packages like Simheaven's X-World.

Xautogen creates a new folder "Custom Scenery/Xautogen" and fills the folder with modified AGS and AGB files from "default scenery/1000 autogen". All other required files are symlinked to the default scenery, so not requiring any additional diskspace. 

Because no files belonging to X-Plane are modified, running updates on X-Plane will not break anything.

Creation of symlinks on Windows requires elevated rights, so you need the EXE to "run as administrator". 

Xautogen works with X-Plane 11 and X-Plane 12 on Windows, Linux and macOS.

Compiling xautogen on Linux or macOS can be done by simply running "make xautogen".







