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
#include <cfloat>
#include <cmath>
#include <cassert>
#include <vector>

#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "ReferDoc.h"
#include "Refer.h"
#include "resource.h"
#include "DB_Model_Maths.h"

#include "DialogProgress.h"

#include "OptimizationFlow.h"
#include "OptimizationCost.h"
#include "OptimizationBasedInflation.h"


void COptimizationInflation::UpdateActiveRegularities(CReferDoc* pDoc)
//Updates the list of regularities that are currently active

//In automatic selection, the type of the model 
//drives the selection of the active regularities
//While in the manual selection, 
//the list of the regularities selected by the user are activated.
{
	//Select active cues
	//------------------
	if (ReferApp.In.iRegularitiesSelection == 0){ //Automatic selection
		switch (ReferApp.Cu.Ty.Typology) {
		case Normalon:
			for (long i=0; i<REGULARITY_LAST +1; i++)
				ReferApp.In.Regularities.ListOfRegularities[i].bActive =
					ReferApp.In.RegularitiesNormalon.ListOfRegularities[i].bActive;
			ReferApp.In.Regularities.NumActiveRegularities =
				ReferApp.In.RegularitiesNormalon.NumActiveRegularities;
			break;

		case Pyramidal:
			for (long i=0; i<REGULARITY_LAST +1; i++)
				ReferApp.In.Regularities.ListOfRegularities[i].bActive =
					ReferApp.In.RegularitiesPyramidal.ListOfRegularities[i].bActive;
			ReferApp.In.Regularities.NumActiveRegularities =
				ReferApp.In.RegularitiesPyramidal.NumActiveRegularities;
			break;

		case Prismatic:
			for (long i=0; i<REGULARITY_LAST +1; i++)
				ReferApp.In.Regularities.ListOfRegularities[i].bActive =
					ReferApp.In.RegularitiesPrismatic.ListOfRegularities[i].bActive;
			ReferApp.In.Regularities.NumActiveRegularities =
				ReferApp.In.RegularitiesPrismatic.NumActiveRegularities;
			break;

		case QuasiNormalon:
			for (long i=0; i<REGULARITY_LAST +1; i++)
				ReferApp.In.Regularities.ListOfRegularities[i].bActive =
					ReferApp.In.RegularitiesQuasiNormalon.ListOfRegularities[i].bActive;
			ReferApp.In.Regularities.NumActiveRegularities =
				ReferApp.In.RegularitiesQuasiNormalon.NumActiveRegularities;
			break;

		case Undefined_D:
			for (long i=0; i<REGULARITY_LAST +1; i++)
				ReferApp.In.Regularities.ListOfRegularities[i].bActive =
					ReferApp.In.RegularitiesIndefined_D.ListOfRegularities[i].bActive;
			ReferApp.In.Regularities.NumActiveRegularities =
				ReferApp.In.RegularitiesIndefined_D.NumActiveRegularities;
			break;

		case Undefined_ND:
			for (long i=0; i<REGULARITY_LAST +1; i++)
				ReferApp.In.Regularities.ListOfRegularities[i].bActive =
					ReferApp.In.RegularitiesIndefined_ND.ListOfRegularities[i].bActive;
			ReferApp.In.Regularities.NumActiveRegularities =
				ReferApp.In.RegularitiesIndefined_ND.NumActiveRegularities;
			break;
		}
	}

	else{ //Manual selection
		for (long i=0; i<REGULARITY_LAST +1; i++)
			ReferApp.In.Regularities.ListOfRegularities[i].bActive =
				ReferApp.In.RegularitiesUser.ListOfRegularities[i].bActive;
		ReferApp.In.Regularities.NumActiveRegularities =
			ReferApp.In.RegularitiesUser.NumActiveRegularities;
	}
}


double COptimizationInflation::GetMUab( double a, double b, double x )
// Returns continuos compliance factor

//An exponential function is used to calculate
//the law of mitigation of the cost of certain regularities

//It is a "dumped" factor that equals 1 for null divergence (x=a)
//and descends to zero like a normal distribution curve 
//for x different to a (with sigma=b)

//In other words:
//	- x is the value to be checked, 
//	- a is the nominal value, and 
//	- b is a reasonable max deviation

//Based on:
//	H Lipson, M Shpitalni
//	Optimization-based reconstruction of a 3D object
//	from a single freehand line drawing
//	Computer-Aided Design, 28(8) 651-663, 1996
{
	double dValor= 1.1 * exp( - pow((x-a)/b, 2) ) - 0.1;
	assert(!_isnan(dValor));

	return max(0, dValor);
}


void COptimizationInflation::GetCues_LineParallelism(CDB_Model *DDBB)
//Explore all possible pairings between model edges
//and calculate a parallelism weight for each pairing.

//Those pairs with non-zero weight will be used to increase the cost of parallelism
//in the event that they are not parallel in the tentative model.
{
	//Parallelism cues are reset to zeros
	ReferApp.In.Cues_LP.clear();
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		ReferApp.In.Cues_LP.push_back(std::vector<double>());
		for (long j=0; j<DDBB->GetSizeEdges(); j++){
			ReferApp.In.Cues_LP[i].push_back(0);
		}
	}

	// Match each edge with all the others
	for(long i=0; i<DDBB->GetSizeEdges(); i++ ){
		//ReferApp.In.Cues_LP[i][i]= 0; 

		//Given the edge Arista1, check its parallelism with all the others
		for(long j=i+1; j<DDBB->GetSizeEdges(); j++ ) {
			//Calculate weight as a damped function
			//which equals 1 when they are parallel (angle 0)
			//reduce a little until reaching the threshold
			//and reduces to zero for values slightly larger than the threshold
			ReferApp.In.Cues_LP[i][j]= 
			ReferApp.In.Cues_LP[j][i]= GetMUab(0, ReferApp.In.InflationAngleThreshold, DDBB->GetAngle2D_PI_half(i, j));
		};
	}
}


void COptimizationInflation::GetCues_LineCollinearity(CDB_Model *DDBB)
//Explore all possible pairings between drawing edges
//and calculate a collinearity weight for each pairing.

//Current implementation assumes that collinearity in the strict sense 
//can only be applied to consecutive lines (that share a vertex).

//Those pairs with non-zero weight will be used to increase the cost of collinearity
//in the event that they are not collinear in the tentative drawing.

