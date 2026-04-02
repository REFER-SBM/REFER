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

#include "Tools_Vector.h"
#include "Tools_Geometry.h"  //DEGREES_TO_RADIANS, MODULE_2D, NUMBRE_PI, GetDistancePointToLine, NearlyParallel

////Used for input params and deprecated acces to the class/////
#include "DB_Model.h"  //Input vertices and edges
#include "Refer.h"     //Configuration parameters (ReferApp)
////////////////////////////////////////////////////////////////

#include "CueVanishingPoints.h"


void CCueVanishingPoints::GetDrawingCentroid()
//Calculates the centroid of the drawing
{
	DrawingCentroid.x= 0;
	DrawingCentroid.y= 0;
	for (long i=0; i<VertexCount; i++){
		DrawingCentroid += Vertex[i];
	}
	DrawingCentroid.x= DrawingCentroid.x/VertexCount;
	DrawingCentroid.y= DrawingCentroid.y/VertexCount;
}


double CCueVanishingPoints::GetBoundingRadius()
//Calculates the radius of the circle that bounds the drawing
{
	double BoundingRadius= MODULE_2D(Vertex[0], DrawingCentroid);

	for (long i=1; i<VertexCount; i++){
		BoundingRadius= max(BoundingRadius, MODULE_2D(Vertex[i], DrawingCentroid));
	}

	return BoundingRadius;
}


void CCueVanishingPoints::InitParams()
//Initializes some parameters

//This function should be called before calculating Vanishing Points by GetVanishingPoints
{
	GetDrawingCentroid();
	BoundingRadius= GetBoundingRadius();

	//Initialize parameters
	//---------------------

	//Percentage expressed in decimal form of how much
	//to increase the bounding ring to get the inner ring
	InnerRingRadius= ReferApp.Cu.VP.InRing * BoundingRadius;  //P1

	//Percentage expressed in decimal form of how much
	//to increase the bounding ring to get the outer ring
	OuterRingRadius= ReferApp.Cu.VP.OutRing * BoundingRadius;  //P2

	//Maximum angle to cluster together two candidate VP
	CuttingDistance= ReferApp.Cu.VP.ClusteringAngle;  //P3

	//Threshold to consider a group of VPs parallel
	//(contained into a slender band)
	ToleranceDispersion= ReferApp.Cu.VP.ToleranceDispersion * BoundingRadius;  //P4

	//Radius of the tolerance circle used to accept as coincident
	//those endpoints that happen to be inside a tolerance circle from each other
	ToleranceCoincident= ReferApp.Cu.VP.ToleranceCoincident;  //P5

	//Threshold to consider two edges as "nearly" parallel
	//(hence, collinear, if they are connected)
	ToleranceParallel= ReferApp.Cu.VP.ToleranceParallel;  //P6
}


bool CCueVanishingPoints::IntersectOutside(POINT2D a1, POINT2D b1,
										   POINT2D a2, POINT2D b2,
										   POINT2D &Int)
//Finds the intersection of two segments
//Input: segment 1 endpoints (a1.x,a1.y) (b1.x,b1.y)
//       segment 2 endpoints (a2.x,a2.y) (b2.x,b2.y)
//Output: false if lines are parallel or intersection is not external to both segments
//        true if intersection point (IntX, IntY) exists outside
{
	double s1_x = b1.x - a1.x;
	double s1_y = b1.y - a1.y;
	double s2_x = b2.x - a2.x;
	double s2_y = b2.y - a2.y;
	double denom= (-s2_x * s1_y + s1_x * s2_y);

	if(denom == 0)
		//Edges are parallel
		return false;

	double s = (-s1_y * (a1.x - a2.x) + s1_x * (a1.y - a2.y)) / denom;
	double t = ( s2_x * (a1.y - a2.y) - s2_y * (a1.x - a2.x)) / denom;

	if ((s >= 0 && s <= 1) || (t >= 0 && t <= 1)){
		//Intersection point is internal to, at least, one of the segments
		return false;
	}

	//External intersection
	Int.x = a1.x + (t * s1_x);
	Int.y = a1.y + (t * s1_y);
	return true;
}


bool CCueVanishingPoints::IsInRing(POINT2D p)
//Returns true if point (p.x, p.y) is inside the ring
//where vanishing points are expected to be

//Warning! Only inner radius is considered,
//thus allowing the algorithm to cope with nearly parallel edges as well
{
	p-= DrawingCentroid;
	double r= sqrt(p.x*p.x + p.y*p.y);

	//return (r>InnerRingRadius && r<OuterRingRadius)
	return(r>InnerRingRadius);
}


void CCueVanishingPoints::GetCandidateVP()
//Calculates candidate Vanishing Points

//Candidate VP are points of intersection of every pair or edges, provided that:
//     *The pair of edges is not parallel
//     *The intersection point is external to both edges
//      (i.e. it is in the extension of those segments)
//     *The intersection point is inside the ring
//      where vanishing points are expected to be
{
	CandidateVP.clear();
	long NumCandidateVP= 0;

	for (long i=0; i<EdgeCount; i++){
		for (long j=i+1; j<EdgeCount; j++){
			POINT2D Intersection;
			if(IntersectOutside(Vertex[EdgeHead[i]], Vertex[EdgeTail[i]],
								Vertex[EdgeHead[j]], Vertex[EdgeTail[j]],
								Intersection)){

				if(IsInRing(Intersection)){
					//Add the candidate vanishing point
					CandidateVP.push_back(Intersection);

					//Add the edges converging into the vanishing point
					EdgesInVP.push_back(std::vector <long>());
					EdgesInVP[NumCandidateVP].push_back(i);
					EdgesInVP[NumCandidateVP].push_back(j);

					NumCandidateVP ++;
				}
			}
		}
	}
}


POINT2D CCueVanishingPoints::GetClusterCentroid(long numCluster)
//Calculates the centroid of a cluster of candidate vanishing points
//as the mean point of the cluster

//Outliers are not detected
{
	POINT2D vp (0, 0);
	long n= Cluster[numCluster].size();
	for (long i= 0; i<n; i++){
		vp += CandidateVP[Cluster[numCluster][i]];
	}
	vp.x/= n;
	vp.y/= n;

	return vp;
}

