
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

#include "DataChamfersAndRounds.h"    //TBD ChamfersAndRounds

CDataChamfersAndRounds::CDataChamfersAndRounds()    //TBD ChamfersAndRounds
//Parameters for Features
{
	ShortEdges= 0.2;
	LongEdges= 2; //*short
	SimilarFacesDistances= 0.2;  //+-20%;
	SimilarFacesAngles= DEGREES_TO_RADIANS(15); //degrees
	CollinearEdges= DEGREES_TO_RADIANS(9); //degrees

	NumChamfers= 0;
	NumRounds= 0;
};


CDataChamfersAndRounds::~CDataChamfersAndRounds()    //TBD ChamfersAndRounds
{
};
