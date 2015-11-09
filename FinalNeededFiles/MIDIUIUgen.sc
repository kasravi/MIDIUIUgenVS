MIDIUIUgen : UGen
{
	// warp 0 = linear
	// warp 1 = exponential
	*kr {
		arg device=0, type=176, channel=0, minval=0, maxval=127, warp=0, lag=0.2;
		if (warp === \linear, { warp = 0 });
		if (warp === \exponential, { warp = 1 });

		^this.multiNew('control',device,type,channel, minval, maxval, warp, lag)
	}
	signalRange { ^\unipolar }
}