double CCueVanishingPoints::GetOrientation(POINT2D p)
//Returns the angle of the point (p.x, p.y)
//relative to the polar coordinate system
//whose origin is in the centroid of the drawing
//(Angles are measured from the horizontal, in the range [0, 2PI])
{
	//return GetAngle_2PI(DrawingCentroid, p);

	if(p.x==0 && p.y==0)
		return 0;
	else{
		p-= DrawingCentroid; //Translate the origin
		double angle= atan2(p.y, p.x);  //range [-PI, PI]
		if(angle<0) angle+= 2*NUMBER_PI;  //range [0, 2PI]
		return angle;
	}
}


void CCueVanishingPoints::ReorderClustersByOrientation(std::vector<double> &Orientation)
//Reorder clusters from minor to major orientation angle
{
	std::vector <std::vector <long>> NewCluster;
	std::vector <double> NewOrientation;
	long SizeNewCluster= 0;

	while(SizeNewCluster < NumGroups){
		double MinAngle= Orientation[0];
		long Position= 0;
		for(long i= 1; i<(long)Cluster.size(); i++){
			if(Orientation[i] < MinAngle){
				MinAngle= Orientation[i];
				Position= i;
			}
		}
		NewCluster.push_back(Cluster[Position]);
		NewOrientation.push_back(Orientation[Position]);
		SizeNewCluster++;
		Cluster.erase(Cluster.begin()+Position);
		Orientation.erase(Orientation.begin()+Position);
	}
	Cluster= NewCluster;
	Orientation= NewOrientation;
}


double CCueVanishingPoints::GetAngleDifference(double Angle1, double Angle2)
//Returns the difference between Angle1 and Angle 2

//Both angles are in the range [0-2*NUMBER_PI]

//(The algorithm considers the discontinuity between eighth and first octants)
{
	if(Angle1<NUMBER_PI/2 && Angle2>3*NUMBER_PI/2)
		return (2*NUMBER_PI - Angle2 + Angle1);

	else if(Angle1>3*NUMBER_PI/2 && Angle2<NUMBER_PI/2)
		return (2*NUMBER_PI - Angle1 + Angle2);

	else
		return std::fabs(Angle1 - Angle2);
}


void CCueVanishingPoints::GetClusters()
//Sequentially groups the closest neighbor clusters
//until the distance becomes higher that CuttingDistance
{
	//Initially each candidate VP is its own cluster
	//----------------------------------------------
	NumGroups= CandidateVP.size();

	Cluster.clear();
	std::vector<double> Orientation;
	for(long i= 0; i<NumGroups; i++){
		Cluster.push_back(std::vector<long>());
		Cluster[i].push_back(i);
		Orientation.push_back(GetOrientation(GetClusterCentroid(i)));
	}

	//Reorder initial clusters by growing orientation
	//-----------------------------------------------
	ReorderClustersByOrientation(Orientation);

	//Loop to group similar clusters
	//------------------------------
	while(NumGroups > 1){
		//Search the closest pair of neighbor clusters
		double MinGap= GetAngleDifference(Orientation[1], Orientation[0]);
		long Position= 0;
		long N= (long)Cluster.size();
		for(long i= 1; i<N; i++){
			double CurrentGap= GetAngleDifference(Orientation[(i+1)% N], Orientation[i]);
			if(CurrentGap < MinGap){
				MinGap= CurrentGap;
				Position= i;
			}
		}

		//Finish if gap is greater than CuttingDistance
		if(MinGap > CuttingDistance)
			break;

		//Merge the closest clusters
		long Position1= (Position+1)%N;
		for (long j= 0; j<(long)Cluster[Position1].size(); j++){
			Cluster[Position].push_back(Cluster[Position1][j]);
		}
		Orientation[Position]= GetOrientation(GetClusterCentroid(Position));

		Cluster.erase(Cluster.begin()+Position1);
		Orientation.erase(Orientation.begin()+Position1);
		NumGroups--;
	}
}


double CCueVanishingPoints::GetMedian(long numCluster, POINT2D vp)
//Calculates the median of the distances between
//candidate vanishing points of every cluster and their mean VP
{
	std::vector<double> distance;
	long n= Cluster[numCluster].size();

	for(long i= 0; i<n; i++){
		distance.push_back(MODULE_2D(CandidateVP[Cluster[numCluster][i]], vp));
	}
	distance= SortMinorFirst(distance);

	if(n%2==0){
		// n is a an even number
		return ((distance[n/2-1] + distance[n/2])/2.);
	}
	else{
		//n is a an odd number
		return distance[(n+1)/2 - 1];
	}
}


double CCueVanishingPoints::erfc(double x)
//WARNING: A library function should be used,
//but Visual C++ does not seem to include it

//An approximate calculation is used instead


//Returns the complementary error function for x (x>0)

//Uses a pure exponential approximation
//for the complementary error function
//given by Chiani, Dardari & Simon
//    Chiani, M.; Dardari, D.; Simon, M. K. (2003),
//    "New Exponential Bounds and Approximations for
//    the Computation of Error Probability in Fading Channels",
//    IEEE Transactions on Wireless Communications 4 (2): 840–845
{
	return (exp(-x*x)/6. + exp(-4/3*x*x)/2.);
}


void CCueVanishingPoints::GetMeanAndDeviation(long numCluster,
											  std::vector<bool> Outliers,
											  POINT2D &vp,
											  double &meanDist,
											  double &StandardDeviation)
//Returns the mean point, the mean distance and the standard deviation
//of points belonging to the cluster numCluster

//Calculations ignore those candidate VP that have been declared outliers
{
	long n= Cluster[numCluster].size();

	//Centroid without outliers
	long nn= 0;
	vp.x= 0.; vp.y= 0.;
	for (long i= 0; i<n; i++){
		if(!Outliers[i]){
			vp += CandidateVP[Cluster[numCluster][i]];
			nn++;
		}
	}
	vp.x/= nn; vp.y/= nn;

	//Mean distance without outliers
	meanDist= 0;
	for (long i= 0; i<n; i++){
		if(!Outliers[i]){
			meanDist+= MODULE_2D(CandidateVP[Cluster[numCluster][i]], vp);
		}
	}
	meanDist/= nn;

	//Standard deviation without outliers
	StandardDeviation= 0;
	for (long i= 0; i<n; i++){
		if(!Outliers[i]){
			double DistI = MODULE_2D(CandidateVP[Cluster[numCluster][i]], vp) - meanDist;
			StandardDeviation+= DistI * DistI;
		}
	}
	StandardDeviation= sqrt(StandardDeviation/nn);
}


POINT2D CCueVanishingPoints::GetRefinedCentroid(long numCluster)
//Calculates the centroid of a cluster of candidate vanishing points
//as the mean point of the cluster

