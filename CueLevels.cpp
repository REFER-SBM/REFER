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
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "resource.h"
#include "ReferDoc.h"

#include "CueTypology.h"
#include "CueLevels.h"


double CCueLevels::GetRotationToPredominant(CDB_Model *DDBB)
//Detects the predominant direction

//Useful to perform the vertex matching
//after the rotation has been neutralized.

//Predominant direction depends on the typology:
//		Prismatic: generatrices define the predominant direction
//		Normalon: vertical is the predominant direction
//		Undefined:the most frequent direction is the dominant one
{
	double AnguloY= 0;

	std::vector<double> EdgeAngles;
	for (long i=0; i<DDBB->GetSizeEdges(); i++){
		EdgeAngles.push_back(RADIANS_TO_DEGREES(DDBB->GetAngle2DX_PI(i, Accuracy)));
	}

	switch (ReferApp.Cu.Ty.Typology){
	case Prismatic:
		{
		long MayorFrecuencia=0;
		double AnguloMayorFrecuencia;
		while ((long)EdgeAngles.size() != 0){
			long Frecuencia= 1;
			double AnguloReferencia= EdgeAngles[0];
			EdgeAngles.erase(EdgeAngles.begin()+0);
			for (long i=0; i<(long)EdgeAngles.size(); i++){
				if (std::fabs(EdgeAngles[i]-AnguloReferencia) <=
					ReferApp.DIL.VerticalAngleThreshold){
					Frecuencia++;
					EdgeAngles.erase(EdgeAngles.begin()+i);
					i--;
				}
			}
			if (Frecuencia > MayorFrecuencia){
				MayorFrecuencia= Frecuencia;
				AnguloMayorFrecuencia= AnguloReferencia;
			}
		}
		AnguloY = AnguloMayorFrecuencia;
		break;
		}

	case Normalon:
		{
		double MenorDiferencia = std::fabs(EdgeAngles[0]-90);
		double AnguloMenorDiferencia = EdgeAngles[0];
		for (long i=1; i<(long)EdgeAngles.size(); i++){
			double Diferencia= std::fabs(EdgeAngles[i]-90);
			if (Diferencia<MenorDiferencia){
				MenorDiferencia = Diferencia;
				AnguloMenorDiferencia = EdgeAngles[i];
			}
		}
		AnguloY = AnguloMenorDiferencia;
		break;
		}

	case QuasiNormalon:
		{
		//Elimino de la lista de angulos, aquellos que no sean paralelos a las direcciones principales
		double tolParallel= 10; //Tolerance in degrees for parallel edges
		for (long IndiceAngulo=0; IndiceAngulo<(long)EdgeAngles.size(); IndiceAngulo++){
			long i= 0;
			bool parallel= false;
			while (i<(long)ReferApp.Cu.MA.MainAngles.size() && !parallel){
				double angleI= fabs(EdgeAngles[IndiceAngulo] - RADIANS_TO_DEGREES(ReferApp.Cu.MA.MainAngles[i]));
				if (angleI < tolParallel || fabs(180-angleI) < tolParallel)
					parallel=true;
				i++;
			}
			if (!parallel){
				EdgeAngles.erase(EdgeAngles.begin()+IndiceAngulo);
				IndiceAngulo--;
			}
		}

		if(EdgeAngles.size() == 0)
			//None edges parallel to the main directions were found!!
			break;

		double MenorDiferencia = std::fabs(EdgeAngles[0]-90);
		double AnguloMenorDiferencia = EdgeAngles[0];
		for (long IndiceAngulo=1; IndiceAngulo<(long)EdgeAngles.size(); IndiceAngulo++){
			double Diferencia= std::fabs(EdgeAngles[IndiceAngulo]-90);
			if (Diferencia < MenorDiferencia){
				MenorDiferencia = Diferencia;
				AnguloMenorDiferencia = EdgeAngles[IndiceAngulo];
			}
		}
		AnguloY = AnguloMenorDiferencia;
		break;
		}

//	case Pyramidal:
//		//La orientación vendrá dada por el ángulo que forma el eje de la pirámide
//		VerticeMayorValencia= GetHighestValence(DDBB);
//
//		NuevaListaVertices(&ListaVertices);
//		DDBB->CopyTo(&ListaVertices);
//		ObtenListaVertices(&ListaVertices, VerticeMayorValencia, &VerticePiramide);
//		EliminaListaVertices(&ListaVertices, VerticeMayorValencia);
//		ListaVertices.XYZMaxMinVertices(&dXmin, &dYmin, &dZmin, &dXmax, &dYmax, &dZmax);
//		//Determino la desviacion del eje de la piramide con respecto al eje Y con el
//		//fin de intentar diferenciar entre piramides rectas y oblicuas
//		CentroBase.x = (dXmax - dXmin)/2+dXmin;
//		CentroBase.y = (dYmax - dYmin)/2+dYmin;
//		AnguloY = acos((VerticePiramide.y - CentroBase.y)/MODULO(VerticePiramide,CentroBase));
//
//		if (VerticePiramide.y>CentroBase.y){
//			if (VerticePiramide.x>CentroBase.x) AnguloY=pi/2-AnguloY;
//			else AnguloY+=pi/2;
//		}
//		else{
//			if (VerticePiramide.x>CentroBase.x) AnguloY+=3*pi/2;
//			else AnguloY=3*pi/2-AnguloY;
//		}
//
//		AnguloY=AnguloY*180/pi;
//		DestruyeListaVertices(&ListaVertices);
//		break;
	}

	if (ReferApp.Cu.Ty.Typology == Undefined ||
		ReferApp.Cu.Ty.Typology == Undefined_D ||
		ReferApp.Cu.Ty.Typology == Undefined_ND){
		//Obtengo los ángulos que se repiten con mayor frecuencia
		std::vector<double> ListaAngulosMayorFrecuencia;
		long MayorFrecuencia=0;
		while ((long)EdgeAngles.size() != 0){
			long Frecuencia=1;
			double AnguloReferencia= EdgeAngles[0];
			EdgeAngles.erase(EdgeAngles.begin()+0);
			for (long IndiceAngulo=0; IndiceAngulo<(long)EdgeAngles.size(); IndiceAngulo++){
				if (std::fabs(EdgeAngles[IndiceAngulo]-AnguloReferencia) <=
					ReferApp.DIL.VerticalAngleThreshold){
					Frecuencia++;
					EdgeAngles.erase(EdgeAngles.begin()+IndiceAngulo);
					IndiceAngulo--;
				}
			}

			if (Frecuencia==MayorFrecuencia)
				ListaAngulosMayorFrecuencia.push_back(AnguloReferencia);
			else{
				if (Frecuencia>MayorFrecuencia){
					MayorFrecuencia=Frecuencia;
					if (Frecuencia-MayorFrecuencia>2){
						ListaAngulosMayorFrecuencia.clear();
						ListaAngulosMayorFrecuencia.push_back(AnguloReferencia);
					}
					else
						ListaAngulosMayorFrecuencia.push_back(AnguloReferencia);
				}
				else{
					if (MayorFrecuencia-Frecuencia>2)
						ListaAngulosMayorFrecuencia.push_back(AnguloReferencia);
				}
			}
		}

		//De los angulos de mayor frecuencia selecciono el más próximo a 90º
		if ((long)ListaAngulosMayorFrecuencia.size() == 1)
			AnguloY= ListaAngulosMayorFrecuencia[0];
		else{
			double MenorDiferencia = std::fabs(ListaAngulosMayorFrecuencia[0]-90);
			double AnguloMenorDiferencia = ListaAngulosMayorFrecuencia[0];
			for (long IndiceAngulo=1; IndiceAngulo<(long)ListaAngulosMayorFrecuencia.size(); IndiceAngulo++){
				double Diferencia=std::fabs(ListaAngulosMayorFrecuencia[IndiceAngulo]-90);
				if (Diferencia<MenorDiferencia){
					MenorDiferencia = Diferencia;
					AnguloMenorDiferencia = ListaAngulosMayorFrecuencia[IndiceAngulo];
				}
			}
			AnguloY = AnguloMenorDiferencia;
		}

//		Angulo= ListaAngulos[0];
//		MenorDiferencia = std::fabs(Angulo-90);
//		AnguloMenorDiferencia = Angulo;
//		for (IndiceAngulo=1; IndiceAngulo<(long)ListaAngulos.size(); IndiceAngulo++){
//			Angulo= ListaAngulos[IndiceAngulo];
//			Diferencia=std::fabs(Angulo-90);
//			if (Diferencia<MenorDiferencia){
//				MenorDiferencia = Diferencia;
//				AnguloMenorDiferencia = Angulo;
//			}
//		}
//		AnguloY = AnguloMenorDiferencia;
//		break;
	}

	if (std::fabs(AnguloY-90) < std::fabs(AnguloY-270))
		return (AnguloY-90);
	else
		return (AnguloY-270);
}


