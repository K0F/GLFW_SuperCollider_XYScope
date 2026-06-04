# GLFW_SuperCollider_XYScope

[![C/C++ CI](https://github.com/K0F/GLFW_SuperCollider_XYScope/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/<OWNER>/<REPOSITORY>/actions/workflows/c-cpp.yml)

Hardware-accelerated XY vector oscilloscope for SuperCollider using GLFW and OpenGL.

![scope_viewer](https://github.com/K0F/GLFW_SuperCollider_XYScope/blob/main/XYScope.jpg?raw=true)


# demo

<p align="center">
  <a href="https://youtu.be/2GhP0nvB7-0">    <img src="https://img.youtube.com/vi/2GhP0nvB7-0.jpg" alt="Watch the video" width="70%">
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

## Usage:

	There are some arguments you can run the viewer with:

	-r sets target FPS rate
	-s set rectangular side size
	-3d turns the third dimmension on
	-a if flag -3d is on, this will disable autorotate by Y axis 


```supercollider
	s.waitForBoot({
	    ~scope = {
	        var src = In.ar(0, 2);
	        XYScope.ar(src[0], src[1]);
	        0.0;
	    }.play(target: s, addAction: \toTail);
	
	    ~sig = {
	        var x = SinOsc.ar(100);
	        var y = SinOsc.ar(150);
	        Out.ar(0, [x, y] * 0.3);
	    }.play;
	});
```