//Outliers are detected and excluded to refine the location of the VP
//(Vanishing Points are considered outliers if they accomplish the Chauvenet’s Criterion)

//Outliers are searched only if:
//	. The group is longer than 2 candidate VP
//	. Standard deviation is low (as there is no need to refine the VP location for parallel groups)
//	. Mean is greater than median (as this suggests that some outliers must exist)

//Chauvent Criterion is used as described in:
//	Lin L, Sherman P.D.
//	Cleaning Data the Chauvenet Way.
//	The Proceedings of the SouthEast SAS Users Group,
//	SESUG Proceedings, Paper SA11 (2007)

//	Max number of iterations searching outliersis fixed as MAXITEROUTLIERS= 1
//	(as we only whant to remove the most distant outliers)
{
	long n= Cluster[numCluster].size();

	//Check if there are no outliers
	if(n<3)
		return GetClusterCentroid(numCluster);

	//Initial mean point and standard deviation
	std::vector<bool> Outliers(n, false);
	POINT2D vp(0,0);
	double meanDist= 0;
	double StandardDeviation= 0;

	GetMeanAndDeviation(numCluster, Outliers,
						vp, meanDist, StandardDeviation);

	if(StandardDeviation > ToleranceDispersion)
		return vp;

	if(meanDist <= GetMedian(numCluster, vp))
		return vp;

	//Filter outliers and recalculate mean VP
	//---------------------------------------
	long MAXITEROUTLIERS= 1;  //Max number of iterations searching outliers
	long nn= n;

	for(long k= 0; k<MAXITEROUTLIERS; k++){
		//Find outliers
		long NewOutliers= 0;
		for(long i= 0; i<n; i++){
			if(!Outliers[i]){
				double distI= MODULE_2D(CandidateVP[Cluster[numCluster][i]], vp);

				if(nn*erfc(std::fabs(distI-meanDist)/StandardDeviation) < 0.5){  //Chauvenet’s Criterion
					Outliers[i]= true;
					NewOutliers++;
				}
			}
		}

		if(NewOutliers == 0)
			break;

		//Mean point, mean distance and standard deviation without outliers
		GetMeanAndDeviation(numCluster, Outliers,
							vp, meanDist, StandardDeviation);

		nn-= NewOutliers;
	}

	return vp;
}


void CCueVanishingPoints::GetMeanVP()
//Uses the clusters of tentative vanishing points
//to find mean vanishing points

//Mean vanishing points are the centroids of the clusters
//Centroids are refined by not taking into acount outliers

//Most populated clusters are placed first,
//as they are considered the most likely candidate
{
	//Save the refined cluster centroids as mean Vanishing Points
	MeanVP.clear();
	for (long i= 0; i<NumGroups; i++){
		MeanVP.push_back(GetRefinedCentroid(i));
	}
}


void CCueVanishingPoints::GetGroupsOfEdges()
//Get groups of edges converging to every mean Vanishing Point

//Returns the set of edges linked to every meanVP
{
	ParallelEdges.clear();

	for (long i= 0; i<NumGroups; i++){
		std::vector<bool> Flags(EdgeCount, false);
		long n= Cluster[i].size();
		for (long j= 0; j<n; j++){
			Flags[EdgesInVP[Cluster[i][j]][0]]= true;
			Flags[EdgesInVP[Cluster[i][j]][1]]= true;
		}

		ParallelEdges.push_back(std::vector <long>());
		for (long j= 0; j<EdgeCount; j++){
			if(Flags[j])
				ParallelEdges[i].push_back(j);
		}
	}
}


void CCueVanishingPoints::ReorderClustersBySize()
//Reorder clusters by decreasing size

//Size of every cluster/group is measured as:
//		the number of candidate VP's
//		plus the number of edges
{
	//Create temporal new lists
	std::vector <std::vector <long>> NewCluster;
	std::vector<POINT2D> NewMeanVP;
	std::vector <std::vector <long>> NewParallelEdges;

	//Loop to reorder groups
	long SizeNewCluster= 0;

	while(SizeNewCluster < NumGroups){
		//Find the greater cluster
		long MaxSize= Cluster[0].size() + ParallelEdges[0].size();
		long Position= 0;
		for(long i= 1; i<(long)Cluster.size(); i++){
			long CurrentSize= Cluster[i].size() + ParallelEdges[i].size();
			if(CurrentSize > MaxSize){
				MaxSize= CurrentSize;
				Position= i;
			}
		}

		//Save the greater cluster in the new lists and delete it from old lists
		NewCluster.push_back(Cluster[Position]);
		Cluster.erase(Cluster.begin()+Position);

		NewMeanVP.push_back(MeanVP[Position]);
		MeanVP.erase(MeanVP.begin()+Position);

		NewParallelEdges.push_back(ParallelEdges[Position]);
		ParallelEdges.erase(ParallelEdges.begin()+Position);

		SizeNewCluster++;
	}

	//Update lists
	Cluster= NewCluster;
	MeanVP= NewMeanVP;
	ParallelEdges= NewParallelEdges;
}


bool CCueVanishingPoints::IntersectInside(POINT2D a1, POINT2D b1,
										  POINT2D a2, POINT2D b2)
//Returns true if the intersection of two edges
//is internal to both and they are not parallel
//False otherwise

//Input: edge 1 endpoints (a1.x,a1.y) (b1.x,b1.y)
//       edge 2 endpoints (a2.x,a2.y) (b2.x,b2.y)

//As a first criterion, the intersection is internal if the distance
//between the respective endpoints is smaller than the tolerance

//As a second criterion, the intersection is considered internal if
//the intersection point belongs to both extended segments

//If none of the two criteria is true, returns false

//If only one of the two criteria is true, then returns true if not parallel
//false otherwise
{
	//Check if endpoints are closer
	double threshold= min(MODULE2D(a1.x, a1.y, b1.x, b1.y),
						  MODULE2D(a2.x, a2.y, b2.x, b2.y)) * ToleranceCoincident;

	if (MODULE_2D(a1, a2)<threshold ||
		MODULE_2D(a1, b2)<threshold ||
		MODULE_2D(b1, a2)<threshold ||
		MODULE_2D(b1, b2)<threshold){
		return (NearlyParallel(a1, b1, a2, b2, ToleranceParallel) == 0);
	}

	//Check if intersection happens inside the extended edges
	double s1x = b1.x - a1.x;
	double s1y = b1.y - a1.y;
	double s2x = b2.x - a2.x;
	double s2y = b2.y - a2.y;
	double denom= (-s2x * s1y + s1x * s2y);

	if(denom == 0)
		//Edges are parallel
		return false;

	double s = (-s1y * (a1.x - a2.x) + s1x * (a1.y - a2.y)) / denom;
	double t = ( s2x * (a1.y - a2.y) - s2y * (a1.x - a2.x)) / denom;

	if ((s < -ToleranceCoincident || s > 1+ToleranceCoincident) || 
		(t < -ToleranceCoincident || t > 1+ToleranceCoincident)){
		//Intersection point is external to, at least, one of the segments
		return false;
	}

	//IntX = a1.x + (t * s1x);
	//IntY = a1.y + (t * s1y);

	return (NearlyParallel(a1, b1, a2, b2, ToleranceParallel) == 0);
}