//Previous implementations compared all possible pairs of axes as potentially collinear
{
	//Collinearity cues are reset to zeros
	ReferApp.In.Cues_LC.clear();
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		ReferApp.In.Cues_LC.push_back(std::vector<double>());
		for (long j=0; j<DDBB->GetSizeEdges(); j++){
			ReferApp.In.Cues_LC[i].push_back(0);
		}
	}

	//Loop over the current drawing
	for (long i= 0; i<DDBB->GetSizeVertices(); i++){
		//Search for consecutive edges
		if(DDBB->GetSizeEdgesInVertex(i) == 2){   // Check only vertices of valence 2
			long numEdge1= DDBB->GetEdgeInVertex(i, 0);
			long numEdge2= DDBB->GetEdgeInVertex(i, 1);

			////Check "nearly" parallelism of consecutive edges
			//double angle= DDBB->GetAngle2D_PI_half(numEdge1, numEdge2);
			//double weight = CalculeMUab( 0, ErrorCollinearity2D, angle);  //Wrong calculation: lines may overlap!!

			CEdge E1= DDBB->GetEdge(numEdge1);
			CEdge E2= DDBB->GetEdge(numEdge2);

			//Encompassed angle around the pivot (vertex "i")
			double angle= 0.;
			if(E1.Head == E2.Head){
				angle= DDBB->GetAngle2D_PI(E1.Tail, E1.Head, E2.Tail);
			}
			else if(E1.Head == E2.Tail){
				angle= DDBB->GetAngle2D_PI(E1.Tail, E1.Head, E2.Head);
			}
			else if(E1.Tail == E2.Head){
				angle= DDBB->GetAngle2D_PI(E1.Head, E1.Tail, E2.Tail);
			}
			else if(E1.Tail == E2.Tail){
				angle= DDBB->GetAngle2D_PI(E1.Head, E1.Tail, E2.Head);
			}
			else //Edges do not share a common vertex!
				continue;

			//Collinear lines sharing a common vertex must encompass an angle of PI
			double cost = GetMUab(0, ReferApp.In.InflationAngleThreshold, std::fabs(NUMBER_PI - angle));

			if(cost > Accuracy){
				ReferApp.In.Cues_LC[numEdge1][numEdge2]= cost; 
				ReferApp.In.Cues_LC[numEdge2][numEdge1]= cost;
			}
			//else{
			//	ReferApp.In.Cues_LC[numEdge1][numEdge2]= 0; 
			//	ReferApp.In.Cues_LC[numEdge2][numEdge1]= 0;
			//}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////

//What follows is the deprecated Ana Piquer's formulation, 
//which differs from the original (deprecated as well) from Lipson and Shpitalni


//bool COptimizationInflation::linesOverlap(CVertex v1, CVertex v2, CVertex v3, CVertex v4)
////Check that lines (v1,v2) and (v3,v4) do not overlap
//
////Return false if lines do not overlap, true otherwise
//{
//	if (v1.x == v2.x || v3.x == v4.x){
//		//At least one line is vertical
//		if (v1.y > v2.y) std::swap(v1, v2);  //line 1 up
//		if (v3.y > v4.y) std::swap(v3, v4);  //line 2 up
//
//		if (v2.y <= v3.y){  //line 2 after line 1
//			return false;
//		}
//
//		if (v4.y <= v1.y){  //line 2 before line 1
//			return false;
//		}
//	}
//
//	else{
//		if (v1.x > v2.x) std::swap(v1, v2);  //line 1 left to right
//		if (v3.x > v4.x) std::swap(v3, v4);  //line 2 left to right
//
//		if (v2.x <= v3.x){  //line 2 after line 1
//			return false;
//		}
//
//		if (v4.x <= v1.x){  //line 2 before line 1
//			return false;
//		}
//	}
//
//	return true;
//
//
//			//bool bien=false;
//			//int es;
//			//if (v1.x < v3.x) es=1;
//			//if (v1.x >v3.x) es=2;
//			//if (v1.x == v3.x) es=0;
//			//switch (es){
//			//case 1:
//			//	if (v2.x <=v4.x)(bien=true);
//			//	break;
//			//case 2:
//			//	if (v4.x<=v1.x) bien=true;
//			//	break;
//			//case 0:
//			//	//este caso es si las lineas son verticales
//			//	if (v1.y<v2.y){
//			//		if (v3.y<v4.y){
//			//			if (v2.y<=v3.y)
//			//				bien=true;
//			//		}
//			//		else{
//			//			if (v2.y<=v4.y)
//			//				bien=true;
//			//		}
//			//	}
//			//	else{
//			//		if (v3.y<v4.y){
//			//			if (v1.y<=v3.y)
//			//				bien=true;
//			//		}
//			//		else{
//			//			if (v1.y<=v4.y)
//			//				bien=true;
//			//		}
//			//	}
//			//}
//}


//void COptimizationInflation::GetCues_LineCollinearity(CDB_Model *DDBB)
////Explore all possible pairings between model edges
////and calculate a collinearity weight for each pairing.
//
////Those pairs with non-zero weight will be used to increase the cost of collinearity
////in the event that they are not collinear in the tentative model.
//
////As implemented in:
////	Ana Piquer Vicent.
////	PERCEPCIÓN ARTIFICIAL DE DIBUJOS LINEALES.
////	PhD Thesis, 2003.
////	Departamento de Tecnología.
////	Universitat Jaume I.
////	http://www.tdx.cat/handle/10803/10554;jsessionid=A882045F99097C47E8CBE8B5D67E6FC3.tdx2
//{
//	//Cues are reset to zeros
//	ReferApp.In.Cues_LC.clear();
//	for (long i=0; i<DDBB->GetSizeEdges(); i++){
//		ReferApp.In.Cues_LC.push_back(std::vector<double>());
//		for (long j=0; j<DDBB->GetSizeEdges(); j++){
//			ReferApp.In.Cues_LC[i].push_back(0);
//		}
//	}
//
//	for (long i=0; i<DDBB->GetSizeEdges(); i++){
//		ReferApp.In.Cues_LC[i][i] =0;
//		CEdge Arista= DDBB->GetEdge(i);
//		CVertex v1= DDBB->GetVertex(Arista.Head);
//		CVertex v2= DDBB->GetVertex(Arista.Tail);
//		if (v1.x > v2.x) std::swap(v1, v2);
//
//		for (long j=i+1; j<DDBB->GetSizeEdges(); j++) {
//			CEdge Arista= DDBB->GetEdge(j);
//			CVertex v3= DDBB->GetVertex(Arista.Head);
//			CVertex v4= DDBB->GetVertex(Arista.Tail);
//			if (v3.x > v4.x)std::swap(v3, v4);
//
//			double costIJ;
//			if (!linesOverlap(v1, v2, v3, v4)){
//				//Angle between line i and line j
//				CVector VectorUnitario2D1= GetUnitVector2D(v1, v2);
//				CVector VectorUnitario2D2= GetUnitVector2D(v3, v4);
//				double dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D2 );
//				//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//				//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//				double angle= acos(dProductoEscalar);
//				if (std::fabs(angle)>(NUMBER_PI/2))
//					angle = std::fabs(angle-NUMBER_PI);
//
//				//Maximum angle between the line i and the four auxiliar lines
//				//(those connecting tips of line i to tips of line j)
//				double angulo3;
//				if ((std::fabs(v1.x-v4.x)<Accuracy) && (std::fabs(v1.y-v4.y)<Accuracy) &&
//					(std::fabs(v1.z-v4.z)<Accuracy)){
//					angulo3= 0;
//				}
//				else{
//					CVector VectorUnitario2D3= GetUnitVector2D(v1, v4);
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D3 );
//					//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//					//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//					angulo3= acos(dProductoEscalar);
//					if (std::fabs(angulo3)>(NUMBER_PI/2)) angulo3 = std::fabs(angulo3-NUMBER_PI);
//				}
//
//				double angulo4;
//				if ((std::fabs(v1.x-v3.x)<Accuracy) && (std::fabs(v1.y-v3.y)<Accuracy) &&
//					(std::fabs(v1.z-v3.z)<Accuracy)){
//					angulo4=0;
//				}
//				else{
//					CVector VectorUnitario2D4= GetUnitVector2D(v1, v3);
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D4 );
//					//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//					//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//					angulo4= acos(dProductoEscalar);
//					if (std::fabs(angulo4)>(NUMBER_PI/2)) angulo4 = std::fabs(angulo4-NUMBER_PI);
//				}
//
//				double angulo5;
//				if ((std::fabs(v2.x-v4.x)<Accuracy) && (std::fabs(v2.y-v4.y)<Accuracy) &&
//					(std::fabs(v2.z-v4.z)<Accuracy)){
//					angulo5=0;
//				}
//				else{
//					CVector VectorUnitario2D5= GetUnitVector2D(v2, v4);
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D5 );
//					//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//					//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//					angulo5=acos(dProductoEscalar);
//					if (std::fabs(angulo5)>(NUMBER_PI/2)) angulo5 = std::fabs(angulo5-NUMBER_PI);
//				}
//
//				double angulo6;
//				if ((std::fabs(v2.x-v3.x)<Accuracy) && (std::fabs(v2.y-v3.y)<Accuracy) &&
//					(std::fabs(v2.z-v3.z)<Accuracy)){
//					angulo6= 0;
//				}
//				else{
//					CVector VectorUnitario2D6= GetUnitVector2D(v2, v3);
//					dProductoEscalar = DOT_PRODUCT_2D( VectorUnitario2D1, VectorUnitario2D6 );
//					//if (dProductoEscalar < 1+Accuracy && dProductoEscalar > 1) dProductoEscalar=1;
//					//if (dProductoEscalar < -1 && dProductoEscalar > -1-Accuracy) dProductoEscalar=-1;
//					angulo6=acos(dProductoEscalar);
//					if (std::fabs(angulo6)>(NUMBER_PI/2)) angulo6 = std::fabs(angulo6-NUMBER_PI);
//				}
//				double angleAux= max(angulo3, angulo4);
//				angleAux= max(angleAux, angulo5);
//				angleAux= max(angleAux, angulo6);
//
//				costIJ = GetMUab(0, ReferApp.In.InflationAngleThreshold, (angle+angleAux));
//			}
//			else{ 
//				costIJ= 0;
//			}
//
//			if(costIJ > Accuracy){
//				//Check if both lines belong to the same face, if required
//				long numFaces= DDBB->GetSizeFaces();
//				if(ReferApp.In.Cues_LC_shareFace && numFaces>0){
//					bool CompartenCara=false;
//					for(long k=0; k<DDBB->GetSizeFaces(); k++){
//						if((DDBB->IsEdgeInFace(k,i)) && (DDBB->IsEdgeInFace(k,j))){
//							CompartenCara=true;
//							break;
//						}
//					}
//
//					if (CompartenCara==true){
//						ReferApp.In.Cues_LC[i][j]= costIJ; 
//						ReferApp.In.Cues_LC[j][i]= costIJ;
//					}
//					else{
//						//ReferApp.In.Cues_LC[j][i]= 0.0;
//						//ReferApp.In.Cues_LC[i][j]= 0.0;
//					}
//				}
//				else{
//					ReferApp.In.Cues_LC[i][j]= costIJ; 
//					ReferApp.In.Cues_LC[j][i]= costIJ;
//				}
//			}
//			else{
//				//ReferApp.In.Cues_LC[j][i]= 0.0;
//				//ReferApp.In.Cues_LC[i][j]= 0.0;
//			}
//		}
//	}
//}
/////////////////////////////////////////////////////////////////////////////



void COptimizationInflation::GetCues_LineVerticality(CDB_Model *DDBB)
//Vertical lines must be parallel to the Y axis in the 2D drawing

//The compliance function to measure parallelism between
//each edge and the vertical axis is calculated
{
	//Verticality cues are reset to zeros
	ReferApp.In.Cues_LO.clear();
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		ReferApp.In.Cues_LV.push_back(0);
	}

	for (long i=0;i<DDBB->GetSizeEdges();i++) {
		CEdge Edge= DDBB->GetEdge(i);

		////////// Verticality by angle ///////////////////////
		CVector V (DDBB->GetVertex(Edge.Head), DDBB->GetVertex(Edge.Tail));
		CVector VV= V;
		VV.x= 0.;
		ReferApp.In.Cues_LV[i]= GetMUab(0, ReferApp.In.InflationAngleThreshold, DDBB->GetAngle2D_PI_half(V, VV));
		////////////////////////////////////////////////////////

		////////// Verticality by x coordinates ///////////////
		//double deltaX= std::fabs(DDBB->GetVertex(Edge.Head).x - DDBB->GetVertex(Edge.Tail).x);
		//ReferApp.In.Cues_LV[i]= GetMUab(0, ReferApp.In.InflationAngleThreshold, deltaX);
		////////////////////////////////////////////////////////
	}
}


void COptimizationInflation::GetCues_LineOrthogonality(CDB_Model *DDBB)
//Assign weights to pairs of lines with the following criterion:
//	- (0,1] if both lines share a common vertex and
//	        run nearly parallel to any of the three main directions 
//	        (which must have been calculated in advance)
//	- 0 Otherwise

