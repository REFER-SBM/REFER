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
#include <cfloat>    // isnan
#include <vector>

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "ReferDoc.h"
#include "DB_Model_Maths.h"


CVector GetUnitVector2D(CVertex VH, CVertex VT)
// Returns the unit vector, in 2D, running from tail vertex VT to head vertex VH
{
	CVector Vector;

	double VX = VH.x - VT.x;
	double VY = VH.y - VT.y;
	double Module = sqrt(VX*VX + VY*VY);
	Vector.x = VX / Module;
	Vector.y = VY / Module;
	Vector.z = 0.;

	return Vector;
};


CVector GetUnitVector3D(CVertex VH, CVertex VT)
// Returns the unit vector, in 3D, running from tail vertex VT to head vertex VH
{
	CVector Vector;

	double VX = VH.x - VT.x;
	double VY = VH.y - VT.y;
	double VZ = VH.z - VT.z;
	double Module = sqrt(VX*VX + VY*VY + VZ*VZ);
	Vector.x = VX / Module;
	Vector.y = VY / Module;
	Vector.z = VZ / Module;

	return Vector;
};


void AddMidPoints(CDB_Model *DDBB,
				  std::vector<long> &NumVertices,
				  CVerticesSet &Vertices)
// Inserts midpoints for all the edges of a polygon
// defined by a sequence of vertices stored in NumVertices

// Vertices contains the detailed information of every vertex

// It returns the numbers of vertices/edges:
//     If vertices, their indices are stored
//     (as positive numbers)
//     If midpoints, the indices of the edges they belong to are stored
//     (switched to negative numbers and displaced by one unit to prevent zero index)

// It also returns the detailed information of every vertex/MidPoint
{
	std::vector<long> NumVerticesPlusMidPoints;
	CVerticesSet VerticesPlusMidPoints;

	long N = Vertices.GetSize();

	for (long i=0; i<N; i++) {
		CVertex Vertice1= Vertices.GetVertex(i);
		CVertex Vertice2= Vertices.GetVertex((i+1)%N);
		CVertex MidPoint;
		MidPoint.x = (Vertice1.x + Vertice2.x)/2;
		MidPoint.y = (Vertice1.y + Vertice2.y)/2;
		MidPoint.z = (Vertice1.z + Vertice2.z)/2;

		VerticesPlusMidPoints.AddVertex(Vertice1);
		VerticesPlusMidPoints.AddVertex(MidPoint);

		//Search the edge that contains the midpoint that matches
		bool MidPointInEdge= false;
		for (long j=0 ; j<DDBB->GetSizeEdges(); j++){
			CEdge EdgeJ= DDBB->GetEdge(j);
			if((EdgeJ.Head == NumVertices[i] && EdgeJ.Tail == NumVertices[(i+1)%N]) ||
			   (EdgeJ.Tail == NumVertices[i] && EdgeJ.Head == NumVertices[(i+1)%N])){
				NumVerticesPlusMidPoints.push_back(NumVertices[i]);
				NumVerticesPlusMidPoints.push_back(-(j+1));
				MidPointInEdge= true;
				break;
			}
		}
		assert(MidPointInEdge);
	};

	//Return lists of vertices interespersed with midpoints
	NumVertices= NumVerticesPlusMidPoints;
	Vertices= VerticesPlusMidPoints;
};


double GetDistance2DVertexToLine(CVertex V, CVertex p0, CVertex p1)
//Gets the distance from an external vertex "V" 
//up to a line defined by segment p0 to p1
//Returns the distance

//Calculations are done in 2D
{
	POINT2D VV(V.x, V.y);
	POINT2D pp0(p0.x, p0.y);
	POINT2D pp1(p1.x, p1.y);

	return GetDistancePointToLine(VV, pp0, pp1);
}


//void GetDistance2DVertexToLine(CVertex V, CVertex p0, CVertex p1,
//								  double &distance, CVertex &Intersection)
////Gets the distance from an external vertex "V" to a line defined by segment p0 to p1
////Gets also the intersection of p0-p1 with the minimum distance segment
//
////Returns the distance and the intersection point
//{
//	POINT2D VV(V.x, V.y);
//	POINT2D pp0(p0.x, p0.y);
//	POINT2D pp1(p1.x, p1.y);
//	POINT2D Int;
//
//	distance= GetDistancePointToLine(VV, pp0, pp1, Int);
//
//	Intersection.x= Int.x;
//	Intersection.y= Int.y;
//}


//double GetDistance3DVertexToLine(CVertex V, CVertex p0, CVertex p1)
////Gets the distance from an external vertex "V" 
////up to a line defined by segment p0 to p1
////Returns the distance
//
////Calculations are done in 3D
//{



//	//TBD



//struct float3
//{
//    float x, y, z;
//
//    float3 operator-( float3 v ) const
//    {
//        return float3{ x - v.x, y - v.y, z - v.z };
//    }
//    void operator -=( float3 v )
//    {
//        x -= v.x; y -= v.y; z -= v.z;
//    }
//    float3 operator*( float s ) const
//    {
//        return float3{ x * s, y * s, z * s };
//    }
//};
//// Dot product of 3D vectors
//float dot( float3 a, float3 b )
//{
//    return a.x * b.x + a.y * b.y + a.z * b.z;
//}
//// Length of a 3D vector
//float length( float3 v )
//{
//    return std::sqrt( dot( v, v ) );
//}
//// Distance between line and point in 3D
//float distanceToLine( float3 line0, float3 lineDir, float3 point )
//{
//    // Translate point making it relative to the start of the line
//    point -= line0;
//    // Coefficient to get closest point on the line,
//    // equal to length(point) * cos(angle) / length(lineDir)
//    float pl = dot( lineDir, point ) / dot( lineDir, lineDir );
//    // Position of the closest point on the line
//    float3 closest = lineDir * pl;
//    // Compute distance between two points
//    return length( point - closest );





