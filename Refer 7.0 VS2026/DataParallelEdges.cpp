
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
#include "DataParallelEdges.h"

CDataParallelEdges::CDataParallelEdges()
//Parameters for parallel edges detection
{
	ParallelEdgesApproach= 0; // Automatic= 0, Vanishing points= 1, Angular distribution= 2;

	TolForParallel= DEGREES_TO_RADIANS(7.0);  //Standard deviation of the gaussian curve used to cope with sketching imperfections
};


CDataParallelEdges::~CDataParallelEdges(){
};
