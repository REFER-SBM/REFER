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

#include <stdafx.h>
#include <cmath>
#include <vector>

#include "Tools_Geometry.h"
#include "Refer.h"
#include "DB_Model.h"
#include "OptimizationFlow.h"


void EstimateIncrementsForHC(CDB_Model *DDBB,
							 std::vector<double> &Increments,
							 double InitialStep,
							 double FinalStep,
							 double StepReduction)
// Calculates an estimation of the best sequence of increment (or decrement)
// of variables for the Hill-Climbing algorithm
{
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double dRange = min(std::fabs(dXmax-dXmin), std::fabs(dYmax-dYmin));

	double CurrentIncrement = dRange * InitialStep;
	double MinimIncrement = dRange * FinalStep;

	Increments.clear();
	while (CurrentIncrement > MinimIncrement){
		Increments.push_back(CurrentIncrement);
		InitialStep *= StepReduction;
		CurrentIncrement = dRange * InitialStep;
	};
}


void EstimateIncrementsForSA(CDB_Model *DDBB,
							   std::vector<double> &Increments,
							   double InitialStep,
							   double FinalStep,
							   double StepReduction)
// Calculates an estimation of the best sequence of increment(or decrement)
// of variables for the Simulated Annealing algorithm
{
	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);
	double dRange = min(std::fabs(dXmax-dXmin), std::fabs(dYmax-dYmin));

	double CurrentIncrement = dRange * InitialStep;
	double MinimIncrement = dRange * FinalStep;

	Increments.clear();
	while (CurrentIncrement > MinimIncrement){ 
		Increments.push_back(CurrentIncrement);
		InitialStep *= StepReduction;
		CurrentIncrement = dRange * InitialStep;
	};
}


void PrepareCoefficientsRegularities( TParametrosCoeficienteRegularidad ListaCoeficientes[],
                                       long lNumeroMaximoIteraciones )
// Inicializa adecuadamente los campos de los coeficientes de las regularidades,
// en funcion del "lNumeroMaximoIteraciones"
{
	TParametrosCoeficienteRegularidad Coeficiente;

	// Inicializa cada coeficiente
	for(long i=COEFFICIENT_A; i<(COEFFICIENT_LAST+1) ; i+=2 ) {
		Coeficiente = ListaCoeficientes[i];

		switch( Coeficiente.iEstiloEvolucion ) {
		case EVOLUTION_STYLE_INACTIVE:
				Coeficiente.dValorActual = 1;
		break;

		case EVOLUTION_STYLE_NOEVOLUTION:
				Coeficiente.lIteracionInicial = (int) ((double)(lNumeroMaximoIteraciones * (double)Coeficiente.lIteracionInicialMenu / (double)10));
				if (Coeficiente.lIteracionFinalMenu == 0)
					Coeficiente.lIteracionFinal = lNumeroMaximoIteraciones;
				else
					Coeficiente.lIteracionFinal = (int) ((double)(lNumeroMaximoIteraciones * (double)Coeficiente.lIteracionFinalMenu / (double)10));
				if (Coeficiente.lIteracionFinal <= Coeficiente.lIteracionInicial) {
					Coeficiente.iEstiloEvolucion = EVOLUTION_STYLE_INACTIVE;
					Coeficiente.dValorActual = 1;
//					Coeficiente.lIteracionInicial = 0;
//					Coeficiente.lIteracionFinal = lNumeroMaximoIteraciones;
//					Coeficiente.lIteracionInicialMenu = 0;
//					Coeficiente.lIteracionFinalMenu = 0;
				}
				else
					Coeficiente.dValorActual = Coeficiente.dValorInicial;
		break;

		case EVOLUTION_STYLE_CONSTANT:
				Coeficiente.lIteracionInicial = (int) ((double)(lNumeroMaximoIteraciones * (double)Coeficiente.lIteracionInicialMenu / (double)10));
				if (Coeficiente.lIteracionFinalMenu == 0)
					Coeficiente.lIteracionFinal = lNumeroMaximoIteraciones;
				else
					Coeficiente.lIteracionFinal = (int) ((double)(lNumeroMaximoIteraciones * (double)Coeficiente.lIteracionFinalMenu / (double)10));
				if (Coeficiente.lIteracionFinal < Coeficiente.lIteracionInicial) {
					Coeficiente.iEstiloEvolucion = EVOLUTION_STYLE_INACTIVE;
					Coeficiente.dValorActual = 1;
					break;
				};
				if (Coeficiente.lPasos == 0) {
					Coeficiente.dIncrementoSuma = (Coeficiente.dValorFinal-Coeficiente.dValorInicial)/(double)(Coeficiente.lIteracionFinal - Coeficiente.lIteracionInicial);
					Coeficiente.lLlamadasPorPaso = 1;
				}
				else {
					Coeficiente.dIncrementoSuma = (Coeficiente.dValorFinal-Coeficiente.dValorInicial)/(double)Coeficiente.lPasos;					
					Coeficiente.lLlamadasPorPaso = (Coeficiente.lIteracionFinal - Coeficiente.lIteracionInicial) / Coeficiente.lPasos;
//					Coeficiente.lLlamadasPorPaso = lNumeroMaximoIteraciones / Coeficiente.lPasos;
				};
//				Coeficiente.dValorActual = Coeficiente.dValorInicial;
				Coeficiente.dValorActual = 0;
				Coeficiente.lContadorLlamadas = 0;
		break;

		case EVOLUTION_STYLE_POTENTIAL:
				Coeficiente.lIteracionInicial = (int) ((double)(lNumeroMaximoIteraciones * (double)Coeficiente.lIteracionInicialMenu / (double)10));
				if (Coeficiente.lIteracionFinalMenu == 0)
					Coeficiente.lIteracionFinal = lNumeroMaximoIteraciones;
				else
					Coeficiente.lIteracionFinal = (int) ((double)(lNumeroMaximoIteraciones * (double)Coeficiente.lIteracionFinalMenu / (double)10));
				if (Coeficiente.lIteracionFinal < Coeficiente.lIteracionInicial) {
					Coeficiente.iEstiloEvolucion = EVOLUTION_STYLE_INACTIVE;
					break;
				};
				if (Coeficiente.lPasos == 0)
					Coeficiente.lLlamadasPorPaso = 1;
				else
					Coeficiente.lLlamadasPorPaso = (Coeficiente.lIteracionFinal - Coeficiente.lIteracionInicial) / Coeficiente.lPasos;

//				Coeficiente.dValorActual = Coeficiente.dValorInicial;
				Coeficiente.dValorActual = 0;
				Coeficiente.lContadorLlamadas = 0;
		break;
		};
		ListaCoeficientes[i] = Coeficiente;
	};
};


