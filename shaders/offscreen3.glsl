// CRT Geom from:
// https://github.com/libretro/slang-shaders/blob/master/crt/shaders/crt-geom.slang
// Under GNU GPL 2 license

#pragma sokol @block config

#define CRTgamma (2.4)
#define monitorgamma (2.2)
#define d (1.5)
#define R (2.0)
#define cornersize (0.03)
#define cornersmooth (1000.0)
#define x_tilt (0.0)
#define y_tilt (0.0)
#define overscan_x (100.0)
#define overscan_y (100.0)
#define DOTMASK (0.3)
#define SHARPER (1.0)
#define scanline_weight (0.3)
#define CURVATURE (1.0)
#define interlace_detect (1.0)
#define lum (0.0)
#define invert_aspect (0.0)
#define vertical_scanlines (0.0)
#define xsize (0.0)
#define ysize (0.0)

// Comment the next line to disable interpolation in linear gamma (and
// gain speed).
#define LINEAR_PROCESSING

// Enable 3x oversampling of the beam profile; improves moire effect caused by scanlines+curvature
#define OVERSAMPLE

// Use the older, purely gaussian beam profile; uncomment for speed
//#define USEGAUSSIAN

// Macros.
#define FIX(c) max(abs(c), 1e-5);
#define PI 3.141592653589

#ifdef LINEAR_PROCESSING
#	define TEX2D(c) pow(texture(Source, (c)), vec4(CRTgamma))
#else
#	define TEX2D(c) texture(Source, (c))
#endif

// aspect ratio
vec2 aspect	= vec2(invert_aspect > 0.5 ? (4.0 / 3.0, 1.0) : (4.0 / 3.0, 1.0));
vec2 overscan	= vec2(1.01, 1.01);

#pragma sokol @end

#pragma sokol @vs vs_offscr3

#pragma sokol @include_block config

in vec4 position;
in vec2 tex_coord;
out vec2 vTexCoord;
out vec2 sinangle;
out vec2 cosangle;
out vec3 stretch;
out vec2 ilfac;
out vec2 one;
out float mod_factor;
out vec2 TextureSize;

uniform vs_offscr3_params{
	vec2 SourceSize;
	vec2 OutputSize;
};

// vec2 height = (ysize > 0.001) ? vec2(ysize, 1./ysize) : params.SourceSize.yw;
// vec2 width = (xsize > 0.001) ? vec2(xsize, 1./xsize) : params.SourceSize.xz;
// vec4 SourceSize = vec4(width.x, height.x, width.y, height.y);

float intersect(vec2 xy)
{
	float A = dot(xy,xy) + d*d;
	float B = 2.0*(R*(dot(xy,sinangle)-d*cosangle.x*cosangle.y)-d*d);
	float C = d*d + 2.0*R*d*cosangle.x*cosangle.y;

	return (-B-sqrt(B*B-4.0*A*C))/(2.0*A);
}

vec2 bkwtrans(vec2 xy)
{
	float c	    = intersect(xy);
	vec2 point  = (vec2(c, c)*xy - vec2(-R, -R)*sinangle) / vec2(R, R);
	vec2 poc    = point/cosangle;

	vec2 tang   = sinangle/cosangle;
	float A	    = dot(tang, tang) + 1.0;
	float B	    = -2.0*dot(poc, tang);
	float C	    = dot(poc, poc) - 1.0;

	float a	    = (-B + sqrt(B*B - 4.0*A*C))/(2.0*A);
	vec2 uv	    = (point - a*sinangle)/cosangle;
	float r	    = FIX(R*acos(a));

	return uv*r/sin(r/R);
}

vec2 fwtrans(vec2 uv)
{
	float r = FIX(sqrt(dot(uv,uv)));
	uv *= sin(r/R)/r;
	float x = 1.0-cos(r/R);
	float D = d/R + x*cosangle.x*cosangle.y+dot(uv,sinangle);

	return d*(uv*cosangle-x*sinangle)/D;
}

vec3 maxscale()
{
	vec2 c	= bkwtrans(-R * sinangle / (1.0 + R/d*cosangle.x*cosangle.y));
	vec2 a	= vec2(0.5,0.5)*aspect;

	vec2 lo = vec2(fwtrans(vec2(-a.x,  c.y)).x,
		       fwtrans(vec2( c.x, -a.y)).y)/aspect;

	vec2 hi = vec2(fwtrans(vec2(+a.x,  c.y)).x,
		       fwtrans(vec2( c.x, +a.y)).y)/aspect;

	return vec3((hi+lo)*aspect*0.5,max(hi.x-lo.x,hi.y-lo.y));
}