// Returns distance from a 3D point to a line segment
//
// Inputs
//   P : vector
//       Position in space
//   A : vector
//       Position of line segment endpoint
//   B : vector
//       Position of other line segment endpoint
//
// Outputs
//   d : double
//       Distance from point to line segment

	//AP = P - A; // Vector from A to point
	//AB = B - A; // Vector from A to B

	//// Project point onto line
	//P_line = A + dot(AP, AB) / dot(AB, AB) * AB;

	//if all(A < P_line) && all(P_line < B)
	//	// The point projected onto the line is in between A and B 

	//	// Projection of point onto segment is the same 
	//	// as projection of point onto line
	//	P_seg = P_line; 
	//else
	//	// The point projected onto the line is outside of A and B

	//	if all(P_line <= A)
	//		// The projected point is closer to A
	//		P_seg = A;  
	//	else
	//		// The projected point is closer to B
	//		P_seg = B;
	//	end
	//end

	//d = norm(P - P_seg); // Distance to line segment


//}


double NearlyParallel(CDB_Model *DDBB, long iedge0, long iedge1, double ThresholdAngle)
//Extracts information from the database,
//to return true if two edges are parallel, withing a "treshold"

//The ThresholdAngle is the maximum angle difference (in radians)
{
	CEdge E0= DDBB->GetEdge(iedge0);
	CVertex h0= DDBB->GetVertex(E0.Head);
	CVertex t0= DDBB->GetVertex(E0.Tail);

	CEdge E1= DDBB->GetEdge(iedge1);
	CVertex h1= DDBB->GetVertex(E1.Head);
	CVertex t1= DDBB->GetVertex(E1.Tail);

	return NearlyParallel(h0.x, h0.y, t0.x, t0.y,
						  h1.x, h1.y, t1.x, t1.y,
						  ThresholdAngle);
}


bool NearlyCollinear(CDB_Model *DDBB, long iedge0, long iedge1,
					 double ThresholdAngle, double ThresholdOffset)
//Extracts information from the database,
//to return true if two edges are collinear, 
//within angular and offset "thresholds":

//	- The ThresholdAngle is the max angle difference (in radians)

//	- The ThresholdOffset is the min perpendicular distance
//	  between the end nodes of each segment
//	  and the line defined by the other segment
//	  (in % of the min length of the segments)
{
	CEdge E0= DDBB->GetEdge(iedge0);
	CVertex h0= DDBB->GetVertex(E0.Head);
	CVertex t0= DDBB->GetVertex(E0.Tail);

	CEdge E1= DDBB->GetEdge(iedge1);
	CVertex h1= DDBB->GetVertex(E1.Head);
	CVertex t1= DDBB->GetVertex(E1.Tail);

	return (NearlyCollinear(h0.x, h0.y, t0.x, t0.y,
							h1.x, h1.y, t1.x, t1.y,
							ThresholdAngle, ThresholdOffset) > 0);
}


double GetFaceArea(CDB_Model *DDBB, long NumFace)
//Return the area enclosed by polygonal face NumFace

//This is an implementation of the shoelace algorithm
{
	std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(NumFace));
	long N= vertices.size();
	double area= 0;

	for(long i= 0; i<N; i++){
		CVertex vi= DDBB->GetVertex(vertices[i]);
		CVertex vj= DDBB->GetVertex(vertices[(i+1)%N]);
		area+= vi.x*vj.y;
		area-= vj.x*vi.y;
	}

	return std::fabs(area)/2;
}


double GetFaceArea(CDB_Model *DDBB, std::vector<long> vertices)
//Return the area enclosed by a polygonal face defined by its vertices

//This is an implementation of the shoelace algorithm
{
	double area= 0;
	for(long i= 0; i<(long)vertices.size(); i++){
		CVertex vi= DDBB->GetVertex(vertices[i]);
		CVertex vj= DDBB->GetVertex(vertices[(i+1)%vertices.size()]);
		area+= vi.x*vj.y;
		area-= vj.x*vi.y;
	}
	return std::fabs(area)/2;
}


CVector GetTriangleNormal(double Vertices[3][3])
//Given 3 vertices, determines the normal vector of the plane that contains them
{
	//Obtains two vectors from the three points
	double Vector1[3], Vector2[3];

	Vector1[0] = Vertices[1][0] - Vertices[0][0];
	Vector1[1] = Vertices[1][1] - Vertices[0][1];
	Vector1[2] = Vertices[1][2] - Vertices[0][2];

	Vector2[0] = Vertices[2][0] - Vertices[1][0];
	Vector2[1] = Vertices[2][1] - Vertices[1][1];
	Vector2[2] = Vertices[2][2] - Vertices[1][2];

	//Calculate normal (through the vectorial product of the two vectors)
	CVector Normal;
	Normal.x = Vector1[1] * Vector2[2] - Vector1[2] * Vector2[1];
	Normal.y = Vector1[2] * Vector2[0] - Vector1[0] * Vector2[2];
	Normal.z = Vector1[0] * Vector2[1] - Vector1[1] * Vector2[0];

	//Normalize the normal
	//double Module = MODULE1_3D(Normal);
	double Module = sqrt( Normal.x*Normal.x + Normal.y*Normal.y + Normal.z*Normal.z );
	if (Module != 0.0){  // Prevents errors
		Normal.x /= Module;
		Normal.y /= Module;
		Normal.z /= Module;
	}

	return Normal;
}


CVector GetFaceNormal(CDB_Model *DDBB, long NumFace, bool saveNormals)
//Determines the normal vector of the face NumFace

//If the polygonal face is triangulated,
//calculates the normals for all the triangular facets
//and then the normal to the whole face is calculated as an average

//Otherwise uses the three first vertices of the convex hull of the polygon
//to calculate the normal to the whole face

//WARNINGS:
//	(1)	It is assumed that the face is flat, although it may be somewhat slanted

//	(2)	The convex hull calculation is done in the XY projection of the face,
//		as it is simpler, and the result is invariant to projections
//		except in cases of degenerate projections

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//		(which should not appear, since the faces were originarily detected
//		on a flat image depicted in the XY plane)

		//WARNING!!!!!!
		//¡¡¡Degenerate cases may appear if model is represented in MAIN AXES!!!
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//The flag saveNormals controls whether the normal is added to the database DDBB
//If model is triangulated, normals to the triangles are added to the database DDBB as well

