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
#include <cassert>     // assert
#include <cmath>
#include <vector>
#include <algorithm>  //sort

#include "Tools_Vector.h"
#include "Tools_Geometry.h"

#include "DB_Model.h"

#include "DB_Sketch.h"
#include "StrokeCornerFinder.h"

// Database for Sketches, strokes and points
// Includes the definition of the function that translates from this database to REFER databases

//----------------------------------------------------------------------
// POINTS IN STROKE
// Stores points that belong to a stroke
// Storage includes location (a pair or real coordinates)
// plus sequence (a time parameter)

CStrokePoint::CStrokePoint()
{
}


CStrokePoint::CStrokePoint(double cx, double cy, double ctime)
// Stores coordinates of one single point sampled when moving the cursor in sketch mode 
:
x (cx),
y (cy),
time (ctime)
{
}


CStrokePoint::CStrokePoint(const CStrokePoint &IP)
// Copy constructor
:
x (IP.x),
y (IP.y),
time (IP.time)
{
}

CStrokePoint::~CStrokePoint()
{
}


bool CStrokePoint::operator==(const CStrokePoint& right) const
//Overload of == operator for CStrokePoint
{
	return (this->x==right.x && this->y==right.y);  //Only position is checked
	//return (this->x==right.x && this->y==right.y && this->time==right.time);
}


bool CStrokePoint::operator!=(const CStrokePoint& right) const
//Overload of != operator for CStrokePoint
{
	return (this->x!=right.x || this->y!=right.y);  //Only position is checked
	//return (this->x!=right.x || this->y!=right.y|| this->time!=right.time);
}


void CStrokePoint::SetPosition(double cx, double cy)
// Updates coordinates of one single point sampled when moving the cursor in sketch mode 
{
	x= cx;
	y= cy;
}


POINT2D CStrokePoint::GetPosition()
// Returns coordinates of one single point sampled when moving the cursor in sketch mode 
{
	POINT2D c;
	c.x= x;
	c.y= y;

	return c;
}


void CStrokePoint::SetTime(double ctime)
// Updates sampling time of one single point sampled when moving the cursor in sketch mode 
{
	time= ctime;
}


double CStrokePoint::GetTime()
// Returns sampling time of one single point sampled when moving the cursor in sketch mode 
{
	return time;
}


//----------------------------------------------------------------------
// STROKES


CStroke::CStroke()
// Stores information of a single stroke:
//    StrokePoints  is a vector that contains pointers to the sequence of points
//                  that belong to the same sketched stroke

// Parameters linked to its vectorization are saved inside the stroke
{
	StrokeCorners.clear();

	Segment.clear();
	MeritLine= 0.0;

	Arc.clear();
	MeritArc= 0.0;

	worstOut_x= 0.;
	worstOut_y= 0.;
	t_Out= 0.;
	worstIn_x= 0.;
	worstIn_y= 0.;
	t_In= 0.;

	Type= SOLID;
}


CStroke::CStroke(const CStroke &Stroke)
// Copy constructor
:
StrokePoints (Stroke.StrokePoints),

StrokeCorners (Stroke.StrokeCorners),

Segment (Stroke.Segment),
MeritLine (Stroke.MeritLine),

Arc (Stroke.Arc),
MeritArc (Stroke.MeritArc),

worstOut_x (Stroke.worstOut_x),   //Used for display
worstOut_y (Stroke.worstOut_y),   //Used for display
t_Out (Stroke.t_Out),
worstIn_x (Stroke.worstIn_x),   //Used for display
worstIn_y (Stroke.worstIn_y),   //Used for display
t_In (Stroke.t_In),

Type (Stroke.Type)
{
}


CStroke::~CStroke()
{
}


CStroke CStroke::operator+(const CStroke& right) const
//Overload of + operator for CStroke