//Te weights are stored in a matrix of numEdges x numEdges:
//	- If lines i and j share a common vertex and
//    are both "nearly" parallel to any of the main axes, weight(i,j)= (0,1]
//	- Otherwise weight(i,j)= 0
{
	//Line Orthogonality cues are reset to zeros
	ReferApp.In.Cues_LO.clear();
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		ReferApp.In.Cues_LO.push_back(std::vector<double>());
		for (long j=0; j<DDBB->GetSizeEdges(); j++){
			ReferApp.In.Cues_LO[i].push_back(0);
		}
	}

	if(ReferApp.Cu.MA.MainAngles.size() != 3){
		//AfxMessageBox ("Regularity of Line Orthogonality requires main axes");
		//CReferDoc::GetDoc()->RefreshWindows();
		return;
	}

	//Search for vertices with at least two incident edges (valence >=2)
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		long NumEdgesI= DDBB->GetSizeEdgesInVertex(i);
		if (NumEdgesI >= 2) {
			//Search for a first edge "nearly" parallel to any main direction
			for (long j=0; j<NumEdgesI; j++) {
				//Calculate the orientation of the edge
				long numEdgeIJ= DDBB->GetEdgeInVertex(i, j);
				double AngleJ= DDBB->GetAngle2DX_PI(numEdgeIJ, Accuracy);
				double AngleJ180= AngleJ + NUMBER_PI;

				//Check if the angle matches any of the three main directions
				bool found= false;
				long jj=-1;
				double meritJ= 0, meritJ180= 0;
				while (jj<2 && !found){
					jj++;
					meritJ= GetMUab(0, ReferApp.In.InflationAngleThreshold, std::fabs(AngleJ - ReferApp.Cu.MA.MainAngles[jj]));
					meritJ180= GetMUab(0, ReferApp.In.InflationAngleThreshold, std::fabs(AngleJ180 - ReferApp.Cu.MA.MainAngles[jj]));
					meritJ= max(meritJ, meritJ180);
					if (meritJ > Accuracy)
						found= true;
				}

				if (found){
					//Check if there is another edge sharing the vertex and aligned as well with another main direction
					for (long k= j+1; k<NumEdgesI; k++){
						//Calculate the angle of the edge
						long numEdgeIK= DDBB->GetEdgeInVertex(i, k);
						double AngleK= DDBB->GetAngle2DX_PI(numEdgeIK, Accuracy);
						double AngleK180= AngleK + NUMBER_PI;

						//Check if the angle matches any of the three main directions
						found= false;
						long kk=-1;
						double meritK= 0;
						while (kk<2 && !found){
							kk++;
							meritK= GetMUab(0, ReferApp.In.InflationAngleThreshold, std::fabs(AngleK - ReferApp.Cu.MA.MainAngles[kk]));
							double meritK180= GetMUab(0, ReferApp.In.InflationAngleThreshold, std::fabs(AngleK180 - ReferApp.Cu.MA.MainAngles[kk]));
							meritK= max(meritK, meritK180);
							if (meritK > Accuracy)
								found= true;
						}

						if (found && kk!=jj){
							//If it is another main address 
							//put a weight of one to the pair of edges
							ReferApp.In.Cues_LO[numEdgeIJ][numEdgeIK]=
							ReferApp.In.Cues_LO[numEdgeIK][numEdgeIJ]= min(meritJ, meritK);
						}
					}
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

//What follows is the deprecated Ana Piquer's formulation, 

//void COptimizationInflation::GetCues_LineOrthogonality(CDB_Model *DDBB)
////Assign weights to pairs of lines with the following criterion:
////	- 1 if both lines run nearly parallel to any of the three main directions 
////	  (which must have been calculated in advance)
////	- 0 Otherwise
//
////Te weights are stored in a matrix of numVertices x numVertices:
////	- If lines i and j are both parallel to any of the main axes, weight(i,j)= 1
////	- Otherwise weight(i,j)= 0
//{
//	//Orthogonallity cues are reset to zeros
//	ReferApp.In.Cues_LO.clear();
//	for (long i=0; i<DDBB->GetSizeEdges(); i++){
//		ReferApp.In.Cues_LO.push_back(std::vector<double>());
//		for (long j=0; j<DDBB->GetSizeEdges(); j++){
//			ReferApp.In.Cues_LO[i].push_back(0);
//		}
//	}
//
//	if(ReferApp.Cu.MA.MainAngles.size() == 0){
//		//Main Angles must have been calculated in advance
//		return;
//	}
//
//	//Visit all the edges of the model
//	for (long i=0; i<DDBB->GetSizeEdges(); i++){
//
//		//Calculate the angle of the edge
//		CEdge Arista= DDBB->GetEdge(i);
//		double Angle1= DDBB->GetAngle2DX_2PI(i, Arista.Head);
//		double Angle11= 2.*NUMBER_PI - Angle1;
//
//		//Check if the angle is one of the main directions
//		bool found= false;
//		long jj=0;
//		while (jj<(long)ReferApp.Cu.MA.MainAngles.size() && !found){
//			if (std::fabs(Angle1 - ReferApp.Cu.MA.MainAngles[jj]) < ReferApp.In.InflationAngleThreshold ||
//				std::fabs(Angle11 - ReferApp.Cu.MA.MainAngles[jj]) < ReferApp.In.InflationAngleThreshold)
//				found= true;
//			jj++;
//		}
//
//		if (!found){
//			//If it is not a main address, put a weight of 0 
//			//in the elements of the matrix where the i edge is
//			//for (long j=0; j<DDBB->GetSizeEdges(); j++){
//			//	ReferApp.In.Cues_LO[i][j]=0;
//			//	ReferApp.In.Cues_LO[j][i]=0;
//			//}
//		}
//		else{
//			//If it is a main address 
//			//check if the other edges are main directions
//			for (long j=0; j<DDBB->GetSizeEdges(); j++){
//				//Calculate the angle of the edge
//				CEdge AristaJ= DDBB->GetEdge(j);
//				double Angulo2= DDBB->GetAngle2DX_2PI(j, AristaJ.Head);
//				double Angulo22= 2.*NUMBER_PI - Angulo2;
//
//				//Check if the angle is one of the principal directions
//				found= false;
//				long k=0;
//				while (k<(long)ReferApp.Cu.MA.MainAngles.size() && !found){
//					if (std::fabs(Angulo1-ReferApp.Cu.MA.MainAngles[k]) < ReferApp.In.InflationAngleThreshold ||
//						std::fabs(Angulo22 - ReferApp.Cu.MA.MainAngles[k]) < ReferApp.In.InflationAngleThreshold)
//						found= true;
//					k++;
//				}
//
//				if (!found){
//					//If it is not the main address 
//					//put a weight of 0 on the elements of the matrix where the i edge is
//					//ReferApp.In.Cues_LO[i][j]=0;
//					//ReferApp.In.Cues_LO[j][i]=0;
//				}
//				else{
//					//If it is main address 
//					//put a weight of one to the pair of edges
//					ReferApp.In.Cues_LO[i][j]=
//					ReferApp.In.Cues_LO[j][i]= 1;
//					//ReferApp.In.Cues_LO[j][i]= GetMUab(0, ReferApp.In.InflationAngleThreshold, DDBB->GetAngle2D_PI_half(V, VV));
//				}
//			}
//		}
//	}
//}
/////////////////////////////////////////////////////////////////////////////


double COptimizationInflation::betaCubicCorner(CVector a1, CVector a2, CVector a3)
//To determine whether a junction of three lines in a plane 
//qualifies as a projection of an orthogonal corner,
//the following test due to Lipson and Sphitalni is applied, 
//based on the fact that the projection of an orthogonal corner spans at least 90 degrees:

		//(1) A junction of three lines has eight variants, 
		//created by flipping the direction of each line
		//and considering the eight resulting permutations

		//(2) The eight variants of the junction in the sketch plane are tested;

		//(3) The tree lies of each corner are described by 2D unit direction vectors,
		//pointing from the junction outwards.

		//(4) If a junction variant spans less than 90 degrees 
		//(i.e. is not a projection of an orthogonal corner), 
		//all of the three dot-products of its direction-vector pairs will be positive.

		//(5) Thus, if any one of the eight variants appears to span less than 90 degrees
		//(shows such an ‘all-positive’ condition), 
		//the tested junction is unlikely to represent an orthogonal corner.

//Based on:
//	H Lipson, M Shpitalni
//	Optimization-based reconstruction of a 3D object
//	from a single freehand line drawing
//	Computer-Aided Design, 28(8) 651-663, 1996
{
	CVector v1,v2,v3;
	double max= (double)-0xfffffffffffffff;   //A very (negative) large value (1.7976931348623157E+308)

	for (long i=0; i<=1; i++){
		for (long j=0; j<=1; j++){
			for (long k=0; k<=1; k++){
				if (i==0){
					v1.Tail= a1.Tail;   v1.Head= a1.Head;
				}
				else{
					v1.Tail= a1.Head;   v1.Head= a1.Tail;
				}

				if (j==0){
					v2.Tail= a2.Tail;   v2.Head= a2.Head;
				}
				else{
					v2.Tail= a2.Head;   v2.Head= a2.Tail;
				}

				if (k==0){
					v3.Tail= a3.Tail;   v3.Head= a3.Head;
				}
				else{
					v3.Tail= a3.Head;   v3.Head= a3.Tail;
				}

				// Calculates the 2D scalar product of vectors made unitary
				double minDP= DOT_PRODUCT_2D(v1, v2);
				double auxDP= DOT_PRODUCT_2D(v2, v3);
				if (minDP > auxDP) minDP= auxDP;
				auxDP= DOT_PRODUCT_2D(v3, v1);
				if (minDP > auxDP) minDP= auxDP;
				if (max < minDP) max= minDP;

			}
		}
	}
	return max;
}


void COptimizationInflation::GetCues_CubicCorners(CDB_Model *DDBB)
//Determines the list of 2D vertices 
//that are susceptible to corresponding to cubic corners

//Vector "corners" returns a value in the range [0 , 1], 
//where 0 means no cubic corner at all, 
//and 1 means a very plausible cubic corner
{
	std::vector<double> corners(DDBB->GetSizeVertices(), 0.);

	//Search for "corners": vertices with exactly three incident edges (valence 3)
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		if (DDBB->GetSizeEdgesInVertex(i) != 3) {    //Only cubic corners!!
			corners[i]= 0.;
		}

		else{
			std::vector<CVector> cornerI(3, CVector());

			//Add the three incident edges to corner
			for (long j=0; j<3; j++) {
				CEdge Arista= DDBB->GetEdge(DDBB->GetEdgeInVertex(i, j));

				if (Arista.Tail == i) {
					CVertex Vertice2= DDBB->GetVertex(Arista.Head);
					cornerI[j].Head.x= Vertice2.x;
					cornerI[j].Head.y= Vertice2.y;

					Vertice2= DDBB->GetVertex(Arista.Tail);
					cornerI[j].Tail.x= Vertice2.x;
					cornerI[j].Tail.y= Vertice2.y;
				}
				else {
					//The corner is rearranged so that all vectors point away from the union.
					//(if the head of the edge coincides with the vertex it is inverted,
					//so that the head of "cornerI" is the vertex opposite to the union vertex)
					CVertex Vertice2= DDBB->GetVertex(Arista.Head);
					cornerI[j].Tail.x= Vertice2.x;
					cornerI[j].Tail.y= Vertice2.y;

					Vertice2= DDBB->GetVertex(Arista.Tail);
					cornerI[j].Head.x= Vertice2.x;
					cornerI[j].Head.y= Vertice2.y;
				}
			}

			double beta= betaCubicCorner(cornerI[0], cornerI[1], cornerI[2]);
			if (beta > 0){
				corners[i]= GetMUab(0, 0.1, beta);
			}
			else{
				corners[i]= 1.;
			}
		}
	}

	//Save candidate cubic corners likelyhood for latter use
	ReferApp.In.Cues_CC= corners;
}


void COptimizationInflation::UpdateCuesOfRegularities(CReferDoc* pDoc)
//Updates the cues required by the active regularities

//Note the cues are calculated in the 2D drawing,
//because it must be decided which are the "true" regularities 
//that should be calculated, based on clues 
//that are measured in the drawing, before reconstructing.

//Thus, they are calculated once, not iteratively
{
	if (ReferApp.In.Regularities.NumActiveRegularities == 0)
		return;

	//Calculate cues
	//--------------
	// Get cues for line parallelism
	if (ReferApp.In.Regularities.ListOfRegularities[REGULARITY_LINE_PARALLELISM].bActive){
		GetCues_LineParallelism(pDoc->DB_DrawingOff);
	}

	//Gets cues for line collinearity
	if (ReferApp.In.Regularities.ListOfRegularities[REGULARITY_LINE_COLLINEARITY].bActive){
		GetCues_LineCollinearity(pDoc->DB_DrawingOff);
	}

	//Gets cues for line Verticality
	if (ReferApp.In.Regularities.ListOfRegularities[REGULARITY_LINE_VERTICALITY].bActive){
		GetCues_LineVerticality(pDoc->DB_DrawingOff);
	}

	//Get cues related to line orthogonality
	//(Cue is 1 for lines parallel to main axes, and 0 otherwise)
	if (ReferApp.In.Regularities.ListOfRegularities[REGULARITY_LINE_ORTHOGONALITY].bActive){
		GetCues_LineOrthogonality(pDoc->DB_DrawingOff);
	}

	//Get cues related to cubic corners
	//(Cue is 1 for corners that qualify as 3D orthogonal corners, and 0 otherwise)
	if (ReferApp.In.Regularities.ListOfRegularities[REGULARITY_CUBIC_CORNERS].bActive){
		GetCues_CubicCorners(pDoc->DB_DrawingOff);
	}
}


void COptimizationInflation::ResetEvolutionHC(std::vector<double> &EvolutionCosts,
											  CDataInflation &ParametrosOpt,
											  CDataOptHC &DataOptHC)
//resets cost to zero

////Reset the list of evolution of the Hill-Climbing variables,
{
	EvolutionCosts.clear();

	//long NumIncrementsOfVariables = (long)DataOptHC.IncrementsOfVariables.size();

	//if (NumIncrementsOfVariables == 0)   // ¿Que pasa en este caso?
	//	return;

	//for (long i = 0; i < NumIncrementsOfVariables; i++ ) {
	//	double dIncrementVariableI= DataOptHC.IncrementsOfVariables[i];

	//	for (long j= 0 ; j<DataOptHC.iMaxIterations; j++ )
	//		for (long k = 0; k<ParametrosOpt.NumVariables; k++) {
	//			EvolutionCosts.push_back(dIncrementVariableI);
	//			EvolutionCosts.push_back(0);

	//			// Si hay varios componentes de coste, hay que ponerlos a cero
	//			if (ParametrosOpt.bViewPartialCostsEvolution)
	//				for(long ii=0; ii<ParametrosOpt.Regularities.NumActiveRegularities; ii++)
	//					EvolutionCosts.push_back(0);
	//		};
	//};
}


void COptimizationInflation::ResetEvolutionSA(std::vector<double> &EvolutionCosts,
											  CDataInflation &ParametrosOpt,
											  CDataOptSA &DataOptSA,
											  CDataOptSAM &DataOptSAM)
//resets cost to zero

//Reset the list of evolution of the Simulated Annealing variables,
{
	EvolutionCosts.clear();

	long iCondicionEquilibrio = DataOptSA.iMaxIterationsPerLevel *
							    ParametrosOpt.NumVariables;
	double dTemperatura = 100.;

	for (long iContadorEscalones=0 ; iContadorEscalones < DataOptSA.iMaxLevels; iContadorEscalones++ ){
		for (long iContadorSoluciones=0; iContadorSoluciones < iCondicionEquilibrio; iContadorSoluciones++) {
			EvolutionCosts.push_back(dTemperatura );
			EvolutionCosts.push_back(0);

			// Si hay varios componentes de coste, hay que ponerlos a cero
			if (ParametrosOpt.bViewPartialCostsEvolution)
				for(long i=0; i<ParametrosOpt.Regularities.NumActiveRegularities; i++)
					EvolutionCosts.push_back(0);
		};
		// Evolucion de temperatura segun una curva de tipo potencial
		dTemperatura *= DataOptSA.dDecrementoTemperaturaPotencial;
	};
}


void COptimizationInflation::ResetEvolutionSAM(std::vector<double> &EvolutionCosts,
											   CDataInflation &ParametrosOpt,
											   CDataOptSA &DataOptSA,
											   CDataOptSAM &DataOptSAM)
//resets cost to zero

//Reset the list of evolution of the Simulated Annealing Multicriterion variables,
{
	EvolutionCosts.clear();

	long iCondicionEquilibrio = DataOptSAM.iMaxIterationsPerLevel *
								ParametrosOpt.NumVariables;
	double dTemperatura = 100.;

	for (long iContadorEscalones=0 ; iContadorEscalones < DataOptSAM.iMaxLevels; iContadorEscalones++ ){
		for (long iContadorSoluciones=0; iContadorSoluciones < iCondicionEquilibrio; iContadorSoluciones++) {
			EvolutionCosts.push_back(dTemperatura);
			EvolutionCosts.push_back(0);

			// Si hay varios componentes de coste, hay que ponerlos a cero
			if (ParametrosOpt.bViewPartialCostsEvolution)
				for(long i=0; i<ParametrosOpt.Regularities.NumActiveRegularities; i++)
					EvolutionCosts.push_back(0);
		};
		// Evolucion de temperatura segun una curva de tipo potencial
		dTemperatura *= DataOptSAM.dDecrementoTemperaturaPotencial;
	};
}


void COptimizationInflation::ResetEvolutionVariables(std::vector<double> &EvolutionCosts,
													 CDataOptHC &DataOptHC,
													 CDataOptSA &DataOptSA,
													 CDataOptSAM &DataOptSAM)
// Initializes the List of the Evolution of the Variables of the Optimization Algorithm, 
// setting the Cost to zero, 
// and creating the Step Length of the variable increment (Hill-Climbing)
// or the Temperature line (Simulated Annealing).
{
	switch (ReferApp.In.iOptAlgorithm){
		case HILL_CLIMBING:
			ResetEvolutionHC(EvolutionCosts, ReferApp.In, DataOptHC);
			break;
		case SIMULATED_ANNEALING:
			ResetEvolutionSA(EvolutionCosts, ReferApp.In, DataOptSA, DataOptSAM);
			break;
		case SIMULATED_ANNEALING_MULTI:
			ResetEvolutionSAM(EvolutionCosts, ReferApp.In, DataOptSA, DataOptSAM);
			break;
	};
	return;
};


bool COptimizationInflation::CheckMetropolis(double CosteActual, double CosteAnterior, double Temperatura)
// Returns true if the the cost has decreased 
//or the Metropolis criteria is accomplished
{
	double IncrementoCoste = CosteActual - CosteAnterior;
	if (IncrementoCoste <= 0)
		// TRUE is cost has decreased
		return true;
	else {
		// TRUE if METROPOLIS
		if ( exp( -IncrementoCoste/Temperatura ) > ( (double)rand() / (double)(RAND_MAX+1)) )
			return true;
		else
			return false;
	};
};


bool COptimizationInflation::ThereAreZeroRegularityCosts(std::vector<double> ListaCostes)
// Returns TRUE if there is any Cost with a value less than or equal to 0
{
	bool ThereAreNulls = false;

	for(long i=0; i < REGULARITY_LAST+1; i++ ) {
		if (ReferApp.In.Regularities.ListOfRegularities[i].bActive){
			if (ListaCostes[i] <= 0){
				ThereAreNulls = true;
				break;
			}
		};
	};
	return ThereAreNulls;
};


void COptimizationInflation::GetNewSolutionHC(std::vector<double> &Solution, 
											  std::vector<double> &NewSolution, 
											  CDataOptHC* pParams)
// Selects which variable to modify, and how long modify it.
// Stores the result in NewSolution

//The strategy to select a new candidate solution is as follows:
//	- Each variable is visited two times
//	- Even visits will result in positive increments, and 
//	- odd visits will result in negative increments
{
	// Assign a new solution equal to the prior solution
	NewSolution= Solution;

	// Select WHICH variable will be modified
	div_t ResultadoDivision = div(pParams->iSolutionCurrent, 2);
	int iVariableModified = ResultadoDivision.quot;

	// Selecct HOW MUCH the variable will be modified
	double dIncrement= pParams->IncrementsOfVariables[pParams->iIncrementVariableCurrent];

	// Selecct HOW the variable will be modifiedd: adding or subtracting
	double step= NewSolution[iVariableModified];
	if ( ResultadoDivision.rem == 0 )
		step += dIncrement;
	else
		step -= dIncrement;

	//assert( step < 1000000.);

	NewSolution[iVariableModified]= step;
};


void COptimizationInflation::GetNewSolutionSA(std::vector<double> &Solucion, std::vector<double> &NuevaSolucion,
											  CDataInflation* pParams, CDataOptSA* DataOptSA, CDataOptSAM* DataOptSAM)
//Randomly select a new solutions that modiffies the current one
{
	//Initialize new solution
	NuevaSolucion= Solucion;

	//Randomly select a variable to modify it
	int iVariableModificada = (int) (((double)rand() / (double)(RAND_MAX+1)) * (double)pParams->NumVariables);
	assert( iVariableModificada <= pParams->NumVariables );
	double dDato= NuevaSolucion[iVariableModificada];

	//Randomly select how the variable is going to be modified
	if ( pParams->iOptAlgorithm == SIMULATED_ANNEALING )
		if ( ((double)rand() / (double)(RAND_MAX+1)) < 0.5 )
			dDato += DataOptSA->dIncrementoVariablesActual;
		else
			dDato -= DataOptSA->dIncrementoVariablesActual;
	else
		if ( ((double)rand() / (double)(RAND_MAX+1)) < 0.5 )
			dDato += DataOptSAM->dIncrementoVariablesActual;
		else
			dDato -= DataOptSAM->dIncrementoVariablesActual;

	NuevaSolucion[iVariableModificada]= dDato;
};


bool COptimizationInflation::OptimizationHC(CReferDoc* pDoc,
											long *pNumeroIteracionesRealizadas,
											CDialogProgress *pProgress)
//Calculates an optimum set of z coordinates according to Hill Climbing approach
{
	bool NoCalcelled= true;

	// INITIALIZATION OF VARIABLES
	double dCoefAceptacionMejorCoste = 1 - ReferApp.In.DataOptHC.dThresholdRejectBestCost;
	double dCoefAceptacionMejorCosteEscalon = 1 - ReferApp.In.DataOptHC.dThresholdRejectBestCostInLevel;
	double dMejorCosteIncremento = -1;

	// Create the lists of solutions and best solutions
	std::vector<double> NuevaSolucion;
	std::vector<double> MejorSolucion (VariablesZ);

	srand( (unsigned)time( NULL ) );  // Reset random numbers with the system clock

	UpdateCoefficientsRegularities( ReferApp.In.Regularities.ListaCoeficientes, 0 );

	// REPEAT FOR EACH VARIABLE INCREMENT (STEP LENGTH)
	long iContadorIncrementosVariables = 0;
	long lContadorTotalIteraciones = 0;
	long lContadorIteraciones_ParaCoeficientesRegularidades = 0;
	do {
		ReferApp.In.DataOptHC.iIncrementVariableCurrent = iContadorIncrementosVariables;

		// REPEAT UNTIL NO IMPROVEMENT IS ACHIEVED, OR MAX ITERATIONS HAPPENS
		bool bHayMejorSolucion= true;
		long iContadorIteraciones = 0;
		do {
			UpdateCoefficientsRegularities(ReferApp.In.Regularities.ListaCoeficientes,
											   lContadorIteraciones_ParaCoeficientesRegularidades);
			
			// Save current step
			EvolutionCosts.push_back(ReferApp.In.DataOptHC.IncrementsOfVariables
									 [ReferApp.In.DataOptHC.iIncrementVariableCurrent]);

			// Look for a solution that improves the current one
			double dMejorCoste = CalculeCost(MejorSolucion, EvolutionCosts, EvolutionZ, true,
											 pDoc->DB_Brep_Model);
			long iMejorSolucion = -1;

			// REPEAT two times FOR EACH VARIABLE
			// (Thus, increments will be positive in even iterations and negative in odd ones)
			for (long i = 0; i < 2 * ReferApp.In.NumVariables; i++ ) {
				ReferApp.In.DataOptHC.iSolutionCurrent = i;

				// Given i-th solution, i+1 solution is calculated
				GetNewSolutionHC(MejorSolucion, NuevaSolucion, &ReferApp.In.DataOptHC);

				std::vector<double> voidEvolutionCosts;
				std::vector<double> voidEvolutionZ;
				double dCosteActual = CalculeCost(NuevaSolucion, 
												  voidEvolutionCosts, voidEvolutionZ, false,
												  pDoc->DB_Brep_Model);

				//Checks if this is the last iteration to apply the appropriate acceptance coefficient
				if (iContadorIncrementosVariables == (long)ReferApp.In.DataOptHC.IncrementsOfVariables.size()-1){
					// Check if it is the best solution so far 
					// with the particular acceptance coefficient for the last increment of variables
					if ( dCosteActual < dMejorCoste * dCoefAceptacionMejorCoste ) {
						dMejorCoste = dCosteActual;
						iMejorSolucion = i;
					}
				}
					// Check if it is the best solution so far 
					// with the common acceptance coefficient for all variable increments
				else{
					if ( dCosteActual < dMejorCoste * dCoefAceptacionMejorCosteEscalon ) {
						dMejorCoste = dCosteActual;
						iMejorSolucion = i;
					}
				}
			}

			// Save new solution, if it is better than previous ones
			if (iMejorSolucion != -1) {
				ReferApp.In.DataOptHC.iSolutionCurrent = iMejorSolucion;
				GetNewSolutionHC( MejorSolucion, NuevaSolucion, &ReferApp.In.DataOptHC );
				MejorSolucion= NuevaSolucion;
				bHayMejorSolucion = true;
			}
			else
				bHayMejorSolucion = false;

			NoCalcelled= pProgress->UpdateProgress( lContadorIteraciones_ParaCoeficientesRegularidades );

			// Draw the object according to the lowest cost solution achieved up to this point
			if (ReferApp.In.bShowInflationInProgress) {
				pDoc->DB_Brep_Model->SetAllVertex_Z(MejorSolucion);
				pDoc->RefreshWindowModelBrep();
			};
		
			// Save ALL the BEST solutions in EVERY ITERATION
			for(long i=0; i < ReferApp.In.NumVariables; i++ ) {
				EvolutionSolutions.push_back(MejorSolucion[i]);
			};
	
			lContadorIteraciones_ParaCoeficientesRegularidades++;
			iContadorIteraciones++;

		} while ((iContadorIteraciones < ReferApp.In.DataOptHC.iMaxIterations) &&
		         (bHayMejorSolucion) && NoCalcelled);

		iContadorIncrementosVariables++;
		lContadorTotalIteraciones+=iContadorIteraciones;
	} while ((iContadorIncrementosVariables < (long)ReferApp.In.DataOptHC.IncrementsOfVariables.size()) &&
	         NoCalcelled);

	// Return best solution in VariablesZ
	if (NoCalcelled) {
		VariablesZ= MejorSolucion;
	};

	*pNumeroIteracionesRealizadas = lContadorTotalIteraciones;
	return NoCalcelled;
};


bool COptimizationInflation::OptimizationSA(CReferDoc* pDoc,
											long *pNumeroIteracionesRealizadas,
											CDialogProgress *pProgress)
// Applies the "Simulated Annealing" algorithm to a set of regularities

// Based on:
//     Company P., Gomis J.M. and Contero M.
//     "Geometrical Reconstruction from Single Line Drawings
//     Using Optimization-Based Approaches".
//     WSCG99 Conference proceedings, edited by Vaclav Skala
//     (ISBN 80-7082-490-5), Volume II, 1999, pp. 361-368

{
	bool NoCalcelled= true;

	int iContadorEscalones,
		iMaxLevels,
		iMaxIterationsPerLevel,
	    iCriterioEquilibrio,
		iNumeroIteracionesEnEscalon,
		iContadorRechazos;
	long lTamanyoEvolucionCoste, lContadorIteracionesRealizadas;
	double dCriterioCongelacion,
	       dCosteActual,
		   dCosteAnterior,
		   dMejorCoste,
		   dIncrementoCoste,
		   dTemperatura,
		   dTemperaturaInicial,
		   dIncrementoVariablesInicial;
	bool bTerminarAlgoritmo;

	// INICIALIZACION DE VARIABLES

	iContadorEscalones = 0;         // Contador de escalones
	iMaxLevels = ReferApp.In.DataOptSA.iMaxLevels;
	iMaxIterationsPerLevel = ReferApp.In.DataOptSA.iMaxIterationsPerLevel * ReferApp.In.NumVariables;
	dCriterioCongelacion = ReferApp.In.DataOptSA.dCriterioCongelacion;  // Terminar si % rechazos > dCriterioCongelacion
	iCriterioEquilibrio = (int) (ReferApp.In.DataOptSA.dCriterioEquilibrio *
							     (double)iMaxIterationsPerLevel);    // Saltar escalon si Nº aciertos > iCriterioEquilibrio

	dIncrementoVariablesInicial= ReferApp.In.DataOptSA.IncrementsOfVariables[iContadorEscalones];
	ReferApp.In.DataOptSA.dIncrementoVariablesActual = dIncrementoVariablesInicial;

	// Reinicia las listas de soluciones y mejores soluciones/
	std::vector<double> Solucion;
	std::vector<double> NuevaSolucion;
	std::vector<double> MejorSolucion;

	Solucion= VariablesZ;
	MejorSolucion= VariablesZ;

	srand( (unsigned)time( NULL ) );  /* Reinicia los numeros aleatorios con el reloj del sistema */

	UpdateCoefficientsRegularities(ReferApp.In.Regularities.ListaCoeficientes, 0);

	std::vector<double> voidEvolutionCosts;
	std::vector<double> voidEvolutionZ;
	dMejorCoste = CalculeCost(Solucion, 
							  voidEvolutionCosts, voidEvolutionZ, false,
							  pDoc->DB_Brep_Model);

	dCosteActual = dMejorCoste;

	// SE CALCULA LA TEMPERATURA INICIAL
	// METODO KIRKPATRICK 
	if (ReferApp.In.DataOptSA.iMethodInitialTemperature == 0){ //Kirkpatrick
		dCosteAnterior = dCosteActual;
		dTemperaturaInicial = ((dCosteAnterior / 10.0) / 2.0);
		long i;
		do {
			dTemperaturaInicial *= 2.0;   // Temperatura inicial demasiado baja; hay que aumentarla
			i = 0;
			for( iNumeroIteracionesEnEscalon = 0 ; iNumeroIteracionesEnEscalon < iMaxIterationsPerLevel ; iNumeroIteracionesEnEscalon++ ) {
				GetNewSolutionSA(Solucion, NuevaSolucion,
								 &ReferApp.In, &ReferApp.In.DataOptSA, &ReferApp.In.DataOptSAM);

				std::vector<double> voidEvolutionCosts;
				std::vector<double> voidEvolutionZ;
				dCosteActual = CalculeCost(NuevaSolucion, 
										   voidEvolutionCosts, voidEvolutionZ, false,
										   pDoc->DB_Brep_Model);

				dIncrementoCoste = dCosteActual - dCosteAnterior;
				if ( exp( -dIncrementoCoste/dTemperaturaInicial ) > ((double)rand() / (double)(RAND_MAX+1)))
						i++;
			};			
		} while ( ((double)i / (double)iMaxIterationsPerLevel) < ReferApp.In.DataOptSA.dCoefAciertosTemperaturaInicial );
	// FIN METODO Juanvi-KIRKPATRICK
	}
	else{
		//Como los incrementos de coste siempre salen negativos, debo desplazar la solución
		//inicial algunas veces para conseguir incrementos de costes positivos que actuen sobre
		//el calculo de la temperatura inicial
		dIncrementoCoste = -1;
		while (dIncrementoCoste <=0){
			dCosteAnterior = dCosteActual;
			GetNewSolutionSA(Solucion, NuevaSolucion,
							 &ReferApp.In, &ReferApp.In.DataOptSA, &ReferApp.In.DataOptSAM);
			Solucion= NuevaSolucion;

			std::vector<double> voidEvolutionCosts;
			std::vector<double> voidEvolutionZ;
			dCosteActual = CalculeCost(NuevaSolucion, 
									   voidEvolutionCosts, voidEvolutionZ, false,
									   pDoc->DB_Brep_Model);

			dIncrementoCoste = dCosteActual - dCosteAnterior;

		}
		dCosteAnterior = dCosteActual;

		//Ahora que tengo garantizados posibles incrementos de coste positivo calculo
		//la temperatura inicial
		double dIncrementoCosteMedio = dIncrementoCoste;
		long i = 1;
		for( iNumeroIteracionesEnEscalon = 1 ; iNumeroIteracionesEnEscalon < iMaxIterationsPerLevel*ReferApp.In.NumVariables; iNumeroIteracionesEnEscalon++ ){
			GetNewSolutionSA(Solucion, NuevaSolucion,
							 &ReferApp.In, &ReferApp.In.DataOptSA, &ReferApp.In.DataOptSAM);

			std::vector<double> voidEvolutionCosts;
			std::vector<double> voidEvolutionZ;
			dIncrementoCoste = CalculeCost(NuevaSolucion, 
										   voidEvolutionCosts, voidEvolutionZ, false,
										   pDoc->DB_Brep_Model) - dCosteAnterior;

			if ( dIncrementoCoste > 0){
				i++;
				dIncrementoCosteMedio+=dIncrementoCoste;
			}
		}
		dTemperaturaInicial=(dIncrementoCosteMedio/i)/log(1/ReferApp.In.DataOptSA.dCoefAciertosTemperaturaInicial);
	}

	dCosteAnterior =  dCosteActual = dMejorCoste;
	Solucion= MejorSolucion;


	//FIN METODO KOUVELIS

	dTemperatura = dTemperaturaInicial;
	lTamanyoEvolucionCoste = 0;
	lContadorIteracionesRealizadas = 0;
	bTerminarAlgoritmo = false;

	/* HACER-MIENTRAS EL SISTEMA NO ESTE CONGELADO */
	do {
		iNumeroIteracionesEnEscalon = 0; //Controla el número de iteraciones en cada escalon
		iContadorRechazos = 0; //Controla el número de rechazos en cada escalon
		UpdateCoefficientsRegularities( ReferApp.In.Regularities.ListaCoeficientes, iContadorEscalones);

		//Creo que debe partir de la mejor solucción encontrada en el escalon anterior
		//pero no estoy seguro
//		Solucion= MejorSolucion;


		/* HACER-MIENTRAS NO SE ALCANCE LA CONDICION DE EQUILIBRIO */
		do {
			lContadorIteracionesRealizadas++;

			/* Guarda la evolucion de la Temperatura */
			EvolutionCosts.push_back(dTemperatura);

			/* Dada la solucion 'Solucion' obtiene la solucion 'Solucion+1' */
			GetNewSolutionSA(Solucion, NuevaSolucion,
							 &ReferApp.In, &ReferApp.In.DataOptSA, &ReferApp.In.DataOptSAM);

			dCosteActual = CalculeCost(NuevaSolucion, EvolutionCosts, EvolutionZ, true,
									   pDoc->DB_Brep_Model);
			dIncrementoCoste = dCosteActual - dCosteAnterior;

			/* Comprueba si el incremento de coste es negativo: ha disminuido el coste */
			if ( dIncrementoCoste <= 0 ) {

				/* Acepta nueva solucion:  pSolucion = pNuevaSolucion  */
				dCosteAnterior = dCosteActual;
				Solucion= NuevaSolucion;

				/* Comprueba si es la mejor solucion hasta el momento */
				if ( dCosteActual < dMejorCoste ) {
					dMejorCoste = dCosteActual;
					MejorSolucion= Solucion;
				};
			}
			else{
				/* Realiza otro criterio de aceptacion : CRITERIO METROPOLIS */
				if ( exp( -dIncrementoCoste/dTemperatura ) > ( (double)rand() / (double)(RAND_MAX+1)) ) {

					/* Acepta nueva solucion:  pSolucion = pNuevaSolucion  */
					dCosteAnterior = dCosteActual;					
					Solucion= NuevaSolucion;

					/* Comprueba si es la mejor solucion hasta el momento */
					if ( dCosteActual < dMejorCoste ) {
						dMejorCoste = dCosteActual;
						MejorSolucion= Solucion;
					};
				}
				else
					iContadorRechazos++;
			}
				
			iNumeroIteracionesEnEscalon++;

		/* COMPRUEBA LA CONDICION DE EQUILIBRIO */
		} while ( (iNumeroIteracionesEnEscalon < iMaxIterationsPerLevel) &&
		          ((iNumeroIteracionesEnEscalon - iContadorRechazos) < iCriterioEquilibrio) ) ;


		/* Guarda TODAS las MEJORES soluciones en CADA ESCALON de temperatura */
		for(long  i=0; i < ReferApp.In.NumVariables; i++ ) {
			EvolutionSolutions.push_back(MejorSolucion[i]);
		};

		NoCalcelled= pProgress->UpdateProgress( iContadorEscalones );

		// Dibuja el objeto segun la solucion de menor coste conseguida hasta este momento
		if (ReferApp.In.bShowInflationInProgress) {
			pDoc->DB_Brep_Model->SetAllVertex_Z(MejorSolucion);
			pDoc->RefreshWindowModelBrep();
		};

		iContadorEscalones++;

		/* EVOLUCION DE LA TEMPERATURA */
		/* Evolucion de temperatura segun una curva de tipo potencial */
		dTemperatura *= ReferApp.In.DataOptSA.dDecrementoTemperaturaPotencial;

		//EVOLUCION DE LOS INCREMENTOS DE VARIABLES
		ReferApp.In.DataOptSA.dIncrementoVariablesActual=
			ReferApp.In.DataOptSA.IncrementsOfVariables[iContadorEscalones];


	/* COMPRUEBA EL CRITERIO DE CONGELACION (TEMPERATURA FINAL) */
/*		if ( (((double)iContadorRechazos/(double)iSolucion) > dCriterioCongelacion) ||||
		     (iContadorEscalones/(double)iSolucion) > dCriterioEquilibrio) )
			bTerminarAlgoritmo = true;*/

		if (((double)iContadorRechazos/(double)iNumeroIteracionesEnEscalon) > dCriterioCongelacion)
			bTerminarAlgoritmo = true;

	} while ((iContadorEscalones < iMaxLevels) &&
	         (bTerminarAlgoritmo == false) && NoCalcelled);

	// Return best solution in VariablesZ	
	if (NoCalcelled) {
		VariablesZ= MejorSolucion;
	};

	*pNumeroIteracionesRealizadas = lContadorIteracionesRealizadas;
	return NoCalcelled;
};


bool COptimizationInflation::OptimizationSAM(CReferDoc* pDoc,
											 long *pNumeroIteracionesRealizadas,
											 CDialogProgress *pProgress)
// Calculates z coordinates of the vertices of the 3D model
// by way of an optimization process trough the "Simulated Annealing Multicriteria"
{
	bool NoCalcelled= true;

	CString sMensaje;
	TRegularity Regularity;
	TParametrosCoeficienteRegularidad Coeficiente;

	/* INICIALIZACION DE VARIABLES */

	long iContadorEscalones = 0;         /* Contador de escalones */
	long iMaxLevels = ReferApp.In.DataOptSAM.iMaxLevels;   /* Numero maximo de escalones de temperatura */
	long iMaxIterationsPerLevel = ReferApp.In.DataOptSAM.iMaxIterationsPerLevel * ReferApp.In.NumVariables;
	double dCriterioCongelacion = ReferApp.In.DataOptSAM.dCriterioCongelacion;  /* Terminar si % rechazos > dCriterioCongelacion */
	long iCriterioEquilibrio = (int) (ReferApp.In.DataOptSAM.dCriterioEquilibrio *
								(double)iMaxIterationsPerLevel);    /* Saltar escalon si Nº aciertos > iCriterioEquilibrio */

	double dIncrementoVariablesInicial= ReferApp.In.DataOptSAM.IncrementsOfVariables[iContadorEscalones];
	ReferApp.In.DataOptSAM.dIncrementoVariablesActual = dIncrementoVariablesInicial;

	/* Reinicia las listas de soluciones y mejores soluciones */
	std::vector<double> Solucion;
	std::vector<double> NuevaSolucion;

	Solucion= VariablesZ;

	/* Reinicia las listas de temperaturas y costes de regularidades */
	std::vector<double> CostesRegularidadesActuales;
	std::vector<double> CostesRegularidadesAnteriores;
	std::vector<double> TemperaturasRegularidadesIniciales;
	std::vector<double> TemperaturasRegularidades;

	for(long i=0 ; i < REGULARITY_LAST+1 ; i++ ) {
		CostesRegularidadesActuales.push_back(0.0);
		CostesRegularidadesAnteriores.push_back(0.0);
		TemperaturasRegularidadesIniciales.push_back(0.0);
	};

	srand( (unsigned)time( NULL ) );  /* Reinicia los numeros aleatorios con el reloj del sistema */

	pProgress->Message("Calculating initial temperatures...");
	pProgress->RangeProgress(0, 12);
	pProgress->UpdateProgress( 0 );

	/* SE CALCULA LA TEMPERATURA INICIAL */
	/* METODO Juanvi-KIRKPATRICK Multicriterio */

	/* Hay que determinar la temperatura inicial de cada regularidad */

	UpdateCoefficientsRegularities( ReferApp.In.Regularities.ListaCoeficientes, 0);

	CalculeCostMulti(Solucion, pDoc->DB_Brep_Model, &ReferApp.In,
					 EvolutionCosts, EvolutionZ,
					 CostesRegularidadesActuales, false, false);

	/* Para cada regularidad, si su coste vale 0 su solucion particular ya es optima, y entonces
	   el algoritmo no empieza.
	   Para obligarle a comenzar, calculare el coste inicial habiendo modificado algunas variables
		hasta que ninguna regularidad tenga coste 0.                                               */
	
	long contador= 0;
	while(ThereAreZeroRegularityCosts(CostesRegularidadesActuales)){

		GetNewSolutionSA(Solucion, NuevaSolucion,
						 &ReferApp.In, &ReferApp.In.DataOptSA, &ReferApp.In.DataOptSAM);
		CalculeCostMulti(NuevaSolucion, pDoc->DB_Brep_Model, &ReferApp.In,
						 EvolutionCosts, EvolutionZ,
						 CostesRegularidadesActuales, false, false);
		Solucion= NuevaSolucion;
		contador++;
		if (contador>1000000){
			bool EsUnaPasada= true;
		}
		assert (contador<1000000);
	};

//	CosteAnterior = MejorCoste = CosteActual;
	CostesRegularidadesAnteriores= CostesRegularidadesActuales;

/*	DestruyeListaDouble( &MejoresCostesRegularidades );
	NuevaListaDouble( &MejoresCostesRegularidades );
	VLCopiaListaDouble(CostesRegularidadesActuales, &MejoresCostesRegularidades );*/

	/* Ahora ya tienen todas las regularidades un coste positivo. En funcion de este coste,
	   podemos estimar el orden de magnitud de su temperatura asociada.                     */

	for(long i=0 ; i < REGULARITY_LAST+1 ; i++ ) {
		TemperaturasRegularidadesIniciales[i]= (CostesRegularidadesAnteriores[i]/10.0) / 2.0;
	};

	/* Hay que ajustar la temperatura de cada regularidad para que efectivamente cumpla las
	   condiciones iniciales del criterio metrópolis.                                       */

	double dDato = 0;

	for(long i=0 ; i < REGULARITY_LAST+1 ; i++ ) {
		Regularity = ReferApp.In.Regularities.ListOfRegularities[ i ];
		long j = 0;
		if ( Regularity.bActive){
			long iAciertos = 0;
			do {
				j++;
				sMensaje.Format("Calculating initial temperatures... (%i/%i)=%f", i, j, dDato);
				pProgress->Message( sMensaje );
				pProgress->UpdateProgress( i );

				/* Temperatura inicial demasiado baja; hay que aumentarla */
				dDato= TemperaturasRegularidadesIniciales[i];
				ASSERT( (dDato>0) && (dDato<1000000));
				TemperaturasRegularidadesIniciales[i]= dDato*2.0;

				iAciertos = 0;
				for(long k= 0 ; k<iMaxIterationsPerLevel ; k++ ) {
					GetNewSolutionSA(Solucion, NuevaSolucion,
									 &ReferApp.In, &ReferApp.In.DataOptSA, &ReferApp.In.DataOptSAM);

					CalculeCostMulti(NuevaSolucion, pDoc->DB_Brep_Model, &ReferApp.In,
									 EvolutionCosts, EvolutionZ,
									 CostesRegularidadesActuales, false, false);
					if (CheckMetropolis(CostesRegularidadesActuales[i],
										CostesRegularidadesAnteriores[i],
										TemperaturasRegularidadesIniciales[i])) iAciertos++;
				};
			} while(((double)iAciertos / (double)iMaxIterationsPerLevel) <
					ReferApp.In.DataOptSAM.dCoefAciertosTemperaturaInicial );
		}
	};

	/* FIN METODO Juanvi-KIRKPATRICK */


	pProgress->Message("Applying Optimization \"Sim. Annealing Multicriteria\"...");
	pProgress->RangeProgress(0, iMaxLevels);
	pProgress->UpdateProgress( 0 );

	/* TemperaturasRegularidades = TemperaturasRegularidadesIniciales; */
	TemperaturasRegularidades= TemperaturasRegularidadesIniciales;
	long lTamanyoEvolucionCoste = 0;
	long lContadorIteracionesRealizadas = 0;
	bool bTerminarAlgoritmo = false;

	/* HACER-MIENTRAS EL SISTEMA NO ESTE CONGELADO */
	do {
		long iSolucion = 0;
		long iContadorRechazos = 0;
		UpdateCoefficientsRegularities( ReferApp.In.Regularities.ListaCoeficientes, iContadorEscalones);

		/* HACER-MIENTRAS NO SE ALCANCE LA CONDICION DE EQUILIBRIO */
		do {
			lContadorIteracionesRealizadas++;

			/* Guarda la evolucion de la Temperatura */
			EvolutionCosts.push_back(TemperaturasRegularidades[REGULARITY_MIN_STD_DEV_ANGLES]);
			ASSERT( TemperaturasRegularidades[REGULARITY_MIN_STD_DEV_ANGLES] < 1000000 );

			/* Dada la solucion 'Solucion' obtiene la solucion 'Solucion+1' */
			GetNewSolutionSA(Solucion, NuevaSolucion,
							 &ReferApp.In, &ReferApp.In.DataOptSA, &ReferApp.In.DataOptSAM);
			double dCosteActual = CalculeCostMulti(NuevaSolucion, pDoc->DB_Brep_Model, &ReferApp.In,
												   EvolutionCosts, EvolutionZ,
												   CostesRegularidadesActuales, true, true);

			// Simulated Annealing Multicriteria algorithm accept the new solution
			// when ALL the regularities accomplish the Metropolis criterion

			// Vamos a ver cuantas regularidades cumplen el criterio Metropolis:
			long iCumplenCriterio=0;
			for(long i=0; i < REGULARITY_LAST+1 ; i++ ) {
				Regularity = ReferApp.In.Regularities.ListOfRegularities[ i ];
				if ( Regularity.bActive)

					/* Si se utiliza un coeficiente estilo "variable sin evolucion", hay que ver
					   entre que limites se utiliza. */
					if ( Regularity.bEsCoeficienteVariable) {
						Coeficiente = ReferApp.In.Regularities.ListaCoeficientes[ Regularity.iCoeficienteVariable/2 ];

						if (Coeficiente.iEstiloEvolucion = EVOLUTION_STYLE_NOEVOLUTION) {

							if ((iContadorEscalones >= Coeficiente.lIteracionInicial) &
								 (iContadorEscalones <= Coeficiente.lIteracionFinal)) {
								if (CheckMetropolis(CostesRegularidadesActuales[i],
													CostesRegularidadesAnteriores[i],
													TemperaturasRegularidades[i])) iCumplenCriterio++;
							}
							else
								iCumplenCriterio++;
						}
						else
							if (CheckMetropolis(CostesRegularidadesActuales[i],
												CostesRegularidadesAnteriores[i],
												TemperaturasRegularidades[i])) iCumplenCriterio++;
					}
					else
						if (CheckMetropolis(CostesRegularidadesActuales[i],
											CostesRegularidadesAnteriores[i],
											TemperaturasRegularidades[i])) iCumplenCriterio++;
			};
/*			if (iCumplenCriterio == REGULARITY_LAST+1) { */

			if (iCumplenCriterio == ReferApp.In.Regularities.NumActiveRegularities) {

				/* Acepta nueva solucion:  pSolucion = pNuevaSolucion  */
				/*	CostesRegularidadesAnteriores = CostesRegularidadesActuales; */
				CostesRegularidadesAnteriores= CostesRegularidadesActuales;

				Solucion= NuevaSolucion;

				// Comprueba si es la mejor solucion hasta el momento
				//if ( dCosteActual < dMejorCoste ) {
				//	dMejorCoste = dCosteActual;
				//	DestruyeListaDouble( &MejorSolucion );
				//	NuevaListaDouble( &MejorSolucion );
				//	VLCopiaListaDouble( Solucion, &MejorSolucion );
				//};
			}
			else
				iContadorRechazos++;

			iSolucion++;

		/* COMPRUEBA LA CONDICION DE EQUILIBRIO */
		} while ( (iSolucion < iMaxIterationsPerLevel) &&
		          ((iSolucion - iContadorRechazos) < iCriterioEquilibrio) ) ;
		
		/* EVOLUCION DE LA TEMPERATURA */
		/* Evolucion de temperatura segun una curva de tipo potencial */
		for(long i=0 ; i < REGULARITY_LAST+1 ; i++ ) {
			dDato= TemperaturasRegularidades[i];
			dDato *= ReferApp.In.DataOptSAM.dDecrementoTemperaturaPotencial;
			TemperaturasRegularidades[i]= dDato;
		};

		// El incremento de variable va evolucionando conforme a la temperatura, si procede
//		if (ReferApp.In.DataOptSAM.iStyleIncrement == 0 ) //Parametrico
//			ReferApp.In.DataOptSAM.dIncrementoVariablesActual =
//				//dIncrementoVariablesInicial * dTemperatura/dTemperaturaInicial;
//				//(iMaxLevels-iContadorEscalones) * dIncrementoVariablesInicial / iMaxLevels;
//				ReferApp.In.DataOptSAM.dIncrementoVariablesActual * ReferApp.In.DataOptSA.dStepReduction;

		//EVOLUCION DE LOS INCREMENTOS DE VARIABLES
		ReferApp.In.DataOptSAM.dIncrementoVariablesActual= 
			ReferApp.In.DataOptSAM.IncrementsOfVariables[iContadorEscalones+1];


		/* Guarda TODAS las MEJORES soluciones en CADA ESCALON de temperatura */
		/*for( i=0; i < NumVariables; i++ ) {
			// pEvolutionSolutions[(iContadorEscalones*NumVariables)+i] = pMejorSolucion[i];
			ObtenListaDouble( &MejorSolucion, i, &dDato );
			AnyadeListaDouble( pEvolutionSolutions, dDato );
			ASSERT( dDato < 1000000 );
		};*/

		/* Guarda la ULTIMA solucion de CADA ESCALON de temperatura */
		for(long i=0; i < ReferApp.In.NumVariables; i++ ) {
			EvolutionSolutions.push_back(Solucion[i]);
			ASSERT( Solucion[i] < 1000000 );
		};

		NoCalcelled= pProgress->UpdateProgress( iContadorEscalones );

		// Dibuja el objeto segun la solucion actual
		if (ReferApp.In.bShowInflationInProgress) {
			pDoc->DB_Brep_Model->SetAllVertex_Z(Solucion);
			pDoc->RefreshWindowModelBrep();
		};

		iContadorEscalones++;

	/* COMPRUEBA EL CRITERIO DE CONGELACION (TEMPERATURA FINAL) */
		if (((double)iContadorRechazos/(double)iSolucion) > dCriterioCongelacion)
			bTerminarAlgoritmo = true;

	} while ((iContadorEscalones < iMaxLevels) &&
	         (bTerminarAlgoritmo == false) && NoCalcelled);

	// Return best solution in VariablesZ	
	if (NoCalcelled) {
		VariablesZ= Solucion;
	};

	*pNumeroIteracionesRealizadas = lContadorIteracionesRealizadas;
	return NoCalcelled;
};


void COptimizationInflation::UpdateInflationDataBase(CReferDoc* pDoc,
													 std::vector<double> IntermediateSolutions,
													 std::vector<double> InitialZ)
//Updates the inflation database (DB_Brep_Inflation),
//which contains a subset of intermediate solutions of the optimization process

// Solutions are displaced to produce and "exploded" view

//This database is only used to show the inflation proccess in WinRf_OpenGLViewInflation
{
	long lNumeroVariables = pDoc->DB_Brep_Model->GetSizeVertices();

	//Add initial model to the inflation model
	*pDoc->DB_Brep_Inflation= *pDoc->DB_Brep_Model;

	//Calculate initial displacement of the first exploded intermediate model
	for (long i=0; i<pDoc->DB_Brep_Inflation->GetSizeVertices(); i++){
		CVertex Vertice= pDoc->DB_Brep_Inflation->GetVertex(i);
		Vertice.z = InitialZ[i];
		//assert (Vertice.z <10000);
		pDoc->DB_Brep_Inflation->SetVertex(i, Vertice);
	}

	double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
	pDoc->DB_Brep_Model->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);

	if (ReferApp.GeneralParams.DirectInflation){
		dZmax= InitialZ[0];
		for (long i=0; i<(long)InitialZ.size(); i++){
			dZmax= max(dZmax, InitialZ[i]);
		}
	}

	double dSeparacionEntreSoluciones = std::fabs(dZmax-dZmin) / 4.0;
	double dPosicionSolucionActual = dZmax+dSeparacionEntreSoluciones;

	//Loop to include all the intermediate models
	for(long i=0; i < ReferApp.In.iNumIntermediateInflations; i++) {

		//Calculate displacement
		double dZminSolucion, dZmaxSolucion;
		dZminSolucion = dZmaxSolucion = IntermediateSolutions[i*lNumeroVariables + 0];
		for (long j=1; j < lNumeroVariables; j++){
			double dZ = IntermediateSolutions[i*lNumeroVariables + j];
			dZminSolucion= min(dZ, dZminSolucion);
			dZmaxSolucion= max(dZ, dZmaxSolucion);
		};
		dPosicionSolucionActual -= dZminSolucion;

		//Add edges of the intermediate model
		for(long j=0; j<pDoc->DB_Brep_Model->GetSizeEdges(); j++ ) {
			CEdge Arista= pDoc->DB_Brep_Model->GetEdge(j);
			CVertex Vertice1= pDoc->DB_Brep_Model->GetVertex(Arista.Head);
			CVertex Vertice2= pDoc->DB_Brep_Model->GetVertex(Arista.Tail);

			//Calculate displaced edge
			double Z1= IntermediateSolutions[i*lNumeroVariables + Arista.Head] + dPosicionSolucionActual;
			double Z2= IntermediateSolutions[i*lNumeroVariables + Arista.Tail] + dPosicionSolucionActual;

			pDoc->DB_Brep_Inflation->AddEdgeByVertices(Vertice1.x, Vertice1.y, Z1,
												  Vertice2.x, Vertice2.y, Z2);
		};
		dPosicionSolucionActual += (dSeparacionEntreSoluciones + dZmaxSolucion);
	};

	bool bXmin, bXmax, bYmin, bYmax, bZmin, bZmax, bIntroducido;
	bXmin = bXmax = bYmin = bYmax = bZmin = bZmax = false;

	//Add initial and final solutions, plus inflation lines
	dPosicionSolucionActual -= dZmin;

	for(long i=0; i<pDoc->DB_Brep_Model->GetSizeEdges(); i++ ) {
		//Add model edges
		CEdge Arista= pDoc->DB_Brep_Model->GetEdge(i);
		CVertex Vertice1= pDoc->DB_Brep_Model->GetVertex(Arista.Head);
		CVertex Vertice2= pDoc->DB_Brep_Model->GetVertex(Arista.Tail);
		double Z1 = Vertice1.z + dPosicionSolucionActual;
		double Z2 = Vertice2.z + dPosicionSolucionActual;

		//Initial solution
		if(ReferApp.In.bViewInflationDrawing){
			pDoc->DB_Brep_Inflation->AddEdgeByVertices(Vertice1.x, Vertice1.y, 0,
												  Vertice2.x, Vertice2.y, 0);
		}

		//Final solution
		pDoc->DB_Brep_Inflation->AddEdgeByVertices(Vertice1.x, Vertice1.y, Z1,
											  Vertice2.x, Vertice2.y, Z2);

		//Add inflation lines
		bIntroducido = false;
		if (!bXmin && (Vertice1.x == dXmin)) bXmin = bIntroducido = true;
		if (!bXmax && (Vertice1.x == dXmax)) bXmax = bIntroducido = true;
		if (!bYmin && (Vertice1.y == dYmin)) bYmin = bIntroducido = true;
		if (!bYmax && (Vertice1.y == dYmax)) bYmax = bIntroducido = true;

		if (bIntroducido) {
			pDoc->DB_Brep_Inflation->AddEdgeByVertices(Vertice1.x, Vertice1.y, Vertice1.z + dPosicionSolucionActual,
												  Vertice1.x, Vertice1.y, 0.0);
			//Change color
			CEdge E= pDoc->DB_Brep_Model->GetEdge(pDoc->DB_Brep_Model->GetSizeEdges()-1);
			E.ColorR= 100;
			E.ColorG= 255;
			E.ColorB= 255;
			pDoc->DB_Brep_Model->SetEdge(pDoc->DB_Brep_Model->GetSizeEdges()-1, E);
		};
	};

	pDoc->DB_Brep_Inflation->ClearFaces();
};


void COptimizationInflation::OptimizationBasedInflation(CReferDoc* pDoc)
//Uses the optimization approach selected by the user 
//to inflate the 2D line-drawing thus obtaining a BRep 3D model

//The input is the pre-inflated 3D model if direct inflation is active

//Based on:
	//Lipson H, Shpitalni M. 
	//Optimization-based reconstruction of a 3D object from a single freehand line drawing
	//Computer-Aided Design, 28(8):651–63, 1996.

	//Julián Conesa Pastor.
	//RECONSTRUCCIÓN GEOMÉTRICA DE SÓLIDOS UTILIZANDO TÉCNICAS DE OPTIMIZACIÓN.
	//PhD Thesis, 2001
	//Departamento de Estructuras y Construcción.
	//Universidad Politécnica de Cartagena
{
	//Updates the list of regularities that must be calculated
	//as much as their related cues
	UpdateActiveRegularities (pDoc);

	if (ReferApp.In.Regularities.NumActiveRegularities == 0)
		return;

	//Update cues of active regularities
	UpdateCuesOfRegularities (pDoc);

	//Copy database to restore it if optimization fails
	CDB_Model DB_Aux1= *pDoc->DB_Brep_Model;

	//Reset inflation database
	*pDoc->DB_Brep_Inflation= *pDoc->DB_Brep_Model;

	//Load direct inflation result
	std::vector<double> InitialZ(pDoc->DB_Brep_Model->GetSizeVertices(), 0);
	for (long i=0; i<pDoc->DB_Brep_Model->GetSizeVertices(); i++){
		CVertex V= pDoc->DB_Brep_Model->GetVertex(i);
		InitialZ[i]= V.z;
	}

	//Initialize optimization evolution data
	ResetEvolutionVariables(ReferApp.In.EvolutionCosts,
							ReferApp.In.DataOptHC,
							ReferApp.In.DataOptSA,
							ReferApp.In.DataOptSAM);  // Is this required?

	//Initialize local databases
	//--------------------------
	ReferApp.In.NumVariables = pDoc->DB_Brep_Model->GetSizeVertices();  //The optimization variables are the Z coordinates of vertices

	VariablesZ= InitialZ;
	EvolutionCosts.clear();
	//EvolutionSolutions.clear();
	EvolutionZ.clear();

	ReferApp.In.FactorNormalizacion.clear();
	ReferApp.In.EstaCosteNormalizado.clear();
	for (long i=0; i<REGULARITY_LAST+1; i++){
		ReferApp.In.FactorNormalizacion.push_back(0.0);
		ReferApp.In.EstaCosteNormalizado.push_back(false);
	}

	// Open the Dialog that shows the progress, and allows cancellation
	// ----------------------------------------------------------------
	CDialogProgress Progress;
	Progress.Create(IDD_DIALOG_Progress);
	CTime HoraInicial= CTime::GetCurrentTime();

	// Select the suitable approach
	// ----------------------------
	bool OptimizationDone= false;
	long lIteracionesRealizadas, lIteracionesRealizadasComparadas;

	switch(ReferApp.In.iOptAlgorithm) {
		case HILL_CLIMBING :
		{
			// Compute the increment of variables
			EstimateIncrementsForHC(pDoc->DB_Brep_Model,
									 ReferApp.In.DataOptHC.IncrementsOfVariables,
									 ReferApp.In.DataOptHC.dInitialStep,
									 ReferApp.In.DataOptHC.dFinalStep,
									 ReferApp.In.DataOptHC.dStepReduction);

			Progress.Message("Applying Optimization \"Hill-Climbing\"...");
			long lRangeProgress = ReferApp.In.DataOptHC.iMaxIterations *
								  (long)ReferApp.In.DataOptHC.IncrementsOfVariables.size();

			// Update the coefficients for the regularities
			PrepareCoefficientsRegularities(ReferApp.In.Regularities.ListaCoeficientes,
											 lRangeProgress);

			Progress.RangeProgress(0, lRangeProgress);
			Progress.UpdateProgress(0);

			// Optimize to find z coordinates of the vertices
			OptimizationDone = OptimizationHC(pDoc,
											  &lIteracionesRealizadas,
											  &Progress);
			break;
		}

		case SIMULATED_ANNEALING :
		{
			//Se repite la optimización tantas veces como se indica en el parametro iRepetitions
			std::vector<double> EvolutionCostsCompared;
			std::vector<double> EvolutionZCompared;
			std::vector<double> EvolutionSolutionsCompared;
			std::vector<double> InitialZCompared;

			double CosteComparado;
			for (long k=0; k<ReferApp.In.DataOptSA.iRepetitions; k++){
				CString Texto;
				Texto.Format("Applying Optimization \"Simulated Annealing\"... %i of %i",
							 k+1, ReferApp.In.DataOptSA.iRepetitions);
				Progress.Message(Texto);

				// Preparar los coeficientes de las regularidades
				long lRangeProgress = ReferApp.In.DataOptSA.iMaxLevels;
				PrepareCoefficientsRegularities(ReferApp.In.Regularities.ListaCoeficientes,
												 lRangeProgress);

				// Si el incremento de variables todavia no ha sido ajustado, se calcula automaticamente
				if ( (long)ReferApp.In.DataOptSA.IncrementsOfVariables.size() == 0){
					EstimateIncrementsForSA(pDoc->DB_Brep_Model,
											ReferApp.In.DataOptSA.IncrementsOfVariables,
											ReferApp.In.DataOptSA.dInitialStep,
											ReferApp.In.DataOptSA.dFinalStep,
											ReferApp.In.DataOptSA.dStepReduction);
				}

				Progress.RangeProgress(0, lRangeProgress);
				Progress.UpdateProgress(0);

				OptimizationDone = OptimizationSA(pDoc,
												  &lIteracionesRealizadas,
												  &Progress);

				//Save the current best solution
				if (k==0){
					//Save initial iteration
					CosteComparado= EvolutionCosts[(long)EvolutionCosts.size()-ReferApp.In.Regularities.NumActiveRegularities-1];

					lIteracionesRealizadasComparadas = lIteracionesRealizadas;

					EvolutionCostsCompared= EvolutionCosts;
					EvolutionSolutionsCompared= EvolutionSolutions;
					EvolutionZCompared= EvolutionZ;
					InitialZCompared= InitialZ;
					EvolutionCosts.clear();
					EvolutionSolutions.clear();
					EvolutionZ.clear();
				}
				else{
					//Save the current best solution
					double UltimoCoste= EvolutionCosts[(long)EvolutionCosts.size()-ReferApp.In.Regularities.NumActiveRegularities-1];
					if (UltimoCoste < CosteComparado){
						EvolutionCostsCompared= EvolutionCosts;
						EvolutionSolutionsCompared= EvolutionSolutions;
						EvolutionZCompared= EvolutionZ;
						InitialZCompared= InitialZ;
						EvolutionCosts.clear();
						EvolutionSolutions.clear();
						EvolutionZ.clear();
					}
				}
			} // for k (iRepetitions)

			//Save the best solution
			lIteracionesRealizadas = lIteracionesRealizadasComparadas;

			EvolutionCosts= EvolutionCostsCompared;
			EvolutionSolutions= EvolutionSolutionsCompared;
			EvolutionZ= EvolutionZCompared;
			InitialZ= InitialZCompared;

			break;
		}

		case SIMULATED_ANNEALING_MULTI :
		{
			Progress.Message("Applying Optimization \"Multi-criteria Simulated Annealing\"... ");

			// Preparar los coeficientes de las regularidades
			long lRangeProgress = ReferApp.In.DataOptSAM.iMaxLevels;
			PrepareCoefficientsRegularities(ReferApp.In.Regularities.ListaCoeficientes,
											 lRangeProgress);

			// Si el incremento de variables todavia no ha sido ajustado, se calcula automaticamente
			if ( (long)ReferApp.In.DataOptSAM.IncrementsOfVariables.size() == 0){
				EstimateIncrementsForSA(pDoc->DB_Brep_Model,
										ReferApp.In.DataOptSAM.IncrementsOfVariables,
										ReferApp.In.DataOptSAM.dInitialStep,
										ReferApp.In.DataOptSAM.dFinalStep,
										ReferApp.In.DataOptSAM.dStepReduction);
			}

			Progress.RangeProgress(0, lRangeProgress);
			Progress.UpdateProgress(0);

			OptimizationDone = OptimizationSAM(pDoc,
											   &lIteracionesRealizadas,
											   &Progress);
			break;
		}
	} //switch

	// UPDATE GLOBAL DATABASES
	// -----------------------
	if (OptimizationDone){
		// Save the evolution of the cost
		ReferApp.In.EvolutionCosts= EvolutionCosts;
		ReferApp.In.EvolutionZ= EvolutionZ;

		// Update DataBase with the Z coordinates obtained from optimization algorithms
		pDoc->DB_Brep_Model->SetAllVertex_Z(VariablesZ);

		// Update Z coordinates in the original BB.DD. 2D
		pDoc->DB_Brep_Model->SetAllVertex_Z(VariablesZ);

		// Store the current intermediate solutions
		std::vector<double> IntermediateSolutions;
		long lNumeroSoluciones = (long)EvolutionSolutions.size() / ReferApp.In.NumVariables;
		for(long i=0; i < ReferApp.In.iNumIntermediateInflations ; i++ )
			for(long j=0; j < ReferApp.In.NumVariables; j++ ) {
				long lPosicion = (i * lNumeroSoluciones / ReferApp.In.iNumIntermediateInflations ) * ReferApp.In.NumVariables+j;
				IntermediateSolutions.push_back(EvolutionSolutions[lPosicion]);
			}

		// Update the DataBase of intermediate solutions
		UpdateInflationDataBase(pDoc,
								IntermediateSolutions,
								InitialZ);
	}
	else{
		// Restore previous DataBase, because optimization has failed
		*pDoc->DB_Brep_Model= DB_Aux1;
		pDoc->DB_Brep_Inflation->Clear();
	}

	//Update calculation time
	CTime HoraFinal = CTime::GetCurrentTime();
	CTimeSpan Tiempo = HoraFinal - HoraInicial;
	ReferApp.m_sTimeCalculating.Format("%i' %i\"", Tiempo.GetMinutes(), Tiempo.GetSeconds() );

	Progress.DestroyWindow();
}