bool CCueVanishingPoints::FindIntersectionsInGroups(long CurrentGroup,
													std::vector<long> &IntersectedEdges)
//Find intersections between edges in the group

//Returns true if at least one intersection is found
//Returns a list of edges of the group that intersect to each other
{
	std::vector<bool> Flags(EdgeCount, false);

	long n= (long)ParallelEdges[CurrentGroup].size();
	for (long i= 0; i<n; i++){
		long EdgeI= ParallelEdges[CurrentGroup][i];
		for (long j= i+1; j<n; j++){
			long EdgeJ= ParallelEdges[CurrentGroup][j];

			if(IntersectInside(Vertex[EdgeHead[EdgeI]], Vertex[EdgeTail[EdgeI]],
							   Vertex[EdgeHead[EdgeJ]], Vertex[EdgeTail[EdgeJ]])){
				Flags[EdgeI]= true;
				Flags[EdgeJ]= true;
			}
		}
	}

	IntersectedEdges.clear();
	bool intersected= false;
	for (long i= 0; i<EdgeCount; i++){
		if(Flags[i]){
			IntersectedEdges.push_back(i);
			intersected= true;
		}
	}

	return intersected;
}


void CCueVanishingPoints::RemoveTheLessLikelyIntersectedEdge(long CurrentGroup,
															 std::vector <long> IntersectedEdges)
//Find the less likely intersected edge in the group CurrentGroup and remove it

//Likeliness is measured based on two metrics:
// * The more VPs the line is linked to, the more likely it is
// * The closer their candidate VP are to the centroid, the more likely the line is
//The second metric is only used to disambiguate between lines linked to the minimum number of VPs
{
	std::vector<std::vector <long>> VPsInEdge;
	std::vector<long> NumVPs;
	std::vector<double> Distance;

	//Find the VPs linked to every intersected edge
	//and their distances to the centroid
	long n= (long)IntersectedEdges.size();
	for (long i= 0; i<n; i++){
		VPsInEdge.push_back(std::vector <long>());
		Distance.push_back(0);

		long nn= (long)Cluster[CurrentGroup].size();
		for (long j= 0; j<nn; j++){
			long VPj= Cluster[CurrentGroup][j];
			if(EdgesInVP[VPj][0] == IntersectedEdges[i] ||
			   EdgesInVP[VPj][1] == IntersectedEdges[i]){
				VPsInEdge[i].push_back(VPj);
				Distance[i]+= MODULE_2D(CandidateVP[VPj], DrawingCentroid);
			}
		}
		NumVPs.push_back(VPsInEdge[i].size());
		Distance[i]/= NumVPs[i];
	}

	//Find the edge that contributes with less VPs
	long MinNumVPs= NumVPs[0];
	for (long i= 1; i<(long)NumVPs.size();i++){
		MinNumVPs= min(MinNumVPs, NumVPs[i]);
	}

	//From all the edges that contribute with the minimum
	//select the one whose VPs are farther away
	long ii= 0;
	while(NumVPs[ii] != MinNumVPs && ii < (long)Distance.size())
		ii++;

	long LessLikely= ii;
	double MaxDistance= Distance[ii];
	for (long i= ii+1; i<(long)Distance.size();i++){
		if(NumVPs[i] == MinNumVPs){
			if(Distance[i] > MaxDistance){
				LessLikely= i;
				MaxDistance= Distance[i];
			}
		}
	}

	//Remove from Cluster
	for (long i= 0; i<(long)VPsInEdge[LessLikely].size(); i++){
		for (long j= 0; j<(long)Cluster[CurrentGroup].size(); j++){
			if(Cluster[CurrentGroup][j] == VPsInEdge[LessLikely][i]){
				Cluster[CurrentGroup].erase(Cluster[CurrentGroup].begin()+j);
				break;
			}
		}
	}

	//Remove IntersectedEdges[LessLikely] from EdgesInMeanVP
	for (long i= 0; i<(long)ParallelEdges[CurrentGroup].size(); i++){
		if(ParallelEdges[CurrentGroup][i] == IntersectedEdges[LessLikely]){
			ParallelEdges[CurrentGroup].erase(ParallelEdges[CurrentGroup].begin()+i);
			break;
		}
	}
}


long CCueVanishingPoints::IntersectExtension(POINT2D a1, POINT2D b1,
											 POINT2D a2, POINT2D b2)
//Evaluates if the intersection of two edges
//is internal to one and external to the other
//(If the former intersects the extension of the later)

//Input: edge 1 endpoints (a1.x,a1.y) (b1.x,b1.y)
//       edge 2 endpoints (a2.x,a2.y) (b2.x,b2.y)

//Returns:
//	1 if segment 1 intersects the extension of segment 2
//	2 if segment 2 intersects the extension of segment 1
//	0 otherwise
{
	double s1x = b1.x - a1.x;
	double s1y = b1.y - a1.y;
	double s2x = b2.x - a2.x;
	double s2y = b2.y - a2.y;
	double denom= (-s2x * s1y + s1x * s2y);

	if(denom == 0)
		//Edges are parallel
		return 0;

	//Get intersection
	double s = (-s1y * (a1.x - a2.x) + s1x * (a1.y - a2.y)) / denom;
	double t = ( s2x * (a1.y - a2.y) - s2y * (a1.x - a2.x)) / denom;

	if ((s < -ToleranceCoincident || s > 1+ToleranceCoincident) && 
		(t > -ToleranceCoincident && t < 1+ToleranceCoincident)){
		//Intersection point is internal to segment 1 and external to segment 2
		return 1;
	}
	if ((s > -ToleranceCoincident && s < 1+ToleranceCoincident) && 
		(t < -ToleranceCoincident || t > 1+ToleranceCoincident)){
		//Intersection point is internal to segment 2 and external to segment 1
		return 2;
	}

	return 0;
}