//Adds the stroke "right" to the end of the current stroke
{
	CStroke Sum;
	Sum.StrokePoints= this->StrokePoints;
	for(long i= 0; i<(long)right.StrokePoints.size(); i++){
		Sum.StrokePoints.push_back(right.StrokePoints[i]);
	}
	return Sum;
}


CStroke CStroke::operator-(const CStroke& right) const
//Overload of - operator for CStroke

//Adds the stroke "right" to the end of the current stroke

//But the stroke "right" is added in reverse order!!!
{
	CStroke Sum;
	Sum.StrokePoints= this->StrokePoints;
	for(long i= (long)right.StrokePoints.size()-1; i>=0; i--){
		Sum.StrokePoints.push_back(right.StrokePoints[i]);
	}
	return Sum;
}


void CStroke::Clear()
// Clears the stroke
{
	StrokePoints.clear();
	StrokeCorners.clear();

	ClearFitsAndMerits();
}


void CStroke::AddPoint(CStrokePoint Point)
// Adds a new point to the end of the stroke

// The point is only added if it is different from the previous point
{
	long previous= StrokePoints.size()-1;
	if(previous<0 || StrokePoints[previous] != Point)
		StrokePoints.push_back(Point);
}


CStrokePoint CStroke::GetPoint(long iPoint)
// Returns the point located in position iPoint
{
	//if(iPoint<0 || iPoint>= (long)StrokePoints.size()){
	//	bool failure= true;
	//}
	assert(iPoint>=0 && iPoint< (long)StrokePoints.size());
	return StrokePoints[iPoint];
}


void CStroke::SetPoint(long iPoint, CStrokePoint Point)
// Updates the point in position iPoint
{
	//if(iPoint<0 || iPoint>= (long)StrokePoints.size()){
	//	bool failure= true;
	//}
	assert(iPoint>=0 && iPoint<(long)StrokePoints.size());
	StrokePoints[iPoint]= Point;
}


void CStroke::RemovePoint(long iPoint)
// Removes the point located in position iPoint
{
	//if(iPoint<0 || iPoint>= (long)StrokePoints.size()){
	//	bool failure= true;
	//}
	assert(iPoint>=0 && iPoint< (long)StrokePoints.size());
	StrokePoints.erase(StrokePoints.begin()+iPoint);
}


CStroke CStroke::GetSubStroke(long PositionI, long PositionF)
//Returns a substroke starting at PositionI and ending at PositionF
{
	assert(PositionF >= PositionI);
	assert(PositionI >= 0);
	assert(PositionF <= (long)StrokePoints.size());

	CStroke SubStroke;

	for (long i=PositionI; i<=PositionF; i++){
		SubStroke.AddPoint(GetPoint(i));
	}

	return SubStroke;
}


long CStroke::GetNumPoints()
// Returns the number of point in the stroke
{
	return StrokePoints.size();
}


RECTANGLE2D CStroke::GetBounds()
// Finds the bounding rectangle of the current stroke

// Since the stroke may be irregular, 
//the bounds are calculated by checking all the points of the stroke

//(i.e. the tips are NOT assumed to be the limits of the bounding box)
{
	RECTANGLE2D bound;

	long n= StrokePoints.size();
	if (n==0){
		bound.left= bound.right= bound.bottom= bound.top= 0;
		return bound;
	}

	POINT2D c= StrokePoints[0].GetPosition();
	bound.left  = bound.right = c.x;
	bound.bottom= bound.top   = c.y;

	for (long ipoint=1; ipoint<n; ipoint++){
		c= StrokePoints[ipoint].GetPosition();
		bound.left  = min(bound.left, c.x);
		bound.right = max(bound.right, c.x);
		bound.bottom= min(bound.bottom, c.y);
		bound.top   = max(bound.top, c.y);
	}

	return bound;
}


