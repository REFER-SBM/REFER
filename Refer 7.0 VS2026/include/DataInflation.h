
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

#include "DataOptHC.h"
#include "DataOptSA.h"
#include "DataOptSAM.h"

#if !defined DATAINFLATION_DEFINED
	#define DATAINFLATION_DEFINED

	// Regularities
	#define REGULARITY_MIN_STD_DEV_ANGLES            0
	#define REGULARITY_FACE_PLANARITY                1

	#define REGULARITY_LINE_PARALLELISM              2  //Used in 2D refinement
	#define REGULARITY_LINE_COLLINEARITY             3  //Used in 2D refinement
	#define REGULARITY_LINE_VERTICALITY              4  //Used in 2D refinement

	#define REGULARITY_LINE_ORTHOGONALITY            5  //  Requires Main axes!!

	#define REGULARITY_ISOMETRY                      6

	#define REGULARITY_CUBIC_CORNERS                 7
	#define REGULARITY_FACE_PERPENDICULARITY         8

	#define REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY  9

	#define REGULARITY_SKEWED_FACIAL_SYMMETRY       10//<-- This is the last implemented regularity 
	#define REGULARITY_LAST                         10

	// Optimization algorithm
	#define NO_OPTIMIZATION            -1
	#define HILL_CLIMBING               0
	#define SIMULATED_ANNEALING         1
	#define SIMULATED_ANNEALING_MULTI   2

	//Types of increments of variables
	#define VAR_INCREMENT_PROPORTIONAL  0
	#define VAR_INCREMENT_FIX           1
	#define VAR_INCREMENT_MARILL        2
	#define VAR_INCREMENT_LECLERC       3

	//Styles of the increments
	#define PARAMETRIC  0
	#define PREFIXED    1

	//Styles of the pre-fixed increments
	#define USER     0
	#define MARILL   1
	#define LECLERC  2

	//Styles of the evolution of the coefficients
	#define EVOLUTION_STYLE_INACTIVE     -1
	#define EVOLUTION_STYLE_NOEVOLUTION   0
	#define EVOLUTION_STYLE_CONSTANT      1
	#define EVOLUTION_STYLE_POTENTIAL     2

	//Parameters of the evolution of the coefficients
	#define COEFFICIENT_A             0
	#define COEFFICIENT_ONE_MINUS_A   1
	#define COEFFICIENT_B             2
	#define COEFFICIENT_ONE_MINUS_B   3
	#define COEFFICIENT_C             4
	#define COEFFICIENT_ONE_MINUS_C   5
	#define COEFFICIENT_D             6
	#define COEFFICIENT_ONE_MINUS_D   7
	#define COEFFICIENT_E             8
	#define COEFFICIENT_ONE_MINUS_E   9
	#define COEFFICIENT_F            10
	#define COEFFICIENT_ONE_MINUS_F  11
	#define COEFFICIENT_G            12
	#define COEFFICIENT_ONE_MINUS_G  13
	#define COEFFICIENT_H            14
	#define COEFFICIENT_ONE_MINUS_H  15
	#define COEFFICIENT_I            16
	#define COEFFICIENT_ONE_MINUS_I  17
	#define COEFFICIENT_J            18
	#define COEFFICIENT_ONE_MINUS_J  19
	#define COEFFICIENT_K            20
	#define COEFFICIENT_ONE_MINUS_K  21
	#define COEFFICIENT_L            22
	#define COEFFICIENT_ONE_MINUS_L  23
	#define COEFFICIENT_LAST         COEFFICIENT_L

	// Parameters to control the COEFFICIENTS of the REGULARITIES
	struct TParametrosCoeficienteRegularidad{
		int iEstiloEvolucion;
		long lContadorLlamadas,
			 lLlamadasPorPaso,
			 lPasos,
			 lIteracionInicial,
			 lIteracionInicialMenu,
			 lIteracionFinal,
			 lIteracionFinalMenu;
		double dValorActual,
			   dValorInicial,
			   dValorFinal,
			   dIncrementoProducto,
			   dIncrementoSuma;
	};

	// Type of Regularity
	struct TRegularity {
		double dCoeficienteFijo;
		int iCoeficienteVariable;
		bool bEsCoeficienteVariable;
		bool bActive;
	};

	// Type of Regularity list
	struct TListOfRegularities{
		TRegularity ListOfRegularities[REGULARITY_LAST+1];// debe contener TODAS las regularidades, aunque no esten implementadas
		TParametrosCoeficienteRegularidad ListaCoeficientes[COEFFICIENT_LAST+1];
		int NumActiveRegularities;
	};

	class CParamsIsolines{
		public:
			int X, Y, TG, V, OtherVariables, InterpolationDegree, Color;
			double X0, XF, INCX, Y0, YF, INCY, INCZ, Z;
			double ZTX0, ZTXF, ZTINCX, ZTY0, ZTYF, ZTINCY;

			CParamsIsolines();
			~CParamsIsolines();
	};

	/////////////////////////////////////////////////////////////////////////////////

	class CDataInflation{
		public:
			long iOptAlgorithm;

			CDataOptHC DataOptHC;
			CDataOptSA DataOptSA;
			CDataOptSAM DataOptSAM;

			long iRegularitiesSelection;
			long iNormalizeCosts;

			std::vector<double> FactorNormalizacion;
			std::vector<bool> EstaCosteNormalizado;

			std::vector<double> EvolutionCosts;
			std::vector<double> EvolutionZ;

			//Params to control visualization of inflation evolution
			bool bShowInflationInProgress;

			bool bViewPartialCostsEvolution;
			bool bViewInflationDrawing;
			long iNumIntermediateInflations;

			TListOfRegularities Regularities;

			TListOfRegularities RegularitiesUser;
			TListOfRegularities RegularitiesNormalon;
			TListOfRegularities RegularitiesQuasiNormalon;
			TListOfRegularities RegularitiesPyramidal;
			TListOfRegularities RegularitiesPrismatic;
			TListOfRegularities RegularitiesIndefined_D;
			TListOfRegularities RegularitiesIndefined_ND;

			//Calculated params
			long NumVariables;

			//Cues
			std::vector<std::vector<double>> Cues_LP;  //Parallelism of Lines
			std::vector<std::vector<double>> Cues_LC;  //Collinearity of Lines
			std::vector<double> Cues_LV;               //Verticality of Lines
			std::vector<std::vector<double>> Cues_LO;  //Orthogonallity of Lines
			std::vector<double> Cues_CC;               //Cubic corners

			//Other parameters
			double InflationAngleThreshold;   //Used for Line parallelis, Line collinearity and Line orthogonality
			bool Cues_LC_shareFace;

			CDataInflation();
			~CDataInflation();
	};

#endif // DATAINFLATION_DEFINED