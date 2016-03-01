Build Instructions:

Windows Only.
Visual Studio 2015 with the C++ components installed is required.

Those components are optional, so you may need to modify your existing location.
Navigate to the src folder and run buildVS2015.bat, this will create win32_wtmap.exe in the bin folder.

The build does work with VS2013 but it requires some more work.

If you have VS2013 Installed with the C++ Components,
Open a cmd window and navigate to your Visual Studio Install directory.
It should be in program files and the folder should be called "Microsoft Visual Studio 12.0".
Navigate to the VC folder and run vcvarsall.bat.
In that same command window, navigate to the src folder near where you found this file.
Run build.bat
Run run.bat

Operation Instructions:

WASD moves the camera around.
Q zooms out, E zooms in.
Space to generate a new layout.

F12 opens a console so you can enter more detailed commands but this feature is not supported.

Key:
        Gray Tile = Floor,
        Yellow Tile = Monster, (or just..thing that moves around)
        Green Tile = Entrance,
        Red Tile = Exit

        
