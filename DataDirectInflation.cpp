
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
#include "DataDirectInflation.h"

CDataDirectInflation::CDataDirectInflation()
//Parameters for direct inflation
{
	//Direct Inflation Approach
	DirectInflationApproach = AUTOMATIC;

	//Parameters for automatic direct inflation
	iPreInfationIfNormalon = AUTOMATIC;
	iPreInfationIfQuasi = AUTOMATIC;
	iPreInfationIfPrism = AXONOMETRIC;
	iPreInfationIfPyramid = AXONOMETRIC;
	iPreInfationIfUndefinedD = LEVELS;
	iPreInfationIfUndefinedND = LEVELS;
};


CDataDirectInflation::~CDataDirectInflation(){
};
