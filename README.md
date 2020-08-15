# A distance estimation renderer

A renderer which uses distance estimation in order to draw shapes and fractals. This is a demonstration of the Julia set:

NOTE: the GIF encoding introduced some artefacts such as banding and noise due to keeping the file size small.

![Render](./julia_preview.gif)

## Building the application

### Ubuntu

Make sure that the following packages are installed:

```
sudo apt install build-essential
sudo apt install cmake
sudo apt install extra-cmake-modules
sudo apt install libglew-dev
sudo apt install qtdeclarative5-dev 
sudo apt install qml-module-qtquick2
sudo apt install qml-module-qtquick-layouts
sudo apt install qml-module-qtquick-controls
sudo apt install qml-module-qtquick-controls2
```

Build and run:

```
git clone https://github.com/Abstract-Everything/distance-estimator-renderer.git
mkdir distance-estimator-renderer/build
cd distance-estimator-renderer/build
cmake -DCMAKE_BUILD_TYPE="Release" -G "Unix Makefiles" ..
make
./bin/viewer
```

### Windows

Install [Visual Studio](https://visualstudio.microsoft.com/downloads/) with C++ development support.
Install [Qt5](https://www.qt.io/download-thank-you)
Install [CMake](https://cmake.org/download/).
Install [GLEW](http://glew.sourceforge.net/)

Build and run:

Launch CMake, select the source and build folder and press configure.
Choose the installation paths for Qt5 and GLEW.
Generate a visual studio solution.
Use visual studio to build and run.


Note: If the target machine does not have a graphics card available a software renderer can be used such as Mesa 3D.

### Using the application

The application currently allows the user to choose a shape to render and tweak some exposed variables.
The shape can be chosen by a drop-down in the upper right corner.
The variables can be changed once a shape has been chosen.
Each shape exposes its own set of variables together with some other ones shared between shapes.
These are previewed in the above GIF.

2D controls:
* W, A, S, D / Drag mouse with left click to move camera.

3D controls:
* W, A, S, D, space, control to move camera.
* Mouse with left click drag to look around.

### Acknowledgements

This was inspired by the fractal series by [Syntopia](http://blog.hvidtfeldts.net/index.php/2011/06/distance-estimated-3d-fractals-part-i/), refer to the latest blog post for more resources on the subject.
