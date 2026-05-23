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

#include <cassert>
#include <cfloat>     // isnan
#include <cmath>

#include <vector>

#include "Tools_Vector.h"    // IsValueInVector
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "Refer.h"
#include "ReferDoc.h"
#include "DB_Model_Maths.h"

#include "OptimizationCost.h"

#include "CueMainAxes.h"

///////////////////////////GEOMETRY///////////////////////////


double det(double e11,double e12,double e13,
		   double e21,double e22,double e23,
		   double e31,double e32,double e33)
//Returns the determinant of a 3x3 matrix
{
	return ( (e11*e22*e33)+(e12*e23*e31)+(e13*e21*e32)
			-(e13*e22*e31)-(e12*e21*e33)-(e11*e23*e32));
}


double GetMUab( double a, double b, double x )
// Calculates continuos compliance factor

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


///////////////////////////END GEOMETRY///////////////////////////


double GetCost_MSDA(std::vector<double> &CurrentSolution, CDB_Model *DDBB)
// Returns the cost of the current set of variables stored in 'Solucion'
// Cost is calculated according to:
//	REGULARITY_MIN_STD_DEV_ANGLES

//Minimum standard deviation of angles (MSDA) 
//was used very successfully by Marill to introduce an optimization approach, 
//but it is a false regularity (unless applied to regular polyhedra).

//		Marill, T.
//		Emulating the Human Interpretation of Line-Drawings as Three-Dimensional Objects. 
//		International Journal of Computer Vision, 6(2), (1991) 147-161.
{
	std::vector<double> Angles;
	double SumOfAngles = 0.0;
	long numAngles = 0;

	// Calculates the average of the angles formed by the edges incident on the current vertex
	for(long i=0; i < DDBB->GetSizeVertices() ; i++ ) {
		std::vector<CVector> UnitVectors;

		//Calculates the unit vectors of the edges that meet at the vertex
		for(long ii=0 ; ii <DDBB->GetSizeEdgesInVertex(i); ii++ ) {
			CEdge Arista= DDBB->GetEdge(DDBB->GetEdgeInVertex(i, ii));

			CVertex HeadVertex= DDBB->GetVertex(Arista.Head);
			HeadVertex.z = CurrentSolution[Arista.Head];

			CVertex TailVertex= DDBB->GetVertex(Arista.Tail);
			TailVertex.z = CurrentSolution[Arista.Tail];

			UnitVectors.push_back(GetUnitVector3D(HeadVertex, TailVertex));
		}

		// Analyze the angle formed by all the pairs of edges
		// that share the current vertex
		for (long j=0; j < (long)UnitVectors.size()-1; j++ ){
			for (long k=0; k < (long)UnitVectors.size()-1; k++ ){
				if(k != j){
					double dot = DOT_PRODUCT_3D( UnitVectors[j], UnitVectors[k] );
					//				assert( !_isnan(dot) );

					double AngleJK = acos(dot);  //Angle in the range 0 (dot= 1), up to PI (dot= -1)
					Angles.push_back(AngleJK);

					SumOfAngles += AngleJK;
					numAngles++;
				}

			};
		};
	}
	double AverageAngles = SumOfAngles/numAngles;

	//Calculates variance
	double variance = 0.0;
	for (long i=0; i<(long)Angles.size(); i++){
		variance+=(Angles[i]-AverageAngles)*(Angles[i]-AverageAngles);
	}

	// Returns variance
	return numAngles*variance/(numAngles-1);
};


double GetCost_FacePlanarity(std::vector<double> &CurrentSolution, CDB_Model *DDBB)
//Face planarity is a true regularity for polyhedral shapes 
//and probably the most important one.

//Its main drawback is that it becomes a direct regularity 
//only after faces have been detected in the line drawing.
//Hence, faces must have been calculated in advance

//Its second drawback is that it is trivially fulfilled in the 2D line-drawing
{
	double totalCost=0;

	if (DDBB->GetSizeFaces() == 0){
		//AfxMessageBox("WARNING: Face planarity cannot be calculated, because faces have not been detected");
		return totalCost;
	}

	//Calculate the cost of each face
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CFace CaraI= DDBB->GetFace(i);

		//Get all the vertices of the face
		std::vector<long> VerticesInFace;
		for (long j=0; j<CaraI.GetSizeEdgesInFace(); j++){
			CEdge Arista= DDBB->GetEdge(CaraI.GetEdgeInFace(j));
			if (!IsValueInVector(Arista.Head, VerticesInFace))
				VerticesInFace.push_back(Arista.Head);
			if (!IsValueInVector(Arista.Tail, VerticesInFace))
				VerticesInFace.push_back(Arista.Tail);
		}

		//Calculate the coefficients of the matrix of the plane
		double e11=0, e12=0, e13=0;
		double e21=0, e22=0, e23=0;
		double e31=0, e32=0, e33=0;
		double ind1=0, ind2=0, ind3=0;

		for (long j=0; j<(long)VerticesInFace.size(); j++){
			CVertex Vertice= DDBB->GetVertex(VerticesInFace[j]);
			double Z= CurrentSolution[VerticesInFace[j]];
			e11+=Vertice.x * Vertice.x;
			e12+=Vertice.x * Vertice.y;
			e13+=Vertice.x * Z;

			e21+=Vertice.x * Vertice.y;
			e22+=Vertice.y * Vertice.y;
			e23+=Vertice.y * Z;

			e31+=Vertice.x * Z;
			e32+=Vertice.y * Z;
			e33+=Z * Z;

			ind1+=Vertice.x;
			ind2+=Vertice.y;
			ind3+=Z;
		}

		//Calculate the coefficients of the equation of the plane
		double denominator=det(e11,e12,e13,e21,e22,e23,e31,e32,e33);
		if (std::fabs(denominator) > Accuracy){
			double a=det(ind1,e12,e13,ind2,e22,e23,ind3,e32,e33)/denominator;
			double b=det(e11,ind1,e13,e12,ind2,e23,e31,ind3,e33)/denominator;
			double c=det(e11,e12,ind1,e21,e22,ind2,e31,e32,ind3)/denominator;

			//normalize the coefficients
			double norm = sqrt(a*a + b*b + c*c);
			a=a/norm;
			b=b/norm;
			c=c/norm;
			double d=-1/norm;

			//Sum the distances of each vertex from the plane
			for (long j=0; j<(long)VerticesInFace.size(); j++){
				CVertex Vertice= DDBB->GetVertex(VerticesInFace[j]);
				double Z= CurrentSolution[VerticesInFace[j]];
				totalCost+= std::fabs(a*Vertice.x + b*Vertice.y + c*Z+d);
			}
		}
	}

	return totalCost;
}


double GetCost_LineParallelism(std::vector<double> &CurrentSolution, CDB_Model *DDBB)
//Calculates a cost that penalizes those lines 
//that have been classified as almost parallel in the drawing, 
//when they are not so in the current 3D model

//Line parallelism, assumes that parallelism between two 2D lines 
//is likely to reflect parallelism of their 3D edge counterparts.

//Lines that are parallel or nearly parallel in the drawing 
//(which are assigned a non-zero parallelism weight) 
//must be parallel in the model, so the less parallel they are in the model, 
//the more the parallelism cost increases.

//It is trivially fulfilled in the 2D line-drawing.
{
	double totalCost = 0;

	if (ReferApp.In.Cues_LP.size() == 0){
		//AfxMessageBox("WARNING: Edge parallelism cannot be calculated, because parallelism between drawing lines has not been detected");
		return totalCost;
	}

	// Check the parallelism of each edge with all the others
	for(long i=0; i<DDBB->GetSizeEdges(); i++ ){
		CEdge Arista1= DDBB->GetEdge(i);

		CVertex HeadVertex1= DDBB->GetVertex(Arista1.Head);
		HeadVertex1.z = CurrentSolution[Arista1.Head];
		CVertex TailVertex1= DDBB->GetVertex(Arista1.Tail);
		TailVertex1.z = CurrentSolution[Arista1.Tail];

		////////// Cost by angles ///////////////////
		//CVector VectorUnitario3D1= GetUnitVector3D(HeadVertex1, TailVertex1);

		CVector V1 (HeadVertex1.x - TailVertex1.x,
					HeadVertex1.y - TailVertex1.y,
					HeadVertex1.z - TailVertex1.z);
		//////////////////////////////////////////////

		//Compares parallelism of edge 'Arista1' with the remaining edges
		for(long j=i+1; j<DDBB->GetSizeEdges(); j++ ) {
			//Check if they were classified as "nearly" parallel in the 2D drawing
			if(ReferApp.In.Cues_LP[i][j] >0.){
				CEdge Arista2= DDBB->GetEdge(j);

				CVertex HeadVertex2= DDBB->GetVertex(Arista2.Head);
				HeadVertex2.z = CurrentSolution[Arista2.Head];
				CVertex TailVertex2= DDBB->GetVertex(Arista2.Tail);
				TailVertex2.z = CurrentSolution[Arista2.Tail];

				////////// Cost by distances ///////////////////
				//double D1= GetDistance3DVertexToLine(HeadVertex2, HeadVertex1, TailVertex1);
				//double D2= GetDistance3DVertexToLine(TailVertex2, HeadVertex1, TailVertex1);
				//double Distance = std::fabs (D1 - D2);
				////Normalize the distance based on the length of the current line 
				//Distance /= DDBB->GetLength2D(j);

				//totalCost += std::fabs( ReferApp.In.Cues_LP[i][j] * Distance);
				//////////////////////////////////////////////

				////////// Cost by angles ///////////////////
				CVector V2 (HeadVertex2.x - TailVertex2.x,
							HeadVertex2.y - TailVertex2.y,
							HeadVertex2.z - TailVertex2.z);
				double angle = DDBB->GetAngle3D_PI_half(V1, V2);

				totalCost += std::fabs(( ReferApp.In.Cues_LP[i][j] * (angle*angle)));
				//////////////////////////////////////////////
			}
		};
	}

	assert( !_isnan(totalCost) );
	return totalCost;
}