double CStroke::GetLength()
//Returns the length of the stroke
//calculated adding the lengths of the chords
//that connect all the consecutive points
{
	double Length= 0.0;
	POINT2D J= StrokePoints[0].GetPosition();
	long numP= StrokePoints.size();
	for (long j=1; j<numP; j++){
		POINT2D JJ= StrokePoints[j].GetPosition();
		Length+= MODULE_2D(J, JJ);
		J= JJ;
	}

	return Length;
}


double CStroke::GetLength(long numIntervals)
//Returns the approximate length of the stroke without oscillations

//The length is calculated considering only the lengths
//of "numIntervals" chords

//A small value of numIntervals (typically 10) is used
//to find the approximate length of the undulating arc
//that should result after removing high frequency oscillations
//from the original arc

//A too small value of numIntervals would also remove undulations
//and even the underlying curvature of the stroke
{
	double Length= 0.0;
	long numP= StrokePoints.size();

	if(numIntervals == 1){
		POINT2D First= StrokePoints[0].GetPosition();
		POINT2D Last= StrokePoints[numP-1].GetPosition();

		SEGMENT2D S(First, Last);
		Length= MODULE_2D(S.TipBegin, S.TipEnd);
	}

	else{
		POINT2D J= StrokePoints[0].GetPosition();
		long delta= max(1, (long) numP/numIntervals);
		long j;
		for (j=delta-1; j<numP; j=j+delta){
			POINT2D JJ= StrokePoints[j].GetPosition();
			Length+= MODULE_2D(J, JJ);
			J= JJ;
		}
		if(j < numP-1){
			POINT2D JJ= StrokePoints[numP-1].GetPosition();
			Length+= MODULE_2D(J, JJ);
		}
	}

	return Length;
}


double CStroke::GetSlope180_180()
//Returns the slope of the segment that connects the endpoints of the stroke

//Slope is returned in the range [-PI, PI]

//Returns null slope if both endpoints are coincident
{
	POINT2D ini= GetPoint(0).GetPosition();
	POINT2D end= GetPoint(GetNumPoints()-1).GetPosition();
	if(MODULE_2D(ini, end) == 0.)
		return 0.;

	return atan2(end.y-ini.y, end.x-ini.x);   //Angle in the range [-PI, PI]
}


double CStroke::GetSlope180()
//Returns the slope of the segment that connects the endpoints of the stroke

//Slope is returned in the range [0, PI]

//Returns null slope if both endpoints are coincident
{
	double slope180= GetSlope180_180();   //Angle in the range [-PI, PI]
	if(slope180 < 0) slope180+= NUMBER_PI;  //Angle in the range [0, PI]
	return slope180;
}


double CStroke::GetMaxSegment()
//Returns the length of the longer segment
//connecting two consecutive points of the stroke
{
	double maxLength= 0.0;
	POINT2D J= StrokePoints[0].GetPosition();
	long numP= StrokePoints.size();
	for (long j=1; j<numP; j++){
		POINT2D JJ= StrokePoints[j].GetPosition();
		maxLength= max(maxLength, MODULE_2D(J, JJ));
		J= JJ;
	}

	return maxLength;
}


POINT2D CStroke::GetStrokeCentroid()
//Returns the centroid of the stroke
{
	double sumx= 0, sumy= 0, sumx2= 0, sumy2= 0, sumxy= 0;
	long numP= StrokePoints.size();
	for (long j=0; j<numP; j++){
		POINT2D J= StrokePoints[j].GetPosition();
		sumx+= J.x;
		sumy+= J.y;
	}
	POINT2D Centroid(sumx/numP, sumy/numP);
	return Centroid;
}