//The face normal is always returned
{
	CFace currentFace= DDBB->GetFace(NumFace);
	std::vector<long> Triangles= currentFace.GetFaceTriangulation();

	//Calculate the normal of each triangle
	std::vector<CVector> TrianglesNormals;
	double Vertices[3][3];
	for(long t=0; t<(long)Triangles.size(); t+=3 ) {
		for(long j=0; j<3; j++) {
			CVertex Vj= DDBB->GetVertex(Triangles[t+j]);
			Vertices[j][0] = Vj.x;
			Vertices[j][1] = Vj.y;
			Vertices[j][2] = Vj.z;
		}
		TrianglesNormals.push_back(GetTriangleNormal(Vertices));
	}

	//Calculate the normal of the face as the average of the normals
	//of all the triangles of the face
	long numTriangles= TrianglesNormals.size();
	if(numTriangles > 0){
		double xx= 0, yy= 0, zz=0;
		for(long i=0; i<numTriangles; i++) {
			CVector Vi= TrianglesNormals[i];
			xx+= Vi.x;
			yy+= Vi.y;
			zz+= Vi.z;
		}
		CVector Normal (xx/numTriangles, yy/numTriangles, zz/numTriangles);

		if(saveNormals){
			//Save triangulation normals and face normal in the database
			currentFace.SetFaceTrianglesNormals(TrianglesNormals);
			currentFace.SetNormal(Normal);
			DDBB->SetFace(NumFace, currentFace);
		}

		return Normal;
	}

	else{
		//Calculate the convex hull of the polygon defined by the vertices of the face
		std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(NumFace));
		POLYGON2D polygonFace;
		for(long iVertex=0; iVertex<(long)vertices.size(); iVertex++){
			CVertex V= DDBB->GetVertex(vertices[iVertex]);
			NumPOINT2D P;
			P.Number= vertices[iVertex];
			P.Point.x= V.x;
			P.Point.y= V.y;
			polygonFace.push_back(P);
		}
		std::vector<long> Hull= polygonFace.ConvexHull();

		//Use the three first vertices of the convex hull to calculate the normal
		//double Vertices[3][3];
		for(long j=0; j<3; j++) {
			CVertex Vj= DDBB->GetVertex(Hull[j]);
			Vertices[j][0] = Vj.x;
			Vertices[j][1] = Vj.y;
			Vertices[j][2] = Vj.z;
		};

		//Calculate and return the normal vector
		CVector Normal= GetTriangleNormal(Vertices);
		if(saveNormals){
			//Save face normal in the database
			currentFace.SetNormal(Normal);
			DDBB->SetFace(NumFace, currentFace);
		}

		return Normal;
	}
}


void GetFaceNormals(CDB_Model *DDBB)
//Determines the normal vectors of all the faces in DDBB
//If faces are triangulated, their normals are calculated as well

//All normals are added to the database DDBB before returning
{
	for (long iFace=0; iFace<DDBB->GetSizeFaces(); iFace++){
		GetFaceNormal(DDBB, iFace, true);
	}
}


bool IsFacePlanar(CDB_Model *DDBB, long NumFace, double threshold)
// Returns true if face NumFace is planar within a threshold,
// false otherwise
{
	std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(NumFace));
	long N = vertices.size();

	if (N<=3)
		return true;

	CVertex P0= DDBB->GetVertex(vertices[0]);
	CVertex P1= DDBB->GetVertex(vertices[1]);
	CVertex P2= DDBB->GetVertex(vertices[2]);

	CVector v1 (P1.x-P0.x, P1.y-P0.y, P1.z-P0.z);
	CVector v2 (P2.x-P0.x, P2.y-P0.y, P2.z-P0.z);

	CVector v1x2 ((v1.y * v2.z - v1.z * v2.y),
				  (v1.z * v2.x - v1.x * v2.z),
				  (v1.x * v2.y - v1.y * v2.x));

	//Calculate the volumes of the prims v1-v2-vi
	//(scalar triple products vi*(v1 x v2))
	for (long i=3; i<N; i++){
		CVertex Pi= DDBB->GetVertex(vertices[i]);
		CVector vi (Pi.x-P0.x, Pi.y-P0.y,Pi.z-P0.z);

		double volumei= vi.x*v1x2.x + vi.y*v1x2.y + vi.z*v1x2.z;

		if(std::abs(volumei) > threshold)
			return false;
	}

	return true;
}


CVertex GetFaceCentroid(CDB_Model *DDBB, long NumFace)
// Returns the centroid of the face NumFace

//The signed area is calculated through the shoelace algorithm

//Since the centroid is invariant on afine transformations (like ortogonal proyections),
//the centroids of the projections are projections of the actual centroid:
//	-The X and Y coordinates are calculated on the polygon projected on the XY plane
//	-The Z coordinate is calculated on the polygon projected on the XZ plane