double GetCost_LineCollinearity(std::vector<double> &CurrentSolution, CDB_Model *DDBB)
//Calculates a cost that penalizes those lines 
//that have been classified as almost collinear in the drawing,
//when they are not so in the current 3D model

//Line collinearity is a particular case of line parallelism, 
//unless it is invariant in perspective views too, 
//and may serve to detect alignment constraints among edges and faces. 
//In other words, it is a cue of a true and direct model constraint: alignment.

//It is trivially fulfilled in the 2D line-drawing.
{
	double totalCost= 0;

	if (ReferApp.In.Cues_LC.size() == 0){
		//AfxMessageBox("WARNING: Edge collinearity cannot be calculated, because collinearity between drawing lines has not been detected");
		return totalCost;
	}

	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		CEdge Arista1= DDBB->GetEdge(i);

		CVertex v1= DDBB->GetVertex(Arista1.Head);
		v1.z= CurrentSolution[Arista1.Head];
		CVertex v2= DDBB->GetVertex(Arista1.Tail);
		v2.z= CurrentSolution[Arista1.Tail];

		for (long j=i+1; j<DDBB->GetSizeEdges(); j++) {
			assert( !_isnan(ReferApp.In.Cues_LC[i][j]) );
			if(ReferApp.In.Cues_LC[i][j]> Accuracy){  //Only pairs that should be collinear

				CEdge Arista2= DDBB->GetEdge(j);

				CVertex v3= DDBB->GetVertex(Arista2.Head);
				v3.z= CurrentSolution[Arista2.Head];
				CVertex v4= DDBB->GetVertex(Arista2.Tail);
				v4.z= CurrentSolution[Arista2.Tail];

				////////// Cost by distances /////////////////
				//double Distance = std::fabs(GetDistance3DVertexToLine(v3, v1, v2)) + 
				//				  std::fabs(GetDistance3DVertexToLine(v4, v1, v2));
				////Normalize the distance based on the length of the lines
				//Distance /= MODULE_3D(v1, v2);

				//totalCost += std::fabs( MatrixCollinearity[i][j] * Distance);
				//////////////////////////////////////////////

				////////// Cost by angles ////////////////////
				//Encompassed angle around the pivot
				double angle= 0.;
				if(Arista1.Head == Arista2.Head){
					angle= DDBB->GetAngle3D_PI(v2, v1, v4);
				}
				else if(Arista1.Head == Arista2.Tail){
					angle= DDBB->GetAngle3D_PI(v2, v1, v3);
				}
				else if(Arista1.Tail == Arista2.Head){
					angle= DDBB->GetAngle3D_PI(v1, v2, v4);
				}
				else if(Arista1.Tail == Arista2.Tail){
					angle= DDBB->GetAngle3D_PI(v1, v2, v3);
				}
				else //Edges do not share a common vertex!
					continue;

				//Collinear lines sharing a common vertex must encompass an angle of PI
				totalCost += std::fabs( ReferApp.In.Cues_LC[i][j] * std::fabs(NUMBER_PI - angle));
				//////////////////////////////////////////////
			}
		}
	}
	return totalCost;
}


//double GetCost_LineCollinearity(std::vector<double> &CurrentSolution, CDB_Model *DDBB)
////Calculates a cost that penalizes those lines 
////that have been classified as almost collinear in the drawing,
////when they are not so in the current 3D model
//
////Line collinearity is a particular case of line parallelism, 
////unless it is invariant in perspective views too, 
////and may serve to detect alignment constraints among edges and faces. 
////In other words, it is a cue of a true and direct model constraint: alignment.
//
////It is trivially fulfilled in the 2D line-drawing.
//
////As implemented in:
////	Ana Piquer Vicent.
////	PERCEPCIÓN ARTIFICIAL DE DIBUJOS LINEALES.
////	PhD Thesis, 2003.
////	Departamento de Tecnología.
////	Universitat Jaume I.
////	http://www.tdx.cat/handle/10803/10554;jsessionid=A882045F99097C47E8CBE8B5D67E6FC3.tdx2
//{
//	double totalCost= 0;
//
//	if (ReferApp.In.Cues_LC.size() == 0){
//		//AfxMessageBox("WARNING: Edge collinearity cannot be calculated, because collinearity between drawing lines has not been detected");
//		return totalCost;
//	}
//
//	for (long i=0; i<DDBB->GetSizeEdges(); i++){
//		CEdge Arista1= DDBB->GetEdge(i);
//		CVertex v1= DDBB->GetVertex(Arista1.Head);
//		v1.z= CurrentSolution[Arista1.Head];
//		CVertex v2= DDBB->GetVertex(Arista1.Tail);
//		v2.z= CurrentSolution[Arista1.Tail];
//
//		for (long j=i+1; j<DDBB->GetSizeEdges(); j++) {
//			assert( !_isnan(ReferApp.In.Cues_LC[i][j]) );
//			if(ReferApp.In.Cues_LC[i][j]> Accuracy){  //Only pairs that should be collinear
//
//				CEdge Arista2= DDBB->GetEdge(j);
//				CVertex v3= DDBB->GetVertex(Arista2.Head);
//				v3.z= CurrentSolution[Arista2.Head];
//				CVertex v4= DDBB->GetVertex(Arista2.Tail);
//				v4.z= CurrentSolution[Arista2.Tail];
//
//				//Angle between line i and line j
//				CVector VectorUnitario3D1 = GetUnitVector3D(v1,v2);
//				CVector VectorUnitario3D2 = GetUnitVector3D(v3,v4);
//				double dot = DOT_PRODUCT_3D( VectorUnitario3D1, VectorUnitario3D2 );
//				//if (dot < 1+Accuracy && dot > 1) dot=1;
//				//if (dot < -1 && dot > -1-Accuracy) dot=-1;
//				double angle=acos(dot);
//				if (std::fabs(angle)>(NUMBER_PI/2)) angle = std::fabs(angle-NUMBER_PI);
//
//				//Maximum angle between the line i and the four auxiliar lines
//				//(those connecting tips of line i to tips of line j)
//				double angulo3, angulo4, angulo5, angulo6;
//				if ((std::fabs(v1.x-v4.x)<Accuracy) && (std::fabs(v1.y-v4.y)<Accuracy) &&
//					(std::fabs(v1.z-v4.z)<Accuracy)){
//					angulo3= 0;
//				}
//				else{
//					CVector VectorUnitario3D3= GetUnitVector3D(v1,v4);
//					dot = DOT_PRODUCT_3D( VectorUnitario3D1, VectorUnitario3D3 );
//					//if (dot < 1+Accuracy && dot > 1) dot=1;
//					//if (dot < -1 && dot > -1-Accuracy) dot=-1;
//					angulo3=acos(dot);
//					if (std::fabs(angulo3)>(NUMBER_PI/2)) angulo3 = std::fabs(angulo3-NUMBER_PI);
//				}
//
//				if ((std::fabs(v1.x-v3.x)<Accuracy) && (std::fabs(v1.y-v3.y)<Accuracy) &&
//					(std::fabs(v1.z-v3.z)<Accuracy)){
//					angulo4= 0;
//				}
//				else{
//					CVector VectorUnitario3D4= GetUnitVector3D(v1,v3);
//					dot = DOT_PRODUCT_3D( VectorUnitario3D1, VectorUnitario3D4 );
//					//if (dot < 1+Accuracy && dot > 1) dot=1;
//					//if (dot < -1 && dot > -1-Accuracy) dot=-1;
//					angulo4=acos(dot);
//					if (std::fabs(angulo4)>(NUMBER_PI/2)) angulo4 = std::fabs(angulo4-NUMBER_PI);
//				}
//
//				if ((std::fabs(v2.x-v4.x)<Accuracy) && (std::fabs(v2.y-v4.y)<Accuracy) &&
//					(std::fabs(v2.z-v4.z)<Accuracy)){
//					angulo5= 0;
//				}
//				else{
//					CVector VectorUnitario3D5= GetUnitVector3D(v2,v4);
//					dot = DOT_PRODUCT_3D(VectorUnitario3D1, VectorUnitario3D5);
//					//if (dot < 1+Accuracy && dot > 1) dot=1;
//					//if (dot < -1 && dot > -1-Accuracy) dot=-1;
//					angulo5=acos(dot);
//					assert( !_isnan(angulo5) );	
//					if (std::fabs(angulo5)>(NUMBER_PI/2)) angulo5 = std::fabs(angulo5-NUMBER_PI);
//				}
//
//				if ((std::fabs(v2.x-v3.x)<Accuracy) &&
//					(std::fabs(v2.y-v3.y)<Accuracy) &&
//					(std::fabs(v2.z-v3.z)<Accuracy)){
//					angulo6= 0;
//				}
//				else{
//					CVector VectorUnitario3D6= GetUnitVector3D(v2,v3);
//					dot = DOT_PRODUCT_3D( VectorUnitario3D1, VectorUnitario3D6 );
//					//if (dot < 1+Accuracy && dot > 1) dot=1;
//					//if (dot < -1 && dot > -1-Accuracy) dot=-1;
//					angulo6=acos(dot);
//					if (std::fabs(angulo6)>(NUMBER_PI/2)) angulo6 = std::fabs(angulo6-NUMBER_PI);
//				}
//				double angleAux= max(angulo3, angulo4);
//				angleAux= max(angleAux, angulo5);
//				angleAux= max(angleAux, angulo6);
//
//				totalCost += std::fabs(( ReferApp.In.Cues_LC[i][j] * (angle+angleAux)*(angle+angleAux)));
//			}
//		}
//	}
//	return totalCost;
//}


