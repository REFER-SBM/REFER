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
#include <cassert>
#include <queue>
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "resource.h"
#include "Refer.h"

#include "CueSubGraphs.h"

#include "CueTypology.h"


bool CCueTypology::IsPyramid(CDB_Model *DDBB)
// Determines whether the model fits the conditions of a pyramid:
//		One vertex connected to all the others
//		The other vertices are connected to three, and only three, edges

//Thetrahedrons are not labelled as pyramids!
{
	long base= 0;
	long cusp= -1;
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		long valence= DDBB->GetSizeEdgesInVertex(i);

		if (valence == 3){
			base++;
		}
		else if(cusp == -1 && valence==(DDBB->GetSizeVertices()-1)){
			cusp= i;
		}
	}
	return (base==(DDBB->GetSizeVertices()-1) && cusp!=-1);
}


void CCueTypology::GetNormalonEdges(CDB_Model *DDBB)
//Labels edges parallel to main axes with values:
//		0 for X axis
//		1 for Y axis
//		2 for Z axis
//Other edges are labelled as -1

//If faces have been calculated,
//it is checked that none pair of consecutive edges
//have been labelled as parallel to the same main axis
{
	//Reset normalon labels
	ReferApp.DIA.NormalonEdges.clear();
	ReferApp.DIA.NormalonEdges.assign(DDBB->GetSizeEdges(), -1);
	ReferApp.DIA.meritNormalonEdges.clear();
	ReferApp.DIA.meritNormalonEdges.assign(DDBB->GetSizeEdges(), 0);

	//Assign threshold
	double Threshold= ReferApp.Cu.Ty.ThresholdForParallel;

	//Compare axes orientations to edges orientations, to assign new normalon labels
	std::vector<POINT2D> pointAxis;
	for (long i= 0; i<(long)ReferApp.Cu.MA.MainAngles.size(); i++){
		POINT2D pI (cos(ReferApp.Cu.MA.MainAngles[i]), sin(ReferApp.Cu.MA.MainAngles[i]));
		pointAxis.push_back(pI);
	}
	POINT2D origin(0, 0);

	for (long i= 0; i<DDBB->GetSizeEdges(); i++){
		CEdge edgeI= DDBB->GetEdge(i);
		CVertex hI= DDBB->GetVertex(edgeI.Head);
		CVertex tI= DDBB->GetVertex(edgeI.Tail);
		POINT2D pedgeI(hI.x-tI.x, hI.y-tI.y);

		double meritParallel= NearlyParallel(origin, pedgeI, origin, pointAxis[0], Threshold);
		if(meritParallel > 0){
			ReferApp.DIA.NormalonEdges[i]= 0;
			ReferApp.DIA.meritNormalonEdges[i]= meritParallel;
		}
		else{
			meritParallel= NearlyParallel(origin, pedgeI, origin, pointAxis[1], Threshold);
			if(meritParallel > 0){
				ReferApp.DIA.NormalonEdges[i]= 1;
				ReferApp.DIA.meritNormalonEdges[i]= meritParallel;
			}
			else{
				meritParallel= NearlyParallel(origin, pedgeI, origin, pointAxis[2], Threshold);
				if (meritParallel > 0){
					ReferApp.DIA.NormalonEdges[i]= 2;
					ReferApp.DIA.meritNormalonEdges[i]= meritParallel;
				}
			}
		}
	}

	//If faces have been calculated,
	//check that there are no consecutive edges of the same face
	//labelled as parallel to the same main axis,
	//unless they span more than pi/2
	//(if so, remove the "less" parallel)
	for (long i= 0; i<DDBB->GetSizeFaces(); i++){
		std::vector<long> edgesInFaceI= DDBB->GetAllEdgesInFace(i);
		long numEdgesI= edgesInFaceI.size();

		for (long j=0; j<numEdgesI; j++){
			//Select consecutive edges parallel to the same main axis
			long current= edgesInFaceI[j];
			long next= edgesInFaceI[(j+1) % numEdgesI];
			if(ReferApp.DIA.NormalonEdges[current] == 
			   ReferApp.DIA.NormalonEdges[next]){

				CEdge edgeC= DDBB->GetEdge(current);
				long hC= edgeC.Head;
				long tC= edgeC.Tail;

				CEdge edgeN= DDBB->GetEdge(next);
				long hN= edgeN.Head;
				long tN= edgeN.Tail;

				double angle= NUMBER_PI;
				if(hN == hC)
					angle= DDBB->GetAngle2D_PI(tN, hN, tC) ;
				else if(hN == tC)
					angle= DDBB->GetAngle2D_PI(tN, hN, hC) ;
				else if(tN == hC)
					angle= DDBB->GetAngle2D_PI(hN, tN, tC) ;
				else if(tN == tC)
					angle= DDBB->GetAngle2D_PI(hN, tN, hC) ;

				//If angle is accute, remove the less parallel
				if(angle < NUMBER_PI/2){
					if(ReferApp.DIA.meritNormalonEdges[current] < 
					   ReferApp.DIA.meritNormalonEdges[next]){
						ReferApp.DIA.NormalonEdges[current]= -1;
						ReferApp.DIA.meritNormalonEdges[current]= 0;
					}
					else{
						ReferApp.DIA.NormalonEdges[next]= -1;
						ReferApp.DIA.meritNormalonEdges[next]= 0;
					}
				}
			}
		}
	}
}


