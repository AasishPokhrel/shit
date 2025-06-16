//
//  easing.c
//
//  Copyright (c) 2011, Auerhaus Development, LLC
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What The Fuck You Want
//  To Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/wtfpl/COPYING for more details.
//

// Originally found on GitHub at : https://github.com/warrenm/AHEasing/
// DISCLAIMER : This file, as well as the linked header file, have been
// modified for compatibility purposes.
//     Edited by Semphris for SuperTux, Aug 14, 2020

#include "easing.hpp"

#include "util/string_util.hpp"

#include <math.h>
#include <stdexcept>
#include <string>
#include <string.h>

// Modeled after the line y = x.
double LinearInterpolation(double p)
{
	return p;
}

// Modeled after the parabola y = x^2.
double QuadraticEaseIn(double p)
{
	return p * p;
}

// Modeled after the parabola y = -x^2 + 2x.
double QuadraticEaseOut(double p)
{
	return -(p * (p - 2));
}

// Modeled after the piecewise quadratic:
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
double QuadraticEaseInOut(double p)
{
	if(p < 0.5)
	{
		return 2 * p * p;
	}
	else
	{
		return (-2 * p * p) + (4 * p) - 1;
	}
}

// Modeled after the cubic y = x^3.
double CubicEaseIn(double p)
{
	return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1.
double CubicEaseOut(double p)
{
	double f = (p - 1);
	return f * f * f + 1;
}

// Modeled after the piecewise cubic:
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
double CubicEaseInOut(double p)
{
	if(p < 0.5)
	{
		return 4 * p * p * p;
	}
	else
	{
		double f = ((2 * p) - 2);
		return 0.5 * f * f * f + 1;
	}
}

// Modeled after the quartic x^4.
double QuarticEaseIn(double p)
{
	return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4.
double QuarticEaseOut(double p)
{
	double f = (p - 1);
	return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic.
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
double QuarticEaseInOut(double p) 
{
	if(p < 0.5)
	{
		return 8 * p * p * p * p;
	}
	else
	{
		double f = (p - 1);
		return -8 * f * f * f * f + 1;
	}
}

// Modeled after the quintic y = x^5.
double QuinticEaseIn(double p) 
{
	return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1.
double QuinticEaseOut(double p) 
{
	double f = (p - 1);
	return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic:
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
double QuinticEaseInOut(double p) 
{
	if(p < 0.5)
	{
		return 16 * p * p * p * p * p;
	}
	else
	{
		double f = ((2 * p) - 2);
		return  0.5 * f * f * f * f * f + 1;
	}
}

// Modeled after quarter-cycle of sine wave.
double SineEaseIn(double p)
{
	return sin((p - 1) * M_PI_2) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase).
double SineEaseOut(double p)
{
	return sin(p * M_PI_2);
}

// Modeled after half sine wave.
double SineEaseInOut(double p)
{
	return 0.5 * (1 - cos(p * M_PI));
}

// Modeled after shifted quadrant IV of unit circle.
double CircularEaseIn(double p)
{
	return 1 - sqrt(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle.
double CircularEaseOut(double p)
{
	return sqrt((2 - p) * p);
}

// Modeled after the piecewise circular function:
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
double CircularEaseInOut(double p)
{
	if(p < 0.5)
	{
		return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
	}
	else
	{
		return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	}
}

// Modeled after the exponential function y = 2^(10(x - 1)).
double ExponentialEaseIn(double p)
{
	return (p == 0.0) ? p : pow(2, 10 * (p - 1));
}

// Modeled after the exponential function y = -2^(-10x) + 1.
double ExponentialEaseOut(double p)
{
	return (p == 1.0) ? p : 1 - pow(2, -10 * p);
}

// Modeled after the piecewise exponential:
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
double ExponentialEaseInOut(double p)
{
	if(p == 0.0 || p == 1.0) return p;
	
	if(p < 0.5)
	{
		return 0.5 * pow(2, (20 * p) - 10);
	}
	else
	{
		return -0.5 * pow(2, (-20 * p) + 10) + 1;
	}
}

// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1)).
double ElasticEaseIn(double p)
{
	return sin(13 * M_PI_2 * p) * pow(2, 10 * (p - 1));
}

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1.
double ElasticEaseOut(double p)
{
	return sin(-13 * M_PI_2 * (p + 1)) * pow(2, -10 * p) + 1;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
double ElasticEaseInOut(double p)
{
	if(p < 0.5)
	{
		return 0.5 * sin(13 * M_PI_2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
	}
	else
	{
		return 0.5 * (sin(-13 * M_PI_2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
	}
}

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi).
double BackEaseIn(double p)
{
	return p * p * p - p * sin(p * M_PI);
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi)).
double BackEaseOut(double p)
{
	double f = (1 - p);
	return 1 - (f * f * f - f * sin(f * M_PI));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
double BackEaseInOut(double p)
{
	if(p < 0.5)
	{
		double f = 2 * p;
		return 0.5 * (f * f * f - f * sin(f * M_PI));
	}
	else
	{
		double f = (1 - (2*p - 1));
		return 0.5 * (1 - (f * f * f - f * sin(f * M_PI))) + 0.5;
	}
}

double BounceEaseIn(double p)
{
	return 1 - BounceEaseOut(1 - p);
}

double BounceEaseOut(double p)
{
	if(p < 4/11.0)
	{
		return (121 * p * p)/16.0;
	}
	else if(p < 8/11.0)
	{
		return (363/40.0 * p * p) - (99/10.0 * p) + 17/5.0;
	}
	else if(p < 9/10.0)
	{
		return (4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0;
	}
	else
	{
		return (54/5.0 * p * p) - (513/25.0 * p) + 268/25.0;
	}
}

double BounceEaseInOut(double p)
{
	if(p < 0.5)
	{
		return 0.5 * BounceEaseIn(p*2);
	}
	else
	{
		return 0.5 * BounceEaseOut(p * 2 - 1) + 0.5;
	}
}

easing getEasingByName(const EasingMode& ease_type)
{
  switch(ease_type) {
  case EaseNone:
    return LinearInterpolation;
  case EaseQuadIn:
    return QuadraticEaseIn;
  case EaseQuadOut:
    return QuadraticEaseOut;
  case EaseQuadInOut:
    return QuadraticEaseInOut;
  case EaseCubicIn:
    return CubicEaseIn;
  case EaseCubicOut:
    return CubicEaseOut;
  case EaseCubicInOut:
    return CubicEaseInOut;
  case EaseQuartIn:
    return QuarticEaseIn;
  case EaseQuartOut:
    return QuarticEaseOut;
  case EaseQuartInOut:
    return QuarticEaseInOut;
  case EaseQuintIn:
    return QuinticEaseIn;
  case EaseQuintOut:
    return QuinticEaseOut;
  case EaseQuintInOut:
    return QuinticEaseInOut;
  case EaseSineIn:
    return SineEaseIn;
  case EaseSineOut:
    return SineEaseOut;
  case EaseSineInOut:
    return SineEaseInOut;
  case EaseCircularIn:
    return CircularEaseIn;
  case EaseCircularOut:
    return CircularEaseOut;
  case EaseCircularInOut:
    return CircularEaseInOut;
  case EaseExponentialIn:
    return ExponentialEaseIn;
  case EaseExponentialOut:
    return ExponentialEaseOut;
  case EaseExponentialInOut:
    return ExponentialEaseInOut;
  case EaseElasticIn:
    return ElasticEaseIn;
  case EaseElasticOut:
    return ElasticEaseOut;
  case EaseElasticInOut:
    return ElasticEaseInOut;
  case EaseBackIn:
    return BackEaseIn;
  case EaseBackOut:
    return BackEaseOut;
  case EaseBackInOut:
    return BackEaseInOut;
  case EaseBounceIn:
    return BounceEaseIn;
  case EaseBounceOut:
    return BounceEaseOut;
  case EaseBounceInOut:
    return BounceEaseInOut;
  default:
    return LinearInterpolation;
  }
}

EasingMode EasingMode_from_string(const std::string& ease_name)
{
  const char* name = ease_name.c_str();
  if (!strcmp(name, "EaseNone"))
  {
    return EaseNone;
  }
  else if (!strcmp(name, "EaseQuadIn"))
  {
    return EaseQuadIn;
  }
  else if (!strcmp(name, "EaseQuadOut"))
  {
    return EaseQuadOut;
  }
  else if (!strcmp(name, "EaseQuadInOut"))
  {
    return EaseQuadInOut;
  }
  else if (!strcmp(name, "EaseCubicIn"))
  {
    return EaseCubicIn;
  }
  else if (!strcmp(name, "EaseCubicOut"))
  {
    return EaseCubicOut;
  }
  else if (!strcmp(name, "EaseCubicInOut"))
  {
    return EaseCubicInOut;
  }
  else if (!strcmp(name, "EaseQuartIn"))
  {
    return EaseQuartIn;
  }
  else if (!strcmp(name, "EaseQuartOut"))
  {
    return EaseQuartOut;
  }
  else if (!strcmp(name, "EaseQuartInOut"))
  {
    return EaseQuartInOut;
  }
  else if (!strcmp(name, "EaseQuintIn"))
  {
    return EaseQuintIn;
  }
  else if (!strcmp(name, "EaseQuintOut"))
  {
    return EaseQuintOut;
  }
  else if (!strcmp(name, "EaseQuintInOut"))
  {
    return EaseQuintInOut;
  }
  else if (!strcmp(name, "EaseSineIn"))
  {
    return EaseSineIn;
  }
  else if (!strcmp(name, "EaseSineOut"))
  {
    return EaseSineOut;
  }
  else if (!strcmp(name, "EaseSineInOut"))
  {
    return EaseSineInOut;
  }
  else if (!strcmp(name, "EaseCircularIn"))
  {
    return EaseCircularIn;
  }
  else if (!strcmp(name, "EaseCircularOut"))
  {
    return EaseCircularOut;
  }
  else if (!strcmp(name, "EaseCircularInOut"))
  {
    return EaseCircularInOut;
  }
  else if (!strcmp(name, "EaseExponentialIn"))
  {
    return EaseExponentialIn;
  }
  else if (!strcmp(name, "EaseExponentialOut"))
  {
    return EaseExponentialOut;
  }
  else if (!strcmp(name, "EaseExponentialInOut"))
  {
    return EaseExponentialInOut;
  }
  else if (!strcmp(name, "EaseElasticIn"))
  {
    return EaseElasticIn;
  }
  else if (!strcmp(name, "EaseElasticOut"))
  {
    return EaseElasticOut;
  }
  else if (!strcmp(name, "EaseElasticInOut"))
  {
    return EaseElasticInOut;
  }
  else if (!strcmp(name, "EaseBackIn"))
  {
    return EaseBackIn;
  }
  else if (!strcmp(name, "EaseBackOut"))
  {
    return EaseBackOut;
  }
  else if (!strcmp(name, "EaseBackInOut"))
  {
    return EaseBackInOut;
  }
  else if (!strcmp(name, "EaseBounceIn"))
  {
    return EaseBounceIn;
  }
  else if (!strcmp(name, "EaseBounceOut"))
  {
    return EaseBounceOut;
  }
  else if (!strcmp(name, "EaseBounceInOut"))
  {
    return EaseBounceInOut;
  }
  else
  {
    return EaseNone;
  }
}

const char* getEasingName(const EasingMode& ease_type)
{
  switch(ease_type) {
  case EaseNone:
    return "EaseNone";
  case EaseQuadIn:
    return "EaseQuadIn";
  case EaseQuadOut:
    return "EaseQuadOut";
  case EaseQuadInOut:
    return "EaseQuadInOut";
  case EaseCubicIn:
    return "EaseCubicIn";
  case EaseCubicOut:
    return "EaseCubicOut";
  case EaseCubicInOut:
    return "EaseCubicInOut";
  case EaseQuartIn:
    return "EaseQuartIn";
  case EaseQuartOut:
    return "EaseQuartOut";
  case EaseQuartInOut:
    return "EaseQuartInOut";
  case EaseQuintIn:
    return "EaseQuintIn";
  case EaseQuintOut:
    return "EaseQuintOut";
  case EaseQuintInOut:
    return "EaseQuintInOut";
  case EaseSineIn:
    return "EaseSineIn";
  case EaseSineOut:
    return "EaseSineOut";
  case EaseSineInOut:
    return "EaseSineInOut";
  case EaseCircularIn:
    return "EaseCircularIn";
  case EaseCircularOut:
    return "EaseCircularOut";
  case EaseCircularInOut:
    return "EaseCircularInOut";
  case EaseExponentialIn:
    return "EaseExponentialIn";
  case EaseExponentialOut:
    return "EaseExponentialOut";
  case EaseExponentialInOut:
    return "EaseExponentialInOut";
  case EaseElasticIn:
    return "EaseElasticIn";
  case EaseElasticOut:
    return "EaseElasticOut";
  case EaseElasticInOut:
    return "EaseElasticInOut";
  case EaseBackIn:
    return "EaseBackIn";
  case EaseBackOut:
    return "EaseBackOut";
  case EaseBackInOut:
    return "EaseBackInOut";
  case EaseBounceIn:
    return "EaseBounceIn";
  case EaseBounceOut:
    return "EaseBounceOut";
  case EaseBounceInOut:
    return "EaseBounceInOut";
  default:
    return "EaseNone";
  }
}

EasingMode get_reverse_easing(const EasingMode& ease)
{
  return EasingMode_from_string(get_reverse_easing_str(getEasingName(ease)));
}

std::string get_reverse_easing_str(const std::string& ease_name)
{
  if (StringUtil::has_suffix(ease_name, "InOut") || ease_name == "EaseNone")
    return ease_name;

  if (StringUtil::has_suffix(ease_name, "In"))
  {
    std::string e = ease_name;
    e.pop_back();
    e.pop_back();
    return e + "Out";
  }

  if (StringUtil::has_suffix(ease_name, "Out"))
  {
    std::string e = ease_name;
    e.pop_back();
    e.pop_back();
    e.pop_back();
    return e + "In";
  }

  throw std::runtime_error("Trying to find opposite easing of non-easing string.");
}

/* EOF */
