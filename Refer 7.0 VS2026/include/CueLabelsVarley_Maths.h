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

#ifndef _CUE_LABELS_VARLEY_MATHS_DEFINED
	#define _CUE_LABELS_VARLEY_MATHS_DEFINED

	lREAL dmax2 (lREAL a, lREAL b);
	lREAL dmax3 (lREAL a, lREAL b, lREAL c);
	lREAL dmax4 (lREAL a, lREAL b, lREAL c, lREAL d);
	lREAL dmax5 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e);
	lREAL dmax6 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f);
	lREAL dmax7 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f, lREAL g);
	lREAL dmax8 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f, lREAL g, lREAL h);
	lREAL dmax9 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f, lREAL g, lREAL h, lREAL i);
	lREAL dmax10 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e, lREAL f, lREAL g, lREAL h, lREAL i, lREAL j);
	lREAL dmin2 (lREAL a, lREAL b);
	lREAL dmin3 (lREAL a, lREAL b, lREAL c);
	lREAL dmin4 (lREAL a, lREAL b, lREAL c, lREAL d);
	lREAL dmin5 (lREAL a, lREAL b, lREAL c, lREAL d, lREAL e);

	lREAL Pmul (lREAL a, lREAL b);

	#define trisize(xx)  (((xx)*((xx)-1))/2)
	int trindex (int A, int B);

	DEGREES midangle (DEGREES A, DEGREES B);
	DEGREES anglesum (DEGREES A, DEGREES B);
	DEGREES anglediff (DEGREES A, DEGREES B);
	lBOOLEAN clockwise (DEGREES A, DEGREES B);
	DEGREES danglecross (DEGREES Direction1, DEGREES Direction2);
	DEGREES dangleclockMaths (DEGREES Direction1, DEGREES Direction2);
	DEGREES danglecwiseMaths (DEGREES Direction1, DEGREES Direction2);

	lINTEGER HighestIndex3 (lREAL A, lREAL B, lREAL C);
	lINTEGER HighestIndex4 (lREAL A, lREAL B, lREAL C, lREAL D);
	lINTEGER HighestIndexN (lREAL V[], lNATURAL Count);

	#define ScalarCross(I,J,K,P,Q,R)  ((I)*((Q)-(R))+(J)*((R)-(P))+(K)*((P)-(Q)))

	bool isinf(double d);
	bool isfinite (lREAL value);

	DEGREES OppositeDirection (DEGREES FromDirection);

	lINTEGER odd3 (lINTEGER A, lINTEGER B, lINTEGER C);
	lINTEGER uniq5 (lINTEGER A, lINTEGER B, lINTEGER C, lINTEGER D, lINTEGER E);


#endif // _CUE_LABELS_VARLEY_MATHS_DEFINED