double CCueTypology::IsNormalon(CDB_Model *DDBB)
// Determines whether the model fits the conditions of a normalon
// (A polyhedron is a normalon if all edges are aligned
//  with one of the three main perpendicular axes)

// Returns 0 if the model is not a normalon, a merit in the range [0, 1] otherwise
{
	double merit= 0;
	//long numParallel= 0;

	for (long i= 0; i<DDBB->GetSizeEdges(); i++){
		if (ReferApp.DIA.NormalonEdges[i] == -1){
			return 0.;  //At least one edge is not normalon
		}

		merit+= ReferApp.DIA.meritNormalonEdges[i];
		//numParallel++;
	}

	//return (merit/= numParallel);
	return (merit/= DDBB->GetSizeEdges());
}


double CCueTypology::IsQuasiNormalon(CDB_Model *DDBB,
									 std::vector<double> &VariablesZ_Aux)
//Determines whether the model fits the conditions of a Quasi-Normalon
//(A polyhedral is a Quasi-Normalon if removal of non-aligned edges does not result
//in the loss of any vertex or any connected subgraph becoming disconnected)

// Returns true if the model is a Quasi-Normalon, false otherwise
{
	//Create an auxiliar database
	CDB_Model DB_Aux= *DDBB;

	std::vector<double> merits= ReferApp.DIA.meritNormalonEdges;

	//Remove non-normalon edges
	for (long i= DB_Aux.GetSizeEdges()-1; i>=0; i--){
		if (ReferApp.DIA.NormalonEdges[i] == -1){
			DB_Aux.RemoveEdge(i, false);
			merits.erase(merits.begin()+i);
		}
	}

	//Return false if, at least, one vertex is connected to 0 vertices
	for (long i= 0; i<DB_Aux.GetSizeVertices(); i++){
		if (DB_Aux.GetSizeEdgesInVertex(i) == 0)
			return 0;
	}

	//Return false if the normalon model is made of more than one graph
	CCueSubGraphs SG;
	if(SG.IsMultiGraph(&DB_Aux))
		return 0;

	//Return merit
	double meritQN= 0;
	for (long i= 0; i<(long)merits.size(); i++){
		meritQN+= merits[i];
	}
	return (meritQN/= merits.size());
}


bool CCueTypology::IsPrismTopology(CDB_Model *DDBB, long &Base0, long &Base1)
//If faces have been detected, the topological conditions are as follows:
//		- All but two faces are quadrilateral
//		- The other two faces are similar (same number of sides, and similar area)
{
	Base0= Base1= -1;

	if(DDBB->GetSizeFaces() == 0){
		//Without faces, topological validation cannot be done
		//AfxMessageBox("No faces available to check if shape is prismatic");
		return false;
	}

	//Classify faces
	std::vector<long> sides;
	std::vector<long> faces;
	long quadrilaterals= 0;
	long numFaces= DDBB->GetSizeFaces();
	for(long i= 0; i<numFaces; i++){
		CFace Cara= DDBB->GetFace(i);
		sides.push_back(Cara.GetSizeEdgesInFace());
		if(sides[i] == 4) quadrilaterals++;
		faces.push_back(i);  //Pointers to reorder sides
	}

	//Check that, at least, all but two faces are quadrilateral
	if(quadrilaterals >= numFaces-2){
		SortMinorFirst(sides, faces);  //Faces with less edges are placed first
		if(sides[0] == 3){
			//Triangular prism
			if(!AreAdjacentFaces(DDBB, faces[0], faces[1])){
				Base0= faces[0];
				Base1= faces[1];
				return true;
			}
			else{
				return false;
			}
		}

		else if(sides[numFaces-1] > 4){
			//Polygonal prism
			if(!AreAdjacentFaces(DDBB, faces[numFaces-2], faces[numFaces-1])){
				Base0= faces[numFaces-2];
				Base1= faces[numFaces-1];
				return true;
			}
			else{
				return false;
			}
		}

		else if(quadrilaterals == numFaces){
			//Quadrilateral prism
			//Base0= faces[0];   //Arbitrarily select the first face as a base face
			//Select the bigger face as base face
			double areaMax= 0;
			for(long i= 0; i<numFaces; i++){
				double areaI= GetFaceArea(DDBB, i);
				if(areaI > areaMax){
					areaMax = areaI;
					Base0= i;
				}
			}

			//Select a non-adjacent face as second base
			for(long i= 0; i<(long)faces.size(); i++){
				if(faces[i] != Base0){
					if(!AreAdjacentFaces(DDBB, Base0, faces[i])){
						Base1= faces[i];
						return true;
					}
				}
			}
		}
	}

	return false;
}


