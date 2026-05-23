
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

#include "DataAxonometric.h"

CDataAxonometric::CDataAxonometric()
//Parameters for axonometric preinflation
{
	AxonometricApproach= 1; //0= 2001 approach, 1= 2011 approach
	bAmendCabinet = true;
	d90degrees= DEGREES_TO_RADIANS(91);
	d135degrees= DEGREES_TO_RADIANS(134);

	SpanningTreeAxonometric= 1; //0= Deep first search, 1= Breath first search
	CabinetThreshold = DEGREES_TO_RADIANS(5);
	CabinetReduction = 0.5;
};


CDataAxonometric::~CDataAxonometric(){
};
