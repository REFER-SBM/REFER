
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

#include "DataCSG.h"

CDataCSG::CDataCSG()
//Parameters for CSG modeling
{
	CSG_Inflation= false;
	CSG_Inflation_Direct= true;
	CSG_Inflation_Optimization= false;

	CSG_ThresholdFMerit= 0.50;                  //Threshold merit to discard poor candidate features
	CSG_ThresholdDMerit= 0.45;                  //Threshold merit to discard poor candidate datums
	CSG_ThresholdAngle= DEGREES_TO_RADIANS(15); //Threshold for angular comparisons


	CSG_ThresholdOffset= 0.20;                  //Threshold for alignment, assigned as
	                                            //10% of the length of the small of the two segments compared

	CSG_ThresholdLocation= 0.05;                //Threshold for identical location

	CSG_UseMultipleOrigins= true;               //Calculate orthodistances to local origins
};


CDataCSG::~CDataCSG()
{
};