//The three projections of the centroid are calculated, just to discard one or two of them,
//in case the face is perpendicular or parallel to one coordinate plane
{
	CVertex Centroid;

	CVertex CentroidXY, CentroidXZ, CentroidYZ;

	std::vector<long> vertices= DDBB->GetAllVerticesInFace(DDBB->GetFace(NumFace));
	long N = vertices.size();

	double Signed2AreaXY= 0;
	double Signed2AreaXZ= 0;
	double Signed2AreaYZ= 0;

	//Calculate the centroids of the three projections of the face
	for (long i=0; i<N; i++){
		CVertex V1= DDBB->GetVertex(vertices[i]);
		CVertex V2= DDBB->GetVertex(vertices[(i+1) % N]);

		CentroidXY.x += ((V1.x + V2.x)*(V1.x * V2.y - V2.x * V1.y));
		CentroidXY.y += ((V1.y + V2.y)*(V1.x * V2.y - V2.x * V1.y));
		Signed2AreaXY += (V1.x * V2.y);
		Signed2AreaXY -= (V1.y * V2.x);

		CentroidXZ.x += ((V1.x + V2.x)*(V1.x * V2.z - V2.x * V1.z));
		CentroidXZ.z += ((V1.z + V2.z)*(V1.x * V2.z - V2.x * V1.z));
		Signed2AreaXZ += (V1.x * V2.z);
		Signed2AreaXZ -= (V1.z * V2.x);

		CentroidYZ.y += ((V1.y + V2.y)*(V1.y * V2.z - V2.y * V1.z));
		CentroidYZ.z += ((V1.z + V2.z)*(V1.y * V2.z - V2.y * V1.z));
		Signed2AreaYZ += (V1.y * V2.z);
		Signed2AreaYZ -= (V1.z * V2.y);
	}

	//Determine the centroid in 3D
	//after checking if the polygon is perpendicular or parallel to any of the reference planes
	if(std::fabs(Signed2AreaXY) > Accuracy){
		Centroid.x = CentroidXY.x / (3*Signed2AreaXY);
		Centroid.y = CentroidXY.y / (3*Signed2AreaXY);

		if(std::fabs(Signed2AreaXZ) > Accuracy)
			Centroid.z = CentroidXZ.z / (3*Signed2AreaXZ);
		else if(std::fabs(Signed2AreaYZ) > Accuracy)
			Centroid.z = CentroidYZ.z / (3*Signed2AreaYZ);
		else{
			CVertex V= DDBB->GetVertex(vertices[0]);
			Centroid.z= V.z;  //The face is nearly parallel to the XY plane, and all the z coordinates must be nearly equal to each other
		}
	}

	else if(std::fabs(Signed2AreaXZ) > Accuracy){
		Centroid.x = CentroidXZ.x / (3*Signed2AreaXZ);
		Centroid.z = CentroidXZ.z / (3*Signed2AreaXZ);

		if(std::fabs(Signed2AreaYZ) > Accuracy)
			Centroid.y = CentroidYZ.y / (3*Signed2AreaYZ);
		else{
			CVertex V= DDBB->GetVertex(vertices[0]);
			Centroid.y= V.y;
		}
	}

	else if(std::fabs(Signed2AreaYZ) > Accuracy){
		Centroid.y = CentroidYZ.y / (3*Signed2AreaYZ);
		Centroid.z = CentroidYZ.z / (3*Signed2AreaYZ);

		CVertex V= DDBB->GetVertex(vertices[0]);
		Centroid.x= V.x;
	}

	else{
		//Polygon with "nearly" null area
		CString sMensaje, sMensajeAux;
		sMensaje= "Failure to calculate its centroid, as face ";
		sMensajeAux.Format("%i", NumFace);
		sMensaje+=sMensajeAux;
		sMensaje+=" has a virtually null area";
		AfxMessageBox (sMensaje);
	}

	return Centroid;
}


bool AreAdjacentFaces(CDB_Model *DDBB, long face0, long face1)
//Returns true if faces are adjacent (share at least one edge),
//false otherwhise
{
	CFace face= DDBB->GetFace(face0);
	for(long i= 0; i<(long)face.GetSizeEdgesInFace(); i++){
		if(DDBB->IsEdgeInFace(face1, face.GetEdgeInFace(i))){
			return true;
		}
	}

	return false;
}


long ConvexHullFace(CDB_Model *DDBB)
//Finds one face of the convex hull,
//to start the procedure of calculating face normals of a BRep model

//The function sequentially parses the faces of the model 
//until one face belonging to the convex hull is found.
//Otherwise, a negative face is returned.

//For each face:
//	- Its normal vector is obtained as the cross product of the two vectors
//	  defined by three consecutive vertices
//	- The dot product is used to determine the side 
//	  where the rest of the vertices of the model are located: 
//		-If at least one vertice is found in each side, the face is discarded
//		-If all vertices are in the same side, the face is in the convex hull,
//		 and the other side is the outer side
{
	//Abort if no faces have been calculated
	if (DDBB->GetSizeFaces()==0) return -1;

	//Loop over all the faces
	for (long iFace=0; iFace<DDBB->GetSizeFaces(); iFace++){
		CVertex V0= DDBB->GetVertex(DDBB->GetVertexInFace(iFace, 0));
		CVector Normal= GetFaceNormal(DDBB, iFace, false);

		bool inside= false;
		bool outside= false;
		//Loop over all the vertices external to the face
		for(long iVertex=0; iVertex<(long)DDBB->GetSizeVertices(); iVertex++){
			if(!DDBB->IsVertexInFace(iFace, iVertex)){
				CVertex V= DDBB->GetVertex(iVertex);
				double NxV0V= Normal.x*(V.x-V0.x) + 
							  Normal.y*(V.y-V0.y) + 
							  Normal.z*(V.z-V0.z);
				//double NxV0V= DOT_PRODUCT_3D(Normal, (V-V0));
				if(NxV0V < 0.)
					inside= true;
				else if(NxV0V > 0.)
					outside= true;
				//else
					//coplanar with the face

				if(inside && outside)
					break;  //Discard current face
			}
		}

		if((inside && !outside) || (!inside && outside)){
			if(!inside && outside){
				//Reverse the winding of the face,
				//since its normal is pointing inward, relative to the rest of the model
				CFace face= DDBB->GetFace(iFace);
				face.ReverseFace();
				DDBB->SetFace(iFace, face);
			}

			//Finish the search and return the face belonging to the convex hull
			return iFace;
		}
	}
	//No face belonging to the convex hull was found
	return -1;
}


void OrderAdjacentFaces(CDB_Model *DDBB,
						long seed,
						std::vector<std::vector<long>> FacesInEdges,
						std::vector<bool> &orderedFace,
						long &iorderedFaces)
