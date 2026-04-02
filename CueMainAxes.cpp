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

#include "Tools_Vector.h"  //SortMinorFirst
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_Model_Maths.h"
#include "Refer.h"

#include "CueMainAxes.h"


void CCueMainAxes::GetTriplets(std::vector<double> Angles,
							   std::vector<double> Frequencies,
							   std::vector<std::vector<double>> &Triplets,
							   std::vector<double> &Merits,
							   std::vector<double> &TotalFrequency)
//Get candidate triplets and their frequencies

//Frequency of each triplet is the sum of the frequencies of their three components

//Initially, triplets are obtained by combining the set of Angles
//Then, each triplet is splitted into eight sub-triplets

//All three angles of each triplet must be in the range [0, 2*PI]
//Triplets out of range are repaired, and repetitious triplets are deleted

//WARNING! Triplets are ordered, as long as Angles are ordered
{
	//Get candidate triplets by permutation
	//and split them to get sub-triplets
	for (long i=0; i<(long)Angles.size()-2; i++){
		for (long j=i+1; j<(long)Angles.size()-1; j++){
			for (long k=j+1; k<(long)Angles.size(); k++){
				//Split current triplet in their eight sub-triplets
				Triplets[0].push_back(Angles[i]);
				Triplets[1].push_back(Angles[j]);
				Triplets[2].push_back(Angles[k]);
				Merits.push_back(0);
				TotalFrequency.push_back(Frequencies[i]+ Frequencies[j]+ Frequencies[k]);

				Triplets[0].push_back(Angles[i]);
				Triplets[1].push_back(Angles[j]);
				Triplets[2].push_back(Angles[k]+NUMBER_PI);
				Merits.push_back(0);
				TotalFrequency.push_back(Frequencies[i]+ Frequencies[j]+ Frequencies[k]);

				Triplets[0].push_back(Angles[i]);
				Triplets[1].push_back(Angles[k]);
				Triplets[2].push_back(Angles[j]+NUMBER_PI);
				Merits.push_back(0);
				TotalFrequency.push_back(Frequencies[i]+ Frequencies[j]+ Frequencies[k]);

				Triplets[0].push_back(Angles[i]);
				Triplets[1].push_back(Angles[j]+NUMBER_PI);
				Triplets[2].push_back(Angles[k]+NUMBER_PI);
				Merits.push_back(0);
				TotalFrequency.push_back(Frequencies[i]+ Frequencies[j]+ Frequencies[k]);

				Triplets[0].push_back(Angles[j]);
				Triplets[1].push_back(Angles[k]);
				Triplets[2].push_back(Angles[i]+NUMBER_PI);
				Merits.push_back(0);
				TotalFrequency.push_back(Frequencies[i]+ Frequencies[j]+ Frequencies[k]);

				Triplets[0].push_back(Angles[j]);
				Triplets[1].push_back(Angles[i]+NUMBER_PI);
				Triplets[2].push_back(Angles[k]+NUMBER_PI);
				Merits.push_back(0);
				TotalFrequency.push_back(Frequencies[i]+ Frequencies[j]+ Frequencies[k]);

				Triplets[0].push_back(Angles[k]);
				Triplets[1].push_back(Angles[i]+NUMBER_PI);
				Triplets[2].push_back(Angles[j]+NUMBER_PI);
				Merits.push_back(0);
				TotalFrequency.push_back(Frequencies[i]+ Frequencies[j]+ Frequencies[k]);

				Triplets[0].push_back(Angles[i]+NUMBER_PI);
				Triplets[1].push_back(Angles[j]+NUMBER_PI);
				Triplets[2].push_back(Angles[k]+NUMBER_PI);
				Merits.push_back(0);
				TotalFrequency.push_back(Frequencies[i]+ Frequencies[j]+ Frequencies[k]);
			}
		}
	}

	//Repair triplets out of range
	for (long t=0; t<(long)Triplets[0].size(); t++){
		while (Triplets[0][t]>2*NUMBER_PI) Triplets[0][t]-= 2*NUMBER_PI;
		while (Triplets[1][t]>2*NUMBER_PI) Triplets[1][t]-= 2*NUMBER_PI;
		while (Triplets[2][t]>2*NUMBER_PI) Triplets[2][t]-= 2*NUMBER_PI;
	}

	//Remove repetitious triplets
	for (long t=(long)Triplets[0].size()-1; t<=0; t--){
		for (long i=0; i<t; i++){
			if ((Triplets[0][t]-Triplets[0][i] < ReferApp.Cu.MA.ThresholdOverlap) &&
				(Triplets[1][t]-Triplets[1][i] < ReferApp.Cu.MA.ThresholdOverlap) &&
				(Triplets[2][t]-Triplets[2][i] < ReferApp.Cu.MA.ThresholdOverlap)){
				//Remove triplet
				Triplets[2].erase(Triplets[2].begin()+t);
				Triplets[1].erase(Triplets[1].begin()+t);
				Triplets[0].erase(Triplets[0].begin()+t);
				Merits.erase(Merits.begin()+t);
				TotalFrequency.erase(TotalFrequency.begin()+t);
			}
		}
	}
}


