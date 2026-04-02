
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

#include "DataRailsAndSlots.h"

CDataRailsAndSlots::CDataRailsAndSlots()
//Parameters for Features
{
	ShortEdges= 0.2;
	//Raquel cambio LongEdges de 2 a 1.75 para que funcione el ejemplo07
	LongEdges= 1.85; //*short
	SimilarFacesDistances= 0.2;  //+-20%;
	SimilarFacesAngles= DEGREES_TO_RADIANS(15); //degrees
	CollinearEdges= DEGREES_TO_RADIANS(9); //degrees

	NumSlots= 0;
	NumRails= 0;
};


CDataRailsAndSlots::~CDataRailsAndSlots(){
};
