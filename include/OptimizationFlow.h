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

#ifndef OPTIMIZATION_FLOW_VISITED
	#define OPTIMIZATION_FLOW_VISITED
	//public (accesed from OptimizationBasedInflation, as much as from dialogs and display):

	void EstimateIncrementsForHC(CDB_Model *DDBB,
								 std::vector<double> &Increments,
								 double InitialStep,
								 double FinalStep,
								 double StepReduction);
	void EstimateIncrementsForSA(CDB_Model *DDBB,
								   std::vector<double> &Increments,
								   double InitialStep,
								   double FinalStep,
								   double StepReduction);

	void PrepareCoefficientsRegularities( TParametrosCoeficienteRegularidad ListaCoeficientes[],
										   long lNumeroMaximoIteraciones );

	void UpdateCoefficientsRegularities(TParametrosCoeficienteRegularidad ListaCoeficientes[],
											long lIteracion );

#endif  //OPTIMIZATION_FLOW_VISITED