//std::vector<std::vector<double>> CStroke::GetNormaliseMatrix()
//// Returns the matrix to normalise the points of a stroke
//// so that their mean distance from their centroid is sqrt(2)
//// and the centroid is the origin
//
//// This process typically improves the conditioning of any equations
////  used to solve homographies, fundamental matrices etc.
//{
//	//Centroid
//	POINT2D c= this->GetStrokeCentroid();
//
//	//Scale so that the mean distance from the centroid to a point equals sqrt(2)
//	double meandist= 0.;
//	long numP= StrokePoints.size();
//	for(long i=0; i<numP; i++){
//		POINT2D I= StrokePoints[i].GetPosition();
//		double px= I.x - c.x;
//		double py= I.y - c.y;
//		meandist+= sqrt(px*px + py*py);
//	}
//	meandist/= (double)numP;
//	double scale = sqrt(2.)/meandist;
//
//	//Normalisation matrix
//	std::vector<std::vector<double>> N;
//	N[0][0] = scale;   N[0][1] = 0.;      N[0][2] = -scale*c.x;
//	N[1][0] = 0.;      N[1][1] = scale;   N[1][2] = -scale*c.y;
//	N[2][0] = 0.;      N[2][1] = 0.;      N[2][2] = 1.;
//
//	return N;
//}


bool CStroke::IsClockwise()
//Returns true if the points of the stroke are sequenced clockwise
//and false if they are sequenced counter-clockwise

//(Calculation is done by the shoelace area formula)
{
	//Detect if stroke points are in clockwise order
	long numP= (long)StrokePoints.size();
	double signedArea= 0;
	for (long i=0; i<numP; i++){
		POINT2D Current= StrokePoints[i].GetPosition();
		POINT2D Next= StrokePoints[(i+1)%numP].GetPosition();
		signedArea+= (Next.x-Current.x) * (Next.y+Current.y);
	}

	return(signedArea > 0);
}



void CStroke::TurnIntoConvexHull()
//Modifies the input stroke to return its convex hull

//First determine if the stroke is clockwise or counter-clockwise
//(by the shoelace area formula)

//Then concave points are sequentially detected and removed
{
	//Detect if stroke points are in clockwise order
	long numP= (long)StrokePoints.size();
	double signedArea= 0;
	for (long i=0; i<numP; i++){
		POINT2D Current= StrokePoints[i].GetPosition();
		POINT2D Next= StrokePoints[(i+1)%numP].GetPosition();
		signedArea+= (Next.x-Current.x) * (Next.y+Current.y);
	}

	bool Clockwise= IsClockwise();

	//Sequentially remove concave points
	//(points to the right of the line defined by
	//the previous and next points if sequence is clockwise,
	//or to the left is sequence is counter clockwise)
	bool repeat= true;
	POINT2D Previous= GetPoint(0).GetPosition();
	while(repeat){
		repeat= false;
		for (long i=1; i<(long)StrokePoints.size()-1; i++){
			POINT2D Current= StrokePoints[i].GetPosition();
			POINT2D Next= StrokePoints[i+1].GetPosition();

			double product= (Next.x-Previous.x)*(Current.y-Previous.y) - (Next.y-Previous.y)*(Current.x-Previous.x);

			if((Clockwise && product < 0.) || (!Clockwise && product >0.)){
				RemovePoint(i);
				i--;
				repeat= true;
			}
			else
				Previous= Current;
		}
	}
}


void CStroke::Translate(POINT2D distance)
//Translates the stroke
{
	for (long i=0; i<(long)StrokePoints.size(); i++){
		POINT2D P= StrokePoints[i].GetPosition();
		//double time= StrokePoints[i].GetTime();

		P.x += distance.x;
		P.y += distance.y;

		StrokePoints[i].SetPosition(P.x, P.y);
		//StrokePoints[i].SetTime(time);
	}
}


void CStroke::Scale(POINT2D Centre, POINT2D Scale)
//Scales the stroke around Centre
{
	for (long i=0; i<(long)StrokePoints.size(); i++){
		POINT2D P= StrokePoints[i].GetPosition();
		//double time= StrokePoints[i].GetTime();

		POINT2D Ps;
		Ps.x= Centre.x + (P.x-Centre.x)*Scale.x;
		Ps.y= Centre.y + (P.y-Centre.y)*Scale.y;

		StrokePoints[i].SetPosition(Ps.x, Ps.y);
		//StrokePoints[i].SetTime(time);
	}
}