void CCueMainAxes::AutomaticDetect(CDB_Model *DDBB,
								   std::vector<double> Angles,
								   std::vector<double> Frequencies)
//Input is the ADG (Angles and Frequencies)
//Output is a triplet of MainAngles

//Candidate triplets (permutations of 3 angles) are obtained first

//Discards triplets that do not accomplish the following rule:
//	Projective rule
//Increases the merits of triplets that accomplish the following rules:
//	Topologic rule (matching corners)
//	Isonometric rule
//	Vertical rule
//	Horizontal rule
//	Frequency rule
//(User may interactively modify active rules; those that are checked)

//Then selects the triplet with the highest merit
{
	if(Angles.size() < 3){
		AfxMessageBox("Unable to find at least three angles to automatically detect main axes");
		return;
	}

	//Get candidate triplets by permutation
	//-------------------------------------
	std::vector<std::vector<double>> Triplets (3,std::vector<double>(0));
	std::vector<double> Merits;
	std::vector<double> TotalFrequency;

	GetTriplets(Angles, Frequencies,
				Triplets, Merits, TotalFrequency);

	if(Triplets[0].size() == 0){
		ReferApp.Cu.MA.MainAngles.clear();
		AfxMessageBox("Unable to get valid triplets required to automatically detect main axes");
		return;
	}

	//Projective rule (Perkins rule):
	//All three angles around the corner must be greater than 90 
	//or two, each less than 90°, must sum to greater than 90

	//If a junction represents a cubic corner, 
	//then either two of its angles are acute and sum to more than 90° 
	//(two-faced cubic corner,also named as W corners); 
	//or else all three of its angles are obtuse
	//(three-faced cubic corner, also named as Y corners)
	//-----------------------------------------------
	if (ReferApp.Cu.MA.ProjectiveRule){
		for (long t=0; t<(long)Triplets[0].size(); t++){

			//Get angles between edges of the triplets
			double t20= Triplets[2][t]-Triplets[0][t];
			if(t20 > NUMBER_PI) t20= 2*NUMBER_PI-t20;
			double t10= Triplets[1][t]-Triplets[0][t];
			if(t10 > NUMBER_PI) t10= 2*NUMBER_PI-t10;
			double t21= Triplets[2][t]-Triplets[1][t];
			if(t21 > NUMBER_PI) t21= 2*NUMBER_PI-t21;

			double PIHalf= NUMBER_PI/2;
			double tolerance= ReferApp.Cu.MA.ToleranceCubicAngles;   //Threshold angle in radians (default is 5 degrees)

			if ((t20 < (PIHalf+tolerance) && t10 < (PIHalf+tolerance) && (t20+t10) > (PIHalf+tolerance)) ||
				(t20 < (PIHalf+tolerance) && t21 < (PIHalf+tolerance) && (t20+t21) > (PIHalf+tolerance)) ||
				(t10 < (PIHalf+tolerance) && t21 < (PIHalf+tolerance) && (t10+t21) > (PIHalf+tolerance)) ||
				(t20 > (PIHalf-tolerance) && t10 > (PIHalf-tolerance) && t21 > (PIHalf-tolerance))){
				continue;
			}
			else{
				//Remove triplet
				Triplets[2].erase(Triplets[2].begin()+t);
				Triplets[1].erase(Triplets[1].begin()+t);
				Triplets[0].erase(Triplets[0].begin()+t);
				Merits.erase(Merits.begin()+t);
				TotalFrequency.erase(TotalFrequency.begin()+t);
				t--;
			}
		}
	}

	if(Merits.size() == 0){
		ReferApp.Cu.MA.MainAngles.clear();
		AfxMessageBox("Projective rule discarded all candidate main axes");
		return;
	}

	//Increase merit according to rules
	//---------------------------------

	//Topologic rule (matching corners):
	//Increase merit for all triplets where three main directions appear simultaneously
	//in the edges connected to the same vertex of the 2D model
	if (ReferApp.Cu.MA.MatchingCornersRule){
		for (long i=0; i<DDBB->GetSizeVertices(); i++){
			//Get the angles of the edges connected to the current "i" vertex
			std::vector<double> AnglesInVertex;
			if(DDBB->GetSizeEdgesInVertex(i) == 3){
				for (long j=0; j<3; j++){
					AnglesInVertex.push_back(DDBB->GetAngle2DX_2PI(DDBB->GetEdgeInVertex(i, j), i, Accuracy));
				}
				AnglesInVertex= SortMinorFirst(AnglesInVertex);

				//Increase the merit of triplets that match current vertex
				for (long t=0; t<(long)Triplets[0].size(); t++){
					double T0= std::fabs(Triplets[0][t]-AnglesInVertex[0]);
					double T1= std::fabs(Triplets[1][t]-AnglesInVertex[1]);
					double T2= std::fabs(Triplets[2][t]-AnglesInVertex[2]);
					double maxT= max(T0, max(T1, T2));
					if (maxT < ReferApp.Cu.MA.ThresholdAngle){
						Merits[t] += ReferApp.Cu.MA.MeritMatchingCorners * (1-maxT/ReferApp.Cu.MA.ThresholdAngle);
					}
				}
			}
		}
	}

	//Isometric rule:
	//Increase merit for all triplets following isometric configuration (90, 210, 330)
	if (ReferApp.Cu.MA.IsometricRule){
		for (long t=0; t<(long)Triplets[0].size(); t++){
			double T0= std::fabs(Triplets[0][t]-(NUMBER_PI/2));
			double T1= std::fabs(Triplets[1][t]-(DEGREES_TO_RADIANS(210)));
			double T2= std::fabs(Triplets[2][t]-(DEGREES_TO_RADIANS(330)));
			double avT= (T0+T1+T2)/3.;
			if (avT < ReferApp.Cu.MA.ThresholdAngle){
				Merits[t] += ReferApp.Cu.MA.MeritIsometric * (1-avT/ReferApp.Cu.MA.ThresholdAngle);
			}
		}
	}

	//Vertical rule:
	//Increase merit for triplets containing one vertical and upward axis
	if (ReferApp.Cu.MA.VerticalRule){
		for (long t=0; t<(long)Triplets[0].size(); t++){
			double minT= std::fabs(Triplets[0][t]-(NUMBER_PI/2));
			minT= min(minT, std::fabs(Triplets[1][t]-(NUMBER_PI/2)));
			minT= min(minT, std::fabs(Triplets[2][t]-(NUMBER_PI/2)));
			if (minT < ReferApp.Cu.MA.ThresholdAngle){
				Merits[t] += ReferApp.Cu.MA.MeritVertical * (1 - minT/ReferApp.Cu.MA.ThresholdAngle);
			}
		}
	}

	//Horizontal rule:
	//Increase merit for triplets containing one horizontal axis
	if (ReferApp.Cu.MA.HorizontalRule){
		for (long t=0; t<(long)Triplets[0].size(); t++){
			double minT= std::fabs(Triplets[0][t]);
			minT= min(minT, std::fabs(Triplets[1][t]));
			minT= min(minT, std::fabs(Triplets[2][t]));
			if (minT < ReferApp.Cu.MA.ThresholdAngle){
				Merits[t] += ReferApp.Cu.MA.MeritHorizontal * (1 - minT/ReferApp.Cu.MA.ThresholdAngle);
			}
		}
	}

	//Frequency rule:
	//Increase merit for triplet with total frequency greater than average
	if (ReferApp.Cu.MA.FrequencyRule){
		std::vector<long> OrderedFrequencies;
		long N= TotalFrequency.size();
		for (long t=0; t<N; t++){OrderedFrequencies.push_back(t);};
		SortMinorFirst(TotalFrequency, OrderedFrequencies);

		long N2= N/2;
		while (N2<N && TotalFrequency[N2] == TotalFrequency[N2-1]) N2++;
		for (long t=N2; t<N; t++){
			Merits[OrderedFrequencies[t]] += ReferApp.Cu.MA.MeritFrequency * (1-TotalFrequency[t]/TotalFrequency[N-1]);
		}
	}

	//Save as main axes the triplet with the highest merit
	//----------------------------------------------------
	double MaxMerit= Merits[0];
	long tMerit= 0;
	for (long t=1; t<(long)Merits.size(); t++){
		if(Merits[t] > MaxMerit){
			MaxMerit= Merits[t];
			tMerit= t;
		}
	}

	ReferApp.Cu.MA.MainAngles.push_back(Triplets[0][tMerit]);
	ReferApp.Cu.MA.MainAngles.push_back(Triplets[1][tMerit]);
	ReferApp.Cu.MA.MainAngles.push_back(Triplets[2][tMerit]);
}