void main()
{
	gl_Position = position;
	vTexCoord = tex_coord * vec2(1.00001);

	// Precalculate a bunch of useful values we'll need in the fragment
	// shader.
	sinangle    = sin(vec2(x_tilt, y_tilt));
	cosangle    = cos(vec2(x_tilt, y_tilt));
	stretch	    = maxscale();

	if(vertical_scanlines < 0.5)
	{
		TextureSize = vec2(SHARPER * SourceSize.x, SourceSize.y);

		ilfac = vec2(1.0, clamp(floor(SourceSize.y/(interlace_detect > 0.5 ? 200.0 : 1000)),  1.0, 2.0));

		// The size of one texel, in texture-coordinates.
		one = ilfac / TextureSize;

		// Resulting X pixel-coordinate of the pixel we're drawing.
		mod_factor = vTexCoord.x * SourceSize.x * OutputSize.x / SourceSize.x;
	}else{
		TextureSize = vec2(SourceSize.x, SHARPER * SourceSize.y);

		ilfac = vec2(clamp(floor(SourceSize.x/(interlace_detect > 0.5 ? 200.0 : 1000)),	 1.0, 2.0), 1.0);

		// The size of one texel, in texture-coordinates.
		one = ilfac / TextureSize;

		// Resulting X pixel-coordinate of the pixel we're drawing.
		mod_factor = vTexCoord.y * SourceSize.y * OutputSize.y / SourceSize.y;
	}
}

#pragma sokol @end

#pragma sokol @fs fs_offscr3

#pragma sokol @include_block config

in vec2 vTexCoord;
in vec2 sinangle;
in vec2 cosangle;
in vec3 stretch;
in vec2 ilfac;
in vec2 one;
in float mod_factor;
in vec2 TextureSize;
out vec4 FragColor;

uniform texture2D tex;
uniform sampler smp;
#define Source (sampler2D(tex, smp))

uniform fs_offscr3_params {
	int FrameCount;
};

float intersect(vec2 xy)
{
	float A = dot(xy,xy) + d*d;
	float B, C;

	if(vertical_scanlines < 0.5)
	{
		B = 2.0*(R*(dot(xy,sinangle) - d*cosangle.x*cosangle.y) - d*d);
		C = d*d + 2.0*R*d*cosangle.x*cosangle.y;
	}else{
		B = 2.0*(R*(dot(xy,sinangle) - d*cosangle.y*cosangle.x) - d*d);
		C = d*d + 2.0*R*d*cosangle.y*cosangle.x;
	}

	return (-B-sqrt(B*B - 4.0*A*C))/(2.0*A);
}

vec2 bkwtrans(vec2 xy)
{
	float c	    = intersect(xy);
	vec2 point  = (vec2(c, c)*xy - vec2(-R, -R)*sinangle) / vec2(R, R);
	vec2 poc    = point/cosangle;
	vec2 tang   = sinangle/cosangle;

	float A	    = dot(tang, tang) + 1.0;
	float B	    = -2.0*dot(poc, tang);
	float C	    = dot(poc, poc) - 1.0;

	float a	    = (-B + sqrt(B*B - 4.0*A*C)) / (2.0*A);
	vec2 uv	    = (point - a*sinangle) / cosangle;
	float r	    = FIX(R*acos(a));

	return uv*r/sin(r/R);
}

vec2 fwtrans(vec2 uv)
{
	float r = FIX(sqrt(dot(uv, uv)));
	uv *= sin(r/R)/r;
	float x = 1.0 - cos(r/R);
	float D;

	if(vertical_scanlines < 0.5)
		D = d/R + x*cosangle.x*cosangle.y + dot(uv,sinangle);
	else
		D = d/R + x*cosangle.y*cosangle.x + dot(uv,sinangle);

	return d*(uv*cosangle - x*sinangle)/D;
}

vec3 maxscale()
{
	if(vertical_scanlines < 0.5)
	{
		vec2 c = bkwtrans(-R * sinangle / (1.0 + R/d*cosangle.x*cosangle.y));
		vec2 a = vec2(0.5, 0.5)*aspect;

		vec2 lo = vec2(fwtrans(vec2(-a.x,  c.y)).x,
			       fwtrans(vec2( c.x, -a.y)).y)/aspect;
		vec2 hi = vec2(fwtrans(vec2(+a.x,  c.y)).x,
			       fwtrans(vec2( c.x, +a.y)).y)/aspect;

		return vec3((hi+lo)*aspect*0.5,max(hi.x-lo.x, hi.y-lo.y));
	}else{
		vec2 c = bkwtrans(-R * sinangle / (1.0 + R/d*cosangle.y*cosangle.x));
		vec2 a = vec2(0.5, 0.5)*aspect;

		vec2 lo = vec2(fwtrans(vec2(-a.y,  c.x)).y,
			       fwtrans(vec2( c.y, -a.x)).x)/aspect;
		vec2 hi = vec2(fwtrans(vec2(+a.y,  c.x)).y,
			       fwtrans(vec2( c.y, +a.x)).x)/aspect;

		return vec3((hi+lo)*aspect*0.5,max(hi.y-lo.y, hi.x-lo.x));
	}
}

