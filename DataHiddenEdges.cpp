
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
#include "DataHiddenEdges.h"

CDataHiddenEdges::CDataHiddenEdges()
//Parameters for hidden edges
{
	HiddenEdgesByDashedLines = true;
	HiddenEdgesRefineDashedLines = false;
	HiddenEdgesApproach = 0; // Grouped by Faces= 0, Edge by edge= 1
	HiddenEdgesAutomatic = true;
};


CDataHiddenEdges::~CDataHiddenEdges(){
};
