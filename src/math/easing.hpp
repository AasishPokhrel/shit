//
//  easing.h
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
// DISCLAIMER : This file, as well as the linked C source file, have been
// modified for compatibility purposes.
//     Edited by Semphris for SuperTux, Aug 14, 2020
//

#ifndef EASING_H
#define EASING_H

#include <string>
#include <string.h>

extern "C" {

enum EasingMode
{
	EaseNone,
	EaseQuadIn,
	EaseQuadOut,
	EaseQuadInOut,
	EaseCubicIn,
	EaseCubicOut,
	EaseCubicInOut,
	EaseQuartIn,
	EaseQuartOut,
	EaseQuartInOut,
	EaseQuintIn,
	EaseQuintOut,
	EaseQuintInOut,
	EaseSineIn,
	EaseSineOut,
	EaseSineInOut,
	EaseCircularIn,
	EaseCircularOut,
	EaseCircularInOut,
	EaseExponentialIn,
	EaseExponentialOut,
	EaseExponentialInOut,
	EaseElasticIn,
	EaseElasticOut,
	EaseElasticInOut,
	EaseBackIn,
	EaseBackOut,
	EaseBackInOut,
	EaseBounceIn,
	EaseBounceOut,
	EaseBounceInOut
};

double LinearInterpolation(double p);
double QuadraticEaseIn(double p);
double QuadraticEaseOut(double p);
double QuadraticEaseInOut(double p);
double CubicEaseIn(double p);
double CubicEaseOut(double p);
double CubicEaseInOut(double p);
double QuarticEaseIn(double p);
double QuarticEaseOut(double p);
double QuarticEaseInOut(double p);
double QuinticEaseIn(double p);
double QuinticEaseOut(double p);
double QuinticEaseInOut(double p);
double SineEaseIn(double p);
double SineEaseOut(double p);
double SineEaseInOut(double p);
double CircularEaseIn(double p);
double CircularEaseOut(double p);
double CircularEaseInOut(double p);
double ExponentialEaseIn(double p);
double ExponentialEaseOut(double p);
double ExponentialEaseInOut(double p);
double ElasticEaseIn(double p);
double ElasticEaseOut(double p);
double ElasticEaseInOut(double p);
double BackEaseIn(double p);
double BackEaseOut(double p);
double BackEaseInOut(double p);
double BounceEaseIn(double p);
double BounceEaseOut(double p);
double BounceEaseInOut(double p);

typedef double(*easing)(double);
easing getEasingByName(const EasingMode& ease_type);
const char* getEasingName(const EasingMode& ease_type);

}

EasingMode EasingMode_from_string(const std::string& ease_name);

EasingMode get_reverse_easing(const EasingMode& ease);
std::string get_reverse_easing_str(const std::string& ease_name);

#endif

/* EOF */
