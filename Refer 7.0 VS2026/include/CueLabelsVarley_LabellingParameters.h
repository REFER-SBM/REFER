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

#ifndef _CUE_LABELS_VARLEY_LABELLINGPARAMETERS_DEFINED
	#define _CUE_LABELS_VARLEY_LABELLINGPARAMETERS_DEFINED

	class CCueLabelsVarley_LabellingParameters{
		public:
			bool UseDeductionsInLamban;
			int LambanAxisOptions;

			int LabellingCount;
			MERIT LabellingCutoff;
			LABELLING LabellingBuffer[MAX_STORED_LABELLINGS];

			int LabellingHypothesisCount;
			LABELLING_HYPOTHESIS LabellingHypotheses[MAX_VERTICES*3];

			int   MaxConsideredLabellings;

			MERIT TCLabellingPreferredBranchSize;

			MERIT TCOccludingNonBoundaryLines;
			MERIT TCLabellingFromBadCofacial;
			MERIT TCMinimiseUnderlyingTypes;

			lBOOLEAN KJunctionsKnownToBePresent;
			lBOOLEAN XJunctionsKnownToBePresent;
			MERIT KJunctionPresenceMerit;
			MERIT XJunctionPresenceMerit;
			char DepthCornerOptions;


			JUNCTION_TYPEMARK LabelLba;
			JUNCTION_TYPEMARK LabelLab;
			JUNCTION_TYPEMARK LabelLac;
			JUNCTION_TYPEMARK LabelLcb;
			JUNCTION_TYPEMARK LabelLbd;
			JUNCTION_TYPEMARK LabelLda;
			JUNCTION_TYPEMARK LabelLaa;
			JUNCTION_TYPEMARK LabelLbb;

			char PreviousLabellingCatalogue;  // for batch runs

			CCueLabelsVarley_LabellingParameters();
			~CCueLabelsVarley_LabellingParameters();

	};

#endif  //_CUE_LABELS_VARLEY_LABELLINGPARAMETERS_DEFINED