long CCueLevels::GetCuspPyramid(CDB_Model *DDBB)
//If the model is a pyramid, then its cusp is the only vertex
//connected to more than three edges
{
	long cusp= -1;
	if (ReferApp.Cu.Ty.Typology != Pyramidal)
		return cusp;

	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		if(DDBB->GetSizeEdgesInVertex(i) >3){
			cusp= i;
			break;
		}
	}
	return cusp;
}


double CCueLevels::GetPyramidVertexRotation(CDB_Model *DDBB, long cusp, long IndexVertex)
//Returns the rotation of vertex "IndexVertex" of the base of a pyramid whose cusp is known
{
	//if (Typology != Pyramidal)
	//	return 0;

	if (IndexVertex == cusp)
		return 0;

	//Detect the edge connecting IndexVertex to the cusp
	long iArista= -1;
	for (long i= 0; i<DDBB->GetSizeEdgesInVertex(IndexVertex); i++){
		long ii= DDBB->GetEdgeInVertex(IndexVertex, i);
		CEdge Arista= DDBB->GetEdge(ii);
		if (Arista.Head == cusp || Arista.Tail == cusp){
			iArista= ii;
			break;
		}
	}
	assert (iArista!=-1);

	//Get the angle of the edge connected to the cusp
	double angle;
	CEdge Arista= DDBB->GetEdge(iArista);
	if (Arista.Head == cusp){
		angle= DDBB->GetAngle2DX_2PI(iArista, Arista.Tail, Accuracy);
	}
	else{
		angle= DDBB->GetAngle2DX_2PI(iArista, Arista.Head, Accuracy);
	}

	//Return the angle of the edge relative to the vertical axis
	return RADIANS_TO_DEGREES(angle - (NUMBER_PI/2));
}


