
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
#include "DataLabels.h"

CDataLabels::CDataLabels()
//Parameters for labelling
{
	LabelsApproach= 0;  //0= Kanatani, 1= Relaxation, 2= Lamb and Bandophaday
	VertexCatalogue;  //0= Hexahedral, 1= Extended thrihedral, 2= Thriedral
};


CDataLabels::~CDataLabels(){
};