void CStroke::Rotate(POINT2D Centre, double angle)
//Rotates the stroke an angle alpha around Centre
{
	for (long i=0; i<(long)StrokePoints.size(); i++){
		POINT2D P= StrokePoints[i].GetPosition();
		//double time= StrokePoints[i].GetTime();

		POINT2D Pr;
		Pr.x= Centre.x+((P.x-Centre.x)*cos(angle)-(P.y-Centre.y)*sin(angle));
		Pr.y= Centre.y+((P.x-Centre.x)*sin(angle)+(P.y-Centre.y)*cos(angle));

		StrokePoints[i].SetPosition(Pr.x, Pr.y);
		//StrokePoints[i].SetTime(time);
	}
}


void CStroke::Filter(long FilterGap)
//Removes one of every "FilterGap" points of the stroke
{
	long SizeI= (long)StrokePoints.size();
	for (long i=SizeI-2; i>FilterGap; i-=FilterGap){
		RemovePoint(i);
	}
}


bool CStroke::Filter(double maxDistance)
//Removes alternate points of the stroke
//But only if the distance between consecutive remaining points
//is smaller than "maxDistance"
{
	bool filtered= false;
	long Last= (long)StrokePoints.size()-1;

	for (long i=Last-1; i>=2; i--){
		POINT2D Previous= StrokePoints[i+1].GetPosition();
		POINT2D Current= StrokePoints[i].GetPosition();
		if(MODULE_2D(Previous, Current) < maxDistance){
			RemovePoint(i);
			filtered= true;
			i--;
		}
	}
	return filtered;
}


bool CStroke::Resample(long NumResampledPoints)
//Resamples the stroke to get the same stroke with
//"NumResampledPoints" nodes approximately equally distanced

//WARNING: note that chords have dissimilar lengths
//(even if archs are the same length),
//when curvature changes abruptly
{
	if((long)StrokePoints.size() <= NumResampledPoints ||
	   NumResampledPoints < 3)
		return false;

	//Add first point
	std::vector<CStrokePoint> New;
	New.push_back(StrokePoints[0]);

	//Loop to add intermediate points
	double Length= GetLength();
	double Interval= Length/(NumResampledPoints-1);

	long Last= (long)StrokePoints.size()-1;
	double accLength= 0.;
	POINT2D Previous= StrokePoints[0].GetPosition();

	for (long i=1; i<=Last; i++){
		POINT2D Current= StrokePoints[i].GetPosition();
		accLength+= MODULE_2D(Previous, Current);
		if(accLength > Interval){
			//Interpolate current point
			double delta= (accLength-Interval)/MODULE_2D(Previous, Current);
			Current.x-= (Current.x - Previous.x)*delta;
			Current.y-= (Current.y - Previous.y)*delta;
			double Current_time= StrokePoints[i].GetTime();
			Current_time-= (Current_time-StrokePoints[i-1].GetTime())*delta;
			CStrokePoint sp(Current.x, Current.y, Current_time);
			New.push_back(sp);
			accLength= 0.;
			i--;
		}
		else if(accLength == Interval){
			New.push_back(StrokePoints[i]);
			accLength= 0.;
		}
		Previous= Current;
	}

	//Add last point
	if(std::fabs(Interval-accLength) < Interval/100.)
		New.push_back(StrokePoints[Last]);

	StrokePoints= New;
	assert(StrokePoints.size() == NumResampledPoints);
	return true;
}


long CStroke::FindCorners(long StrokeCornersApproach)
//Finds the corners of the stroke
//and returns the number of corners found

//The two endpoints are not counted in the number of corners