bool CCueLevels::GetAngles(CDB_Model *DDBB, double Rotation, long cusp, long Index,
						   double &MaxAngle, double &MidAngle, double &MinAngle)
//Gets the main angles of the vertex "Index"

//Returns true if two or more edges are connected to the vertex
//Returns false otherwise

//The angles of all edges connected to vertex "Index" are calculated
//Then, they are rotated, to make predominant direction vertical
//Then, the minimum, maximum and intermediate are returned

//The intermediate angle is interpolated for vertices connected to just two edges
{
	//Rotation is recalculated for pyramid cusps
	if(cusp != -1) // is a pyramid
		Rotation= GetPyramidVertexRotation(DDBB, cusp, Index);

	//Get angles of all edges connected to the current vertex
	std::vector<double> Angles;
	for (long i=0; i<DDBB->GetSizeEdgesInVertex(Index); i++){
		double Angle= RADIANS_TO_DEGREES(DDBB->GetAngle2DX_2PI(DDBB->GetEdgeInVertex(Index, i), Index, Accuracy));
		Angle -= Rotation;  //Rotate angle according to predominant direction

		if (Angle<0)
			Angle+=360;
		if(std::fabs(Angle-180)<Accuracy)
			Angle=180;

		Angles.push_back(Angle);
	}

	//Angles are undefined if vertex is shared just by one edge
	if (Angles.size() < 2)
		return false;

	//Get the three main angles
	if (Angles.size() == 2){
		MinAngle= min(Angles[0], Angles[1]);
		MaxAngle= max(Angles[0], Angles[1]);

		//Produce an "extended" junction, adding a bisecting fictitious third edge
		MidAngle= (MaxAngle+MinAngle)/2;

		double SupplementaryAngle = MidAngle+180;
		if (SupplementaryAngle>360) SupplementaryAngle-=360;
		if (std::fabs(SupplementaryAngle-MinAngle)<std::fabs(MidAngle-MinAngle) ||
			std::fabs(SupplementaryAngle-MaxAngle)<std::fabs(MidAngle-MaxAngle))
			MidAngle= SupplementaryAngle;
	}
	else{
		//Sort angles
		Angles= SortMinorFirst(Angles);

		//Get min, max and intermediate angles
		long NumAngles= Angles.size();
		MinAngle= Angles[0];
		MaxAngle= Angles[NumAngles-1];
		MidAngle= Angles[(long)(NumAngles/2)];
	}

	return true;
}


