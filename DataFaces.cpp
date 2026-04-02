
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
#include "DataFaces.h"

CDataFaces::CDataFaces()
//Parameters for finding faces
{
	DetectFacesApproach = 0;   // 0= Varley, 1= Lipson & Sphitalni, 2= Courter & Brewer
	EdgeCollinearityAngleThreshold = DEGREES_TO_RADIANS(5);  //Used in Lipson & Sphitalni
};


CDataFaces::~CDataFaces(){
};