//Searches all the faces adjacent to all the edges of the seed face
//to order them counter clockwise (CCW), by reversing its winding if necessary
{
	//Loop over all the edges of the seed face
	//to visit its adjacent faces
	std::vector<long> edgesInSeedFace= DDBB->GetAllEdgesInFace(seed);
	long nEdgesSeed= edgesInSeedFace.size();

	for (long iEdgeSeed=0; iEdgeSeed < nEdgesSeed; iEdgeSeed++){
		//Get i-th adjacent face
		long iAdjacentFace;
		if(FacesInEdges[edgesInSeedFace[iEdgeSeed]].size() == 2) {
			if(FacesInEdges[edgesInSeedFace[iEdgeSeed]][0] == seed)
				iAdjacentFace= FacesInEdges[edgesInSeedFace[iEdgeSeed]][1];
			else
				iAdjacentFace= FacesInEdges[edgesInSeedFace[iEdgeSeed]][0];
		}
		else{
				CString sMensaje, sMensajeAux;
				sMensaje+="ERROR in normals to faces: non-manifold model. Edge ";
				sMensajeAux.Format("%i", edgesInSeedFace[iEdgeSeed]);
				sMensaje+=sMensajeAux;
				sMensaje+=" not shared by two faces";
				AfxMessageBox(sMensaje);
				continue;
		}

		if(!orderedFace[iAdjacentFace]){
			//Determine the orientation for the edge shared with the seed face
					//WARNING: Orientation of the edge may not match orientation of the face
					//The tail of the edge is not necessarily connected to the previous edge in the face
					//and the head of the edge is not necessarily connected to the next edge in the face
			CEdge currentSeed= DDBB->GetEdge(edgesInSeedFace[iEdgeSeed]);
			long iEdgeSeed1= iEdgeSeed+1;
			if(iEdgeSeed1 == nEdgesSeed) iEdgeSeed1= 0;   //next to the last is first
			CEdge nextSeed= DDBB->GetEdge(edgesInSeedFace[iEdgeSeed1]);
			long seedTail, seedHead;
			if(currentSeed.Head == nextSeed.Tail || currentSeed.Head == nextSeed.Head){
				//Head is connected to the next edge (sense of the vertex matches sense of the face)
				seedHead= currentSeed.Head;
				seedTail= currentSeed.Tail;
			}
			else{
				//Tail is connected to the next edge (sense of the vertex is opposite to sense of the face)
				seedHead= currentSeed.Tail;
				seedTail= currentSeed.Head;
			}

			//Search for the shared edge in the adjacent face
			std::vector<long> edgesInAdjacentFace= DDBB->GetAllEdgesInFace(iAdjacentFace);
			long nEdgesAdjacent= edgesInAdjacentFace.size();
			long iEdgeAdjacent= -1;

			for (long iAdjacentEdge=0; iAdjacentEdge < nEdgesAdjacent; iAdjacentEdge++){
				if(edgesInAdjacentFace[iAdjacentEdge] == edgesInSeedFace[iEdgeSeed]){
					iEdgeAdjacent= iAdjacentEdge;
					break;
				}
			}

			if(iEdgeAdjacent == -1){
				AfxMessageBox("Failure in OrderAdjacentFaces");
				continue;
			}

			//Determine if edge is already in opossite sense to the seed face
			CEdge currentAdjacent= DDBB->GetEdge(edgesInAdjacentFace[iEdgeAdjacent]);
			long iEdgeAdjacent1= iEdgeAdjacent+1;
			if(iEdgeAdjacent1 == nEdgesAdjacent) iEdgeAdjacent1= 0;   //next to the last is first
			CEdge nextAdjacent= DDBB->GetEdge(edgesInAdjacentFace[iEdgeAdjacent1]);

			long adjacentTail, adjacentHead;
			if(currentAdjacent.Head == nextAdjacent.Tail || currentAdjacent.Head == nextAdjacent.Head){
				//Head is connected to the next edge
				adjacentHead= currentSeed.Head;
				adjacentTail= currentSeed.Tail;
			}
			else{
				//Tail is connected to the next edge
				adjacentHead= currentSeed.Tail;
				adjacentTail= currentSeed.Head;
			}

			if (adjacentHead == seedTail && adjacentTail == seedHead){
				//The adjacent face is CCW, and does not require further changes
			}
			else{
				//Reverse adjacent face
				CFace adjacentFace= DDBB->GetFace(iAdjacentFace);
				adjacentFace.ReverseFace();
				DDBB->SetFace(iAdjacentFace, adjacentFace);
			}

			orderedFace[iAdjacentFace]= true;
			iorderedFaces++;
		}
	}
}


void ReorientFaces(CDB_Model *DDBB)
//Candidate faces are detected in the original sketch or line-drawing.
//However, face normals of a BRep model must point outward the model,
//hence, they cannot be calculaded before the model is inflated.

//We first fix the orientation of one face of our polyhedron (we name it as "seed")
//This is done by detecting one face that belongs to the convex hull
//Since, for those faces, all the vertices of the model are in only one of the semispaces
//determined by the plane that contains the face, the outward direction of the normal
//points to the empty semispace.

//We reorder the list of vertices that defines the face so that counter clockwise order
//matches the normal pointing outwards

//Adjacent faces that share one edge with the seed may be then reordered 
//to ensure that their normals point outwards as well.

//At this end, we must ensure that the common edge is oriented in opposite sense
//to its orientation in the seed face

//Once all adjacent faces have been visited, 
//the approach follows by selecting one of the already ordered faces as the new seed

//The approach is done when all the faces have been ordered.
{
	//Return if no faces have been searched
	if (DDBB->GetSizeFaces()==0){
		AfxMessageBox("Normals to faces requires faces calculated in advance");
		return;
	}

	//Search for seed face
	long seed= ConvexHullFace(DDBB);
	//Return if no seed face is found
	if (seed < 0){
		CString sMensaje, sMensajeAux;
		sMensaje+="ERROR: unable to find seed face to calculate normals pointing out of the 3D model (check if 3D model was calculated)";
		sMensaje+=sMensajeAux;
		AfxMessageBox(sMensaje);
		return;
	}

	//Initialize counters and flags
	std::vector<bool> visitedFace(DDBB->GetSizeFaces(), false);
	std::vector<bool> orderedFace(DDBB->GetSizeFaces(), false);
	orderedFace[seed]= true;
	long iorderedFaces= 1;

	std::vector<std::vector<long>> FacesInEdges= DDBB->GetAllFacesSharingEdges();

	//Loop to search for ordered but non-visited faces,
	//to visit their adjacent faces and order them
	do{
		OrderAdjacentFaces(DDBB,
							seed,
							FacesInEdges,
							orderedFace,
							iorderedFaces);
		visitedFace[seed]= true;

		for (long i=0; i<(long)visitedFace.size(); i++){
			if(!visitedFace[i] && orderedFace[i]){
				seed= i;
				break;
			}
		}
	} while(iorderedFaces < DDBB->GetSizeFaces());
}


