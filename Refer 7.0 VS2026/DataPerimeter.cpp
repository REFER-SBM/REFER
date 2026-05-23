
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

#include "DataPerimeter.h"

CDataPerimeter::CDataPerimeter()
//Data for cue perimeter detection
{
	IncludeDanglingEdges= true;
	ThresholdIntersection= 0.01; //1%   It must be increased if "Merge edge and vertex in T-vertex" is not active in 2D Refine
};


CDataPerimeter::~CDataPerimeter(){
};