//Corners (including endpoints) are stored in vector "StrokeCorners"
{
	StrokeCorners.clear();

	CStrokeCornerFinder CF;
	CF.FindStrokeCorners(*this, StrokeCorners, StrokeCornersApproach);

	return (StrokeCorners.size()-2);
}


long CStroke::NumCorners()
//Returns the number of (internal) corners of the stroke
//discounting the two tips
{
	return (StrokeCorners.size()-2);
}


void CStroke::ClearFitsAndMerits()
// Clears the fits and merits of the stroke
{
	Segment.clear();
	MeritLine= 0.0;

	Arc.clear();
	MeritArc= 0.0;

	worstOut_x= 0.;
	worstOut_y= 0.;
	t_Out= 0.;
	worstIn_x= 0.;
	worstIn_y= 0.;
	t_In= 0.;
}


//----------------------------------------------------------------------
// SKETCHES


CDB_Sketch::CDB_Sketch()
// Stores information of a set of strokes

// Constants for the LineRectangleIntersection algorithm
:INSIDE (0), LEFT (1), RIGHT (2), BOTTOM (4), TOP(8)
// 0000      0001       0010        0100     1000
{
}


CDB_Sketch::CDB_Sketch(const CDB_Sketch &Sketch)
:
SketchStrokes (Sketch.SketchStrokes),

// Constants for the LineRectangleIntersection algorithm
INSIDE (0), LEFT (1), RIGHT (2), BOTTOM (4), TOP(8)
// 0000      0001       0010        0100     1000

{
}


CDB_Sketch::~CDB_Sketch()
{
}


int CDB_Sketch::ComputeOutCode(double x, double y,
								 double xmin, double ymin, double xmax, double ymax)
// Auxiliar of Cohen–Sutherland clipping algorithm
//	Compute the bit code for a point (x, y) using the clip rectangle
//	bounded diagonally by (xmin, ymin), and (xmax, ymax)
{
	int code= INSIDE; // initialised as being inside of clip window
 
	if (x < xmin)           // to the left of clip window
		code |= LEFT;
	else if (x > xmax)      // to the right of clip window
		code |= RIGHT;
	if (y < ymin)           // below the clip window
		code |= BOTTOM;
	else if (y > ymax)      // above the clip window
		code |= TOP;
 
	return code;
}


bool CDB_Sketch::LineRectangleIntersection(double x0, double y0, double x1, double y1,
											 double xmin, double ymin, double xmax, double ymax)
// Cohen–Sutherland clipping algorithm clips a line from
// P0 = (x0, y0) to P1 = (x1, y1) against a rectangle with 
// diagonal from (xmin, ymin) to (xmax, ymax)

// Function modified to just return true if the line intersects the rectangle, and false otherwise
{
	// compute outcodes for P0, P1, and whatever point lies outside the clip rectangle
	int outcode0 = ComputeOutCode(x0, y0, xmin, ymin, xmax, ymax);
	int outcode1 = ComputeOutCode(x1, y1, xmin, ymin, xmax, ymax);
	bool intersect = false;
 
	while (true) {
		if (!(outcode0 | outcode1)) {     //logical or is 0. Trivially accept and get out of loop
			intersect = true;
			break;
		}
		else if (outcode0 & outcode1) { //logical and is not 0. Trivially reject and get out of loop
			break;
		}
		else {
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			double x, y;

			// At least one endpoint is outside the clip rectangle; pick it.
			int outcodeOut = outcode0? outcode0 : outcode1;

			// Now find the intersection point;
			// use formulas y = y0 + slope * (x - x0), x = x0 + (1 / slope) * (y - y0)
			if (outcodeOut & TOP) {           // point is above the clip rectangle
				x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
				y = ymax;
			} else if (outcodeOut & BOTTOM) { // point is below the clip rectangle
				x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
				y = ymin;
			} else if (outcodeOut & RIGHT) {  // point is to the right of clip rectangle
				y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
				x = xmax;
			} else if (outcodeOut & LEFT) {   // point is to the left of clip rectangle
				y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
				x = xmin;
			}
			// Now we move outside point to intersection point to clip
			// and get ready for next pass
			if (outcodeOut == outcode0) {
				x0 = x;
				y0 = y;
				outcode0 = ComputeOutCode(x0, y0, xmin, ymin, xmax, ymax);
			}
			else {
				x1 = x;
				y1 = y;
				outcode1 = ComputeOutCode(x1, y1, xmin, ymin, xmax, ymax);
			}
		}
	}

	return intersect;
}