void CCueVanishingPoints::FilterIntersectedEdges()
//Removes intersecting lines from groups of lines converging to a VP

//Lines in a group cannot intersect to each other
//(as they represent parallel edges)
//If intersecting lines are detected, the less likely one is recursively erased

//A tolerance is applied, to prevent removing nearly parallel lines,
//which may appear as intersecting in strict geometrical calculations
{
	//Initialization
	std::vector<bool> Refined(NumGroups, false);
	long NonRefined= NumGroups;

	//Loop to remove mutually intersecting lines
	while(NonRefined > 0){
		//Search for the first non refined group of lines
		long CurrentGroup= 0;
		while(Refined[CurrentGroup]){
			CurrentGroup++;
			if(CurrentGroup > NumGroups){NonRefined= 0; break;}
		}

		//Find intersections between lines in the group
		std::vector<long> IntersectedEdges;
		if(FindIntersectionsInGroups(CurrentGroup, IntersectedEdges)){
			//Remove the less likely intersected edge 
			RemoveTheLessLikelyIntersectedEdge(CurrentGroup, IntersectedEdges);
		}
		else{
			Refined[CurrentGroup]= true;
			NonRefined--;
		}
	}

	//Loop to remove lines that intersect the extensions of other lines
	for (long i= 0; i<NumGroups; i++){
		bool ToDelete= false;
		long n= ParallelEdges[i].size();
		std::vector<bool> ToRemove(n, false);

		for (long ii= 0; ii<n-1; ii++){
			for (long jj= ii+1; jj<n; jj++){
				if(NearlyParallel(Vertex[EdgeHead[ParallelEdges[i][ii]]],
								  Vertex[EdgeTail[ParallelEdges[i][ii]]],
								  Vertex[EdgeHead[ParallelEdges[i][jj]]],
								  Vertex[EdgeTail[ParallelEdges[i][jj]]],
								  ToleranceParallel) == 0){
					long Int= IntersectExtension(Vertex[EdgeHead[ParallelEdges[i][ii]]],
												 Vertex[EdgeTail[ParallelEdges[i][ii]]],
												 Vertex[EdgeHead[ParallelEdges[i][jj]]],
												 Vertex[EdgeTail[ParallelEdges[i][jj]]]);
					if(Int == 1){
						ToRemove[ii]= true;
						ToDelete= true;
					}
					else if(Int == 2){
						ToRemove[jj]= true;
						ToDelete= true;
					}
				}
			}
		}

		if(ToDelete){
			//Remove the less likely first
			for (long ii= n-1; ii>= 0; ii--){
				if(ToRemove[ii]){
					//Remove edge ii
					ParallelEdges[i].erase(ParallelEdges[i].begin()+ii);
				}
			}
		}
	}
}


double CCueVanishingPoints::GetDispersion(long numCluster)
//Returns the dispersion of VPs belonging to group numCluster

//All candidate VPs are considered to calculate dispersion
{
	long n= Cluster[numCluster].size();
	double StandardDeviation= 0;

	POINT2D vp= GetClusterCentroid(numCluster);

	if(n<3)
		return StandardDeviation;

	double meanDist= 0;
	for (long i= 0; i<n; i++){
		meanDist+= MODULE_2D(CandidateVP[Cluster[numCluster][i]], vp);
	}
	meanDist/= n;

	for (long i= 0; i<n; i++){
		double DistI = MODULE_2D(CandidateVP[Cluster[numCluster][i]], vp) - meanDist;
		StandardDeviation+= DistI * DistI;
	}
	StandardDeviation= sqrt(StandardDeviation/n);

	return StandardDeviation;
}


void CCueVanishingPoints::GetParallelGroups()
//Detects groups of parallel lines

//Groups of nearly parallel lines:
// * have a distant centroid
// * are disperse (their VPs are distant from each other)
{
	ParallelGroup.assign(NumGroups, false);

	for (long i= 0; i<NumGroups; i++){
		double dist= MODULE_2D(MeanVP[i], DrawingCentroid);
		if(dist > OuterRingRadius){
			double Dispersion= GetDispersion(i);
			if((Dispersion > ToleranceDispersion || Dispersion==0)){ //If group is disperse
				ParallelGroup[i]= true;
			}
		}
	}
}


double CCueVanishingPoints::GetGroupOrientation(long NumGroup)
//Group orientation is calculated as the orientation of the Mean Line of the group

//WARNING! There is a second choice, not used in this version:
//Group orientation is calculated as the average orientation
//of the lines belonging to the group

//This choice is not used as it takes more time
//and does not significantly improve results
{
	double AlphaMeanLine= GetOrientation(MeanVP[NumGroup]);

	////////////////////////////////////////////////
	////Average orientation of all the lines belonging to the group
	//double AlphaMeanLine= 0;
	//long n= ParallelEdges[NumGroup].size();
	//for (long i= 0; i<n; i++){
	//	AlphaMeanLine += GetAngle_PI(Vertex[EdgeHead[ParallelEdges[NumGroup][i]]],
	//								 Vertex[EdgeTail[ParallelEdges[NumGroup][i]]]);
	//}
	//AlphaMeanLine/= n;
	////////////////////////////////////////////////

	return AlphaMeanLine;
}


void CCueVanishingPoints::FilterParallelGroups()
//Removes non-parallel lines from parallel groups

//Edges are considered parallel to the group if
//its convergence angle with the mean line is less than a threshold

//The mean line is the line passing through
//the drawing centroid and the mean vanishing point of the group

//Current threshold is CuttingDistance
// WARNING! For better tuning the algorithm, a specific parameter
//          should be used here, instead of CuttingDistance
{

	double MaxnParallelThreshold= CuttingDistance;

	for (long i= 0; i<NumGroups; i++){
		if(ParallelGroup[i]){
			double AlphaMeanLine= GetGroupOrientation(i);
			if(AlphaMeanLine > NUMBER_PI) AlphaMeanLine-= NUMBER_PI;

			for (long j= (long)ParallelEdges[i].size()-1; j>=0 ; j--){
				long EdgeJ= ParallelEdges[i][j];
				double AlphaJ= GetAngle2D_PI(Vertex[EdgeHead[EdgeJ]], Vertex[EdgeTail[EdgeJ]]);

				//If edge J is clearly non-parallel to the group
				if(std::fabs(AlphaMeanLine-AlphaJ) > MaxnParallelThreshold &&
				   std::fabs(AlphaMeanLine-AlphaJ) < NUMBER_PI-MaxnParallelThreshold){
					//Remove edge J
					ParallelEdges[i].erase(ParallelEdges[i].begin()+j);
				}
			}
		}
	}
}