void UpdateCoefficientsRegularities(TParametrosCoeficienteRegularidad ListaCoeficientes[],
										long lIteracion)
//Actualiza los coeficientes que se usan para calcular costes variables 

// Actualiza los coeficientes de las regularidades, en funcion de "lIteracion",
//es decir, del progreso de la optimización.

// Solo se debe llamar a esta funcion UNA VEZ POR CADA ITERACION PRINCIPAL.
{
	TParametrosCoeficienteRegularidad Coeficiente;

	// Actualiza cada coeficiente
	for(long i=COEFFICIENT_A; i<(COEFFICIENT_LAST+1) ; i+=2) {
		Coeficiente = ListaCoeficientes[i];
		
		switch( Coeficiente.iEstiloEvolucion ) {
		case EVOLUTION_STYLE_INACTIVE:
				break;

		case EVOLUTION_STYLE_NOEVOLUTION:
				break;

		case EVOLUTION_STYLE_CONSTANT:
				if ((lIteracion > Coeficiente.lIteracionInicial) && (lIteracion < Coeficiente.lIteracionFinal)){
					Coeficiente.lContadorLlamadas++;
					if ( Coeficiente.lContadorLlamadas == Coeficiente.lLlamadasPorPaso ) {
						Coeficiente.dValorActual += Coeficiente.dIncrementoSuma;
						Coeficiente.lContadorLlamadas = 0;
					};
					break;
				};
				if (lIteracion == Coeficiente.lIteracionInicial)
					Coeficiente.dValorActual = Coeficiente.dValorInicial;
				if (lIteracion == Coeficiente.lIteracionFinal)
					Coeficiente.dValorActual = Coeficiente.dValorFinal;
				break;

		case EVOLUTION_STYLE_POTENTIAL:
				if ((lIteracion > Coeficiente.lIteracionInicial) && (lIteracion < Coeficiente.lIteracionFinal)){
					Coeficiente.lContadorLlamadas++;
					if ( Coeficiente.lContadorLlamadas == Coeficiente.lLlamadasPorPaso ) {
						Coeficiente.dValorActual *= Coeficiente.dIncrementoProducto;
						Coeficiente.lContadorLlamadas = 0;
					};
					break;
				};
				if (lIteracion == Coeficiente.lIteracionInicial)
					Coeficiente.dValorActual = Coeficiente.dValorInicial;
				if (lIteracion == Coeficiente.lIteracionFinal)
					Coeficiente.dValorActual = Coeficiente.dValorFinal;
				break;
		}; // Fin del SWITCH

		ListaCoeficientes[i] = Coeficiente;
	};
};
