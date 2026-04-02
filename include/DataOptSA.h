
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

#if !defined DATAOPTSA_DEFINED
	#define DATAOPTSA_DEFINED

	class CDataOptSA{
		public:
			int		iMaxLevels,
					iMaxIterationsPerLevel,
					iStyleIncrement,
					iMethodInitialTemperature,
					iRepetitions;
			double	dCriterioCongelacion,
					dCriterioEquilibrio,
					dIncrementoVariablesActual,
					dCoefAciertosTemperaturaInicial,
					dDecrementoTemperaturaPotencial,
					dInitialStep,
					dStepReduction,
					dFinalStep;
			std::vector<double> IncrementsOfVariables;

			CDataOptSA();
			~CDataOptSA();
	};

#endif // DATAOPTSA_DEFINED