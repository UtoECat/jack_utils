#include <math.h>
// normalize value to log[0 .. 1]
static float logn(float v, float n) {
	return log10(v)/log10(n);
}

static float logone(float v, float n) {
	return logn(v*n + 1, n)/logn(n + 1, n);
}

// log index
static size_t _logindex(size_t i, size_t s, float n) {
	float f = (float)i / (float)s;
	f = 1 - f;
	float r = logn(f*n + 1, n)/logn(n + 1.0f, n);
	r = 1 - r;
	return (size_t)(r * s);
}

#define DWIN static float
#define EWIN (float v, float s)

DWIN _wrect (float, float) {return 1;}
DWIN _wgausse EWIN {
	float a = (s-1)/2.0;
	float t = (v-a)/(0.5*a);
	t *= t;
	return exp(-t/2);
}

static inline float _wnorm EWIN {
	return (2*PI*v)/(s-1);
}

DWIN _whamming EWIN {return 0.54 - 0.46*cos(_wnorm(v, s));}
DWIN _whann    EWIN {return 0.5*(1-cos(_wnorm(v, s))); }

#undef DWIN
#undef EWIN

static float (*windows[]) (float, float) = {
	_wrect, _wgausse, _whamming, _whann
};