// Calculate the influence of a scanline on the current pixel.
//
// 'distance' is the distance in texture coordinates from the current
// pixel to the scanline in question.
// 'color' is the colour of the scanline at the horizontal location of
// the current pixel.
vec4 scanlineWeights(float distance, vec4 color)
{
	// "wid" controls the width of the scanline beam, for each RGB
	// channel The "weights" lines basically specify the formula
	// that gives you the profile of the beam, i.e. the intensity as
	// a function of distance from the vertical center of the
	// scanline. In this case, it is gaussian if width=2, and
	// becomes nongaussian for larger widths. Ideally this should
	// be normalized so that the integral across the beam is
	// independent of its width. That is, for a narrower beam
	// "weights" should have a higher peak at the center of the
	// scanline than for a wider beam.
#ifdef USEGAUSSIAN
	vec4 wid = 0.3 + 0.1 * pow(color, vec4(3.0));
	vec4 weights = vec4(distance / wid);

	return (lum + 0.4) * exp(-weights * weights) / wid;
#else
	vec4 wid = 2.0 + 2.0 * pow(color, vec4(4.0));
	vec4 weights = vec4(distance / scanline_weight);

	return (lum + 1.4) * exp(-pow(weights * inversesqrt(0.5 * wid), wid)) / (0.6 + 0.2 * wid);
#endif
}

vec2 transform(vec2 coord)
{
	coord = (coord - vec2(0.5, 0.5))*aspect*stretch.z + stretch.xy;

	return (bkwtrans(coord) /
		vec2(overscan_x / 100.0, overscan_y / 100.0)/aspect + vec2(0.5, 0.5));
}

float corner(vec2 coord)
{
	coord = (coord - vec2(0.5)) * vec2(overscan_x / 100.0, overscan_y / 100.0) + vec2(0.5, 0.5);
	coord = min(coord, vec2(1.0) - coord) * aspect;
	vec2 cdist = vec2(cornersize);
	coord = (cdist - min(coord, cdist));
	float dist = sqrt(dot(coord, coord));

	if(vertical_scanlines < 0.5)
		return clamp((cdist.x - dist)*cornersmooth, 0.0, 1.0);
	else
		return clamp((cdist.y - dist)*cornersmooth, 0.0, 1.0);
}