double GetCost_LineVerticality(std::vector<double> &CurrentSolution, CDB_Model *DDBB)
//Line verticality assumes that lines that are vertical in the sketch 
//must have a counterpart edge parallel to the vertical axis in 3D 
//(which requires the previous detection of the main axes).

//Line verticality is a true regularity. 
//However, it is more concerned with the orientation of the model 
//than with its shape.

//It is trivially fulfilled in the 2D line-drawing.
{
	double totalCost = 0;

	if (ReferApp.In.Cues_LV.size() == 0){
	//	//AfxMessageBox("WARNING: Edge Verticality cannot be calculated, because Verticality of drawing lines has not been detected");
		return totalCost;
	}

	//Update the current solution in the 3D model
	CDB_Model DDBBaux= *DDBB;

	for (long i=0; i<DDBBaux.GetSizeEdges(); i++) {
		if(ReferApp.In.Cues_LV[i]> Accuracy){  //Only edges that should be vertical
			CEdge Arista= DDBBaux.GetEdge(i);
			CVertex HeadVertex= DDBBaux.GetVertex(Arista.Head);
			CVertex TailVertex= DDBBaux.GetVertex(Arista.Tail);
			HeadVertex.z= CurrentSolution[Arista.Head];
			TailVertex.z= CurrentSolution[Arista.Tail];
			DDBBaux.SetVertex(Arista.Head, HeadVertex);
			DDBBaux.SetVertex(Arista.Tail, TailVertex);
		}
	}

	//Transform the 3D model to the main axes
	//(otherwise the database aligned with the inflation reference system is selected)
	CCueMainAxes MainAxes;
	MainAxes.TransformToMainAxes(&DDBBaux, false);

	//Calculate cost of vertical lines
	for (long i=0; i<DDBB->GetSizeEdges(); i++) {
		if(ReferApp.In.Cues_LV[i]> Accuracy){  //Only edges that should be vertical
			CEdge Arista= DDBBaux.GetEdge(i);
			CVertex HeadVertex= DDBBaux.GetVertex(Arista.Head);
			CVertex TailVertex= DDBBaux.GetVertex(Arista.Tail);

			CVector V (HeadVertex.x - TailVertex.x,
					   HeadVertex.y - TailVertex.y,
					   HeadVertex.z - TailVertex.z);
			CVector VV (0, 0, V.z);
			double angle = DDBBaux.GetAngle3D_PI_half(V, VV);

			totalCost += std::fabs(( ReferApp.In.Cues_LV[i] * (angle*angle)));
		}
	}

	return totalCost;
}


double GetCost_LineOrthogonality(std::vector<double> &CurrentSolution, CDB_Model *DDBB)
//Calculates a cost that is zero if the pairs of lines of the model
//are orthogonal to each other, 
//and increases the more pairs of lines that are non-orthogonal.

//Line orthogonality is not a projective invariant. 
//as it was originarliy proposed. 
//However, it has been reformulated to become indirectly related to a parallelism cue:
//two edges must be orthogonal if their images are respectively parallel 
//to two directions perceived as being orthogonal to each other. 
//Consequently, it is linked to detection of the main orthogonal directions
//and some limitations about perspective projections appear as for line parallelism.

//Only pairs of lines that share a common vertex
//and run nearly parallel to any of the three main directions are considered

// Note that main directions must have been calculated in advance

//Still, this is a false regularity, since its application does not correspond 
//to any perceptual signal detected in the 2D image.
//Furthermore, it is a generalization of the orthogonality of corners
//and overlaps another false regularity such as the minimum standard deviation of angles
//Thus, its application is totally discouraged, 
//since it does not respond with real detections in the image, 
//and therefore not with the intentions of the sketcher.
{
	double totalCost = 0;

	if (ReferApp.In.Cues_LO.size() == 0){
		//AfxMessageBox("WARNING: Edge orthogonality cannot be calculated, because orthogonality between drawing lines has not been detected");
		return totalCost;
	}

	// Check the orthogonality of each edge with all the others
	for(long i=0; i<DDBB->GetSizeEdges(); i++ ){
		CEdge Arista1= DDBB->GetEdge(i);

		CVertex HeadVertex1= DDBB->GetVertex(Arista1.Head);
		HeadVertex1.z = CurrentSolution[Arista1.Head];
		CVertex TailVertex1= DDBB->GetVertex(Arista1.Tail);
		TailVertex1.z = CurrentSolution[Arista1.Tail];

		CVector V1 (HeadVertex1.x - TailVertex1.x,
					HeadVertex1.y - TailVertex1.y,
					HeadVertex1.z - TailVertex1.z);

		//Compares orthogonality of edge 'Arista1' with the remaining edges
		for(long j=i+1; j<DDBB->GetSizeEdges(); j++ ) {
			//Check if they were classified as "nearly" parallel in the 2D drawing
			if(ReferApp.In.Cues_LO[i][j] > 0.){
				CEdge Arista2= DDBB->GetEdge(j);

				CVertex HeadVertex2= DDBB->GetVertex(Arista2.Head);
				HeadVertex2.z = CurrentSolution[Arista2.Head];
				CVertex TailVertex2= DDBB->GetVertex(Arista2.Tail);
				TailVertex2.z = CurrentSolution[Arista2.Tail];

				CVector V2 (HeadVertex2.x - TailVertex2.x,
							HeadVertex2.y - TailVertex2.y,
							HeadVertex2.z - TailVertex2.z);
				double angle = DDBB->GetAngle3D_PI_half(V1, V2);

				totalCost += std::fabs(( ReferApp.In.Cues_LO[i][j] * ((NUMBER_PI/2.) - angle)));
			}
		};
	}

	assert( !_isnan(totalCost) );
	return totalCost;
}


double GetCost_Isometry(std::vector<double> &Solucion, CDB_Model *DDBB)
//This regularity assumes that lengths of entities in the 3D model
//are uniformly proportional to their lengths in the sketch plane.

//Isometry is a variant of the standard deviation of segment lengths:
//		Brown EW, Wang PS. 
//		Three-dimensional object recovery from two-dimensional images: a new approach. 
//		Proceedings of SPIE. Intelligent robots and computer vision XV: 
//		algorithms, techniques, active vision, and materials handling 1996;2904:138-47.