void CCueMainAxes::GetBestAngles(CDB_Model *DDBB)
//Calculates the orientations of the main axes of the model
//depending on the current approach

//The angles are saved in the global vector "MainAngles"
{
	switch (ReferApp.Cu.MA.MainAxesApproach){
		case 0: //Automatic detection based on rules
		{
			//Get angular distribution
			std::vector<double> Angles;
			std::vector<double> Frequencies;

			DDBB->GetAngularDistribution(Angles, Frequencies, true);

			//Get main angles that better fit rules
			ReferApp.Cu.MA.MainAngles.clear();
			if(Angles.size() == 3){
				ReferApp.Cu.MA.MainAngles.push_back(Angles[0]);
				ReferApp.Cu.MA.MainAngles.push_back(Angles[1]);
				ReferApp.Cu.MA.MainAngles.push_back(Angles[2]);
			}
			else{
				AutomaticDetect(DDBB, Angles, Frequencies);
			}

			break;
		}

		case 1: //Orientations of the three first edges
		{
			ReferApp.Cu.MA.MainAngles.clear();
			if ((long)DDBB->GetSizeEdges()>=3){
				ReferApp.Cu.MA.MainAngles.push_back(DDBB->GetAngle2DX_PI(0, Accuracy));
				ReferApp.Cu.MA.MainAngles.push_back(DDBB->GetAngle2DX_PI(1, Accuracy));
				ReferApp.Cu.MA.MainAngles.push_back(DDBB->GetAngle2DX_PI(2, Accuracy));
			}
			break;
		}

		case 2: //User defined three main axes
		{
			ReferApp.Cu.MA.MainAngles.clear();

			//Update main angles with user main axes 
			//that should have been selected by user while activating this case
			if ((long)ReferApp.Cu.MA.UserMainAxes.size() == 3){
				//Search for common vertex
				CEdge Arista0= DDBB->GetEdge(ReferApp.Cu.MA.UserMainAxes[0]);
				CEdge Arista1= DDBB->GetEdge(ReferApp.Cu.MA.UserMainAxes[1]);
				long Common;
				if(Arista0.Head == Arista1.Head || Arista0.Head == Arista1.Tail)
					Common= Arista0.Head;
				else
					Common= Arista0.Tail;

				//WARNING: If vertices have not been merged, no common vertex exists.
				//Still, using any arbitrary tip as common vertex does not result in failure
				//because main axes are later reordered (see ReorderMainAngles())!

				//Evaluate the angles of the selected edges
				for (long i= 0; i<3; i++){
					double Angle= DDBB->GetAngle2DX_2PI(ReferApp.Cu.MA.UserMainAxes[i], Common, Accuracy);
					ReferApp.Cu.MA.MainAngles.push_back(Angle);
				}
			}
			break;
		}
	} // End switch
}


