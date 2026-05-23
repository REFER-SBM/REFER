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

//Get labels from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003
#include <stdafx.h>
#include <cfloat>    // isnan

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Maths.h"


lREAL dmax2 (lREAL a, lREAL b){return a > b ? a : b;}
lREAL dmax3 (lREAL a, lREAL b, lREAL c){return dmax2(dmax2(a,b),c);}
lREAL dmax4 (lREAL a, lREAL b, lREAL c, lREAL d){return dmax2(dmax2(a,b),dmax2(c,d));}
lREAL dmax5 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e){return dmax2(dmax2(a,b),dmax3(c,d,e));}
lREAL dmax6 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f){return dmax2(dmax2(a,b),dmax4(c,d,e,f));}
lREAL dmax7 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f, lREAL g){return dmax2(dmax3(a,b,c),dmax4(d,e,f,g));}
lREAL dmax8 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f, lREAL g, lREAL h){return dmax2(dmax4(a,b,c,d),dmax4(e,f,g,h));}
lREAL dmax9 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f, lREAL g, lREAL h, lREAL i){return dmax3(dmax3(a,b,c),dmax3(d,e,f),dmax3(g,h,i));}
lREAL dmax10 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f, lREAL g, lREAL h, lREAL i, lREAL j){return dmax3(dmax3(a,b,c),dmax3(d,e,f),dmax4(g,h,i,j));}

lREAL dmin2 (lREAL a, lREAL b) {return a < b ? a : b;}
lREAL dmin3 (lREAL a, lREAL b, lREAL c){return dmin2(dmin2(a,b),c);}
lREAL dmin4 (lREAL a, lREAL b, lREAL c, lREAL d){return dmin2(dmin2(a,b),dmin2(c,d));}
lREAL dmin5 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e){return dmin2(dmin2(a,b),dmin3(c,d,e));}

lREAL Pmul (lREAL a, lREAL b)
{
   lREAL Pval = 1.0 - (1.0-a)*(1.0-b);
   return Pval;
}

int trindex (int A, int B)
{
	if (B < A)  return trindex(B,A);

	if (A == B)
	{
		AfxMessageBox("TRINDEX ERROR: A=B");
		exit(255);
	}

	return trisize(B)+A;
}


DEGREES midangle (DEGREES A, DEGREES B)
{
   return anglesum(A,anglediff(A,B)/2.0);
}


DEGREES anglesum (DEGREES A, DEGREES B)
{
   DEGREES C = A+B;
   if (C >= 360.0)  C -= 360.0;
   return C;
}


DEGREES anglediff (DEGREES A, DEGREES B)
{
   if (clockwise(A,B))
   {
      while (B < A)  B += 360.0;
   }
   else
   {
      while (A < B)  A += 360.0;
   }
   return B-A;
}


lBOOLEAN clockwise (DEGREES A, DEGREES B)
{
   DEGREES C = B-A;
   while (C < 0.0)     C += 360.0;
   while (C >= 360.0)  C -= 360.0;
   return C < 180.0;
}


DEGREES danglecross (DEGREES Direction1, DEGREES Direction2)
{
   DEGREES dD = Direction1 - Direction2;
   while (dD < 0.0)     dD += 180.0;
   while (dD >= 180.0)  dD -= 180.0;
   return (dD <= 90.0) ? dD : 180.0-dD;
}


DEGREES dangleclockMaths (DEGREES Direction1, DEGREES Direction2)
/* -------------------------------------------------------------------------------- */
/* dangleclock returns the absolute value (in degrees) of the smaller angle between */
/* two lines on a clock face                                                        */
/* e.g. dangleclock (TWELVE_O_CLOCK, ONE_O_CLOCK)     is  30 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, TWO_O_CLOCK)     is  60 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, THREE_O_CLOCK)   is  90 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, FOUR_O_CLOCK)    is 120 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, FIVE_O_CLOCK)    is 150 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, SIX_O_CLOCK)     is 180 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, SEVEN_O_CLOCK)   is 150 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, EIGHT_O_CLOCK)   is 120 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, NINE_O_CLOCK)    is  90 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, TEN_O_CLOCK)     is  60 degrees                */
/* e.g. dangleclock (TWELVE_O_CLOCK, ELEVEN_O_CLOCK)  is  30 degrees                */
{
   DEGREES dD = Direction1 - Direction2;
   while (dD < 0.0)     dD += 360.0;
   while (dD >= 360.0)  dD -= 360.0;
   return (dD <= 180.0) ? dD : 360.0-dD;
}


DEGREES danglecwiseMaths (DEGREES Direction1, DEGREES Direction2)
{
   while (Direction2 < Direction1)  Direction2 += 360.0;
   return Direction2-Direction1;
}


lINTEGER HighestIndex3 (lREAL A, lREAL B, lREAL C)
{
   if ((A >= B) && (A >= C))  return 0;
   if ((B >= A) && (B >= C))  return 1;
   return 2;
}


lINTEGER HighestIndex4 (lREAL A, lREAL B, lREAL C, lREAL D)
{
   if ((A >= B) && (A >= C) && (A >= D))  return 0;
   if ((B >= A) && (B >= C) && (B >= D))  return 1;
   if ((C >= A) && (C >= B) && (C >= D))  return 2;
   return 3;
}

lINTEGER HighestIndexN (lREAL V[], lNATURAL Count)
{
   int H = 0;
   for (int I = 1;  I < (int)Count;  ++I)  if (V[I] > V[H])  H = I;
   return H;
}


bool isinf(double d)
{
   int fpc = _fpclass(d);
   return fpc == _FPCLASS_NINF || fpc == _FPCLASS_PINF;
}


bool isfinite (lREAL value)
{
   if (_isnan(value))
      return false;
   if (isinf(value))
      return false;

   return true;
}

DEGREES OppositeDirection (DEGREES FromDirection)
{
   return (FromDirection >= 180.0) ? FromDirection-180.0 : FromDirection+180.0;
}


lINTEGER odd3 (lINTEGER A, lINTEGER B, lINTEGER C)
{
   if (A == B)  return C;
   if (A == C)  return B;
   if (B == C)  return A;
   return -1;
}


lINTEGER uniq5 (lINTEGER A, lINTEGER B, lINTEGER C, lINTEGER D, lINTEGER E)
{
   if (A == B)  return odd3(C,D,E);
   if (A == C)  return odd3(B,D,E);
   if (A == D)  return odd3(B,C,E);
   if (A == E)  return odd3(B,C,D);
   return A;
}
