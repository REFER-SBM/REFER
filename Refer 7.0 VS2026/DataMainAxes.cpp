
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
#include "Tools_Geometry.h"  // DEGREES_TO_RADIANS

#include "DataMainAxes.h"

CDataMainAxes::CDataMainAxes()
//Parameters for Main Axes detection
{
	//Approach for main axes
	MainAxesApproach= 0;   // 0 Automatic. 1 Three first edges, 2 User interactive selection

	//Criteria for automatic detection of main axes
	ProjectiveRule= true;
	ToleranceCubicAngles= DEGREES_TO_RADIANS(5);

	MatchingCornersRule= true;
	IsometricRule= true;
	VerticalRule= true;
	HorizontalRule= true;
	FrequencyRule= true;

	MeritMatchingCorners= 2.0;
	MeritIsometric= 0.5;
	MeritVertical= 1.0;
	MeritHorizontal= 0.1;
	MeritFrequency= 1.0;

	ThresholdOverlap= DEGREES_TO_RADIANS(10);  //Maximum difference to consider two directions the same
	ThresholdAngle= DEGREES_TO_RADIANS(15);  //Maximum deviation to consider non-null merits of candidate main axes

	//Reset user main axes
	UserMainAxes.clear();

	//Reset calculated parameters
	MainAngles.clear();
	CoefE.clear();
	AngleXYZ.clear();
	obliqueAngle= -1;

	M.assign(3, std::vector<double>());
	for(long i= 0; i<3; i++){
		M[i].assign(3, 0.);
	}
	M[0][0]= 1; M[1][1]= 1; M[2][2]= 1;
};


CDataMainAxes::~CDataMainAxes(){
};
