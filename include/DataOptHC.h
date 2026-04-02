
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

#if !defined DATAOPTHC_DEFINED
	#define DATAOPTHC_DEFINED

	class CDataOptHC{
		public:
			int    iMaxIterations,
				   iSolutionCurrent,
				   iIncrementVariableCurrent,
				   iStyleIncrement,
				   iIncrementPrefixed,
				   iPreConfigured;

			double dThresholdRejectBestCost,
				   dThresholdRejectBestCostInLevel,
				   dInitialStep,
				   dFinalStep,
				   dStepReduction;
			std::vector<double> IncrementsOfVariables;

			CDataOptHC();
			~CDataOptHC();
	};

#endif // DATAOPTHC_DEFINED