//Certainly, it may be a true regularity, 
//since a human observer assumes that the object represented in the line drawing 
//must have regular dimensions in all directions, 
//but according to projective laws, 
//this isometry cannot usually be measured directly in the drawing. 
//Hence, it is a false, or at best indirect, cue. 
//Additionally, the sketches of the designers often maintain proportions, 
//but are hardly faithful to exact measurements
{
	if (DDBB->GetSizeEdges()<2)
		return 0;

	std::vector<double> ri;
	for (long i=0; i<DDBB->GetSizeEdges(); i++) {
		CEdge Arista= DDBB->GetEdge(i);
		CVertex HeadVertex= DDBB->GetVertex(Arista.Head);
		CVertex TailVertex= DDBB->GetVertex(Arista.Tail);
		HeadVertex.z= Solucion[Arista.Head];
		TailVertex.z= Solucion[Arista.Tail];

		double Longitud2D = DDBB->GetLength2D(i);
		if (Longitud2D != 0)
			ri.push_back(DDBB->GetLength3D(i)/Longitud2D);
		else
			ri.push_back(0);
	}

	double media=0;
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		media+= ri[i];
	}
	media/= DDBB->GetSizeEdges();

	double Varianza=0;
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		Varianza+=(ri[i]-media)*(ri[i]-media);
	}
	Varianza=Varianza*DDBB->GetSizeEdges()/(DDBB->GetSizeEdges()-1);

	return Varianza;
}


//-------------------- Cubic Corners -----------------------------


double GetCost_CubicCorners(std::vector<double> &CurrentSolution, CDB_Model *DDBB)
//Perkins' remarkable work gave rise to the development of cubic corners regularity 
//that takes a value of zero when the three edges of each corner form right angles, 
//and takes values far from zero when the corners perceived as orthogonal in the drawing
//are not reconstructed with perpendicular edges.
//		Perkins D.N. (1971) Cubic Corners, Oblique Views of Pictures, 
//		the Perception of Line Drawings of Simple Space Forms. 
//		Geometry and the Perception of Pictures: Three Studies. 
//		Harvard Project Zero. Technical Report No. 5.

//Corner orthogonality is a particular case of MSDA, 
//where all angles defined by edges connected to the save vertex are equal to 90 degrees. 
//Consequently, it is a false regularity unless applied to normalons. 

//Here, it is converted into a cubic corner cue 
//by limiting its application to vertices of valence 3.
{
	double totalCost= 0.;

	//Search for "corners": vertices with exactly three incident edges (valence 3)
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		if (ReferApp.In.Cues_CC[i] > 0) {    //Only cubic corners!!
			//Add the three incident edges to corner
			std::vector<CVector> cornerI(3, CVector());
			for (long j=0; j<3; j++) {
				CEdge Arista= DDBB->GetEdge(DDBB->GetEdgeInVertex(i, j));

				if (Arista.Tail == i) {
					CVertex Vertice2= DDBB->GetVertex(Arista.Head);
					cornerI[j].Head.x= Vertice2.x;
					cornerI[j].Head.y= Vertice2.y;
					cornerI[j].Head.z= CurrentSolution[Arista.Head];

					Vertice2= DDBB->GetVertex(Arista.Tail);
					cornerI[j].Tail.x= Vertice2.x;
					cornerI[j].Tail.y= Vertice2.y;
					cornerI[j].Tail.z= CurrentSolution[Arista.Tail];
				}
				else {
					//The corner is rearranged so that all vectors point away from the union.
					//(if the head of the edge coincides with the vertex it is inverted,
					//so that the head of "cornerI" is the vertex opposite to the union vertex)
					CVertex Vertice2= DDBB->GetVertex(Arista.Head);
					cornerI[j].Tail.x= Vertice2.x;
					cornerI[j].Tail.y= Vertice2.y;
					cornerI[j].Tail.z= CurrentSolution[Arista.Head];

					Vertice2= DDBB->GetVertex(Arista.Tail);
					cornerI[j].Head.x= Vertice2.x;
					cornerI[j].Head.y= Vertice2.y;
					cornerI[j].Head.z= CurrentSolution[Arista.Tail];
				}
			}

			double sigma1= asin(DOT_PRODUCT_3D(GetUnitVector3D(cornerI[0].Head, cornerI[0].Tail),
											   GetUnitVector3D(cornerI[1].Head, cornerI[1].Tail)));
			double sigma2= asin(DOT_PRODUCT_3D(GetUnitVector3D(cornerI[0].Head, cornerI[0].Tail),
											   GetUnitVector3D(cornerI[2].Head, cornerI[2].Tail)));
			double sigma3= asin(DOT_PRODUCT_3D(GetUnitVector3D(cornerI[2].Head, cornerI[2].Tail),
											   GetUnitVector3D(cornerI[1].Head, cornerI[1].Tail)));

			double partialCost= (sigma1*sigma1 + sigma2*sigma2 + sigma3*sigma3) *
								ReferApp.In.Cues_CC[i];

			totalCost+= partialCost;
		}
	}

	return totalCost;
}

//-------------------------- End Cubic Corners ----------------


//-------------------- Face Orthogonality ----------------------------
bool Comparten_Arista(CDB_Model *DDBB, long i, long j)
//determina si dos caras comparten alguna arista
{
	bool comparten= false;

	CFace CaraJ= DDBB->GetFace(j);
	for (long jj=0; jj<CaraJ.GetSizeEdgesInFace(); jj++){
		long IndiceArista= CaraJ.GetEdgeInFace(jj);
		if (DDBB->IsEdgeInFace(i, IndiceArista)){
			comparten= true;
			break;
		}
	}
	return comparten;
}


double GetCost_FaceOrthogonality(std::vector<double> &Solucion, CDB_Model *DDBB)
//Face perpendicularity assumes that all adjacent faces must be perpendicular

//According to LIson and Shpitalni,
//this is a "false regularity" that serves to initially inflate the flat projection
//to a rough convex shape in 3D, from which optimization is more easily achieved

