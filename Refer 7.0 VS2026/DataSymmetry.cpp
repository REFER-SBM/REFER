
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

#include <vector>
#include "Tools_Geometry.h"
#include "DataSymmetry.h"

CDataSymmetry::CDataSymmetry()
{
	//Parameters for Symmetry detection
	//---------------------------------
	DetectNormalCrossing= true;
	MaxParallelCrossing= DEGREES_TO_RADIANS(45.); //More than 45 degrees is bad
	MinParallelCrossing= DEGREES_TO_RADIANS(5.);           //Less than 5 degrees is good
	MeritWeight_1= 0.5; //50%

	DetectDistorted= true;
	MinSymDistortion= 0.05;  //Less than 5% distortion is good
	MaxSymDistortion= 0.4;  //More than 40% distortion is bad
	MaxParallelFaceSymLines= DEGREES_TO_RADIANS(45.); //More than 45 degrees is bad
	MinParallelFaceSymLines= DEGREES_TO_RADIANS(5.);           //Less than 5 degrees is good
	MeritWeight_2= 0.35; //35%

	SymmetricalEdges= true;
	MaxParallelEdgeSymLines= DEGREES_TO_RADIANS(45.); //More than 45 degrees is bad
	MinParallelEdgeSymLines= DEGREES_TO_RADIANS(10.);           //Less than 10 degrees is good
	MeritWeight_3= 0.30; //30%

	PrioritizeVertical= true;
	minVerticalThreshold= DEGREES_TO_RADIANS(5.); //Less than 5 degrees is good
	maxVerticalThreshold= DEGREES_TO_RADIANS(10.); //More than 10 degrees is bad
	MeritWeight_4= 0.15; //15%

	RemoveBadMerit= false;
	MinMerit= 0.;

	//Output data
	//-----------
	//Intermediate data (useful for representation)
	FaceAxesU.clear();
	FaceAxesV.clear();

	//Circuis delimiting the bilateral symmetry planes
	SymPlanes.clear();
	SymMerits.clear();
};


CDataSymmetry::~CDataSymmetry(){
};