//POINT2D CCueVanishingPoints::IntersectDistance(POINT2D a1, POINT2D b1,
//											   POINT2D a2, POINT2D b2)
bool CCueVanishingPoints::IntersectDistance(POINT2D a1, POINT2D b1,
											POINT2D a2, POINT2D b2)
//Returns the distance from the intersection point of edges (a1, a2) and (b1, b2)
//to the drawing centroid
{
	//POINT2D I= DrawingCentroid;
	//IntersectOutside(a1, b1, a2, b2, I);
	//return MODULE_2D(I, DrawingCentroid);

	double s1_x = b1.x - a1.x;
	double s1_y = b1.y - a1.y;
	double s2_x = b2.x - a2.x;
	double s2_y = b2.y - a2.y;
	double denom= (-s2_x * s1_y + s1_x * s2_y);

	if(denom == 0)
		//Edges are parallel
		return false;

	double s = (-s1_y * (a1.x - a2.x) + s1_x * (a1.y - a2.y)) / denom;
	double t = ( s2_x * (a1.y - a2.y) - s2_y * (a1.x - a2.x)) / denom;

	//if ((s >= 0 && s <= 1) || (t >= 0 && t <= 1)){
	//	//Intersection point is internal to, at least, one of the edges
	//	return true;
	//}

	//External intersection
	POINT2D I;
	I.x= (a1.x + (t * s1_x));
	I.y= (a1.y + (t * s1_y));
	return (MODULE_2D(I, DrawingCentroid)  < OuterRingRadius);
}


bool CCueVanishingPoints::FilterAntiConvergentEdges()
//Removes anticonvergent lines from convergent groups

//Filters edges that converge
//on the opposite side of the vanishing point

//The mean line is the line passing through
//the drawing centroid and the mean vanishing point of the group

//Edges are retained in the group if their prolongations intersect
//////the mean line in the near side of the vanishing points
//the mean line close and in the far side of the vanishing points
//Otherwise are discarded
{
	bool filtered= false;

	for (long i= 0; i<NumGroups; i++){
		bool filter= !ParallelGroup[i];  //Filter only non-parallel groups
		POINT2D MeanVPI= MeanVP[i];

		while(filter){
			filter= false;

			//Find all anticonvergent edges of the current group
			std::vector <long> AntiEdges;
			for (long j= 0; j<(long)ParallelEdges[i].size(); j++){
				POINT2D Head= Vertex[EdgeHead[ParallelEdges[i][j]]];
				double dHH= GetDistancePointToLine(Head, DrawingCentroid, MeanVPI);
				double dHV= MODULE_2D(Head, MeanVPI);

				POINT2D Tail= Vertex[EdgeTail[ParallelEdges[i][j]]];
				double dTH= GetDistancePointToLine(Tail, DrawingCentroid, MeanVPI);
				double dTV= MODULE_2D(Tail, MeanVPI);

				if((dHH<dTH && dHV>dTV) || (dHH>dTH && dHV<dTV)){
					//Do not check edges nearly parallel to the mean line
					//(as their intersection is irrelevant)

					//if (!NearlyCollinear(Head, Tail, MeanVPI, DrawingCentroid,
					//					ToleranceParallel, ToleranceCoincident)){
					if (NearlyParallel(Head, Tail, MeanVPI, DrawingCentroid, ToleranceParallel) == 0){

						//Check if intersection happens close to the drawing centroid
						if(IntersectDistance(DrawingCentroid, MeanVPI, Head, Tail)){

							//Edge is anticonvergent
							AntiEdges.push_back(ParallelEdges[i][j]);
						}
					}
				}
			}

			//Remove the worst anticonvergent edge
			if(AntiEdges.size() > 0){
				//Select the anticonvergent edge connected to the most distant VP
				long WorstAntiEdge= AntiEdges[0];
				double maxDist= 0;
				long worstCluster= 0;
				for(long k= 0; k<(long)Cluster[i].size(); k++){
					long position;
					if(IsValueInVector(EdgesInVP[Cluster[i][k]][0], AntiEdges, position) ||
					   IsValueInVector(EdgesInVP[Cluster[i][k]][1], AntiEdges, position)){
						double dist= MODULE_2D(MeanVPI, CandidateVP[Cluster[i][k]]);
						if(dist > maxDist){
							maxDist= dist;
							worstCluster= k;
							WorstAntiEdge= AntiEdges[position];
						}
					}
				}

				//Remove the candidate VPs related to the worst anticonvergent edge
				for(long k= (long)Cluster[i].size()-1; k>=0; k--){
					if(EdgesInVP[Cluster[i][k]][0] == WorstAntiEdge ||
					   EdgesInVP[Cluster[i][k]][1] == WorstAntiEdge){
						Cluster[i].erase(Cluster[i].begin()+k);
					}
				}

				//Remove the worst anticonvergentd edge
				long position;
				if(IsValueInVector(WorstAntiEdge, ParallelEdges[i], position))
					ParallelEdges[i].erase(ParallelEdges[i].begin()+position);

				filter= true;
				filtered= true;

				//Recalculate meanVP (and Mean Line)
				MeanVPI= GetRefinedCentroid(i);
			}
		}
	}

	return filtered;
}


void CCueVanishingPoints::RemoveVanishingPoints(long CurrentGroup,long RepeatedEdge)
	//Remove the vanishing points linked to repeated edge
{
	for (long i= (long)Cluster[CurrentGroup].size()-1; i>=0; i--){
		if(EdgesInVP[Cluster[CurrentGroup][i]][0] == RepeatedEdge ||
		   EdgesInVP[Cluster[CurrentGroup][i]][1] == RepeatedEdge){
			Cluster[CurrentGroup].erase(Cluster[CurrentGroup].begin()+i);
		}
	}
}


void CCueVanishingPoints::FilterRepeatedEdges()
//Removes duplicated lines from groups of lines converging to a VP

//Lines can only belong to a single group
//(as they can only converge to a single vanishing point)
//If duplicated lines are detected, the less likely one is erased

//Likelihood is measured based on one criterion:
// * The less populated group cannot contain lines that belong to a more populated group