bool IsASlenderFace(CFace Face, 
					double ThresholdShort, double ThresholdLong,
					std::vector<double> edgeLength)
//Returns true if Face is a slender quadrilateral face
//Returns false otherwise

//Face is assumed to be quadrilateral

//The quadrilateral is slender if two opposed edges are short
//and two opposed edges are long:
//		ThresholdShort is the threshold to consider sides as short
//		Sides are long if they are longer ThresholdLong times longer than the short ones
{
	//if (Face.GetSizeEdgesInFace() != 4) return false;

	//Calculate lengths
	std::vector<double> Side(4, 0);
	for (long j=0; j<4; j++){
		Side[j]= edgeLength[Face.GetEdgeInFace(j)];
	}

	//Detect two opposed short edges and two opposed long edges
	if ((Side[0]<ThresholdShort && Side[2]<ThresholdShort) &&
		(Side[1]>ThresholdLong*max(Side[0], Side[2]) &&
		 Side[3]>ThresholdLong*max(Side[0], Side[2]))){
		return true;
	}
	else if ((Side[1]<ThresholdShort && Side[3]<ThresholdShort) &&
			 (Side[0]>ThresholdLong*max(Side[1], Side[3]) &&
			  Side[2]>ThresholdLong*max(Side[1], Side[3]))){
		return true;
	}

	return false;
};


bool IsParallelogram(CFace Face, 
					 double ThresholdLength, double ThresholdAngle,
					 bool shortSides,
					 std::vector<double> edgeLength,
					 std::vector<double> edgeAngle)
//Returns true if Face is a quadrilateral with the
//long opposed sides parallel to each other

//Short sides are only checked if flag shortSides is true
{
	if (Face.GetSizeEdgesInFace() != 4)
		return false;

	//Get lengths and angles
	std::vector<double> Side(4, 0);
	std::vector<double> Angle(4, 0);
	for (long j=0; j<4; j++){
		long jj= Face.GetEdgeInFace(j);
		Side[j]= edgeLength[jj];
		Angle[j]= edgeAngle[jj];
	}

	if(shortSides){
		//Return true if the two pairs of oppossed sides are similar in length
		//and parallel within a threshold
		double angle02= std::fabs(Angle[0]-Angle[2]);
		angle02= min(angle02, std::fabs(angle02-NUMBER_PI));
		double angle13= std::fabs(Angle[1]-Angle[3]);
		angle13= min(angle13, std::fabs(angle13-NUMBER_PI));

		if (std::fabs(Side[0]-Side[2]) < ThresholdLength &&
			angle02 < ThresholdAngle &&
			std::fabs(Side[1]-Side[3]) < ThresholdLength && 
			angle13 < ThresholdAngle){
			return true;
		}
	}

	else{
		//Search for the longest side
		long longest= 0;
		for (long j=1; j<4; j++){
			if(Side[j]> Side[longest])
				longest= j;
		}
		//Return true if long oppossed sides are similar in legth 
		//and parallel within a threshold
		double angleLong= std::fabs(Angle[longest]-Angle[(longest+2) % 4]);
		angleLong= min(angleLong, std::fabs(angleLong-NUMBER_PI));

		if (std::fabs(Side[longest]-Side[(longest+2) % 4]) < ThresholdLength &&
			angleLong < ThresholdAngle){
			return true;
		}
	}

	return false;
}


void PairEdgesOfParallelogramFace(CFace Face,
								  std::vector<double> edgeLength,
								  long &long1, long &long2, 
								  long &short1, long &short2)
//Given a parallelogram face, returns:
//		the numbers of their two opposite long edges (long1, long2)
//		the numbers of their two opposite short edges (short1, short2)
{
	//Compare face edges' lengths
	if (edgeLength[Face.GetEdgeInFace(0)]+
		edgeLength[Face.GetEdgeInFace(2)] <
		edgeLength[Face.GetEdgeInFace(1)]+
		edgeLength[Face.GetEdgeInFace(3)]){
		long1= Face.GetEdgeInFace(1);
		long2= Face.GetEdgeInFace(3);

		short1= Face.GetEdgeInFace(0);
		short2= Face.GetEdgeInFace(2);

	}
	else{
		long1= Face.GetEdgeInFace(0);
		long2= Face.GetEdgeInFace(2);

		short1= Face.GetEdgeInFace(1);
		short2= Face.GetEdgeInFace(3);
	}
};


void ReorderToMatch(std::vector<long> &Vector1,
					std::vector<long> &Vector2,
					long Origin1, long Next1,
					long Origin2, long Next2)
//Pairs the components of two vectors

//Pairing means reordering both vectors to match two of their consecutive numbers
//Begins pairing Origin1 and Origin2, and continues pairing Next1 and Next2

//Vectors may contain the number running in opossite sense

//Vectors are assumed to have the same size!!
{
	if(Vector1.size() != Vector2.size()){
	//	CString sMensaje;
	//	sMensaje= "Failure reordering vectors: they are of different size";
	//	AfxMessageBox (sMensaje);
	//	CReferDoc::GetDoc()->RefreshWindows();

		return;
	}

	//Search for the first pair of vertices
	long pair1= -1;
	long pair2= -1;

	if(IsValueInVector(Origin1, Vector1, pair1)){
		IsValueInVector(Origin2, Vector2, pair2);
	}
	else{
		IsValueInVector(Origin2, Vector1, pair1);
		IsValueInVector(Origin1, Vector2, pair2);
	}

	//Search for the sense to find the next pair
	long sizeV1= Vector1.size();

	long Congruence1, Congruence2;
	if(Vector1[(pair1+1)%sizeV1] == Next1 ||
		Vector1[(pair1+1)%sizeV1] == Next2)
		Congruence1= +1;
	else
		Congruence1= -1;

	if(Vector2[(pair2+1)%sizeV1] == Next1 ||
		Vector2[(pair2+1)%sizeV1] == Next2)
		Congruence2= +1;
	else
		Congruence2= -1;

	//Loop to pair all the vertices
	std::vector<long> Copy1(sizeV1, 0);
	std::vector<long> Copy2(sizeV1, 0);
	for (long j=0; j<sizeV1; j++){
		//Get the j-th pair of vertices
		long jj1= (pair1+(Congruence1*j)) % sizeV1;
		if(jj1<0) jj1 += sizeV1;
		long jj2= (pair2+(Congruence2*j)) % sizeV1;
		if(jj2<0) jj2 += sizeV1;

		Copy1[j]= Vector1[jj1];
		Copy2[j]= Vector2[jj2];
	}

	Vector1= Copy1;
	Vector2= Copy2;
};