int CCueLevels::GetLevel(double MaxAngle, double MidAngle, double MinAngle)
//Returns a level typology
//Matches the angles of the current vertex with one of the valid configurations:
//	Level 0= inverted Y
//	Level 1= inverted W
//	Level 2= lateral W
//	Level 3= lateral inverted W
//	Level 4= W
//	Level 5= Y

//If necessary, the vertex is rotated
//to convert into vertical the edge initially closest to vertical
{
	//Rotate the triplet if does not contain any vertical edge
	//if (std::fabs(MinAngle-90)>ReferApp.DIL.VerticalAngleThreshold &&
	//	std::fabs(MidAngle-90)>ReferApp.DIL.VerticalAngleThreshold &&
	//	std::fabs(MaxAngle-90)>ReferApp.DIL.VerticalAngleThreshold &&
	//	std::fabs(MinAngle-270)>ReferApp.DIL.VerticalAngleThreshold &&
	//	std::fabs(MidAngle-270)>ReferApp.DIL.VerticalAngleThreshold &&
	//	std::fabs(MaxAngle-270)>ReferApp.DIL.VerticalAngleThreshold){

		//Calculate the suitable rotation
		double Rotation= min(std::fabs(MinAngle-90), min(std::fabs(MidAngle-90), std::fabs(MaxAngle-90)));
		Rotation= min(Rotation, min(std::fabs(MidAngle-270), std::fabs(MaxAngle-270)));
		Rotation= min(Rotation, std::fabs(MinAngle-270));

		//Rotate the angles to get a vertical edge
		MinAngle -= Rotation;
		MidAngle -= Rotation;
		MaxAngle -= Rotation;
	//}

	//Assign level
	if (std::fabs(MinAngle-90) < ReferApp.DIL.VerticalAngleThreshold ||
		std::fabs(MidAngle-90) < ReferApp.DIL.VerticalAngleThreshold ||
		std::fabs(MaxAngle-90) < ReferApp.DIL.VerticalAngleThreshold){

		//Assign level if vertical edge is 90 degrees
		if (std::fabs(MaxAngle-90) < ReferApp.DIL.VerticalAngleThreshold)
			return 2;  //Lateral W
		else{
			if (std::fabs(MinAngle-90) < ReferApp.DIL.VerticalAngleThreshold){
				if (MidAngle<180){
					if (MaxAngle<270)
						return 2;  //Lateral W
					else{
						if (MaxAngle < MidAngle+180)
							return 0;  //Inverted Y
						else
							return 4;  //W
					}
				}
				else{
					if (MidAngle<270){
						if (MaxAngle<270)
							return 2;  //Lateral W
						else
							return 0;  //Inverted Y
					}
					else
						return 2;  //Lateral W
				}
			}
			else{
				if (MaxAngle<MinAngle+180)
					return 4;  //W
				else{
					if (MaxAngle<270)
						return 0;  //Inverted Y
					else
						return 2;  //Lateral W
				}
			}
		}
	}
	else{
		//Assign level if vertical edge is 270 degrees
		if (std::fabs(MinAngle-270) < ReferApp.DIL.VerticalAngleThreshold)
			return 3;
		else{
			if (std::fabs(MaxAngle-270) < ReferApp.DIL.VerticalAngleThreshold){
				if (MinAngle<90){
					if (MidAngle<90)
						return 3;  //Lateral inverted W
					else{
						if (MidAngle<MinAngle+180)
							return 5;
						else
							return 1;  //Inverted W
					}
				}
				else
					return 3;  //Lateral inverted W
			}
			else{
				if (MinAngle<90)
					return 3;  //Lateral inverted W
				else{
					if (MinAngle<MaxAngle-180)
						return 5;  //Y
					else
						return 1;  //Inverted W
				}
			}
		}
	}
}


