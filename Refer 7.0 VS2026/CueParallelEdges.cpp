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

#include "Tools_Geometry.h"  //DEGREES_TO_RADIANS, MODULE_2D, NUMBRE_PI

#include "DB_Model.h"  //Input vertices and edges
#include "Refer.h"     //Configuration parameters (ReferApp)

#include "CueVanishingPoints.h"
#include "CueParallelEdges.h"


bool CCueParallelEdges::InputModel(CDB_Model *DDBB)
//The input required by the algorithms is:
//		VertexCount= number of vertex in the line drawing
//		Vertex[i].x= X coordinate of the i-th vertex
//		Vertex[i].y= Y coordinate of the i-th vertex

//		EdgeCount= number of edges in the line drawing
//		EdgeHead[i]= Head vertex defining the i-th edge
//		EdgeTail[i]= Tail vertex defining the i-th edge
{
	//Copy external vertices into local database
	Vertex.clear();
	VertexCount= DDBB->GetSizeVertices();
	for (long i=0; i<VertexCount; i++){
		CVertex Vertice= DDBB->GetVertex(i);
		POINT2D p(Vertice.x, Vertice.y);
		Vertex.push_back(p);
	}

	//Copy external edges into local database
	EdgeHead.clear();
	EdgeTail.clear();
	EdgeCount= DDBB->GetSizeEdges();
	for (long i=0; i<EdgeCount; i++){
		CEdge Arista= DDBB->GetEdge(i);
		EdgeHead.push_back(Arista.Head);
		EdgeTail.push_back(Arista.Tail);
	}

	return (VertexCount>0 && EdgeCount>0);
}


double CCueParallelEdges::GetAngle_PI(long NumVertex1, long NumVertex2)
//Gets the angle (in radians) between the segment defined by
//vertices "NumVertex1" and "NumVertex2" and the X axis

//Since the orientation of the edge is not considered,
//returns a value in the range [0, PI)
{
	double angle= atan2(Vertex[NumVertex2].y-Vertex[NumVertex1].y,
						Vertex[NumVertex2].x-Vertex[NumVertex1].x);
	if(angle<0) angle+= NUMBER_PI;

	if(angle==NUMBER_PI) angle= 0;

	return angle;
}


void CCueParallelEdges::GetAngularDistribution(std::vector<double> &Angles)

//WARNING a similar function can be found in DB_Model!!
//(Although this version does not require vertices having been merged in advance)


//Calculates the angular distribution of the model
//	Main angles are stored in the vector "Angles"
//	Their frequencies are stored in the vector "Frequencies"

//Calculates the histogram by calculating angles and their frequencies
//Angles are sampled at intervals "delta"

//Then, main angles are obtained
//	When an angle is added to the graph, a Gaussian distribution curve
//	with a mean value equal to the ange and with standard deviation
//	equal to sigma (defaul 7 degrees) is superpositioned onto the graph
//	to account for the inaccuracy of the sketch

//	After all the curves have been superpositioned and combined
//	with their different weights, the graph is normalized with its maximum at 1.0.
//	The resulting ADG qualitatively shows the prevailing angles as humps

//	Crests of the humps must have a frequency greater than a thereshold (MinCrest)

//Based on:
//	H Lipson, M Shpitalni
//	Optimization-based reconstruction of a 3D object
//	from a single freehand line drawing
//	Computer-Aided Design, 28(8) 651-663, 1996
{
	//Adjustable parameters
	//---------------------
	//Interval of the histogram
	double delta= 1.0; //One interval per degree
	long Intervals= (long) (180/delta);

	//Minimum crest height considered
	double MinCrest= 0.25; //Crests smaller than 25% of maxADG are ignored

	//Standard deviation (used to group nearly parallel edges)
	double sigma= RADIANS_TO_DEGREES(ReferApp.Cu.PE.TolForParallel);

	//Histogram
	//---------
	std::vector<long> Histogram(Intervals, 0);

	//Loop over all the edges
	for (long i=0; i<EdgeCount; i++){
		double Angle= RADIANS_TO_DEGREES(GetAngle_PI(EdgeHead[i], EdgeTail[i]));
		long lAngle= (long)(Angle/delta);
		Histogram[lAngle] += 1;
	}

	//Angular Distribution Graph
	//--------------------------
	std::vector<double> ADG(Intervals, 0);
	double a= 1/(sigma*sqrt(2*NUMBER_PI));
	double sigma2= 2*sigma*sigma;

	for(long i= 0; i<Intervals; i++){
		if(Histogram[i]!=0){
			long jmin= (long)(i-3*sigma);
			long jmax= (long)(i+3*sigma);
			long N= (long)ADG.size();
			for(long j= jmin; j<jmax; j++){
				double b= (j-i)*(j-i)/sigma2;
				ADG[(N+j)%N] += (a * exp(-b)) * Histogram[i];
			}
		}
	}

	//Normalize ADG
	double maxADG= ADG[0];
	for(long i= 1; i<Intervals; i++){
		maxADG= max(maxADG, ADG[i]);
	}
	for(long i= 0; i<Intervals; i++){
		ADG[i]= ADG[i]/maxADG;
	}

	//Save the humps as main angles
	//-----------------------------
	Angles.clear();
	std::vector<double> Frequencies;
	//Frequencies.clear();

	//Save as zero those humps that are either close to zero or close to PI
	if(ADG[0]>MinCrest && (ADG[0]>=ADG[1] && ADG[0]>=ADG[Intervals-1]) ||
		ADG[Intervals-1]>MinCrest && (ADG[Intervals-1]>=ADG[Intervals-2] && ADG[Intervals-1]>=ADG[0])){
			Angles.push_back(0);
			Frequencies.push_back(max(ADG[0], ADG[Intervals-1]));
	}

	//Loop over the remainder humps
	for(long i= 1; i<Intervals-1; i++){
		if(ADG[i]>MinCrest && (ADG[i-1]<=ADG[i] && ADG[i]>=ADG[i+1])){
			Angles.push_back(DEGREES_TO_RADIANS(i*delta));
			Frequencies.push_back(ADG[i]);
			i+= 1;  //Jump to prevent consecutive false humps due to roundoffs
		}
	}
}