void main()
{
	// Here's a helpful diagram to keep in mind while trying to
	// understand the code:
	//
	//  |	   |	  |	 |	|
	// -------------------------------
	//  |	   |	  |	 |	|
	//  |  01  |  11  |  21	 |  31	| <-- current scanline
	//  |	   | @	  |	 |	|
	// -------------------------------
	//  |	   |	  |	 |	|
	//  |  02  |  12  |  22	 |  32	| <-- next scanline
	//  |	   |	  |	 |	|
	// -------------------------------
	//  |	   |	  |	 |	|
	//
	// Each character-cell represents a pixel on the output
	// surface, "@" represents the current pixel (always somewhere
	// in the bottom half of the current scan-line, or the top-half
	// of the next scanline). The grid of lines represents the
	// edges of the texels of the underlying texture.

	// Texture coordinates of the texel containing the active pixel.
	vec2 xy;
	if (CURVATURE > 0.5)
		xy = transform(vTexCoord);
	else
		xy = vTexCoord;

	float cval = corner(xy);

	// Of all the pixels that are mapped onto the texel we are
	// currently rendering, which pixel are we currently rendering?
	vec2 ilvec;
	if(vertical_scanlines < 0.5)
		ilvec = vec2(0.0, ilfac.y * interlace_detect > 1.5 ? mod(float(FrameCount), 2.0) : 0.0);
	else
		ilvec = vec2(ilfac.x * interlace_detect > 1.5 ? mod(float(FrameCount), 2.0) : 0.0, 0.0);

	vec2 ratio_scale = (xy * TextureSize - vec2(0.5, 0.5) + ilvec) / ilfac;
	vec2 uv_ratio = fract(ratio_scale);

	// Snap to the center of the underlying texel.
	xy = (floor(ratio_scale)*ilfac + vec2(0.5, 0.5) - ilvec) / TextureSize;

	// Calculate Lanczos scaling coefficients describing the effect
	// of various neighbour texels in a scanline on the current
	// pixel.
	vec4 coeffs;
	if(vertical_scanlines < 0.5)
		coeffs = PI * vec4(1.0 + uv_ratio.x, uv_ratio.x, 1.0 - uv_ratio.x, 2.0 - uv_ratio.x);
	else
		coeffs = PI * vec4(1.0 + uv_ratio.y, uv_ratio.y, 1.0 - uv_ratio.y, 2.0 - uv_ratio.y);

	// Prevent division by zero.
	coeffs = FIX(coeffs);

	// Lanczos2 kernel.
	coeffs = 2.0 * sin(coeffs) * sin(coeffs / 2.0) / (coeffs * coeffs);

	// Normalize.
	coeffs /= dot(coeffs, vec4(1.0));

	// Calculate the effective colour of the current and next
	// scanlines at the horizontal location of the current pixel,
	// using the Lanczos coefficients above.
	vec4 col, col2;
	if(vertical_scanlines < 0.5)
	{
		col = clamp(
			mat4(
				TEX2D(xy + vec2(-one.x, 0.0)),
				TEX2D(xy),
				TEX2D(xy + vec2(one.x, 0.0)),
				TEX2D(xy + vec2(2.0 * one.x, 0.0))
				) * coeffs,
			0.0, 1.0
			);
		col2 = clamp(
			mat4(
				TEX2D(xy + vec2(-one.x, one.y)),
				TEX2D(xy + vec2(0.0, one.y)),
				TEX2D(xy + one),
				TEX2D(xy + vec2(2.0 * one.x, one.y))
				) * coeffs,
			0.0, 1.0
			);
	}else{
		col = clamp(
			mat4(
				TEX2D(xy + vec2(0.0, -one.y)),
				TEX2D(xy),
				TEX2D(xy + vec2(0.0, one.y)),
				TEX2D(xy + vec2(0.0, 2.0 * one.y))
				) * coeffs,
			0.0, 1.0
			);
		col2 = clamp(
			mat4(
				TEX2D(xy + vec2(one.x, -one.y)),
				TEX2D(xy + vec2(one.x, 0.0)),
				TEX2D(xy + one),
				TEX2D(xy + vec2(one.x, 2.0 * one.y))
				) * coeffs,
			0.0, 1.0
			);
	}

#ifndef LINEAR_PROCESSING
	col  = pow(col , vec4(CRTgamma));
	col2 = pow(col2, vec4(CRTgamma));
#endif

	// Calculate the influence of the current and next scanlines on
	// the current pixel.
	vec4 weights, weights2;
	if(vertical_scanlines < 0.5)
	{
		weights	 = scanlineWeights(uv_ratio.y, col);
		weights2 = scanlineWeights(1.0 - uv_ratio.y, col2);

#ifdef OVERSAMPLE
		float filter_ = fwidth(ratio_scale.y);
		uv_ratio.y    = uv_ratio.y + 1.0/3.0*filter_;
		weights	      = (weights  + scanlineWeights(uv_ratio.y, col))/3.0;
		weights2      = (weights2 + scanlineWeights(abs(1.0 - uv_ratio.y), col2))/3.0;
		uv_ratio.y    = uv_ratio.y - 2.0/3.0*filter_;
		weights	      = weights	 + scanlineWeights(abs(uv_ratio.y), col)/3.0;
		weights2      = weights2 + scanlineWeights(abs(1.0 - uv_ratio.y), col2)/3.0;
#endif
	}else{
		weights	 = scanlineWeights(uv_ratio.x, col);
		weights2 = scanlineWeights(1.0 - uv_ratio.x, col2);

#ifdef OVERSAMPLE
		float filter_ = fwidth(ratio_scale.x);
		uv_ratio.x    = uv_ratio.x + 1.0/3.0*filter_;
		weights	      = (weights  + scanlineWeights(uv_ratio.x, col))/3.0;
		weights2      = (weights2 + scanlineWeights(abs(1.0 - uv_ratio.x), col2))/3.0;
		uv_ratio.x    = uv_ratio.x - 2.0/3.0*filter_;
		weights	      = weights	 + scanlineWeights(abs(uv_ratio.x), col)/3.0;
		weights2      = weights2 + scanlineWeights(abs(1.0 - uv_ratio.x), col2)/3.0;
#endif
	}

	vec3 mul_res  = (col * weights + col2 * weights2).rgb * vec3(cval);

	// dot-mask emulation:
	// Output pixels are alternately tinted green and magenta.
	vec3 dotMaskWeights = mix(
		vec3(1.0, 1.0 - DOTMASK, 1.0),
		vec3(1.0 - DOTMASK, 1.0, 1.0 - DOTMASK),
		floor(mod(gl_FragCoord.x, 2.0))
		);

	mul_res *= dotMaskWeights;

	// Convert the image gamma for display on our output device.
	mul_res = pow(mul_res, vec3(1.0 / monitorgamma));

	FragColor = vec4(mul_res, 1.0);
}

#pragma sokol @end

#pragma sokol @program offscreen3 vs_offscr3 fs_offscr3