//Remove the entire groups if void after removing its repeated edges
{
	std::vector<bool> AlreadyUsedEdge(EdgeCount, false);

	for (long i= 0; i<(long)ParallelEdges.size(); i++){
		std::vector<long> UsedI;
		for (long j= (long)ParallelEdges[i].size()-1; j>=0 ; j--){
			if(AlreadyUsedEdge[ParallelEdges[i][j]]){
				//Remove the edge and its corresponding VPs
				RemoveVanishingPoints(i, ParallelEdges[i][j]);
				ParallelEdges[i].erase(ParallelEdges[i].begin()+j);
			}
			else
				UsedI.push_back(ParallelEdges[i][j]);
		}

		if(Cluster[i].size() <= 0 || ParallelEdges[i].size() < 2){
		//if(Cluster[i].size() <= 0){
			//Remove void cluster
			Cluster.erase(Cluster.begin()+i);
			MeanVP.erase(MeanVP.begin()+i);
			ParallelEdges.erase(ParallelEdges.begin()+i);
			ParallelGroup.erase(ParallelGroup.begin()+i);
			NumGroups--;
			i--;
		}
		else{
			//Add flags to already used edges
			for (long j= 0; j<(long)UsedI.size(); j++){
				AlreadyUsedEdge[UsedI[j]]= true;
			}
		}
	}
}


void CCueVanishingPoints::AddEdgeInGroup(long NewEdge, long NumGroup)
//Adds edge NewEdge into group NumGroup

//Keeps the increasing order of edges in the group
{
	long size= ParallelEdges[NumGroup].size();

	long k= 0;
	while(k<size && ParallelEdges[NumGroup][k]<NewEdge)k++;

	if(k<size)
		ParallelEdges[NumGroup].insert(ParallelEdges[NumGroup].begin()+k, NewEdge);
	else
		ParallelEdges[NumGroup].push_back(NewEdge);
}


void CCueVanishingPoints::FilterOppositeConvergence()
//Remove opposite VPs
//and group their lines together with the lines of their actual VPs

//Intended parallel lines are usually only "nearly" parallel.
//As a result, the orientations of such lines are all close to, 
//but not exactly, the intended angle alpha.
//So they produce some candidate VPs located around alpha
//and others located around alpha+NUMBER_PI
{
	std::vector<bool> ToRemove(NumGroups, false);

	//Loop to detect pairs of candidate opposite VPs
	for (long i= 0; i<NumGroups-1; i++){
		if(ParallelGroup[i]){ //If first VP group is made of parallel lines
			double AlphaMeanLine= GetGroupOrientation(i);

			for (long j= i+1; j<NumGroups; j++){
				if(ParallelGroup[j]){ //If second VP group is made of parallel lines
					double AlphaJ= GetOrientation(MeanVP[j]);

					//If they have "nearly" opposite orientations
					if(std::fabs(GetAngleDifference(AlphaMeanLine, AlphaJ) - NUMBER_PI) < (CuttingDistance/2)){

						//Check if groups intersect each other
						bool merge= true;
						for (long ii= 0; ii<(long)ParallelEdges[i].size(); ii++){
							for (long jj= 0; jj<(long)ParallelEdges[j].size(); jj++){
								if(IntersectInside(Vertex[EdgeHead[ParallelEdges[i][ii]]],
												   Vertex[EdgeTail[ParallelEdges[i][ii]]],
												   Vertex[EdgeHead[ParallelEdges[j][jj]]],
												   Vertex[EdgeTail[ParallelEdges[j][jj]]])){
									merge= false;
									break;
								}
							}
							if(!merge) break;
						}

						if(merge){
							//Add the smaller group into the large one
							long keep, remove;
							if(ParallelEdges[i].size() > ParallelEdges[j].size()){
								keep= i; remove= j;
							}
							else{
								keep= j; remove= i;
							}

							for (long k= 0; k<(long)ParallelEdges[remove].size(); k++){
								//ParallelEdges[keep].push_back(ParallelEdges[remove][k]);
								AddEdgeInGroup(ParallelEdges[remove][k], keep);
							}

							//Mark the smaller opposite group to remove
							ToRemove[remove]= true;
						}
					}
				}
			}
		}
	}

	//Loop to remove opposite groups
	for (long i= NumGroups-1; i>=0 ; i--){
		if(ToRemove[i]){
			Cluster.erase(Cluster.begin()+i);
			MeanVP.erase(MeanVP.begin()+i);
			ParallelEdges.erase(ParallelEdges.begin()+i);
			ParallelGroup.erase(ParallelGroup.begin()+i);
			NumGroups--;
		}
	}
}


void CCueVanishingPoints::ResampleIsolatedParallelEdges(std::vector<bool> &IsolatedEdge)
//Resample edges that have been removed from all groups
//to check if they belong to any group of parallel edges
{
	//Loop for all isolated edges
	for (long i= 0; i<EdgeCount; i++){
		if(IsolatedEdge[i]){
			//Get orientation
			double AlphaI= GetAngle2D_PI(Vertex[EdgeHead[i]], Vertex[EdgeTail[i]]);

			//Search for a nearly parallel group parallel to the edge
			for (long j= 0; j<NumGroups; j++){
				if(ParallelGroup[j]){
					//Orientation of the VP
					double AlphaMeanLine= GetGroupOrientation(j);
					if(AlphaMeanLine > NUMBER_PI) AlphaMeanLine-= NUMBER_PI;

					if(std::fabs(AlphaI-AlphaMeanLine) < CuttingDistance ||
					   std::fabs(AlphaI-AlphaMeanLine) > NUMBER_PI-CuttingDistance){

						//Check if edge intersects with the group
						bool add= true;
						for (long k= 0; k<(long)ParallelEdges[j].size(); k++){
							if(IntersectInside(Vertex[EdgeHead[i]],
											   Vertex[EdgeTail[i]],
											   Vertex[EdgeHead[ParallelEdges[j][k]]],
											   Vertex[EdgeTail[ParallelEdges[j][k]]])){
								add= false;
								break;
							}
						}

						//Add the non-intersecting edge into the group
						if(add){
							AddEdgeInGroup(i, j);
							IsolatedEdge[i]= false;
							break;
						}
					}
				}
			}
		}
	}
}


