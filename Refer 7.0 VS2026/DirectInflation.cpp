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

#include <stdafx.h>
#include <cmath>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "resource.h"

#include "CueTypology.h"

#include "InflationAxonometric.h"
#include "InflationLevels.h"
#include "DirectInflation.h"


DIRINFLATION CDirectInflation::SelectApproach(TYPOLOGY Typology)
// Returns the direct inflation approach selected by the user

// If the user has selected and "automatic" approach,
// then the approach more suitable for the typology of the current model is selected
{
	int TypeInflation;
	switch (Typology){
		case Normalon:
			TypeInflation = ReferApp.DI.iPreInfationIfNormalon;
			break;
		case QuasiNormalon:
			TypeInflation = ReferApp.DI.iPreInfationIfQuasi;
			break;
		case Pyramidal:
			TypeInflation = ReferApp.DI.iPreInfationIfPyramid;
			break;
		case Prismatic:
			TypeInflation = ReferApp.DI.iPreInfationIfPrism;
			break;
		case Undefined_D:
			TypeInflation = ReferApp.DI.iPreInfationIfUndefinedD;
			break;
		case Undefined_ND:
			TypeInflation = ReferApp.DI.iPreInfationIfUndefinedND;
			break;
	}

	DIRINFLATION DirInflation= NO_DIRINFLATE;
	switch (TypeInflation){
		case 0:
			DirInflation = AXONOMETRIC;
			break;
		case 1:
			DirInflation = LEVELS;
			break;
		case 2:
			DirInflation = RANDOM;
			break;
	}

	return DirInflation;
}


DIRINFLATION CDirectInflation::SelectPreInflation()
//Selects the PreInflation approach
{
	//if (!ReferApp.GeneralParams.DirectInflation)
	//	return NO_DIRINFLATE;

	DIRINFLATION PreInflation= ReferApp.DI.DirectInflationApproach;
	if (PreInflation == AUTOMATIC){
		PreInflation= SelectApproach(ReferApp.Cu.Ty.Typology);
	}
	return PreInflation;
}


void CDirectInflation::RandomInflation(CDB_Model *DDBB)
// Produces a 3D model, assigning random z coordinates to all vertices

// Useful as "preinflation" for optimization approaches,
// as it prevents flat figures that are trivial solutions during some optimization approaches
{
	//Determines the range of random z coordinates
	//to be equal to the maximum differerence in X or Y coordinates
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double dRango= max(std::fabs(dXmax-dXmin), std::fabs(dYmax-dYmin));

	//Determines random z coordinates contained in the range (0, dRango)
	srand( (unsigned)time( NULL ) );  /* Reinicia los numeros aleatorios con el reloj del sistema */

	std::vector<double> VariablesZ;
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		VariablesZ.push_back(dRango*(double)rand() / (double)(RAND_MAX+1));
	}

	DDBB->SetAllVertex_Z(VariablesZ);
}


//public:

void CDirectInflation::GetDirectInflation(CDB_Model *DDBB)
// Produces a set of z coordinates
// following the direct inflation criteria currently configured

// Based on:
//     Conesa J., Company P. and Gomis J.M.
//     "Initial Modeling Strategies for
//     Geometrical Reconstruction Optimization-Based Approaches"
//     11th ADM International Conference on Design Tools and
//     Methods in Industrial Engineering.
//     Conference Proceedings. Volume A, 1999 (ISBN 88-900081-9-9), pp. 161-171.
{
	switch (SelectPreInflation()){
	case AXONOMETRIC:{
		CInflationAxonometric AI;
		AI.GetAxonometricInflation(DDBB);
		break;
	}

	case LEVELS:{
		if (ReferApp.DIL.LevelOfVertex.size() == 0){
			AfxMessageBox("Levels of vertices are required for Levels direct inflation");
			return;
		};

		CInflationLevels LI;
		LI.InflateByLevels(DDBB);
		break;
		}

	case RANDOM:{
		RandomInflation(DDBB);
		break;
		}
	}
}