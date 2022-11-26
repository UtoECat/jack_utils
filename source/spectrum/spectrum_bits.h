#define DNORM static float
#define ENORM (float* f, size_t i, size_t s)

static inline float _mini(float v) {return ABS(v) > 1.0 ? 1.0 : ABS(v);}
// normalize value to log[0 .. 1]
static float _nlog(float v, size_t) {return log10(v*10 + 1)/log10(11.0f);}

// log index
static size_t _logindex(size_t i, size_t s) {
	float f = (float)i / (float)s;
	f = 1 - f;
	float r = log10(f*10 + 1)/log10(11.0f);
	r = 1 - r;
	return (size_t)(r * s);
}

DNORM _n1 ENORM {return f[i];};
DNORM _n2 ENORM {return _nlog(f[i], s);};
DNORM _n3 ENORM {return _nlog(f[_logindex(i, s/2)], s);};
DNORM _n4 ENORM {return f[_logindex(i, s/2)];};

#undef DNORM
#undef ENORM
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

// normalizers array
static bool switchnorm = false;
static bool switchwin  = false;

static float (*normalizers[]) (float*, size_t, size_t) = {
	_n1, _n2, _n3, _n4
};

static float (*windows[]) (float, float) = {
	_wrect, _wgausse, _whamming, _whann
};

static void ctrl_bits(ju_win_t* w) {
	if(ju_win_getkey(w, GLFW_KEY_1)) {
		if (!switchnorm) {
			normalizer += 1;
			if (normalizer > 3) normalizer = 0;
			switchnorm = true;
		}
	} else switchnorm = false;
	
	if(ju_win_getkey(w, GLFW_KEY_2)) {
		if (!switchwin) {
			window += 1;
			if (window > 3) window = 0;
			switchwin = true;
		}
	} else switchwin = false;
}


