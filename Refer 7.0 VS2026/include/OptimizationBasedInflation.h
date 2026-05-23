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

#if !defined OPTIMIZATIONBASEDINFLATION_DEFINED
#define OPTIMIZATIONBASEDINFLATION_DEFINED 

	class COptimizationInflation{
		private:
			//Information common to all approaches
			//used to monitor the progress of the optimization algorithms
			std::vector<double> VariablesZ;
			std::vector<double> EvolutionCosts;
			std::vector<double> EvolutionSolutions;
			std::vector<double> EvolutionZ;

			void UpdateActiveRegularities(CReferDoc* pDoc);

			double GetMUab( double a, double b, double x );

			//------Detect cues in the 2D drawing-------------
			//private:
			void GetCues_LineParallelism(CDB_Model *DDBB);
			void GetCues_LineCollinearity(CDB_Model *DDBB);
			void GetCues_LineVerticality(CDB_Model *DDBB);
			void GetCues_LineOrthogonality(CDB_Model *DDBB);

			double betaCubicCorner(CVector, CVector, CVector);
			void GetCues_CubicCorners(CDB_Model *DDBB);
			//public:
			void UpdateCuesOfRegularities(CReferDoc* pDoc);
			//------------------------------------------------

			//------Reset optimization variables--------------
			//private:
			void ResetEvolutionHC(std::vector<double> &EvolutionCosts,
								  CDataInflation &ParametrosOpt,
								  CDataOptHC &DataOptHC);
			void ResetEvolutionSA(std::vector<double> &EvolutionCosts,
								  CDataInflation &ParametrosOpt,
								  CDataOptSA &DataOptSA,
								  CDataOptSAM &DataOptSAM);
			void ResetEvolutionSAM(std::vector<double> &EvolutionCosts,
								   CDataInflation &ParametrosOpt,
								   CDataOptSA &DataOptSA,
								   CDataOptSAM &DataOptSAM);
			//public:
			void ResetEvolutionVariables(std::vector<double> &EvolutionCosts,
										 CDataOptHC &DataOptHC,
										 CDataOptSA &DataOptSA,
										 CDataOptSAM &DataOptSAM);
			//------------------------------------------------

			void GetNewSolutionHC(std::vector<double> &Solucion,
								  std::vector<double> &NuevaSolucion,
								  CDataOptHC* pParams);
			bool OptimizationHC(CReferDoc* pDoc,
								long *pNumeroIteracionesRealizadas,
								CDialogProgress *pProgress);

			void GetNewSolutionSA(std::vector<double> &Solucion,
								  std::vector<double> &NuevaSolucion,
								  CDataInflation *pParams, CDataOptSA* DataOptSA,
								  CDataOptSAM* DataOptSAM);
			bool OptimizationSA(CReferDoc* pDoc,
								long *pNumeroIteracionesRealizadas,
								CDialogProgress *pProgress);

			bool ThereAreZeroRegularityCosts(std::vector<double> ListaCostes);
			bool CheckMetropolis(double CosteActual, double CosteAnterior, double Temperatura);
			bool OptimizationSAM(CReferDoc* pDoc,
								 long *pNumeroIteracionesRealizadas,
								 CDialogProgress *pProgress);

			void UpdateInflationDataBase(CReferDoc* pDoc,
										 std::vector<double> IntermediateSolutions,
										 std::vector<double> InitialZ);

		public:
			void OptimizationBasedInflation(CReferDoc* pDoc);
	};

#endif // OPTIMIZATIONBASEDINFLATION_DEFINED