//		Lipson, H., Shpitalni, M.: 
//		Optimization-Based Reconstruction of a 3D Object from a Single Freehand Line Drawing. 
//		Computer Aided Design, 28(8), (1996) 651-663.
{
	long IndiceArista, IndiceVertice, Aux;
	double a, b, c, d, aux;
	double Coste, Z;

	std::vector<long> VVerticesEnCara;

	Coste=0;
	//compruebo si cada par de caras comparte aristas
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CFace CaraI= DDBB->GetFace(i);


		for (long j=i+1; j<DDBB->GetSizeFaces(); j++){
			CFace CaraJ= DDBB->GetFace(j);


			if (Comparten_Arista(DDBB, i, j)){
				//Si las caras i y j comparten una arista, calculo los planos de mejor
				//ajuste de dichas caras
				
				//Saco las aristas que forman la cara i
				//Obtengo todos los vértices de la cara
				VVerticesEnCara.clear();
				for (long k=0; k<CaraI.GetSizeEdgesInFace(); k++){
					IndiceArista= CaraI.GetEdgeInFace(k);
					CEdge Arista= DDBB->GetEdge(IndiceArista);
					if (!IsValueInVector(Arista.Head, VVerticesEnCara, Aux))
						VVerticesEnCara.push_back(Arista.Head);
					if (!IsValueInVector(Arista.Tail, VVerticesEnCara, Aux))
						VVerticesEnCara.push_back(Arista.Tail);
				}

				//Obtengo el plano de mejor ajuste de la cara i

				//Calculo los coeficientes de la matriz
				double e11=0, e12=0, e13=0;
				double e21=0, e22=0, e23=0;
				double e31=0, e32=0, e33=0;
				double ind1=0, ind2=0, ind3=0;

				for (long k=0; k<(long)VVerticesEnCara.size(); k++){
					CVertex Vertice= DDBB->GetVertex(VVerticesEnCara[k]);
					Z= Solucion[IndiceVertice];
					e11+=Vertice.x * Vertice.x;
					e12+=Vertice.x * Vertice.y;
					e13+=Vertice.x * Z;
					e21+=Vertice.x * Vertice.y;
					e22+=Vertice.y * Vertice.y;
					e23+=Vertice.y * Z;
					e31+=Vertice.x * Z;
					e32+=Vertice.y * Z;
					e33+=Z * Z;
					ind1+=Vertice.x;
					ind2+=Vertice.y;
					ind3+=Z;
				}

				//Calculo los coeficientes de la ecuación del plano
				double denominador=det(e11,e12,e13,e21,e22,e23,e31,e32,e33);
				if (denominador!=0){
					a=det(ind1,e12,e13,ind2,e22,e23,ind3,e32,e33)/denominador;
					b=det(e11,ind1,e13,e12,ind2,e23,e31,ind3,e33)/denominador;
					c=det(e11,e12,ind1,e21,e22,ind2,e31,e32,ind3)/denominador;
				}
	
				//Normalizo los coeficientes
				aux = sqrt(a*a + b*b + c*c);
				a=a/aux;
				b=b/aux;
				c=c/aux;
				d=-1/aux;

				//Determino tres puntos del plano i
				CVertex Punto1, Punto2, Punto3;
				Punto1.x=1; Punto1.y=0; Punto1.z=-1*(a*Punto1.x+b*Punto1.y+d)/c;
				Punto2.x=0; Punto2.y=1; Punto2.z=-1*(a*Punto2.x+b*Punto2.y+d)/c;
				Punto3.x=1; Punto3.y=1; Punto3.z=-1*(a*Punto3.x+b*Punto3.y+d)/c;
				//Determino el vector normal al plano
				CVector v1, v2;
				v1.x = Punto2.x-Punto1.x;
				v1.y = Punto2.y-Punto1.y;
				v1.z = Punto2.z-Punto1.z;
				v2.x = Punto3.x-Punto1.x;
				v2.y = Punto3.y-Punto1.y;
				v2.z = Punto3.z-Punto1.z;

				CVector Vector1;
				Vector1.x= v1.y*v2.z-v2.y*v1.z;
				Vector1.y= v1.z*v2.x-v1.x*v2.z;
				Vector1.z= v1.x*v2.y-v2.x*v1.y;

				//Repito los mismo para la otra cara
				//Saco las aristas que forman la cara j
				//Obtengo todos los vértices de la cara
				VVerticesEnCara.clear();
				for (long k=0; k<CaraJ.GetSizeEdgesInFace(); k++){
					IndiceArista= CaraJ.GetEdgeInFace(k);
					CEdge Arista= DDBB->GetEdge(IndiceArista);
					if (!IsValueInVector(Arista.Head, VVerticesEnCara, Aux))
						VVerticesEnCara.push_back(Arista.Head);
					if (!IsValueInVector(Arista.Tail, VVerticesEnCara, Aux))
						VVerticesEnCara.push_back(Arista.Tail);
				}

				//Obtengo el plano de mejor ajuste de la cara j

				//Calculo los coeficientes de la matriz
				e11=0; e12=0; e13=0;
				e21=0; e22=0; e23=0;
				e31=0; e32=0; e33=0;
				ind1=0; ind2=0; ind3=0;

				for (long k=0; k<(long)VVerticesEnCara.size(); k++){
					IndiceVertice= VVerticesEnCara[k];
					CVertex Vertice= DDBB->GetVertex(IndiceVertice);
					Z= Solucion[IndiceVertice];
					e11+=Vertice.x * Vertice.x;
					e12+=Vertice.x * Vertice.y;
					e13+=Vertice.x * Z;
					e21+=Vertice.x * Vertice.y;
					e22+=Vertice.y * Vertice.y;
					e23+=Vertice.y * Z;
					e31+=Vertice.x * Z;
					e32+=Vertice.y * Z;
					e33+=Z * Z;
					ind1+=Vertice.x;
					ind2+=Vertice.y;
					ind3+=Z;
				}

				//Calculo los coeficientes de la ecuación del plano
				denominador=det(e11,e12,e13,e21,e22,e23,e31,e32,e33);
				if (denominador!=0){
					a=det(ind1,e12,e13,ind2,e22,e23,ind3,e32,e33)/denominador;
					b=det(e11,ind1,e13,e12,ind2,e23,e31,ind3,e33)/denominador;
					c=det(e11,e12,ind1,e21,e22,ind2,e31,e32,ind3)/denominador;
				}
	
				 //Normalizo los coeficientes
				aux = sqrt(a*a + b*b + c*c);
				a=a/aux;
				b=b/aux;
				c=c/aux;
				d=-1/aux;

				//Determino tres puntos del plano i
				//CVertex Punto1, Punto2, Punto3;
				Punto1.x=1; Punto1.y=0; Punto1.z=-1*(a*Punto1.x+b*Punto1.y+d)/c;
				Punto2.x=0; Punto2.y=1; Punto2.z=-1*(a*Punto2.x+b*Punto2.y+d)/c;
				Punto3.x=1; Punto3.y=1; Punto3.z=-1*(a*Punto3.x+b*Punto3.y+d)/c;
				//Determino el vector normal al plano
				//CVector v1, v2;
				v1.x = Punto2.x-Punto1.x;
				v1.y = Punto2.y-Punto1.y;
				v1.z = Punto2.z-Punto1.z;
				v2.x = Punto3.x-Punto1.x;
				v2.y = Punto3.y-Punto1.y;
				v2.z = Punto3.z-Punto1.z;

				CVertex Vector2;
				Vector2.x=v1.y*v2.z-v2.y*v1.z;
				Vector2.y=v1.z*v2.x-v1.x*v2.z;
				Vector2.z=v1.x*v2.y-v2.x*v1.y;

				//Incremento el valor del coste debido a este nuevo par de caras
				//Primero normalizo los vectores
				aux = sqrt(Vector1.x*Vector1.x + Vector1.y*Vector1.y + Vector1.z*Vector1.z);
				Vector1.x = Vector1.x/aux; Vector1.y = Vector1.y/aux; Vector1.z = Vector1.z/aux;
				aux = sqrt(Vector2.x*Vector2.x + Vector2.y*Vector2.y + Vector2.z*Vector2.z);
				Vector2.x = Vector2.x/aux; Vector2.y = Vector2.y/aux; Vector2.z = Vector2.z/aux;

				//Ahora calculo el incremento de coste
				aux = Vector1.x*Vector2.x + Vector1.y*Vector2.y + Vector1.z*Vector2.z;
				if (aux>1) aux=1;
				if (aux<-1) aux = -1;
				aux = asin(aux);
				Coste+=aux *aux;
			}
		}
	}

	return Coste;

//	vec3d v1,v2;
//    vert3d *n1,*n2;
//    long *LA1,*LA2;
//    double total,num;
//    long i,j,TamCar=pGrafo->CFaces.lTamanyo;
//    CVertex *Ver=pGrafo->ListaVertices.aLista;
//    CEdge *Ari=pGrafo->ListaAristas.aLista;
//    TListaLong *A;
//
//    for (i=0;i<TamCar;i++){
//		A=&(pGrafo->CFaces.aLista[i].ListaAristas);
//        QuickSort(A->aLista,0,(A->lTamanyo)-1);
//	}
//
//	total=0;
//    //compruebo si cada par de caras comparte aristas
//    for (i=0;i<TamCar;i++){
//		for (j=i+1;j<TamCar;j++){
//			if (Comparten_Arista(DDBB,i,j)){
//				LA1=pGrafo->CFaces.aLista[i].ListaAristas.aLista;
//                LA2=pGrafo->CFaces.aLista[j].ListaAristas.aLista;
//
//                v1.v1.x=Ver[Ari[LA1[0]].Head].x;
//                v1.v1.y=Ver[Ari[LA1[0]].Head].y;
//                v1.v1.z=pSolucion->aLista[Ari[LA1[0]].Head];
//                v1.v2.x=Ver[Ari[LA1[0]].Tail].x;
//                v1.v2.y=Ver[Ari[LA1[0]].Tail].y;
//                v1.v2.z=pSolucion->aLista[Ari[LA1[0]].Tail];
//
//                v2.v1.x=Ver[Ari[LA1[1]].Head].x;
//                v2.v1.y=Ver[Ari[LA1[1]].Head].y;
//                v2.v1.z=pSolucion->aLista[Ari[LA1[1]].Head];
//                v2.v2.x=Ver[Ari[LA1[1]].Tail].x;
//                v2.v2.y=Ver[Ari[LA1[1]].Tail].y;
//                v2.v2.z=pSolucion->aLista[Ari[LA1[1]].Tail];
//                
//				//calculo el vector normal al plano
//                n1=prod_vec_3d(v1,v2);
//
//                v1.v1.x=Ver[Ari[LA2[0]].Head].x;
//                v1.v1.y=Ver[Ari[LA2[0]].Head].y;
//                v1.v1.z=pSolucion->aLista[Ari[LA2[0]].Head];
//                v1.v2.x=Ver[Ari[LA2[0]].Tail].x;
//                v1.v2.y=Ver[Ari[LA2[0]].Tail].y;
//                v1.v2.z=pSolucion->aLista[Ari[LA2[0]].Tail];
//
//                v2.v1.x=Ver[Ari[LA2[1]].Head].x;
//                v2.v1.y=Ver[Ari[LA2[1]].Head].y;
//                v2.v1.z=pSolucion->aLista[Ari[LA2[1]].Head];
//                v2.v2.x=Ver[Ari[LA2[1]].Tail].x;
//                v2.v2.y=Ver[Ari[LA2[1]].Tail].y;
//                v2.v2.z=pSolucion->aLista[Ari[LA2[1]].Tail]; 
//
//                n2=prod_vec_3d(v1,v2);
//                
//				//se calcula el producto escalar de los vectores normalizados
//                num=prod_esc_3d(*n1,*n2);
//
//                free(n1); free(n2);
//                assert(!((num>1)||(num<-1)));
//                num=asin(num);
//                if (num<0)
//					num=num+NUMBER_PI;
//				total=total+cuad(num);
//			}
//		}
//	}
//
//	return total;
}