void CCueMainAxes::ReorderMainAngles()
//Reorder three main angles

//Three axonometric axes may be ordered in up to eigth different configurations
//Here we choose the configuration closest to the isometric:
//		edge Z is the closest to vertical (90)
//		edge Y is the closest to -30
//		edge X is the closest to 210

//In the particular case of cabinet, where one of the non-Z angles is 0 or 180, it is labelled as:
//		Y if the other angle is in the range (180, 225]
//		X if the other angle is in the range (0,-45]
{

	//Ensure that input angles are in the range [0, 180]
	for (long i=0; i<3; i++){
		while (ReferApp.Cu.MA.MainAngles[i] > NUMBER_PI) ReferApp.Cu.MA.MainAngles[i] -= NUMBER_PI;
	}

	//Angle closer to 90 (vertical) is labelled as Z
	long AxisZ= 0;
	for (long i=1; i<3; i++){
		if (std::fabs(ReferApp.Cu.MA.MainAngles[i]-(NUMBER_PI/2)) <
			std::fabs(ReferApp.Cu.MA.MainAngles[AxisZ]-(NUMBER_PI/2))){
			AxisZ= i;
		}
	}

	if(AxisZ == 0){
		std::swap(ReferApp.Cu.MA.MainAngles[0], ReferApp.Cu.MA.MainAngles[2]);
	}
	else if(AxisZ == 1){
		std::swap(ReferApp.Cu.MA.MainAngles[1], ReferApp.Cu.MA.MainAngles[2]);
	}

	//Set the non-Z angles in the configuration closest to isometric (or Cabinet)
	if(ReferApp.Cu.MA.MainAngles[0]!=0. && ReferApp.Cu.MA.MainAngles[1]!=0.){
		ReferApp.Cu.MA.MainAngles[0] += NUMBER_PI;
		ReferApp.Cu.MA.MainAngles[1] += NUMBER_PI;

		//Biggest angle is labelled as Y,
		//while the other remains as X
		if (ReferApp.Cu.MA.MainAngles[0] > ReferApp.Cu.MA.MainAngles[1]){
			std::swap(ReferApp.Cu.MA.MainAngles[0], ReferApp.Cu.MA.MainAngles[1]);
		}
	}
	else if(ReferApp.Cu.MA.MainAngles[1] == 0.){
		if(ReferApp.Cu.MA.MainAngles[0]>NUMBER_PI/2){
			double OldAngle1= ReferApp.Cu.MA.MainAngles[1];
			ReferApp.Cu.MA.MainAngles[1]= ReferApp.Cu.MA.MainAngles[0]+NUMBER_PI;
			ReferApp.Cu.MA.MainAngles[0]= OldAngle1+NUMBER_PI;
		}
		else{
			ReferApp.Cu.MA.MainAngles[0] += NUMBER_PI;
		}
	}
	else if(ReferApp.Cu.MA.MainAngles[0] == 0.){
		if(ReferApp.Cu.MA.MainAngles[1]>NUMBER_PI/2){
			double OldAngle0= ReferApp.Cu.MA.MainAngles[0];
			ReferApp.Cu.MA.MainAngles[0]= ReferApp.Cu.MA.MainAngles[1]+NUMBER_PI;
			ReferApp.Cu.MA.MainAngles[1]= OldAngle0+NUMBER_PI;
		}
		else{
			ReferApp.Cu.MA.MainAngles[1] += NUMBER_PI;
			std::swap(ReferApp.Cu.MA.MainAngles[0], ReferApp.Cu.MA.MainAngles[1]);
		}
	}
}


