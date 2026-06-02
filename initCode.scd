s.waitForBoot({
    ~scopeListener.free;
    ~synthSignal.free;
    s.sync;

    ~scopeListener = {
        var src = In.ar(0, 2);
        XYScope.ar(src[0], src[1]);
        0.0;
    }.play(target: s, addAction: \toTail);

    (
    ~synthSignal = {
        var freq = 120;
        var lfo = SinOsc.ar(0.3).range(0.99, 1.01);
        var mod = SinOsc.ar(freq * 2) * Line.kr(0, 1, 5);
        var x = SinOsc.ar(freq * lfo + mod);
        var y = SinOsc.ar(freq * 1.5 + (mod * 0.5));
        
        Out.ar(0, [x, y] * 0.3);
    }.play;
    );
});