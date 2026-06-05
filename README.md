# GLFW SuperCollider XY(Z) Scope

[![C/C++ CI](https://github.com/K0F/GLFW_SuperCollider_XYScope/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/<OWNER>/<REPOSITORY>/actions/workflows/c-cpp.yml)

Hardware-accelerated XY(Z) vector oscilloscope for SuperCollider using GLFW.

![scope_viewer](https://github.com/K0F/GLFW_SuperCollider_XYScope/blob/main/XYScope.jpg?raw=true)

# demo
<p align="center">
  <a href="https://www.youtube.com/watch?v=X6hnJ_Xu4L0">    <img src="https://img.youtube.com/vi/X6hnJ_Xu4L0.jpg" alt="Watch the video" width="70%">
  </a>
</p>

## Requirements

* SuperCollider server headers (`SC_PlugIn.h`)
* GLFW3
* OpenGL

## Build & Install

```bash
	./configure
	make
	make install
```
Sorry tested on Linux only (it should not be very difficult to port it to other OSes).

Now recompile the Supercollider libraries, it should tell you something about plugin loaded on SuperCollider server boot.

## Run scope_viewer

```bash
	./scope_viewer
```

There are some arguments you can run scope_viewer with:

	-r sets target FPS rate
	-s set rectangular side size
	-3d turns the third dimmension on
	-a if flag -3d is on, this will disable autorotate by Y axis 


## Usage (SuperCollider):


```supercollider
	s.waitForBoot({
	    ~scope = {
	        var src = In.ar(0, 3);
	        XYScope.ar(src[0], src[1], src[3]); // only if you feed 3rd dimmension into graph
	        0.0;
	    }.play(target: s, addAction: \toTail);
	
	    ~sig = {
	        var x = SinOsc.ar(100);
	        var y = SinOsc.ar(150);
	        var z = SinOsc.ar(75);

	        Out.ar(0, [x, y, z] * 0.3);
	    }.play;
	});
```
