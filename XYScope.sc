XYScope : UGen {
    *ar { arg bufnum, phase = 0.0;
        ^this.multiNew('audio', bufnum, phase)
    }

    initOutputs { arg numChannels, rate;
        // Explicitly create and return an array of OutputProxies
        ^[
            OutputProxy(rate, this, 0),
            OutputProxy(rate, this, 1)
        ];
    }
}
