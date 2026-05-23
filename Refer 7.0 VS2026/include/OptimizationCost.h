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

#ifndef OPTIMIZATIONCOST_VISITED
	#define OPTIMIZATIONCOST_VISITED

	//private:
	double det(double e11,double e12,double e13,
			   double e21,double e22,double e23,
			   double e31,double e32,double e33);
	double GetMUab( double a, double b, double x );

	double SymLipson(int j, CVerticesSet *pListaVertices2);
	double SkewLipson(int j, CVerticesSet *pListaVertices2);
	void GetSymmetryCoefs (CVerticesSet *pListaVertices2, 
						   CVertex &VerticeEje, CVertex &VerticeEje2,
						   int &k, double &W);

	double GetCost_MSDA(std::vector<double> &CurrentSolution, CDB_Model *DDBB);
	double GetCost_FacePlanarity(std::vector<double> &CurrentSolution, CDB_Model *DDBB);

	double GetCost_LineParallelism(std::vector<double> &CurrentSolution, CDB_Model *DDBB);
	double GetCost_LineCollinearity(std::vector<double> &CurrentSolution, CDB_Model *DDBB);
	double GetCost_LineVerticality(std::vector<double> &CurrentSolution, CDB_Model *DDBB);

	double GetCost_LineOrthogonality(std::vector<double> &CurrentSolution, CDB_Model *DDBB);

	double GetCost_Isometry(std::vector<double> &Solucion, CDB_Model *DDBB);

	//double betaCubicCorner(CVector a1, CVector a2, CVector a3);
	double GetCost_CubicCorners(std::vector<double> &CurrentSolution, CDB_Model *DDBB);
	double GetCost_FaceOrthogonality(std::vector<double> &Solucion, CDB_Model *DDBB);

	double GetCost_ObliqueFacialOrthogonality(std::vector<double> &Solucion, CDB_Model *DDBB);
	double GetCost_SkewSymmetry(std::vector<double> &Solucion, CDB_Model *DDBB);

	//public:
	double CalculeCost(std::vector<double> &Solucion,
					   std::vector<double> &EvolutionCost,
					   std::vector<double> &EvolutionZ, bool evolution,
					   CDB_Model *DDBB);

	double CalculeCostMulti(std::vector<double> &Solucion,
							CDB_Model *DDBB,
							CDataInflation *pParams, 
							std::vector<double> &ListaEvolucionCoste,
							std::vector<double> &ListaEvolucionZ,
							std::vector<double> &ListaCostesRegularidades,
							bool bGuardarEvolucionVariables,
							bool bAplicarCoeficientes);

#endif  //OPTIMIZATIONCOST_VISITED