//-------------------- End Face Orthogonality ----------------------------


//-----------------------Oblique Facial Orthogonality----------------------------------------
double GetCost_ObliqueFacialOrthogonality(std::vector<double> &Solucion, CDB_Model *DDBB)
//Following to LIpson and Sphitalni, skewed facial orthogonality implies that 
//a face contour that shows skewed orthogonality
//is probably orthogonal in space

//		Lipson, H., Shpitalni, M.: 
//		Optimization-Based Reconstruction of a 3D Object from a Single Freehand Line Drawing. 
//		Computer Aided Design, 28(8), (1996) 651-663.


//Hence, non orthogonal face contours produce positive costs
{
	double Coste=0;
	long k=0;

	//Para cada cara del grafo
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CFace CaraI= DDBB->GetFace(i);

		//Memorizo el vertice cabeza y cola de la primera arista
		long IndiceArista= CaraI.GetEdgeInFace(k);
		CEdge Arista= DDBB->GetEdge(IndiceArista);
		long IndiceVerticeInicio=Arista.Head;
		long IndiceVertice1=Arista.Head;
		long IndiceVertice2=Arista.Tail;
		long ContadorArista=1;
		double SumaBetas=0;
		double SumaArcoSenos=0;
		std::vector<double> ValoresBeta;

		bool fin = false;

		while (!fin){
			//Busco una arista convergente con la primera y memorizo el vértice no común
			long j, IndiceVertice3;
			for (j=0; j<CaraI.GetSizeEdgesInFace(); j++){
				if (j!=k){
					CEdge Arista= DDBB->GetEdge(CaraI.GetEdgeInFace(j));
					if (Arista.Head == IndiceVertice2){
						IndiceVertice3=Arista.Tail;
						break;
					}
					if (Arista.Tail == IndiceVertice2){
						IndiceVertice3=Arista.Head;
						break;
					}
				}
			}

			//Realizo los calculos con los tres vértices de que dispongo
			CVertex Vertice1= DDBB->GetVertex(IndiceVertice1);
			CVertex Vertice2= DDBB->GetVertex(IndiceVertice2);
			CVertex Vertice3= DDBB->GetVertex(IndiceVertice3);
			Vertice1.z= Solucion[IndiceVertice1];
			Vertice2.z= Solucion[IndiceVertice2];
			Vertice3.z= Solucion[IndiceVertice3];

			double Modulo1= MODULE2D (Vertice1.x, Vertice1.y, Vertice2.x, Vertice2.y);
			double Modulo2= MODULE2D (Vertice2.x, Vertice2.y, Vertice3.x, Vertice3.y);

			double pEscalar= ((Vertice2.x-Vertice1.x)/Modulo1)*((Vertice2.x-Vertice3.x)/Modulo2) +
							  ((Vertice2.y-Vertice1.y)/Modulo1)*((Vertice2.y-Vertice3.y)/Modulo2);

			double pVectorial = (Vertice1.x-Vertice2.x)*(Vertice3.y-Vertice2.y)
								- (Vertice1.y-Vertice2.y)*(Vertice3.x-Vertice2.x);

			double Beta = pow((double)-1, ContadorArista) * pEscalar * pVectorial;
			SumaBetas+=Beta;
			ValoresBeta.push_back(Beta);

			//Ahora voy a analizar los vectores 3D
			Modulo1= MODULE3D(Vertice1.x, Vertice1.y, Vertice1.z, Vertice2.x, Vertice2.y, Vertice2.z);
			Modulo2= MODULE3D(Vertice3.x, Vertice3.y, Vertice3.z, Vertice2.x, Vertice2.y, Vertice2.z);		
			double aux= ((Vertice2.x-Vertice1.x)/Modulo1)*((Vertice2.x-Vertice3.x)/Modulo2) +
						 ((Vertice2.y-Vertice1.y)/Modulo1)*((Vertice2.y-Vertice3.y)/Modulo2) +
						 ((Vertice2.z-Vertice1.z)/Modulo1)*((Vertice2.z-Vertice3.z)/Modulo2);

			aux= asin(aux);
			assert(std::fabs(aux)<=NUMBER_PI);

			SumaArcoSenos+=(aux*aux);

			//Repito el proceso mientras no se cierre la cara
			IndiceVertice1 = IndiceVertice2;
			IndiceVertice2 = IndiceVertice3;
			k= j;
			//ANA++ Aqui se podria ahorrar mas tiempo de compuacion 
			//pasando mas valores y no se necesitarian volver a 
			//calcular. Pero lo hare mas adelante.

			if (IndiceVertice1==IndiceVerticeInicio)
				fin = true;
			else
				ContadorArista++;
		}

		double MediaBetas=SumaBetas/ContadorArista;
		double Numerador=0;
		for (long j=0; j<(long)ValoresBeta.size(); j++){
			Numerador+= (ValoresBeta[j]-MediaBetas)*(ValoresBeta[j]-MediaBetas);
		}
		double Sigma = sqrt(Numerador/(ContadorArista-1));
		double w = GetMUab(0,0.2,Sigma);
		Coste+= w*SumaArcoSenos;
	}
	return Coste;
}

//-----------------------End Oblique Facial Orthogonality----------------------------------------


//-----------------------Skew Symmetry----------------------------------------


double SymLipson(int j, CVerticesSet *pListaVertices2)
//para calcular la parte de Simetria de la Simetria Facial Oblicua, para ello necesita como entradas
//el j que es el índice del posible eje evaluado según el orden que tienen la listaVertices2 que es
//la lista de los vértices de la cara evaluada y sus puntos medios (todos ordenados)
{
	long N= pListaVertices2->GetSize();
	//De esta forma N es el doble del número de vértices en la cara

	CVertex VerticeEje= pListaVertices2->GetVertex(j);
	long ll=(j+N/2) % N;
	CVertex VerticeEje2= pListaVertices2->GetVertex(ll);

	double Media= 0;
	double Sym= 0;
	std::vector<double> ListaSym;

	for (long i=1; i<N/2; i++){
		long l= (j+i) % N;
		long m= (j-i) % N; if(m<0) m += N;
		CVertex VerticeDespues= pListaVertices2->GetVertex(l);
		CVertex VerticeAntes= pListaVertices2->GetVertex(m);
		//La funcion DistanciaPuntoRecta solo tiene en cuenta las coordenadas x e y (no las z) 
		double DistanciaDespues= GetDistance2DVertexToLine(VerticeDespues, VerticeEje, VerticeEje2);
		double DistanciaAntes= GetDistance2DVertexToLine(VerticeAntes, VerticeEje, VerticeEje2);
		
		assert(!_isnan(DistanciaAntes));
		if (DistanciaAntes ==0) 
			Sym =-1;
		else
			Sym=(DistanciaDespues/DistanciaAntes)-1;
		
		Media +=Sym;
		ListaSym.push_back(Sym);
	}
	//Ahora falta por calcular la varianza (la desviación tipica)

	Media  /= ((long)ListaSym.size());

	double Varianza =0;
	for (long i=0;i<(long)ListaSym.size();i++){
		Sym= ListaSym[i];
		Varianza+=(Sym-Media)*(Sym-Media);
	}
	Varianza/=(N-1);

	return sqrt(Varianza);
}


double SkewLipson(int j, CVerticesSet *pListaVertices2)
//Igual que la anterior pero para valorar la parte Skew de Lipson
{
	//N es el doble del número de vértices en la cara
	long N= pListaVertices2->GetSize();

	CVertex VerticeEje= pListaVertices2->GetVertex(j);
	CVertex VerticeEje2= pListaVertices2->GetVertex((j+N/2) % N);

	std::vector<double> ListaSkew;
	double Media=0;

	for (long i=1; i<N/2; i++){
		double ModuloEje= 0, ModuloTransversal=0;
		double ProductoEscalar= 0, ProductoVectorial=0;

		long l=(j+i) % N;
		long m=(j-i) % N; if(m<0) m += N;
		CVertex VerticeDespues= pListaVertices2->GetVertex(l);
		CVertex VerticeAntes= pListaVertices2->GetVertex(m);

		ModuloEje=MODULE2D(VerticeEje.x, VerticeEje.y, VerticeEje2.x, VerticeEje2.y);
		ModuloTransversal=MODULE2D(VerticeDespues.x, VerticeDespues.y, VerticeAntes.x, VerticeAntes.y);

		ProductoEscalar= ((VerticeEje2.x-VerticeEje.x)/ModuloEje)*((VerticeAntes.x-VerticeDespues.x)/ModuloTransversal) +
						 ((VerticeEje2.y-VerticeEje.y)/ModuloEje)*((VerticeAntes.y-VerticeDespues.y)/ModuloTransversal);

		ProductoVectorial = (((VerticeEje.x -VerticeEje2.x)*(VerticeDespues.y-VerticeAntes.y))-
			((VerticeEje.y -VerticeEje2.y)*(VerticeDespues.x-VerticeAntes.x)))/(ModuloEje*ModuloTransversal);

		double Skew= ProductoEscalar * ProductoVectorial;
		
		Media +=Skew;
		ListaSkew.push_back(Skew);
	}

	Media =Media /N;

	double Varianza=0;
	for (long i=0; i<(long)ListaSkew.size(); i++){
		Varianza+=(ListaSkew[i]-Media)*(ListaSkew[i]-Media);
	}
	Varianza/=(N-1);

	return sqrt(Varianza);
}


