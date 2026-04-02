
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
#include "DataTypology.h"

CDataTypology::CDataTypology()
//Parameters for typology classification
{
	Typology= Undefined;

	dFrequencyPrismaticObjects = 0.50;   //%
	dFrequencyUndefinedObjects_D = 0.65;   //%
	ThresholdForParallel = DEGREES_TO_RADIANS(10); //10 degrees difference allowed
	ThresholdForArea = 0.15;   //15% allowed difference
};


CDataTypology::~CDataTypology(){
};
