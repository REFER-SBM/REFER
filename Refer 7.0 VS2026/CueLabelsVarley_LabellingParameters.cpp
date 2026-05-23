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

#include <vector>
#include "DataLabels.h"

#include "CueLabelsVarley_Types.h"
#include "CueLabelsVarley_Constants.h"
#include "CueLabelsVarley_Object.h"
#include "CueLabelsVarley_LabellingParameters.h"

CCueLabelsVarley_LabellingParameters::CCueLabelsVarley_LabellingParameters()
{
	LabellingCutoff = 0.0;

	LabellingCount = 0;

	//Lamban axes are either fixed or determined from the lines in the drawing 
	UseDeductionsInLamban = false;
	//LambanAxisOptions= ;

	MaxConsideredLabellings     = 5000;

	TCOccludingNonBoundaryLines = 0.100;   /* was 0.000 */
	TCLabellingFromBadCofacial  = 0.580;   /* was 0.340 */
	TCMinimiseUnderlyingTypes   = 0.254;   /* was 0.563 */

	LabellingCount = 0;

	PreviousLabellingCatalogue = 0;

	LabellingHypothesisCount = 0;

	TCLabellingPreferredBranchSize = 0.693;


	KJunctionsKnownToBePresent = cFALSE;
	XJunctionsKnownToBePresent = cFALSE;
	KJunctionPresenceMerit = 0.0;
	XJunctionPresenceMerit = 0.0;
	DepthCornerOptions    = DEPTH_AUTO_SELECT;

	LabelLba.Shape = JunctionIsL; LabelLba.Vexity = Lba;
	LabelLab.Shape = JunctionIsL; LabelLab.Vexity = Lab;
	LabelLac.Shape = JunctionIsL; LabelLac.Vexity = Lac;
	LabelLcb.Shape = JunctionIsL; LabelLcb.Vexity = Lcb;
	LabelLbd.Shape = JunctionIsL; LabelLbd.Vexity = Lbd;
	LabelLda.Shape = JunctionIsL; LabelLda.Vexity = Lda;
	LabelLaa.Shape = JunctionIsL; LabelLaa.Vexity = Laa;
	LabelLbb.Shape = JunctionIsL; LabelLbb.Vexity = Lbb;
}


CCueLabelsVarley_LabellingParameters::~CCueLabelsVarley_LabellingParameters()
{
}