bool CCueMainAxes::GetTransformationMatrix(std::vector<std::vector<double>> &M)
//Obtains the matrix M that transforms from main axes to inflation axes.

//(Its reverse matrix transforms from inflation axes to main axes)

//Based on:
//	Company, P., Conesa, J and Aleixos, N. 
//	Axonometric Inflation in Line Drawings Reconstruction.
//	Regeo Technical Report 01/2001.
//	URL: http://www.regeo.uji.es/technicalreports.htm

//In turn, based on:
//	Gomis J.M. y Company P.
//	La proyección en el proceso de representación y en los sistemas de representación
//	Anales de Ingeniería Gráfica (ISSN 1137-7704). Vol. 4, num. 1. 1995. pp. 20-31
//	(In spanish)

//In turn, based on:
//	Sellarés J.A.
//	Fundamentos de los gráficos con ordenador
//	EDUNSA. Colección informática básica. Barcelona 1988. ISBN 84-7747-012-X
//	(In spanish)

{
	//Initializations
	//---------------
	//Assign default matrix
	M.assign(3, std::vector<double>());
	for(long i= 0; i<3; i++){
		M[i].assign(3, 0.);
	}
	M[0][0]= 1; M[1][1]= 1; M[2][2]= 1;

	//Check that main axes must have been previously obtained
	if(ReferApp.Cu.MA.MainAngles.size() != 3){
		AfxMessageBox("Non-trivial Transformation Matrix requires calculating the main axes in advance");
		return false;
	}

	//Assign transformation matrix
	//----------------------------
	if(ReferApp.Cu.MA.obliqueAngle == -1){
		//Orthogonal projection
		double theta= NUMBER_PI/2 - acos(sqrt(1/tan(ReferApp.Cu.MA.AngleXYZ[1])*1/tan(ReferApp.Cu.MA.AngleXYZ[0])));
		double phi= acos(sqrt(1/tan(ReferApp.Cu.MA.AngleXYZ[1])*1/tan(ReferApp.Cu.MA.AngleXYZ[2])) / cos(theta));

		M[0][0]= -sin(phi);
		M[0][1]=  cos(phi);
		M[0][2]=  0;

		M[1][0]= -cos(phi) * sin(theta);
		M[1][1]= -sin(phi) * sin(theta);
		M[1][2]= cos(theta);

		M[2][0]= cos(phi) * cos(theta);
		M[2][1]= sin(phi) * cos(theta);
		M[2][2]= sin(theta);
	}

	else{
		//Oblique projection
		if(ReferApp.Cu.MA.obliqueAngle == 0){
			M[0][0]= 0;
			M[0][1]= 1;
			M[0][2]= 0;

			M[1][0]= 0;
			M[1][1]= 0;
			M[1][2]= 1;

			M[2][0]= 1;
			M[2][1]= 0;
			M[2][2]= 0;
		}
		else if(ReferApp.Cu.MA.obliqueAngle == 1){
			M[0][0]=-1;
			M[0][1]= 0;
			M[0][2]= 0;

			M[1][0]= 0;
			M[1][1]= 0;
			M[1][2]= 1;

			M[2][0]= 0;
			M[2][1]= 1;
			M[2][2]= 0;
		}
		else if(ReferApp.Cu.MA.obliqueAngle == 2){
			M[0][0]= cos(ReferApp.Cu.MA.AngleXYZ[0]);
			M[0][1]= sin(ReferApp.Cu.MA.AngleXYZ[0]);
			M[0][2]= 0;

			M[1][0]=-sin(ReferApp.Cu.MA.AngleXYZ[0]);
			M[1][1]= cos(ReferApp.Cu.MA.AngleXYZ[0]);
			M[1][2]= 0;

			M[2][0]= 0;
			M[2][1]= 0;
			M[2][2]= 1;
		}
	}

	return true;
}


