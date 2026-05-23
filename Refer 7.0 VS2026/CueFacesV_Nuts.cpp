//+-----------------------------------------------------------------------------+
//| Copyleft (C) 2025 REGEO (http://www.regeo.uji.es/)
//|
//| This program is free software
//| You can redistribute it and/or modify it under the terms of the GNU General Public License,
//| as published by the Free Software Foundation.
//| 
//| This program is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//+----------------------------------------------------------------------------*/

#include <CueFacesV_Nuts.h>


bool permute2 (long A, long B, long X, long Y)
{
	return ((A == X) && (B == Y)) || ((A == Y) && (B == X));
}


bool permute3 (long A, long B, long C, long X, long Y, long Z)
{
	if (A == X)  return ((B == Y) && (C == Z)) || ((B == Z) && (C == Y));
	if (A == Y)  return ((B == X) && (C == Z)) || ((B == Z) && (C == X));
	if (A == Z)  return ((B == X) && (C == Y)) || ((B == Y) && (C == X));
	return false;
}


long common2of4 (long A, long B, long C, long D, long Default)
{
	if (A == B)  return A;
	if (A == C)  return A;
	if (A == D)  return A;
	if (B == C)  return B;
	if (B == D)  return B;
	if (C == D)  return C;
	return Default;
}


long majority4 (long a, long b, long c, long d)
{
   if ((a == b) || (a == c) || (a == d))  return a;
   if ((b == c) || (b == d))  return b;
   if (c == d)  return c;
   return 0;
}


bool AllDifferent (int A, int B, int C, int D)
{
	if (A == B)  return false;
	if (A == C)  return false;
	if (A == D)  return false;
	if (B == C)  return false;
	if (B == D)  return false;
	if (C == D)  return false;
	return true;
}
