#SuperCollider plugin XYScope (GLFW)

## dependecies

	- GLFW
	- SuperCollider

## build

```bash
	git clone https://github.com/K0F/GLFW_SuperCollider_XYScope.git
	cd GLFW_Supecollider_XYScope
	make
	cd ..
	mv GLFW_SuperCollider_XYScope SuperCollider/Extensions/Directory
```	

You need to copy folder to any location SuperCollider sees the Extensions... on linux usually: `$HOME/.local/share/SuperCollider/Extensions`.


## to run NodeProxies on XYScope:

```supercollider
s.boot;
p.push();

p = ProxySpace.push(s);

//// global listener //////////////////////

(
~globalScopeListener.clear;
~globalScopeListener.ar(2);
(
~globalScopeListener = {
    var masterOuts = In.ar(0, 2);
    XYScope.ar(masterOuts[0], masterOuts[1]);
    0.0;
};
~globalScopeListener.play(to: \end);

)

//// testing synth ///////////////////////////
(
~one.fadeTime=5;
~one = {
    var base = 220 * (2**([1,7]/12+1));
	var sig = SinOsc.ar(base,[0,pi]) * Pulse.ar(4);
	
    sig = GVerb.ar(sig,60,3)/16+sig;
    Out.ar(0,Splay.ar(sig,1,0.3));
};
~one.play;
)
```