bool CCueMainAxes::GetAxonometricParams(CDB_Model *DDBB)
//Calculates the INVERSE of the axonometric coefficients, 
//assuming that the view is an orthogonal projection.

//Alternatively, detects oblique projections, and reformulates their coefficients accordingly.
//(Note that if a cabinet/cavalier/military projection is detected,
//their coefficients are arbitrarily assigned)

//Calculates as well angles between consecutive axes.

//Based on:
//	Gomis J.M. y Company P.
//	La proyección en el proceso de representación y en los sistemas de representación
//	Anales de Ingeniería Gráfica (ISSN 1137-7704). Vol. 4, num. 1. 1995. pp. 20-31
//	(In spanish)

//	Company, P., Conesa, J and Aleixos, N. 
//	Axonometric Inflation in Line Drawings Reconstruction.
//	Regeo Technical Report 01/2001.
//	URL: http://www.regeo.uji.es/technicalreports.htm
{
	//Default
	ReferApp.Cu.MA.CoefE.clear();
	ReferApp.Cu.MA.CoefE.assign (3,0);

	//Calculate angles between consecutive main axes
	//----------------------------------------------
	//(AngleXYZ[i] will contain the angle between the axes other than i-th)
	ReferApp.Cu.MA.AngleXYZ.clear();
	for(long i= 0; i<3; i++){
		double angleII= ReferApp.Cu.MA.MainAngles[(i+2) % 3];
		double angleI= ReferApp.Cu.MA.MainAngles[(i+1) % 3];
		if (angleII < angleI) angleII+= 2*NUMBER_PI;
		ReferApp.Cu.MA.AngleXYZ.push_back(angleII - angleI);
	}

	//Detect cabinet, and assign oblique coefficients
	//-----------------------------------------------
	ReferApp.Cu.MA.obliqueAngle= -1;
	for(long i= 0; i<3; i++){
		if(std::fabs(ReferApp.Cu.MA.AngleXYZ[i]-NUMBER_PI/2) < ReferApp.DIA.CabinetThreshold){
			//Assign (inverse) Cabinet coefficients
			ReferApp.Cu.MA.CoefE[i]= 1/ReferApp.DIA.CabinetReduction;
			ReferApp.Cu.MA.CoefE[(i+1) % 3]= 1;
			ReferApp.Cu.MA.CoefE[(i+2) % 3]= 1;

			//Activate obliqueness flag
			ReferApp.Cu.MA.obliqueAngle= i;
			break;
		}
	}

	//Loop to get the three orthogonal axonometric coefficients
	//---------------------------------------------------------
	if(ReferApp.Cu.MA.obliqueAngle == -1){
		for(long i= 0; i<3; i++){
			double Alfa= ReferApp.Cu.MA.AngleXYZ[(i+2) % 3];
			double Beta= ReferApp.Cu.MA.AngleXYZ[(i+1) % 3];

			//Get the axonometric coefficient of the ith axis
			if (std::fabs(Alfa+Beta-NUMBER_PI)>Accuracy && 
				std::fabs(Alfa-NUMBER_PI)>Accuracy && 
				std::fabs(Beta-NUMBER_PI)>Accuracy){
				//Get the angle Phi between the Central edge and its projection
				double CotangentA=1/tan(Alfa);
				//double CotangentA= cos(Alfa)/sin(Alfa);   //Undefined for Alfa= 0 or PI
				double CotangentB=1/tan(Beta);
				double root= sqrt(std::fabs(CotangentA*CotangentB));
				if(root > 1){
					AfxMessageBox("Unable to find all the axonometric coefficients");
					return false;
				}
				double Fi= asin(root);

				ReferApp.Cu.MA.CoefE[i]= cos(Fi);
				assert(ReferApp.Cu.MA.CoefE[i] != 0);

				//Get the inverse of the axonometric coefficient
				ReferApp.Cu.MA.CoefE[i]= 1/ReferApp.Cu.MA.CoefE[i];
			}
			else{
				AfxMessageBox("Unable to find all the axonometric coefficients");
				return false;
			}
		}
	}

	//Transformation matrix
	//---------------------
	if (!GetTransformationMatrix(ReferApp.Cu.MA.M))
		return false;

	return true;
}