void CDB_Sketch::Clear()
// Clears the strokes of the sketch
{
	SketchStrokes.clear();
}


void CDB_Sketch::CopyFrom(CDB_Sketch *Origin)
// Copies the strokes of the sketch Origin into the current sketch
{
	SketchStrokes= Origin->SketchStrokes;
}


void CDB_Sketch::AddStroke(CStroke Stroke)
// Adds a new stroke to the end of the sketch
{
	SketchStrokes.push_back(Stroke);
}


void CDB_Sketch::InsertStroke(long iPosition, CStroke Stroke)
// Inserts a new stroke following current ith stroke
{
	SketchStrokes.insert(SketchStrokes.begin()+iPosition+1, Stroke);
}


CStroke CDB_Sketch::GetStroke(long iStroke)
// Returns the stroke located in position iStroke
{
	assert(iStroke>=0 && iStroke<(long)SketchStrokes.size());
	return SketchStrokes[iStroke];
}


void CDB_Sketch::SetStroke(long iStroke, CStroke Stroke)
// Updates the stroke located in position iStroke
{
	assert(iStroke>=0 && iStroke< (long)SketchStrokes.size());
	SketchStrokes[iStroke]= Stroke;
}


void CDB_Sketch::erase(long istroke)
// Erases the stroke istroke:
{
	SketchStrokes.erase(SketchStrokes.begin()+istroke);
}


bool CDB_Sketch::EraseSelectedStrokes(RECTANGLE2D rect)
// Erase all strokes whose diagonal intersects the enclosing rectangle delimited by (xmin, ymin, xmax, ymax)

// Returns true if at least one stroke is erased; false otherwise
{
	bool erased= false;

	long numS= (long)GetNumStrokes();
	if (numS==0){
		return erased;
	}

	// Loop over the current strokes to erase strokes whose diagonal is intersected by the gesture
	// (Loop back to prevent rearranging strokes not yet checked)
	for (long istroke= numS-1; istroke>= 0; istroke--){
		CStroke StrokeI= GetStroke(istroke);
		CStrokePoint PointI= StrokeI.GetPoint(0);
		POINT2D tail= PointI.GetPosition();

		long position= StrokeI.GetNumPoints()-1;
		CStrokePoint PointII= StrokeI.GetPoint(position);
		POINT2D head= PointII.GetPosition();

		if (LineRectangleIntersection(head.x, head.y, tail.x, tail.y,
									  rect.left, rect.bottom, rect.right, rect.top)){
			//Erase stroke intersected by the gesture
			erased= true;
			erase(istroke);
		}
	}
	return erased;
}


long CDB_Sketch::GetNumStrokes()
// Returns the number of strokes in the sketch
{
	return SketchStrokes.size();
}


RECTANGLE2D CDB_Sketch::GetBounds(long istroke)
//Finds the bounding rectangle of the stroke istroke

//Searches all the sub-strokes
{
	//RECTANGLE2D bound(0, 0, 0, 0);
	//if(istroke <0 || istroke >= (long)SketchStrokes.size())
	//	return bound;
	assert(istroke>=0 && istroke<(long)SketchStrokes.size());

	return SketchStrokes[istroke].GetBounds();
}