void PairVertices(CDB_Model *DDBB,
				  long connect1, long connect2,
				  long pair1, long pair2,
				  std::vector<long> &VertPair1, std::vector<long> &VertPair2)
//Gets all the vertices of two faces and "pairs" them

//Pairing means reordering both lists of vertices
//so as to get in VertPair1[j] the vertext similar to VertPair2[j]

//Pairing is done by first matching the two vertices of the segment connect1, 
//then continues matching the two vertices of the segment connect2, and so on
//Therefore, connect1 and connect2 must be edges that connect these pairs of vertices
{
	//Get the first pair of the series
	CEdge Edge1= DDBB->GetEdge(connect1);
	long Origin1= Edge1.Tail;
	long Origin2= Edge1.Head;

	//Get the second pair of the series
	CEdge Edge2= DDBB->GetEdge(connect2);
	long Next1= Edge2.Tail;
	long Next2= Edge2.Head;

	//Get the vertices of the faces to pair
	VertPair1= DDBB->GetAllVerticesInFace(DDBB->GetFace(pair1));
	VertPair2= DDBB->GetAllVerticesInFace(DDBB->GetFace(pair2));

	//Get the vertices paired
	ReorderToMatch(VertPair1, VertPair2,
				   Origin1, Next1, Origin2, Next2);
};


void PairVertices(CDB_Model *DDBB, 
				  long connectFace, long pair1, long pair2,
				  std::vector<long> &VertPair1, 
				  std::vector<long> &VertPair2)
//Pairs the vertices of two lateral faces sharing a common base face

//Pairing means reordering both lists of vertices
//so as to get in VertPair[j] the vertex conected to VertPair[j]
//through an edge of the base face

//Pairing is done by first matching the two vertices of pair 1 and 2
//connected through a common edge of the connectFace, 
//then continues matching the two vertices of pair 1 and 2
//connected through a common edge of the connectFace, and so on
//Therefore, connect1 and connect2 must be edges that connect these pairs of vertices
{
	VertPair1= DDBB->GetAllVerticesInFace(DDBB->GetFace(pair1));
	VertPair2= DDBB->GetAllVerticesInFace(DDBB->GetFace(pair2));

	CFace FaceConnect= DDBB->GetFace(connectFace);
	std::vector<long> EdgesBase= FaceConnect.GetAllEdgesInFace();

	//Identify two vertices of pair 1 and 2
	//connected through a common edge of the connectFace
	long Origin1= -1;
	long Origin2= -1;
	long OriginEdge= -1;
	for(long i= 0; i<(long)EdgesBase.size(); i++){
		CEdge edge= DDBB->GetEdge(EdgesBase[i]);
		if((IsValueInVector(edge.Head,VertPair1) && (IsValueInVector(edge.Tail, VertPair2))) ||
			(IsValueInVector(edge.Tail, VertPair1) && (IsValueInVector(edge.Head, VertPair2)))){
			Origin1= edge.Head;
			Origin2= edge.Tail;
			OriginEdge= EdgesBase[i];
			break;
		}
	}

	//Identify two consecutive vertices of pair 1 and 2
	//connected through a common edge of the connectFace
	long Next1= -1;
	long Next2= -1;
	for(long i= 0; i<(long)EdgesBase.size(); i++){
		if(EdgesBase[i]!= OriginEdge){
			CEdge edge= DDBB->GetEdge(EdgesBase[i]);
			if((IsValueInVector(edge.Head,VertPair1)&&(IsValueInVector(edge.Tail, VertPair2)))||
				(IsValueInVector(edge.Tail, VertPair1)&&(IsValueInVector(edge.Head, VertPair2)))){
				Next1= edge.Head;
				Next2= edge.Tail;
				break;
			}
		}
	}

	//Get paired vertices
	ReorderToMatch(VertPair1, VertPair2,
				   Origin1, Next1, Origin2, Next2);
}


bool GetLateralFaces(CDB_Model *DDBB, long centralFace,
					 long side1, long side2,
					 long &lateral1, long &lateral2)
//Searches for lateral faces connected to one pair of opposite edges (sides)
//of the central face

//Returns false if two lateral faces are not found
//Otherwise, returns true and the numbers of the lateral faces (lateral1, lateral2)
{
	bool lat1= false;
	bool lat2= false;

	for (long j=0; j<DDBB->GetSizeFaces(); j++){
		if(j != centralFace){
			std::vector<long> EdgesJ= DDBB->GetFace(j).GetAllEdgesInFace();
			if(!lat1 && IsValueInVector(side1,EdgesJ)){
				lateral1= j;
				lat1= true;
				if (lat2) break;
			};
			if(!lat2 && IsValueInVector(side2, EdgesJ)){
				lateral2= j;
				lat2= true;
				if (lat1) break;
			};
		}
	}

	return (lat1 && lat2);
}


bool SimilarDistances(CDB_Model *DDBB,
					  double ThresholdSimilarDistance,
					  std::vector<long> &VertInFace1,
					  std::vector<long> &VertInFace2)
