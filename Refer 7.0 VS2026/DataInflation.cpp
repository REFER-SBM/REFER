
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
#include "Tools_Geometry.h"
#include "DataInflation.h"


CParamsIsolines::CParamsIsolines()
{
	TG= 1; //0= One variable; // 1= two variables
	OtherVariables= 1; //0= Reset all other Z to zero; 1= Maintain current Z'; 2= Modify m_V to m_Z
	V= 2;
	Z= 0;
	InterpolationDegree= 0;  //0= no interpolation; 10= max interpolation

	X= 0;
	X0= -5.0;
	INCX= 1.0;
	XF= 5.0;

	Y= 1;
	Y0= -5.0;
	INCY= 1.0;
	YF= 5.0;

	INCZ= 0.1;
	Color= 0;
};


CParamsIsolines::~CParamsIsolines(){
};


CDataInflation::CDataInflation()
//Parameters for inflation
{
	//Optimization parameters
	iOptAlgorithm = HILL_CLIMBING;   //HILL_CLIMBING, SIMULATED_ANNEALING, SIMULATED_ANNEALING_MULTI

	iRegularitiesSelection = 0; //0 Automatic (derived from typology), 1 Fixed by the user
	iNormalizeCosts = 0;    // 0 Normalize, 1 Apply variable coefficients



	iNumIntermediateInflations = 3;
	bViewInflationDrawing= true;

	bShowInflationInProgress = true;  //Displays current 3D model after each inflation step

	bViewPartialCostsEvolution = true; //Displays the partial cost of each regularity in the cost function diagram

	//Other parameters
	//----------------
	InflationAngleThreshold = DEGREES_TO_RADIANS(7);  //7 degrees was the threshold used in Lipson & Sphitalni
	Cues_LC_shareFace= false;  //Collinear lines do not need to be coplanar

	//Parameters of Regularities
	TRegularity Regularity;

	Regularity.bActive = true;
	Regularity.dCoeficienteFijo = 0.1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_MIN_STD_DEV_ANGLES ] = Regularity;

	Regularity.bActive = false;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_FACE_PLANARITY ] = Regularity;


	Regularity.bActive = true;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_ONE_MINUS_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_LINE_PARALLELISM ] = Regularity;

	Regularity.bActive = false;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_LINE_COLLINEARITY ] = Regularity;

	Regularity.bActive = false;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_LINE_VERTICALITY ] = Regularity;


	Regularity.bActive = false;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_LINE_ORTHOGONALITY ] = Regularity;


	Regularity.bActive = false;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_ISOMETRY ] = Regularity;


	Regularity.bActive = false;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_CUBIC_CORNERS ] = Regularity;

	Regularity.bActive = false;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_FACE_PERPENDICULARITY ] = Regularity;

	
	Regularity.bActive = false;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY ] = Regularity;

	Regularity.bActive = false;
	Regularity.dCoeficienteFijo = 1;
	Regularity.iCoeficienteVariable = COEFFICIENT_A;
	Regularity.bEsCoeficienteVariable = false;
	Regularities.ListOfRegularities[ REGULARITY_SKEWED_FACIAL_SYMMETRY ] = Regularity;

	Regularities.NumActiveRegularities = 2;

	//Adjust the parameters of the regularities
	TParametrosCoeficienteRegularidad Coeficiente;
	
	//Adjust regularity A
	Coeficiente.iEstiloEvolucion = EVOLUTION_STYLE_CONSTANT;
	Coeficiente.lIteracionInicialMenu = 0;
	Coeficiente.lIteracionFinalMenu = 0;
	Coeficiente.lPasos = 0;
	Coeficiente.dValorInicial = 1;
	Coeficiente.dValorFinal = 0;
	Coeficiente.dIncrementoProducto = 0.98;
	Regularities.ListaCoeficientes[COEFFICIENT_A] = Coeficiente;

	//Adjust the other regularities
	for(long i = COEFFICIENT_B; i < (COEFFICIENT_LAST + 1); i +=2 ) {
		Coeficiente.iEstiloEvolucion = EVOLUTION_STYLE_INACTIVE;
		Coeficiente.lIteracionInicialMenu = 0;
		Coeficiente.lIteracionFinalMenu = 0;
		Coeficiente.lPasos = 0;
		Coeficiente.dValorInicial = 1;
		Coeficiente.dValorFinal = 0;
		Coeficiente.dIncrementoProducto = 0.98;
		Regularities.ListaCoeficientes[i] = Coeficiente;
	};

	//Reset type-dependent regularities
	RegularitiesNormalon = Regularities;
	RegularitiesQuasiNormalon = Regularities;
	RegularitiesPrismatic = Regularities;
	RegularitiesPyramidal = Regularities;
	RegularitiesIndefined_D = Regularities;
	RegularitiesIndefined_ND = Regularities;

	//Reset user-defined regularities
	RegularitiesUser = Regularities;

	//Automatic regularities
	//----------------------
	//Reset all the regularities to default (false)
	for (long i=0; i<REGULARITY_LAST +1; i++){
		RegularitiesNormalon.ListOfRegularities[i].bActive= false;
		RegularitiesQuasiNormalon.ListOfRegularities[i].bActive= false;
		RegularitiesPyramidal.ListOfRegularities[i].bActive= false;
		RegularitiesPrismatic.ListOfRegularities[i].bActive= false;
		RegularitiesIndefined_D.ListOfRegularities[i].bActive= false;
		RegularitiesIndefined_ND.ListOfRegularities[i].bActive= false;

		RegularitiesUser.ListOfRegularities[i].bActive= false;
	}

	//Now activate regulatiries depending on the type

		//Type Normalon
		RegularitiesNormalon.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive=true;
		RegularitiesNormalon.NumActiveRegularities = 1;

		//Type QuasiNormalon
		RegularitiesQuasiNormalon.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive=true;
		RegularitiesQuasiNormalon.NumActiveRegularities = 1;

		//Type Pyramidal
		RegularitiesPyramidal.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive=true;
		RegularitiesPyramidal.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive=true;
		RegularitiesPyramidal.ListOfRegularities[REGULARITY_ISOMETRY].bActive=true;
		RegularitiesPyramidal.NumActiveRegularities = 3;

		//Type Prismatic
		RegularitiesPrismatic.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive=true;
		RegularitiesPrismatic.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive=true;
		RegularitiesPrismatic.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive=true;
		RegularitiesPrismatic.ListOfRegularities[REGULARITY_ISOMETRY].bActive=true;
		RegularitiesPrismatic.NumActiveRegularities = 4;

		//Type Indefined_D
		RegularitiesIndefined_D.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive=true;
		RegularitiesIndefined_D.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive=true;
		RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive=true;
		RegularitiesIndefined_D.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive=true;
		RegularitiesIndefined_D.ListOfRegularities[REGULARITY_ISOMETRY].bActive=true;
		RegularitiesIndefined_D.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive=true;

		RegularitiesIndefined_D.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive=true;
		RegularitiesIndefined_D.NumActiveRegularities = 7;

		//Type Indefined_ND
		RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive=true;
		RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive=true;
		RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive=true;
		RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive=true;
		RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_ISOMETRY].bActive=true;

		RegularitiesIndefined_ND.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive=true;
		RegularitiesIndefined_ND.NumActiveRegularities = 6;

	//User defined
	//------------
	//Default regularities for user-defined regularities
	RegularitiesUser.ListOfRegularities[REGULARITY_MIN_STD_DEV_ANGLES].bActive=true;
	RegularitiesUser.ListOfRegularities[REGULARITY_FACE_PLANARITY].bActive=true;
	RegularitiesUser.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive=true;
	RegularitiesUser.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive=true;
	RegularitiesUser.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive=true;
	//RegularitiesUser.ListOfRegularities[REGULARITY_SKEWED_FACIAL_SYMMETRY].bActive=true;

	RegularitiesUser.NumActiveRegularities = 5;
	//RegularitiesUser.NumActiveRegularities = 6;
};


CDataInflation::~CDataInflation()
{
};