void CCueMainAxes::GetMainAxes(CDB_Model *DDBB)
//Calculates the orientations of the main axes of the model

//Three best angles are first calculated,
//by combining different selection criteria

//Angles are calculated in the range [0, 2*PI], 
//where 0 is horizontal and extending to the right.
//(This impiles that they are calculated relative to the Drawing reference system)

//Then ,the three axes are reordered as follows:
//		We choose the configuration closest to the isometric:
//		edge Z is the closest to vertical (90)
//		edge Y is the closest to -30
//		edge X is the closest to 210

//		In the particular case of cabinet, where one of the non-Z angles is 0 or 180, it is labelled as:
//		Y if the other angle is in the range (180, 225]
//		X if the other angle is in the range (0,-45]


//The angles are stored in the global vector "MainAngles"
{
	if (DDBB->GetSizeEdges() < 3){
		//AfxMessageBox("Main axes not found, as there are not enough edges in the database");
		return;
	}

	//Find main angles depending on the current approach
	GetBestAngles(DDBB);

	//Reorder main axes
	//-----------------
	if(ReferApp.Cu.MA.MainAngles.size() != 3){
		AfxMessageBox("Main axes not found");
		ReferApp.Cu.MA.MainAngles.clear();  //Clear to prevent visualization failures (see OnUpdateDisplayMainAxes)or other improper use
	}
	else{
		ReorderMainAngles();
	}

	//Calculate axonometric parameters
	//--------------------------------
	if(ReferApp.Cu.MA.MainAngles.size() == 3 && !GetAxonometricParams(DDBB)){
		AfxMessageBox ("Cue Main Axes failed in finding axonometric parameters");
	}
}


void CCueMainAxes::TransformToMainAxes(CDB_Model *DDBB, bool Translate)
//Transforms the model in the database from the inflation 3D model to main axes:
//	The DDBB input is oriented according to the "inflation" reference system
//	The DDBB_MA output is oriented according to the main directions
{
	//If no transformation is available, output database is made equal to the input database
	if(ReferApp.Cu.MA.MainAngles.empty())
		return;

	//Transform coordinates
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		CVertex Vertice= DDBB->GetVertex(i);
		CVertex NVertice= Vertice;
		NVertice.x = Vertice.x*ReferApp.Cu.MA.M[0][0] + Vertice.y*ReferApp.Cu.MA.M[1][0] + Vertice.z*ReferApp.Cu.MA.M[2][0];
		NVertice.y = Vertice.x*ReferApp.Cu.MA.M[0][1] + Vertice.y*ReferApp.Cu.MA.M[1][1] + Vertice.z*ReferApp.Cu.MA.M[2][1];
		NVertice.z = Vertice.x*ReferApp.Cu.MA.M[0][2] + Vertice.y*ReferApp.Cu.MA.M[1][2] + Vertice.z*ReferApp.Cu.MA.M[2][2];
		DDBB->SetVertex(i, NVertice);
	}

	//Transform face normals
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CFace currentFace= DDBB->GetFace(i);
		CVector Normal= currentFace.GetNormal(false);
		//Recalculate Normal
		CVector NNormal;
		NNormal.x = Normal.x*ReferApp.Cu.MA.M[0][0] + Normal.y*ReferApp.Cu.MA.M[1][0] + Normal.z*ReferApp.Cu.MA.M[2][0];
		NNormal.y = Normal.x*ReferApp.Cu.MA.M[0][1] + Normal.y*ReferApp.Cu.MA.M[1][1] + Normal.z*ReferApp.Cu.MA.M[2][1];
		NNormal.z = Normal.x*ReferApp.Cu.MA.M[0][2] + Normal.y*ReferApp.Cu.MA.M[1][2] + Normal.z*ReferApp.Cu.MA.M[2][2];

		//Recalculate triangle normals
		std::vector<CVector> Normals= currentFace.GetFaceTrianglesNormals();
		std::vector<CVector> NNormals;
		for (long ii=0; ii < (long)Normals.size(); ii++) {
			NNormal.x = Normals[ii].x*ReferApp.Cu.MA.M[0][0] + Normals[ii].y*ReferApp.Cu.MA.M[1][0] + Normals[ii].z*ReferApp.Cu.MA.M[2][0];
			NNormal.y = Normals[ii].x*ReferApp.Cu.MA.M[0][1] + Normals[ii].y*ReferApp.Cu.MA.M[1][1] + Normals[ii].z*ReferApp.Cu.MA.M[2][1];
			NNormal.z = Normals[ii].x*ReferApp.Cu.MA.M[0][2] + Normals[ii].y*ReferApp.Cu.MA.M[1][2] + Normals[ii].z*ReferApp.Cu.MA.M[2][2];

			NNormals.push_back(NNormal);
		}

		//Save Normals
		currentFace.SetNormal(NNormal);
		currentFace.SetFaceTrianglesNormals(NNormals);
		DDBB->SetFace(i, currentFace);
	}

	//Displacement to move the closer vertex up to the origin of coordinates
	if(Translate){
		double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
		DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);

		for (long i=0; i<DDBB->GetSizeVertices(); i++){
			CVertex Vertice= DDBB->GetVertex(i);
			Vertice.x-=dXmin;
			Vertice.y-=dYmin;
			Vertice.z-=dZmin;
			DDBB->SetVertex(i, Vertice);
		}
	}
}