//Returns true if paired vertices of given faces
//are at nearly the same distance from each other
{
	long numLines= (long)VertInFace1.size();
	if(numLines != (long)VertInFace2.size()){
		return false; //Polygonal lines are not of the same rank
	}

	//Loop to calculate lengths of edges
	double distI, distmin, distmax;
	distmin= distmax= DDBB->GetDistance2D(DDBB->GetVertex(VertInFace1[0]),
										  DDBB->GetVertex(VertInFace2[0]));

	for (long i=1; i<numLines; i++){
		distI= DDBB->GetDistance2D(DDBB->GetVertex(VertInFace1[i]),
									 DDBB->GetVertex(VertInFace2[i]));
		distmin= min(distmin, distI);
		distmax= max(distmax, distI);
	}

	return (std::abs(distmax-distmin) < (distmax*ThresholdSimilarDistance));
};


bool SimilarAngles(CDB_Model *DDBB,
				   double ThresholdSimilarAngle,
				   std::vector<long> &VertInFace1,
				   std::vector<long> &VertInFace2)
//Returns true if paired edges of polygonal lines 
//defined by consecutive vertices
//are nearly parallel to each other
{
	long numLines= (long)VertInFace1.size();
	if(numLines != (long)VertInFace2.size()){
		return false; //Polygonal lines are not of the same rank
	}

	//Loop to calculate angles of edges
	double angleI, anglemin, anglemax;
	CVertex Vertex1= DDBB->GetVertex(VertInFace1[0]);
	CVertex Vertex2= DDBB->GetVertex(VertInFace2[0]);
	POINT2D p1(Vertex1.x, Vertex1.y);
	POINT2D p2(Vertex2.x, Vertex2.y);
	anglemin= anglemax= GetAngle2D_PI(p1, p2);

	for (long i=1; i<numLines; i++){
		CVertex Vertex1i= DDBB->GetVertex(VertInFace1[i]);
		CVertex Vertex2i= DDBB->GetVertex(VertInFace2[i]);
		POINT2D p1i(Vertex1.x, Vertex1.y);
		POINT2D p2i(Vertex2.x, Vertex2.y);
		angleI= GetAngle2D_PI(p1i, p2i)
			;
		anglemin= min(anglemin, angleI);
		anglemax= max(anglemax, angleI);
	}

	return (std::abs(anglemax-anglemin) < (2*ThresholdSimilarAngle));
};


bool LateralsAreSimilar(CDB_Model *DDBB,
						double ThresholdSimilarDistance,
						double ThresholdSimilarAngle,
						long connect1, long connect2,
						long lateral1, long lateral2)
//Returns true if lateral faces lateral1 and lateral2 are quite similar (i.e. homotetic)

//Faces are similar if:
//		They have the same number of vertices
//		Vertices may be paired in pairs whose distance is always nearly the same
//		Edges may be paired in pairs whose orientation is always nearly the same
{
	//Check if lateral faces have the same number of edges
	if(DDBB->GetFace(lateral1).GetSizeEdgesInFace() != 
		DDBB->GetFace(lateral2).GetSizeEdgesInFace())
		return false;

	//Get all vertices in each face and pair them
	std::vector<long> VertInFace1;
	std::vector<long> VertInFace2;
	PairVertices(DDBB, connect1, connect2, lateral1, lateral2, VertInFace1, VertInFace2);

	//Check if paired vertices of lateral faces
	//are at nearly the same distance from each other
	if(!SimilarDistances(DDBB, ThresholdSimilarDistance, VertInFace1, VertInFace2))
		return false;

	//Check if paired edges (defined by consecutive vertices) of lateral faces
	//are nearly parallel
	if(!SimilarAngles(DDBB, ThresholdSimilarAngle, VertInFace1, VertInFace2))
		return false;

	return true;
};


bool ExternalAreOpposite(CDB_Model *DDBB, long centralFace, 
						 double ThresholdAngle,
						 long connect1, long connect2,
						 long lateral1, long lateral2)
//Returns true if
//external semiedges of one lateral face are collinear and run in opposite direction
//to the external semiedges of the paired vertex of the other lateral face
{
	bool flag_opposite= true;

	//Get all vertices in each face and pair them
	std::vector<long> VertInFaceL1;
	std::vector<long> VertInFaceL2;
	PairVertices(DDBB, connect1, connect2, lateral1, lateral2, 
				 VertInFaceL1, VertInFaceL2);

	//Loop to check all collinearities
	for (long j= 0; j<(long)VertInFaceL1.size(); j++){
		//Find the external edge e1 of vertex j in face L1
		long e1= -1;
		for (long k= 0; k<(long)DDBB->GetSizeEdgesInVertex(VertInFaceL1[j]); k++){
			long NumEdge= DDBB->GetEdgeInVertex(VertInFaceL1[j], k);

			if(!DDBB->IsEdgeInFace(centralFace, NumEdge) &&
			   !DDBB->IsEdgeInFace(lateral1, NumEdge) &&
			   !DDBB->IsEdgeInFace(lateral2, NumEdge)){
					if(e1 == -1){
						e1= NumEdge;
					}
					else{
						//More than one external edge
						e1= -1;
						break;
					}
			}
		}

		long e2= -1;
		if(e1!=-1){
		//Find the external edge e2 of vertex j in face L2
			for (long k= 0; k<(long)DDBB->GetSizeEdgesInVertex(VertInFaceL2[j]); k++){
				long NumEdge= DDBB->GetEdgeInVertex(VertInFaceL2[j], k);

				if(!DDBB->IsEdgeInFace(centralFace, NumEdge) &&
				   !DDBB->IsEdgeInFace(lateral1, NumEdge) &&
				   !DDBB->IsEdgeInFace(lateral2, NumEdge)){
						if(e2 == -1){
							e2= NumEdge;
						}
						else{
							//More than one external edge
							e2= -1;
							break;
						}
				}
			}
		}

		//If one, and only one, external edge exists in both paired vertices
		//check if they are parallel
		if (e1!= -1 && e2!=-1){
			double Angle1= DDBB->GetAngle2DX_2PI(e1, VertInFaceL1[j], Accuracy);
			double Angle2= DDBB->GetAngle2DX_2PI(e2, VertInFaceL2[j], Accuracy);
			if(std::fabs(std::fabs(Angle1-Angle2)-NUMBER_PI) > ThresholdAngle){
				flag_opposite= false;
			}
		}
	}

	return flag_opposite;
}