double CCueTypology::IsPrismGeometry(CDB_Model *DDBB, long Base0, long Base1)
//Uses the following properties 
//to determines whether the model fits the geometrical conditions of a prism 
//		- All the vertices are thriedral
//		- There is a dominant direction (lateral edges are parallel to each other)
//		- Bases have similar area
{
	//Check if vertices are thriedral
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		if (DDBB->GetSizeEdgesInVertex(i) != 3){
			//AfxMessageBox("WARNING: not a Prism, because at least one vertex is non-thriedral");
			return 0;
		}
	}

	//Check if lateral edges are parallel to each other
	std::vector<long> edges0= DDBB->GetAllEdgesInFace(Base0);
	double averageAngle= 0;
	std::vector<double> angles;
	for (long i=0; i<(long)DDBB->GetSizeVerticesInFace(Base0); i++){
		std::vector<long> edgesI= DDBB->GetAllEdgesInVertex(DDBB->GetVertexInFace(Base0, i));
		for (long j=0; j<(long)edgesI.size(); j++){
			if(!IsValueInVector(edgesI[j], edges0)){
				angles.push_back(DDBB->GetAngle2DX_PI(edgesI[j], Accuracy));
				averageAngle+= angles[i];
			}
		}
	}
	averageAngle/= angles.size();

	//Reduce merit for non parallel lines
	double meritAngle= 1.;
	long numAngles= (long)angles.size();
	for (long i=0; i<numAngles; i++){
		double delta= std::abs(angles[i]-averageAngle);
		delta= min(delta, std::abs(delta - NUMBER_PI));
		if(delta > ReferApp.Cu.Ty.ThresholdForParallel){
			meritAngle-= 1/numAngles;
		}
	}

	//Reduce merit if bases have disimilar area
	double area0= GetFaceArea(DDBB, Base0);
	double area1= GetFaceArea(DDBB, Base1);
	double meritArea= 1. - min(1, (std::abs(area0-area1) / (ReferApp.Cu.Ty.ThresholdForArea*area0)));

	//Return combined merit
	return (meritAngle*0.5 + meritArea*0.5);
}


double CCueTypology::IsPrismGeometry(CDB_Model *DDBB)
//Uses the following heuristics 
//to determines whether the model fits the geometrical conditions of a prism 
//(without information about faces):
//		- All the vertices are thriedral
//		- The most dominant direction includes
//		  a number of edges "nearly" equal to half the number of vertices
{
	//Check if vertices are thriedral
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		if (DDBB->GetSizeEdgesInVertex(i) != 3){
			//AfxMessageBox("WARNING: not a Prism, because at least one vertex is non-thriedral");
			return 0;
		}
	}

	//Get angular distribution
	//(To detect edges "nearly" parallel to each other)
	std::vector<double> Angles;
	std::vector<double> Frequencies;
	DDBB->GetAngularDistribution(Angles, Frequencies, false);

	double totalFrequency= 0.;
	double maxFrequency= 0.;
	long numAngle= 0;
	for (long i=0; i<(long)Frequencies.size(); i++){
		totalFrequency+= Frequencies[i];
		if(Frequencies[i] > maxFrequency){
			maxFrequency= Frequencies[i];
			numAngle= i;
		}
	}

	double numFrequentEdges= ceil(Frequencies[numAngle]/totalFrequency*DDBB->GetSizeEdges());

	//Return merit 1 if exactly a "golden number" of edges are "nearly" parallel to each other
	//(Golden frequency is a percent of the number of vertices, typically 50%)
	long numVertices= (long)DDBB->GetSizeVertices();
	double merit= 1. - (std::abs(numFrequentEdges - (numVertices*ReferApp.Cu.Ty.dFrequencyPrismaticObjects)) / numVertices);

	return merit;
}


