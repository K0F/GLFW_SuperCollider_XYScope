# GLFW_SuperCollider_XYScope

Hardware-accelerated XY vector oscilloscope for SuperCollider using GLFW and OpenGL.

# SuperCollider plugin XYScope (GLFW)

This is kind of obviously silly one. I had some sketches back in 0's doing practically the same in the most weird environments imaginable.

What changed since then is that, now it is fast lightweight GLFW app with a plugin for SuperCollider. It should be close to metal (running almost 120fps). There are probably many out there but still it is super handy visualiser for any synthmaker in SuperCollider.

![scope_viewer](https://github.com/K0F/GLFW_SuperCollider_XYScope/blob/main/XYScope.jpg?raw=true)


# demo

<p align="center">
  <a href="https://youtu.be/2GhP0nvB7-0">    <img src="https://img.youtube.com/vi/2GhP0nvB7-0.jpg" alt="Watch the video" width="70%">
  </a>
</p>

## dependecies

	- GLFW
	- SuperCollider

## build

```bash
	make
	make install
```	

You need to copy folder to any location SuperCollider sees the Extensions... on linux usually: `$HOME/.local/share/SuperCollider/Extensions`.

Now recompile the Supercollider libraries, it should tell you something about plugin loaded on server boot.

## run the scope_viewer

```bash
	./scope_viewer
```

## Usage:

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