void CCueVanishingPoints::ResampleIsolatedConvergentEdges(std::vector<bool> &IsolatedEdge)
//Resample edges that have been removed from all groups
//to check if they belong to any group of convergent edges
{
	//Loop for all isolated edges
	for (long i= 0; i<EdgeCount; i++){
		if(IsolatedEdge[i]){
			//Get orientation
			double AlphaI= GetAngle2D_PI(Vertex[EdgeHead[i]], Vertex[EdgeTail[i]]);

			//Search for a convergent group at which the edge may converge
			for (long j= 0; j<NumGroups; j++){
				if(!ParallelGroup[j]){
					//Orientation of the VP
					double AlphaMeanLine= GetGroupOrientation(j);
					if(AlphaMeanLine > NUMBER_PI) AlphaMeanLine-= NUMBER_PI;

					if(std::fabs(AlphaI-AlphaMeanLine) < CuttingDistance ||
					   std::fabs(AlphaI-AlphaMeanLine) > NUMBER_PI-CuttingDistance){

						//Check if edge intersects with the group
						bool add= true;
						for (long k= 0; k<(long)ParallelEdges[j].size(); k++){
							if(IntersectInside(Vertex[EdgeHead[i]],
											   Vertex[EdgeTail[i]],
											   Vertex[EdgeHead[ParallelEdges[j][k]]],
											   Vertex[EdgeTail[ParallelEdges[j][k]]])){
								add= false;
								break;
							}
						}

						//Add the non-intersecting edge into the group
						if(add){
							AddEdgeInGroup(i, j);
							IsolatedEdge[i]= false;
							break;
						}
					}
				}
			}
		}
	}
}


void CCueVanishingPoints::ResampleIsolatedEdges()
//Resample edges that have been removed from all groups
//to check if they belong to any group of:
// * Parallel edges
// * Convergent edges
{
	//Search for edges not included in any group
	std::vector<bool> IsolatedEdge(EdgeCount, true);

	for (long i= 0; i<NumGroups; i++){
		for (long j= 0; j<(long)ParallelEdges[i].size(); j++){
			IsolatedEdge[ParallelEdges[i][j]]= false;
		}
	}

	ResampleIsolatedParallelEdges(IsolatedEdge);
	ResampleIsolatedConvergentEdges(IsolatedEdge);
}


void CCueVanishingPoints::Output_VP()
//Copies information from local DataBase of the FindingVanishingPoints algorithm
//into the external database

//This function should be adapted to different external databases
//This function should be called after calculating vanishing points by GetVanishingPoints
{
	//Internal information used to display the clustering process
	//-----------------------------------------------------------
	//Candidate vanishing points
	ReferApp.Cu.VP.CandidateVP= CandidateVP;

	//Drawing centroid
	ReferApp.Cu.VP.DrawingCentroid= DrawingCentroid;

	//Vanishing points ring
	ReferApp.Cu.VP.InnerRingRadius= InnerRingRadius;
	ReferApp.Cu.VP.OuterRingRadius= OuterRingRadius;

	//Clusters of candidate vanishig points
	ReferApp.Cu.VP.ClusterOfVPs= Cluster;

	//Clusters of parallel lines
	ReferApp.Cu.VP.ParallelGroup= ParallelGroup;

	//Mean vanishing points
	//---------------------
	ReferApp.Cu.VP.MeanVP= MeanVP;
	//ReferApp.Cu.VP.ParallelEdges= ParallelEdges;
}


CCueVanishingPoints::CCueVanishingPoints(CDB_Model *DDBB)
//Constructor that updates local database with the drawing to analyse

//The drawing is taken from CDB_Model

//The input required by the algorithms is:
//		VertexCount= number of vertex in the line drawing
//		Vertex[i].x= X coordinate of the i-th vertex
//		Vertex[i].y= Y coordinate of the i-th vertex

//		EdgeCount= number of edges in the line drawing
//		EdgeHead[i]= Head vertex defining the i-th edge
//		EdgeTail[i]= Tail vertex defining the i-th edge
{
	//Copy external vertices into local database
	//------------------------------------------
	Vertex.clear();
	VertexCount= DDBB->GetSizeVertices();
	for (long i=0; i<VertexCount; i++){
		CVertex Vertice= DDBB->GetVertex(i);
		POINT2D p(Vertice.x, Vertice.y);
		Vertex.push_back(p);
	}

	//Copy external edges into local database
	//---------------------------------------
	EdgeHead.clear();
	EdgeTail.clear();
	EdgeCount= DDBB->GetSizeEdges();
	for (long i=0; i<EdgeCount; i++){
		CEdge Arista= DDBB->GetEdge(i);
		EdgeHead.push_back(Arista.Head);
		EdgeTail.push_back(Arista.Tail);
	}
}


CCueVanishingPoints::CCueVanishingPoints(int IVertexCount,
										 std::vector <POINT2D> IVertex,
										 int IEdgeCount,
										 std::vector <int> IEdgeHead,
										 std::vector <int> IEdgeTail)
//Constructor that updates local database with the drawing to analyse

//The drawing is taken from a global database with the same structure

//The input required by the algorithms is:
//		VertexCount= number of vertex in the line drawing
//		Vertex[i].x= X coordinate of the i-th vertex
//		Vertex[i].y= Y coordinate of the i-th vertex

//		EdgeCount= number of edges in the line drawing
//		EdgeHead[i]= Head vertex defining the i-th edge
//		EdgeTail[i]= Tail vertex defining the i-th edge

:
VertexCount (IVertexCount),
Vertex (IVertex),
EdgeCount (IEdgeCount),
EdgeHead (IEdgeHead),
EdgeTail (IEdgeTail)
{
}


std::vector <std::vector <long>> CCueVanishingPoints::GetVanishingPoints()
//Vanishing point detection algorithm which mimics the human perception process

//The input is a list of vertices and edges (see constructor)

//The output is a list lines of the drawing grouped in bundles of lines
//that are supossed to represent edges parallel to each other in the 3D model

//More output information is also saved for visualization purposes (see Output_VP)

//The approach is described in
	//P.Company, P.A.C. Varley, R. Plumed (2014)
	//An algorithm for grouping lines which converge to
	//vanishing points in perspective sketches of polyhedra
	//GREC 2013 PostProceedings. LNCS 8746, pp. 77-95
{
	//Initialize parameters
	InitParams();

	//Find vanishing points
	GetCandidateVP();
	GetClusters();
	GetMeanVP();

	//Find groups of edges
	GetGroupsOfEdges();
	ReorderClustersBySize();

	//Filter results
	FilterIntersectedEdges();
	GetParallelGroups();
	FilterParallelGroups();

	FilterAntiConvergentEdges();

	FilterRepeatedEdges();

	FilterOppositeConvergence();
	ResampleIsolatedEdges();

	//Save internal information for external use
	Output_VP();

	//Output results
	return ParallelEdges;
}