void GetSymmetryCoefs(CVerticesSet *pListaVertices2,
					  CVertex &VerticeEje,
					  CVertex &VerticeEje2,
					  int &k, double &W)
//Funcion que calcula el coeficiente W para cada cara. 
//Como salida debe dar las coordenadas del eje de la cara, 
//y como entrada en vez de la cara 
//entra la lista de vertices de la cara y sus puntos medios.

//Ademas de calcular el coeficiente me devuelve la linea del eje
//Y k es el vertice por el que pasa el eje: el orden segun la lista doble.
{
	std::vector<double> ListaMu;
	//j es el indice del posible eje evaluado, pero está referido al conjunto ListaVertices2
	for (long i=0; i<(pListaVertices2->GetSize()/2); i++){
		double dDato=(SymLipson(i, pListaVertices2)+SkewLipson(i, pListaVertices2));

		assert(!_isnan(dDato));

		ListaMu.push_back(GetMUab(0,0.2,dDato));
	}

	//W es el valor de ponderación que Lipson da en su tesis para cada Cara
	W= ListaMu[0];

	//k es el orden del vértice que es el posible eje. 
	//Pero es el orden dentro de la listaVertices2
	//que es lista de la cara y doble. 
	k=0;
	for (long j=0; j<(long)ListaMu.size(); j++){
		if (ListaMu[j] > W) {
			k= j;
			W= ListaMu[j];}
	}

	//AQUI SI W ES =0 DEBERIAMOS PUENTEAR LO QUE VIENE
	assert(!_isnan(W));

	//Ahora falta calcular el coste
	VerticeEje.Clear();
	VerticeEje2.Clear();

	VerticeEje= pListaVertices2->GetVertex(k);
	VerticeEje2= pListaVertices2->GetVertex(k+(pListaVertices2->GetSize()/2));
}


double GetCost_SkewSymmetry(std::vector<double> &Solucion, CDB_Model *DDBB)
//Skewed facial symmetry implies that a face showing skewed symmetry in 2D 
//denotes a truly symmetrical face in 3D.

//		Lipson, H., Shpitalni, M.: 
//		Optimization-Based Reconstruction of a 3D Object from a Single Freehand Line Drawing. 
//		Computer Aided Design, 28(8), (1996) 651-663.

//Hence, those simetrical faces are identified 
//and a cost applies if they are not symmetric while inflating
{
	double Total= 0;

	//Loop over the faces
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CVerticesSet VerticesPlusMid;

		//Add vertices
		std::vector<long> NumVerticesPlusMidPoints;
		std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(i));
		for (long j=0; j<(long)vertices.size(); j++){
			CVertex Vertex= DDBB->GetVertex(vertices[j]);
			Vertex.z= Solucion[vertices[j]];
			VerticesPlusMid.AddVertex(Vertex);
			NumVerticesPlusMidPoints.push_back(vertices[j]);
		}

		//Add midpoints
		AddMidPoints(DDBB, NumVerticesPlusMidPoints, VerticesPlusMid);

		double W= 0;
		int k=0;
		CVertex VerticeEje, VerticeEje2;
		GetSymmetryCoefs(&VerticesPlusMid, VerticeEje, VerticeEje2, k, W);

		long NumVertices= VerticesPlusMid.GetSize();
		double Coste=0;

		//Get opposite pairs around k-th vertex/midpoint
		for (long j=1; j<(NumVertices/2); j++){
			long next= (k+j)% NumVertices;
			long prev= (k-j)% NumVertices; if(prev < 0) prev += NumVertices;
			CVertex VerticeDespues= VerticesPlusMid.GetVertex(next);
			CVertex VerticeAntes= VerticesPlusMid.GetVertex(prev);

			double ModuloEje= MODULE3D(VerticeEje.x, VerticeEje.y, VerticeEje.z, 
									   VerticeEje2.x, VerticeEje2.y, VerticeEje2.z);
			double ModuloTransversal= MODULE3D(VerticeDespues.x, VerticeDespues.y,VerticeDespues.z, 
											   VerticeAntes.x, VerticeAntes.y, VerticeAntes.z);
			assert (ModuloEje !=0 || ModuloTransversal !=0);

			double EscalarProduct= ((VerticeEje2.x-VerticeEje.x)/ModuloEje)*((VerticeAntes.x-VerticeDespues.x)/ModuloTransversal) +
									((VerticeEje2.y-VerticeEje.y)/ModuloEje)*((VerticeAntes.y-VerticeDespues.y)/ModuloTransversal) +
									((VerticeEje2.z-VerticeEje.z)/ModuloEje)*((VerticeAntes.z-VerticeDespues.z)/ModuloTransversal);

			Coste += (asin(EscalarProduct)) * (asin(EscalarProduct));
		}
		assert(!_isnan(Coste));

		//The cost of each symmetrical face is the degree of asymmetry
		Coste = Coste * W;

		//The symmetry cost of the entire model is the sum of the costs of all its faces
		Total += Coste;
	}

	assert(!_isnan(Total));
	return Total;
}
//-----------------------End Skew Symmetry----------------------------------------


double CalculeCost(std::vector<double> &Solucion,
				   std::vector<double> &EvolutionCost,
				   std::vector<double> &EvolutionZ,
				   bool evolution,
				   CDB_Model *DDBB)
// Cost function for the optimization algorithm
// Returns the cost of the current "solucion"
// according to the regularities that are active (and their coefficients)
{
	CReferDoc *pDoc = CReferDoc::GetDoc();

	TParametrosCoeficienteRegularidad Coeficiente;
	std::vector<double> aCostesRegularidades(REGULARITY_LAST+1, 0);

	// Calculate the cost of each Regularity that is activated
	for(long i=0; i < REGULARITY_LAST+1; i++ ) {
		TRegularity Regularity = ReferApp.In.Regularities.ListOfRegularities[i];

		if (Regularity.bActive){
			switch(i) {
				case REGULARITY_MIN_STD_DEV_ANGLES:
					aCostesRegularidades[i] = GetCost_MSDA(Solucion, DDBB);
					break;
				case REGULARITY_FACE_PLANARITY:
					aCostesRegularidades[i] = GetCost_FacePlanarity(Solucion, DDBB);
					break;

				case REGULARITY_LINE_PARALLELISM:
					aCostesRegularidades[i] = GetCost_LineParallelism(Solucion, DDBB);
					break;
				case REGULARITY_LINE_COLLINEARITY:
					aCostesRegularidades[i] = GetCost_LineCollinearity(Solucion, DDBB);
					break;
				case REGULARITY_LINE_VERTICALITY:
					aCostesRegularidades[i] = GetCost_LineVerticality(Solucion, DDBB);
					break;

				case REGULARITY_LINE_ORTHOGONALITY:
					aCostesRegularidades[i] = GetCost_LineOrthogonality(Solucion, DDBB);
					break;

				case REGULARITY_ISOMETRY:
					aCostesRegularidades[i] = GetCost_Isometry(Solucion, DDBB);
					break;

				case REGULARITY_CUBIC_CORNERS:
					aCostesRegularidades[i] = GetCost_CubicCorners(Solucion, DDBB);
					break;
				case REGULARITY_FACE_PERPENDICULARITY:
					aCostesRegularidades[i] = GetCost_FaceOrthogonality(Solucion, DDBB);
					break;

				case REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY:
					aCostesRegularidades[i] = GetCost_ObliqueFacialOrthogonality(Solucion, DDBB);
					break;

				case REGULARITY_SKEWED_FACIAL_SYMMETRY:
					aCostesRegularidades[i] = GetCost_SkewSymmetry(Solucion, DDBB);
					break;
			};

			//Normalize the costs of the regularities if this option is activated
			if (ReferApp.In.iNormalizeCosts == 0){
				if (!ReferApp.In.EstaCosteNormalizado[i]){
					ReferApp.In.EstaCosteNormalizado[i]= true;
					if (aCostesRegularidades[i] < 0.0000001)
						ReferApp.In.FactorNormalizacion[i]= 100;
					else
						ReferApp.In.FactorNormalizacion[i]= 100/aCostesRegularidades[i];
					aCostesRegularidades[i]=100.;
				}
				else{
					aCostesRegularidades[i]*= ReferApp.In.FactorNormalizacion[i];
				}
			}
			else{  //ReferApp.In.iNormalizeCosts == 1
				// Apply variable coefficients
				if ( Regularity.bEsCoeficienteVariable) {
					Coeficiente = ReferApp.In.Regularities.ListaCoeficientes[ Regularity.iCoeficienteVariable/2 ];

					if ( (Regularity.iCoeficienteVariable % 2) == 0 )
					   // If it is even, it will be X
						aCostesRegularidades[ i ] *= Coeficiente.dValorActual;
					else
						// ... and if it is odd, it will be 1-X
						aCostesRegularidades[ i ] *= (1 - Coeficiente.dValorActual);
				};

				// Finally, multiply by the fixed coefficient
				aCostesRegularidades[ i ] *= Regularity.dCoeficienteFijo;
			}
		}
		else
			// If Regularity is not activated, its cost is 0
			aCostesRegularidades[i]= 0;
	};

	// THE TOTAL COST IS THE SUM OF THE COSTS OF ALL REGULARITIES
	double dCosteTotal = 0;
	for(long i=0; i < REGULARITY_LAST+1; i++ )
		dCosteTotal += aCostesRegularidades[ i ];

	assert( !_isnan(dCosteTotal) );   // Make sure it is a representable finite number

	if(evolution){
		// SAVE THE EVOLUTION OF COSTS AND VARIABLES
		EvolutionCost.push_back(dCosteTotal);

		if (ReferApp.In.bViewPartialCostsEvolution)
		// You must also save each cost of each regularity, if it is selected.

			for(long i=0; i < REGULARITY_LAST+1; i++ ) {
				if ( ReferApp.In.Regularities.ListOfRegularities[ i ].bActive)
					EvolutionCost.push_back(aCostesRegularidades[i]);
			};

		// SAVE ALL THE VARIABLES (the Z's)
		for(long i=0; i<ReferApp.In.NumVariables; i++) {
			EvolutionZ.push_back(Solucion[i]);
		};
	}

	return dCosteTotal;
};


