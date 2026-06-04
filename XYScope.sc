XYScope : UGen {
    *ar { arg inX, inY, inZ = 0.0;
        ^this.multiNew('audio', inX, inY, inZ)
    }
}
