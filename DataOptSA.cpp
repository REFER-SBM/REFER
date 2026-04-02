
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
#include "DataOptSA.h"

CDataOptSA::CDataOptSA()
//Parameters to configure the optimization-based inflation by Simmulated Annealing
{
	//Parametros de Optimizacion SIMULATED ANNEALING
	iMaxLevels = 200;
	iMaxIterationsPerLevel = 2;
	dCriterioCongelacion = 0.98;
	dCriterioEquilibrio = 0.8;
	iStyleIncrement = 0;   // Parametric= 0, Prefixed= 1
	dInitialStep = 0.01;
	dCoefAciertosTemperaturaInicial = 0.8;
	dDecrementoTemperaturaPotencial = 0.96;
	dStepReduction = 0.995;
	dFinalStep = 0.00135;
	iMethodInitialTemperature = 1;
	iRepetitions = 1;
	IncrementsOfVariables.clear(); // Automatically recalculated
};


CDataOptSA::~CDataOptSA(){
};