void CCueLevels::GetLevelWithHiddenEdges(CDB_Model *DDBB, long NumVertex, long &Level)
//Assigns level to the vertext NumVertex

//Distinguishes between hidden and visible edges (if this information is available):
//	Levels 0-5= all edges hidden
//	Levels 6-11= some edges hidden
//	levels 12-17= all edges visible

// Requires edges to be previously marked as visible or hidden
{
	//Determine vertex visibility
	long NumEdges= DDBB->GetSizeEdgesInVertex(NumVertex);
	long visible= 0;
	for (long i= 0; i<NumEdges; i++){
		if(ReferApp.Cu.Hi.TypesOfEdges[DDBB->GetEdgeInVertex(NumVertex, i)] == VISIBLE)
			visible++;
	}

	//Modify level according to the visibility
	if(visible == NumEdges)
		Level= Level+12; // V_VISIBLE;
	else if(visible == 0)
		Level= Level; // V_HIDDEN;
	else
		Level= Level+6; // V_HIDDEN_VISIBLE;
}


void CCueLevels::GetLevels(CDB_Model *DDBB)
// Manager for finding levels of vertices in polyhedral shapes
// The input is a list of vertices and edges contained in DDBB
// The oputput is a list of labels "LevelOfVertex"

// 1.In order to calculate levels of the vertices, uses hidden edges (if available)
// 2.Assigns labels to all the vertices
// 3.Levels depend on the configuration of the vertices
//   (configuration is the angular distribution of the edges connected to the vertices)
// 4.Valid levels range from L0 to L17

//Based on:
	//Company P., Contero M., Conesa J. and Piquer A.
	//An optimisation-based reconstruction engine for 3D modelling by sketching.
	//Computers & Graphics (ISSN 0097-8493). Vol. 28, No 6. December 2004. pp. 955-979

	//Conesa-Pastor J.
	//RECONSTRUCCIÓN GEOMÉTRICA DE SÓLIDOS UTILIZANDO TÉCNICAS DE OPTIMIZACIÓN.
	//PhD Thesis, 2001
	//Departamento de Estructuras y Construcción.
	//Universidad Politécnica de Cartagena

//Assigns a level to every vertex
//Vertices are classified according to their configuration:
//	Level 0= inverted Y
//	Level 1= inverted W
//	Level 2= lateral W
//	Level 3= lateral inverted W
//	Level 4= W
//	Level 5= Y

//Distinguishes between hidden and visible edges (if this information is available):
//	Levels 0-5= all edges hidden
//	Levels 6-11= some edges hidden
//	levels 12-17= all edges visible
{
	//if(ReferApp.Cu.Hi.TypesOfEdges.size() > 0){
	//	AfxMessageBox("No information of hidden edges available to refine levels calculation");
		//return;
	//}

	//Previous calculations
	//---------------------
	//Get the predominant direction
	double Rotation= GetRotationToPredominant(DDBB);

	//Detect the cusp (only for pyramids)
	long cusp= GetCuspPyramid(DDBB);

	//Loop over all the vertices to assign levels
	//-------------------------------------------
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		double MaxAngle, MidAngle, MinAngle;

		//Get level
		long Level;
		if (GetAngles(DDBB, Rotation, cusp, i, MaxAngle, MidAngle, MinAngle)){
			//Get level
			Level= GetLevel(MaxAngle, MidAngle, MinAngle);

			//Modify level according to its visibility
			if(ReferApp.GeneralParams.DetectHiddenEdges &&
				ReferApp.Cu.Hi.TypesOfEdges.size() > 0){
				GetLevelWithHiddenEdges(DDBB, i, Level);
			}
		}
		else{ //Level is undefined if vertex is shared just by one edge
			Level= -1;
		}

		//Save level
		ReferApp.DIL.LevelOfVertex.push_back(Level);
	}
}