bool CCueParallelEdges::GetADGParallelEdges(std::vector<double> Angles)
//Returns groups of edges (ParallelEdges) aroung main angles (Angles) previously found by ADG

//Returns TRUE iff all the edges are grouped
{
	//ParallelEdges.clear();
	long numGroups= (long)Angles.size();
	long numParallel= 0;

	//Ensure that threshold is never greater than 
	//the minimum angle between consecutive main directions
	double Threshold= ReferApp.Cu.PE.TolForParallel;
	for (long k= 1; k<(long)Angles.size(); k++){
		Threshold= min(Threshold, (Angles[k]-Angles[k-1])/2.);
	}

	std::vector<double> anglesEdges;
	for (long j= 0; j<EdgeCount; j++){
		anglesEdges.push_back(GetAngle_PI(EdgeHead[j], EdgeTail[j]));
	}

	//Group edges aligned with main angles
	for (long i= 0; i<numGroups; i++){
		ParallelEdges.push_back(std::vector <long>());
		for (long j= 0; j<EdgeCount; j++){
			double delta= std::fabs(anglesEdges[j] - Angles[i]);
			if(delta < Threshold || std::fabs(delta - NUMBER_PI) < Threshold){
				ParallelEdges[i].push_back(j);
				numParallel++;
			}
		}
	}

	//Remove groups that include just one single line or none
	for (long i= numGroups-1; i>= 0; i--){
		if(ParallelEdges[i].size() <= 1){
			ParallelEdges.erase(ParallelEdges.begin()+i);
			numParallel--;
		}
	}

	return (numParallel == EdgeCount);
}


void CCueParallelEdges::GetAutomaticParallelEdges()
//Automatic approach for parallel edges detection

//Uses Angular distribution for normalon shapes depicted in parallel projection
//Uses Vanishing points otherwise

//Shape is normalon if:
//	* Three and only three prevailing angles exist
//	* All the edges fit in one of the three main directions

//Returns the groups of lines that are parallel to each other in "ParallelEdges"
{
	//Get angular distribution
	std::vector<double> Angles;
	GetAngularDistribution(Angles);

	if(Angles.size() == 3 && GetADGParallelEdges(Angles)){
		//Normalon object represented in parallel projection
	}
	else{
		//Find vanishing points and group edges
		CCueVanishingPoints VP(VertexCount, Vertex,
							   EdgeCount, EdgeHead, EdgeTail);
		ParallelEdges= VP.GetVanishingPoints();
	}
}


std::vector <std::vector <long>> CCueParallelEdges::GetParallelEdges(CDB_Model *DDBB)
//Parallel edges detection

//Returns the lines of the drawing grouped in bundles of lines
//that are supossed to represent edges parallel to each other in the 3D model
{
	ParallelEdges.clear();

	if(!InputModel(DDBB))
		return ParallelEdges;

	switch (ReferApp.Cu.PE.ParallelEdgesApproach){
		case 0:{  //Automatic
			GetAutomaticParallelEdges();
		}
		break;

		case 1:{  //Vanishing points
			CCueVanishingPoints VP(VertexCount, Vertex,
								   EdgeCount, EdgeHead, EdgeTail);
			ParallelEdges= VP.GetVanishingPoints();
		}
		break;

		case 2:{  //Angular distribution
			//Get angular distribution
			std::vector<double> Angles;
			GetAngularDistribution(Angles);

			//Get groups of edges
			GetADGParallelEdges(Angles);
		}
		break;
	}//end switch

	return ParallelEdges;
}
//
//
//CCueParallelEdges::CCueParallelEdges(int IVertexCount,
//									 std::vector <POINT2D> IVertex,
//									 int IEdgeCount,
//									 std::vector <int> IEdgeHead,
//									 std::vector <int> IEdgeTail)
////Constructor that updates local database with the drawing to analyse
//
////The input required by the algorithms is:
////		VertexCount= number of vertex in the line drawing
////		Vertex[i].x= X coordinate of the i-th vertex
////		Vertex[i].y= Y coordinate of the i-th vertex
//
////		EdgeCount= number of edges in the line drawing
////		EdgeHead[i]= Head vertex defining the i-th edge
////		EdgeTail[i]= Tail vertex defining the i-th edge
//
//:
//VertexCount (IVertexCount),
//Vertex (IVertex),
//EdgeCount (IEdgeCount),
//EdgeHead (IEdgeHead),
//EdgeTail (IEdgeTail)
//{
//}
