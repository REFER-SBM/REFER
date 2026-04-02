
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

#include "DataStepsAndPockets.h"

CDataStepsAndPockets::CDataStepsAndPockets()
//Parameters for Features
{
	ShortEdges= 0.25;
	LongEdges= 1.45; //*short
	SimilarFacesDistances= 0.3;  //+-20%;
	SimilarFacesAngles= DEGREES_TO_RADIANS(15); //degrees
	CollinearEdges= DEGREES_TO_RADIANS(9); //degrees
	ThresholdAreaBases= 1;  //top and bottom faces are 1.0 times than the side faces

	NumSteps= 0;
	NumPockets= 0;
};


CDataStepsAndPockets::~CDataStepsAndPockets(){
};
