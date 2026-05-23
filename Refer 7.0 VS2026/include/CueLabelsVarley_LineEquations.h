//+-----------------------------------------------------------------------------+
//| Copyright (C) 2013 REGEO (http://www.regeo.uji.es/)
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

#ifndef _CUE_LABELS_VARLEY_LINEEQUATIONS_DEFINED
	#define _CUE_LABELS_VARLEY_LINEEQUATIONS_DEFINED

	//Finding labels in natural line-drawings
	//---------------------------------------
	class CCueLabelsVarley_LineEquations{
		private:
			lBOOLEAN Close (lREAL p, lREAL q);
			void NormaliseImplicitLine (IMPLICIT_LINE2 *G);
			void FindImplicitLine (IMPLICIT_LINE2 *ImplicitLine, POINT2Dv *P0, POINT2Dv *P1);
			void GetLineEquation (OBJECT *Object, EDGE_INDEX Nedge);

		public:
			CCueLabelsVarley_LineEquations();
			~CCueLabelsVarley_LineEquations();

			void GetUnknownLineEquations (OBJECT *Object);
			void GetAllLineEquations (OBJECT *Object);
	};

#endif // _CUE_LABELS_VARLEY_LINEEQUATIONS_DEFINED
