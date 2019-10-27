# A distance estimation renderer

A renderer which uses distance estimation in order to draw shapes and fractals. This is a demonstration of the Julia set:

NOTE: the GIF encoding introduced some artefacts such as banding and noise due to keeping the file size small.

![Render](./julia_preview.gif)

## Running the application

### Dependencies

* A C++ 17 compiler: tested using g++ 9
* Cmake: minimum version 3.11
* opengl + glew: supports version 3.0 es or above
* Qt5: developed using 5.1.2 (Available [here](https://www.qt.io/download-qt-installer?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4), requires compiler tools to be selected during installation to work with cmake)

### Compiling and running

```
git clone https://github.com/Abstract-Everything/distance-estimator-renderer.git
mkdir distance-estimator-renderer/build
cd distance-estimator-renderer/build
cmake -DCMAKE_BUILD_TYPE="Release" -G "Unix Makefiles" ..
make
./bin/viewer
```

### Using the application

The application currently allows the user to choose a shape to render and tweak some exposed variables.
The shape can be chosen by a drop-down in the upper right corner.
The variables can be changed once a shape has been chosen.
Each shape exposes its own set of variables together with some other ones shared between shapes.
These are previewed in the above GIF.

2D controls:
* W, A, S, D / Drag mouse with left click to move camera.
* Mouse scroll to zoom.

3D controls:
* W, A, S, D, space, control to move camera.
* Mouse with left click drag to look around.

Use shift and control to speed up or slow down the camera movement speed.

### Acknowledgements

This was inspired by the fractal series by [Syntopia](http://blog.hvidtfeldts.net/index.php/2011/06/distance-estimated-3d-fractals-part-i/), refer to the latest blog post for more resources on the subject.