double CCueTypology::IsPrism(CDB_Model *DDBB, long &Base0, long &Base1)
//Returns the merit of the drawing contained in DDBB
//to likely represent a topologically
//and geometrically valid prismatic shape

//Merit is 0.25 for topologically valid shapes, is 0 otherwise.
//Merit ranges from 0.25 to 1 for geometrically valid shapes.

//Uses minimal information about faces
//Returns the numbers of the base faces

//Optionally, uses heuristics if faces are not available

//Base faces are returned as well
{
	Base0= Base1= -1;

	double merit= 0;

	//Uses topological criteria if faces are available 
	//to check if shape is prismatic
	if(DDBB->GetSizeFaces() > 0){
		if(!IsPrismTopology(DDBB, Base0, Base1)){   //Bases are calculated here
			return 0;
		}
		else{
			merit= 0.25;  //25% topology
		}
	}

	if(Base0!=-1 && Base1!=-1){
		//Examine geometry using the information about bases
		double meritGeometry= IsPrismGeometry(DDBB, Base0, Base1);
		merit+= meritGeometry *0.75;  //75% geometry
	}
	else{
		//Examine geometry without information about bases
		double meritGeometry= IsPrismGeometry(DDBB);
		merit+= meritGeometry *0.75;  //75% geometry
	}

	return merit;
}


TYPOLOGY CCueTypology::GetTypology(CDB_Model *DDBB)
//Uses some heuristics to determine the typology of the object depicted in the 2D model

//This cue is based on the idea of "divide and conquer"
//The better we can classify the typology of the object,
//the better we will be able to identify other cues and features
//which will ease its reconstruction

//Detects the following kinds of objects:
//		Pyramids
//		Normalons
//		Quasi-normalons
//		Prisms
//		Undefined
{

	//Typology is undefined if the line-drawing has less than three edges
	if (DDBB->GetSizeEdges() < 3){
		return Undefined_ND;
	}

	//Typology is undefined if all vertices have valence 2 or 1
	long maxValence= 0;
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		long valence= DDBB->GetSizeEdgesInVertex(i);

		if (valence > maxValence)
			maxValence= valence;
	}
	if (maxValence < 3){
		return Undefined_ND;
	}

	if(ReferApp.Cu.MA.MainAngles.size() != 3){
		AfxMessageBox("Unable to determine typology, as main axes are missing");
		return Undefined_ND;
	}

	//If still undefined, check if it is a pyramid
	//(Before detectign quasi-normalons, to prevent pyramids classified as such)
	if (IsPyramid(DDBB))
		return Pyramidal;

	//Other typologies cannot be evaluated if Main Axes detection is not active
	//if (!ReferApp.Cu.DetectMainAxes){
	//	AfxMessageBox("ERROR Main axes are required to detect Typology");
	//	return Indefinido_ND;
	//}

	//Pre-calculation of normalon edges
	GetNormalonEdges(DDBB);

	if (IsNormalon(DDBB) > 0){
		return Normalon;
	}

	//If still undefined, check if it is a QuasiNormalon
	std::vector<double> Aux;
	if (IsQuasiNormalon(DDBB, Aux) > 0)
		return QuasiNormalon;

	//If still undefined, check if it is a prism
	//(After detecting normalons, to prevent quadrilateral primsm to be labelled as such)
	long base0, base1;
	if (IsPrism(DDBB, base0, base1) > 0.5)   //At least 50% probability of being prismatic
		return Prismatic;

	//If still undefined, check if it is a particular type of undefined
	if (ReferApp.Cu.Ty.Typology == Undefined){
		long NumNormalonEdges= 0;

		//for(long i= 0; i<(long)ReferApp.Cu.MA.MainAngles.size(); i++){
		//	if(ReferApp.Cu.MA.MainAngles[i] != -1) NumNormalonEdges++;
		//}

		for (long i= 0; i<DDBB->GetSizeEdges(); i++){
			if (ReferApp.DIA.NormalonEdges[i] == -1){
				NumNormalonEdges++;
		}
	}

		if ((double)NumNormalonEdges/(double)DDBB->GetSizeEdges()>
			ReferApp.Cu.Ty.dFrequencyUndefinedObjects_D)
			ReferApp.Cu.Ty.Typology= Undefined_D;
		else
			ReferApp.Cu.Ty.Typology= Undefined_ND;
	}

	return ReferApp.Cu.Ty.Typology;
}
