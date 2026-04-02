
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
#include "DataLevels.h"

CDataLevels::CDataLevels()
//Parameters for levels preinflation
{
	IncrementalLevels= true;
	SpanningTreeLevels= 1; //1= Deep first search, 0= Breath first search
	Coef_TreeUp= 1; //(100%)
	Coef_TreeDown= 1; //(100%)

	LevelsV.assign(6, 0);
	for(long i= 0; i<6; i++){
		LevelsV[i]= (1./5.)*(double)i;
	}

	LevelsH.assign(18, 0);
	for(long i= 0; i<18; i++){
		LevelsH[i]= (1./17.)*(double)i;
	}

	//Maximum threshold to consider an angle as vertical
	VerticalAngleThreshold = 10.0;  //Degrees

};


CDataLevels::~CDataLevels(){
};