RECTANGLE2D CDB_Sketch::GetBounds()
//Finds the bounding rectangle of the current full sketch
{
	RECTANGLE2D bound(0, 0, 0, 0);
	if(SketchStrokes.size() == 0) return bound;

	bound= GetBounds(0);

	for (long istroke= 1; istroke<(long)SketchStrokes.size(); istroke++){
		RECTANGLE2D rect= GetBounds(istroke);

		bound.left  = min(bound.left, rect.left);
		bound.right = max(bound.right, rect.right);
		bound.bottom= min(bound.bottom, rect.bottom);
		bound.top   = max(bound.top, rect.top);
	}

	return bound;
}


POINT2D CDB_Sketch::GetSketchCentroid()
// Returns the centroid (centre of mass) of the current sketch
{
	POINT2D com(0.0 , 0.0);
	long numpoints= 0;
	for (long istroke= 0; istroke<(long)SketchStrokes.size(); istroke++){
		CStroke StrokeI= GetStroke(istroke);
		for (long ipoint=0; ipoint<StrokeI.GetNumPoints(); ipoint++){
			POINT2D c= StrokeI.GetPoint(ipoint).GetPosition();
			com.x+= c.x,
			com.y+= c.y;
			numpoints++;
		}
	}
	com.x= com.x/(double)numpoints;
	com.y= com.y/(double)numpoints;

	return com;
}


POINT2D CDB_Sketch::GetSubSketchCentroid(std::vector<long> Selection)
// Returns the centroid (centre of mass) of the selected strokes of the sketch
{
	POINT2D com(0.0 , 0.0);
	long numpoints= 0;
	for (long istroke= 0; istroke<(long)SketchStrokes.size(); istroke++){
		if(IsValueInVector(istroke, Selection)){
			CStroke StrokeI= GetStroke(istroke);
			for (long ipoint=0; ipoint<StrokeI.GetNumPoints(); ipoint++){
				POINT2D c= StrokeI.GetPoint(ipoint).GetPosition();
				com.x+= c.x,
				com.y+= c.y;
				numpoints++;
			}
		}
	}
	com.x= com.x/(double)numpoints;
	com.y= com.y/(double)numpoints;

	return com;
}


void CDB_Sketch::Translate(long NumStroke, POINT2D distance)
//Translates all the sub-strokes of the stroke NumStroke
{
	assert(NumStroke>=0 && NumStroke<(long)SketchStrokes.size());

	SketchStrokes[NumStroke].Translate(distance);
}


void CDB_Sketch::Rotate(long NumStroke, POINT2D Centre, double angle)
//Rotates all the sub-strokes of the stroke NumStroke

//WARNING!! Every substroke should be rotated relative to its own centre
{
	assert(NumStroke>=0 && NumStroke<(long)SketchStrokes.size());

	SketchStrokes[NumStroke].Rotate(Centre, angle);
}


void CDB_Sketch::Scale(long NumStroke, POINT2D Centre, POINT2D Scale)
//Rotates all the sub-strokes of the stroke NumStroke

//WARNING!! Every substroke should be scaled relative to its own centre
{
	assert(NumStroke>=0 && NumStroke<(long)SketchStrokes.size());

	SketchStrokes[NumStroke].Scale(Centre, Scale);
}


void CDB_Sketch::EraseSketchCorners()
// Erases the corners of the strokes
{
	for (long istroke= 0; istroke<(long)SketchStrokes.size(); istroke++){
		CStroke StrokeI= GetStroke(istroke);
		StrokeI.StrokeCorners.clear();
		SetStroke(istroke, StrokeI);
	}
}


void CDB_Sketch::EraseSketchFitsAndMerits()
// Erases the fits and merits of the strokes
{
	for (long istroke= 0; istroke<(long)SketchStrokes.size(); istroke++){
		CStroke StrokeI= GetStroke(istroke);
		StrokeI.ClearFitsAndMerits();
		SetStroke(istroke, StrokeI);
	}
}
