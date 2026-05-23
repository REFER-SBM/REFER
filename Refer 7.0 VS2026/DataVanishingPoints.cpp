
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

#include "Tools_Geometry.h"  //POINT2D
#include "DataVanishingPoints.h"

CDataVanishingPoints::CDataVanishingPoints()
//Parameters for Vanishing Points detection
{
	InRing = 1.5;          // Inner radius of the ring to detect vanishing points
	OutRing = 5.0;         // Outer radius of the ring to detect vanishing points
	ClusteringAngle = DEGREES_TO_RADIANS(12);  // Maximum angle to cluster vanishing points
	ToleranceCoincident = 0.15;  // Radius of tolerance circle to consider two endpoints coincident
	ToleranceParallel = DEGREES_TO_RADIANS(5.0);   // Tolerance angle (in degrees) to consider two edges parallel (and collinear if they are connected)
	ToleranceDispersion = 10.0; // Minimum dispersion to consider a bundle of lines parallel
};


CDataVanishingPoints::~CDataVanishingPoints(){
};
