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

#ifndef _CUE_LABELS_VARLEY_TYPES_DEFINED
	#define _CUE_LABELS_VARLEY_TYPES_DEFINED

	typedef signed long int lINTEGER;
	typedef unsigned long int lNATURAL;
	typedef double lREAL;
	typedef unsigned short int lBOOLEAN;

	#define cFALSE ((lBOOLEAN)0)
	#define cTRUE  ((lBOOLEAN)1)

	#define cNULL  ((void *)0)

	#ifndef TRUE
		#define TRUE  1
		#define FALSE 0
	#endif

	#ifndef NULL
		#define NULL ((void *)(0))
	#endif

	typedef lREAL DEGREES;
	typedef lREAL RADIANS;

	typedef double MERIT;
	typedef struct { MERIT Amerit, Bmerit, Cmerit, Dmerit; } EPROBABILITIES;

	typedef unsigned char FUZZY;
	#define cYES   (2)
	#define cMAYBE (1)
	#define cNO    (0)

#endif