void CCueMainAxes::TransformFromMainAxes(CDB_Model *DDBB, bool Translate)
//Transforms the model in the database from global axes model back to inflation 3D model:
//	The DDBB_MA input is oriented according to the main directions
//	The DDBB output is oriented according to the "inflation" reference system
{
	//If no transformation is available, output database is made equal to the input database
	if(ReferApp.Cu.MA.MainAngles.empty())
		return;

	//Transform coordinates
	for (long i=0; i<DDBB->GetSizeVertices(); i++){
		CVertex Vertice= DDBB->GetVertex(i);
		CVertex NVertice= Vertice;
		NVertice.x = Vertice.x*ReferApp.Cu.MA.M[0][0] + Vertice.y*ReferApp.Cu.MA.M[0][1] + Vertice.z*ReferApp.Cu.MA.M[0][2];
		NVertice.y = Vertice.x*ReferApp.Cu.MA.M[1][0] + Vertice.y*ReferApp.Cu.MA.M[1][1] + Vertice.z*ReferApp.Cu.MA.M[1][2];
		NVertice.z = Vertice.x*ReferApp.Cu.MA.M[2][0] + Vertice.y*ReferApp.Cu.MA.M[2][1] + Vertice.z*ReferApp.Cu.MA.M[2][2];
		DDBB->SetVertex(i, NVertice);
	}

	//Transform face normals
	for (long i=0; i<DDBB->GetSizeFaces(); i++){
		CFace currentFace= DDBB->GetFace(i);
		CVector Normal= currentFace.GetNormal(false);
		//Recalculate Normal
		CVector NNormal;
		NNormal.x = Normal.x*ReferApp.Cu.MA.M[0][0] + Normal.y*ReferApp.Cu.MA.M[0][1] + Normal.z*ReferApp.Cu.MA.M[0][2];
		NNormal.y = Normal.x*ReferApp.Cu.MA.M[1][0] + Normal.y*ReferApp.Cu.MA.M[1][1] + Normal.z*ReferApp.Cu.MA.M[1][2];
		NNormal.z = Normal.x*ReferApp.Cu.MA.M[2][0] + Normal.y*ReferApp.Cu.MA.M[2][1] + Normal.z*ReferApp.Cu.MA.M[2][2];

		//Recalculate triangle normals
		std::vector<CVector> Normals= currentFace.GetFaceTrianglesNormals();
		std::vector<CVector> NNormals;
		for (long ii=0; ii < (long)Normals.size(); ii++) {
			NNormal.x = Normals[ii].x*ReferApp.Cu.MA.M[0][0] + Normals[ii].y*ReferApp.Cu.MA.M[0][1] + Normals[ii].z*ReferApp.Cu.MA.M[0][2];
			NNormal.y = Normals[ii].x*ReferApp.Cu.MA.M[1][0] + Normals[ii].y*ReferApp.Cu.MA.M[1][1] + Normals[ii].z*ReferApp.Cu.MA.M[1][2];
			NNormal.z = Normals[ii].x*ReferApp.Cu.MA.M[2][0] + Normals[ii].y*ReferApp.Cu.MA.M[2][1] + Normals[ii].z*ReferApp.Cu.MA.M[2][2];

			NNormals.push_back(NNormal);
		}

		//Save Normals
		currentFace.SetNormal(NNormal);
		currentFace.SetFaceTrianglesNormals(NNormals);
		DDBB->SetFace(i, currentFace);
	}

	//Displacement to move the closer vertex up to the origin of coordinates
	if(Translate){
		double dXmin, dYmin, dZmin, dXmax, dYmax, dZmax;
		DDBB->GetBoundingBox(dXmin, dYmin, dZmin, dXmax, dYmax, dZmax);

		for (long i=0; i<DDBB->GetSizeVertices(); i++){
			CVertex Vertice= DDBB->GetVertex(i);
			Vertice.x-=dXmin;
			Vertice.y-=dYmin;
			Vertice.z-=dZmin;
			DDBB->SetVertex(i, Vertice);
		}
	}
}
