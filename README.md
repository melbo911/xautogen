
# Xautogen

Xautogen hides the ground textures of the X-Plane default autogen houses. This might be usefull if you use Ortho satellite images as ground textures while still using the default XP autogen sceneries instead of detailed scenery packages like Simheaven's X-World.

Xautogen creates a new folder named "Custom Scenery/Xautogen" and fills the folder with modified AGS and AGB files from "default scenery/1000 autogen". All other files required are symlinked to the default scenery, so no additional diskspace is required.

Because no files belonging to X-Plane are modified, running software updates on X-Plane will not overwrite or break anything.

Creation of symlinks on Windows requires elevated rights, so you need the EXE to "run as administrator".

Xautogen works with X-Plane 11 and X-Plane 12 on Windows, Linux and macOS.

Compiling Xautogen on Linux or macOS can be simply done by running "make xautogen". The C-source is included in the ZIP file.



      $ ./xautogen.app -h
      Xautogen - 0.3 - melbo @x-plane.org

        usage: ./xautogen.app [-d] [-f] [-h]

        -d  increase debug level
        -f  force link creation
        -h  this help




# Installation

Copy the binary ( .exe for Windows, .app for macOS ) to your X-Plane main folder. On Windows you need to start it with right-click -> run-as-administrator.


# Uninstall

Remove the binary from your X-Plane main folder and the folder "Xautogen" from the "Custom Scenery" folder.


# Download

https://github.com/melbo911/xautogen


# Changes

0.3 initial upload