double CalculeCostMulti(std::vector<double> &Solucion,
						CDB_Model *DDBB,
						CDataInflation *pParams,
						std::vector<double> &EvolutionCost,
						std::vector<double> &EvolutionZ,
						std::vector<double> &ListaCostesRegularidades,
						bool bGuardarEvolucionVariables,
						bool bAplicarCoeficientes)
/* Esta es la "Funcion de Coste Multicriterio" del Algoritmo de Optimizacion.
   Devuelve el coste de la 'pSolucion' teniendo en cuenta la Base de Datos 'pDB',
	las regularidades activadas y sus coeficientes.                                     */
/* El funcionamiento es identico que la funcion "CalculaCoste", solo que ahora tambien
   se devuelve la lista del coste de cada regularidad.                                 */
{
	double dCosteTotal;
	double aCostesRegularidades[REGULARITY_LAST+1];
	TParametrosCoeficienteRegularidad Coeficiente;
	TRegularity Regularity;

	/* Calcula el coste de cada Regularidad que este activada */
	for(long iRegularidad=0; iRegularidad < REGULARITY_LAST+1; iRegularidad++ ) {
		
		Regularity = pParams->Regularities.ListOfRegularities[ iRegularidad ];

		if (Regularity.bActive) {
			switch(iRegularidad) {
				case REGULARITY_MIN_STD_DEV_ANGLES:
					aCostesRegularidades[ iRegularidad ] = GetCost_MSDA(Solucion, DDBB);
					break;
				case REGULARITY_FACE_PLANARITY:
					aCostesRegularidades[ iRegularidad ] = GetCost_FacePlanarity(Solucion, DDBB);
					break;

				case REGULARITY_LINE_PARALLELISM:
					aCostesRegularidades[ iRegularidad ] = GetCost_LineParallelism(Solucion, DDBB);
					break;
				case REGULARITY_LINE_COLLINEARITY:
					aCostesRegularidades[ iRegularidad ] = GetCost_LineCollinearity(Solucion, DDBB);
					break;
				case REGULARITY_LINE_VERTICALITY:
					aCostesRegularidades[ iRegularidad ] = GetCost_LineVerticality(Solucion, DDBB);
					break;

				case REGULARITY_LINE_ORTHOGONALITY:
					aCostesRegularidades[ iRegularidad ] = GetCost_LineOrthogonality(Solucion, DDBB);
					break;

				case REGULARITY_ISOMETRY:
					aCostesRegularidades[ iRegularidad ] = GetCost_Isometry(Solucion, DDBB);
					break;

				case REGULARITY_CUBIC_CORNERS:
					aCostesRegularidades[ iRegularidad ] = GetCost_CubicCorners(Solucion, DDBB);
					break;
				//case REGULARITY_FACE_PERPENDICULARITY:
					aCostesRegularidades[ iRegularidad ] = GetCost_FaceOrthogonality(Solucion, DDBB);
					break;

				case REGULARITY_OBLIQUE_FACIAL_ORTHOGONALITY:
					aCostesRegularidades[ iRegularidad ] = GetCost_ObliqueFacialOrthogonality(Solucion, DDBB);
					break;

				case REGULARITY_SKEWED_FACIAL_SYMMETRY:
					aCostesRegularidades[ iRegularidad ] = GetCost_SkewSymmetry(Solucion, DDBB);
					break;
			};
			// Si se utiliza un coeficiente variable, se obtiene y se multiplica por el. */
			if (( bAplicarCoeficientes) && ( Regularity.bEsCoeficienteVariable)) {
				Coeficiente = pParams->Regularities.ListaCoeficientes[ Regularity.iCoeficienteVariable/2 ];

				if ( (Regularity.iCoeficienteVariable % 2) == 0 )
				   // Si es par, sera X
					aCostesRegularidades[ iRegularidad ] *= Coeficiente.dValorActual;
				else
					// ... y si es impar, sera 1-X
					aCostesRegularidades[ iRegularidad ] *= (1 - Coeficiente.dValorActual);
			};

			// Finalmente, multiplica por el coeficiente fijo.

			//   QUIETORRR...!!!
			//   NO SE DEBE MULTIPLICAR, PORQUE NO DEBEMOS PONDERAR (ESTAMOS EN MULTICRITERIO)

			//aCostesRegularidades[ iRegularidad ] *= Regularity.dCoeficienteFijo;
		}
		else
			// Si la Regularidad no esta activada, su coste es 0.
			aCostesRegularidades[ iRegularidad ] = 0;
	};


	// EL COSTE TOTAL ES LA SUMA DE LOS COSTES DE TODAS LAS REGULARIDADES */

	// NOTA: AQUI EN MULTICRITERIO NO TIENE MUCHO SENTIDO SUMAR TODOS LOS COSTES */

	dCosteTotal = 0;
	for(long iRegularidad=0; iRegularidad < REGULARITY_LAST+1; iRegularidad++ )
		dCosteTotal += aCostesRegularidades[ iRegularidad ];

	assert( !_isnan(dCosteTotal) );   /* Asegurate que es un numero finito representable. */
	assert( (dCosteTotal>-1000000) && (dCosteTotal<1000000));
	
	// GUARDAR LA EVOLUCION DE LAS VARIABLES */
	if (bGuardarEvolucionVariables) {
		EvolutionCost.push_back(dCosteTotal);

		if (pParams->bViewPartialCostsEvolution)
		// Tambien hay que guardar cada coste de cada regularidad, en caso que este seleccionada. */

			for(long iRegularidad=0; iRegularidad < REGULARITY_LAST+1; iRegularidad++ ) {		
				Regularity = pParams->Regularities.ListOfRegularities[ iRegularidad ];
				if (Regularity.bActive)
					EvolutionCost.push_back(aCostesRegularidades[iRegularidad]);
			};

		// GUARDO TODAS LAS VARIABLES (las Z's) */
		for(long i=0; i<pParams->NumVariables; i++) {
			EvolutionZ.push_back(Solucion[i]);
		};
	};

	// AHORA COPIA LOS COSTES DE CADA REGULARIDAD PARA DEVOLVERLOS */
//	ListaCostesRegularidades.clear();

	for(long iRegularidad=0; iRegularidad < REGULARITY_LAST+1; iRegularidad++ ) {
		ListaCostesRegularidades[iRegularidad]= aCostesRegularidades[ iRegularidad ];
		assert( (aCostesRegularidades[iRegularidad] < 1000000) && (aCostesRegularidades[iRegularidad] > -1000000) );
	};

//		ListaCostesRegularidades.push_back(aCostesRegularidades[ iRegularidad ]);

	return dCosteTotal;
};
