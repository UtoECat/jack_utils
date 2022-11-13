#define DNORM static float
#define ENORM (float* f, size_t i, size_t s)

#define IND(i) cb(f[(i >= (int)s ? i - (int)s : (i < 0 ? (int)s + i : i))], s)
static inline float filt(float*f, size_t d, size_t s, float (*cb) (float, size_t)) {
	float v = 0.0f;
	int i = (int) d;
	v += IND(i) * 0.50;
	v += IND(i-1) * 0.15;  v += IND(i+1) * 0.15;
	v += IND(i-2) * 0.05;  v += IND(i+2) * 0.05;
	v += IND(i-3) * 0.025; v += IND(i+3) * 0.025;
	v += IND(i-4) * 0.025; v += IND(i+4) * 0.025;
	return v;
}
#undef IND

static inline float _mini(float v) {return ABS(v) > 1.0 ? 1.0 : ABS(v);}
// normalize fourie transform output to [0 .. 1]
static float _norm(float v, size_t s) {return ABS(v) / (float)s * PI;}
//static float _nlog(float v, size_t s) {return log10(_mini(ABS(v)/sqrt((float)s)/PI)*10 + 1);}
static float _nlog(float v, size_t s) {return _mini(ABS(v)/sqrt((float)s));}

DNORM _n1 ENORM {return _norm(f[i], s);};
DNORM _n2 ENORM {return _nlog(f[i], s);};
DNORM _n3 ENORM {return filt(f, i, s, _norm);};
DNORM _n4 ENORM {return filt(f, i, s, _nlog);};

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


