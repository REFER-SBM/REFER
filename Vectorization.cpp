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
#include <limits>   //_isnan, numeric_limits, max_exponent
#include <cmath>    // sqrt
#include <vector>
#include <algorithm>  //sort

#include "Tools_Vector.h"
#include "Tools_Geometry.h"
#include <complex>
#include "Tools_Matrix.h"

#include "DB_Model.h"
#include "DB_CSG.h"
#include "Refer.h"
#include "ReferDoc.h"

#include "DB_Sketch.h"
#include "CIRecognizer.h"   // CALI library

#include "resource.h"
#include "DialogProgress.h"

#include "Vectorization.h"

// Recognition approaches
#define REFER	0
#define CALI	1


#define CROSS(a, b, ab) ab[0] = a[1]*b[2] - a[2]*b[1]; \
						ab[1] = a[2]*b[0] - a[0]*b[2]; \
						ab[2] = a[0]*b[1] - a[1]*b[0];


//----------------------------------------------------------------------------------


//CFitLine::CFitLine()
////Initialize with default parameters
//{
//	FitLineApproach= 0;       //0= Regression line, 1= Connect tips
//	RemoveTails= true;        //Remove tails from the tips of the strokes that depict straight lines
//	MaxTrimPointsInTail= 5;   //remove up to five points in the tail
//	MinAngleInTail= 45;       //Select as tail to those bends rotated more that this min angle
//
//	LineTolMin= 0.035;
//	LineTolMax= 0.100;
//	LineSmoothingPenalty= 0.10;
//
//	StrokeCornersApproach= 0; //0= automatic, 1= IStraw, 2= Silding Strips
//
//	//MinMeritForLine= 0.5;     //Acceptance criteria for fitting straigth lines under REFER approach
//}


CFitLine::CFitLine(long FitLApproach,
				   bool Remove_Tails,
				   long Max_Trim_Points_In_Tail,
				   double Min_Angle_In_Tail,
				   double LTMin, double LTMax, double SPenalty)
//Initialize with input parameters
: FitLineApproach (FitLApproach),
  RemoveTails (Remove_Tails),
  MaxTrimPointsInTail (Max_Trim_Points_In_Tail),
  MinAngleInTail (Min_Angle_In_Tail),

  LineTolMin(LTMin),
  LineTolMax (LTMax),
  LineSmoothingPenalty (SPenalty)
{
	StrokeCornersApproach= 0; //0= automatic, 1= IStraw, 2= Silding Strips
}


CFitLine::~CFitLine()
{
}


void CFitLine::GetRegressionFit(CStroke &InputStroke)
//Get a line of best fit, as a line that passes as close as possible to the points of the stroke

//Coordinates are swapped for lines with slope greater than 1, 
//to prevent the well known ill-working of the regression for vertical lines

//The endpoints (Segment.TipBegin, Segment.TipEnd) are calculated as the closer points of the line
//to the respective sides of the bounding box of the stroke
//(They are calculated as intersection points between the regression line
//and its perpendiculars passing through the "regression" tips of the stroke)
{
	long n= InputStroke.GetNumPoints();
	if(n == 0) return;

	//Regression tips
	POINT2D Centroid= InputStroke.GetStrokeCentroid();
	double slope180_180= InputStroke.GetSlope180_180();  //WARNING: Approximate slope (based on tips' location)!!!

	CStroke RotatedStroke= InputStroke;
	RotatedStroke.Rotate(Centroid, -slope180_180);

	RECTANGLE2D bounds= RotatedStroke.GetBounds(); //Approximate minimum bounding box

	POINT2D First(bounds.left, ((bounds.top+bounds.bottom)/2.));
	First= First.Rotate(Centroid, slope180_180);

	POINT2D Last(bounds.right, ((bounds.top+bounds.bottom)/2.));
	Last= Last.Rotate(Centroid, slope180_180);

	//Swap coordinates to prevent calculating regressions of nearly vertical lines
	bool proneToVertical= (std::abs(First.x-Last.x) < std::abs(First.y-Last.y));
	if(proneToVertical){
		std::swap(First.x, First.y);
		std::swap(Last.x, Last.y);
	}

	//Regression coefficients
	double xSum = 0., ySum = 0., xxSum = 0., xySum = 0.;

	if(proneToVertical){
		//Swap coordinates for nearly vertical lines
		for (long i = 0; i < n; i++){
			POINT2D Pi= InputStroke.GetPoint(i).GetPosition();
			xSum += Pi.y;
			ySum += Pi.x;
			xxSum += Pi.y * Pi.y;
			xySum += Pi.y * Pi.x;
		}
	}
	else{
		for (long i = 0; i < n; i++){
			POINT2D Pi= InputStroke.GetPoint(i).GetPosition();
			xSum += Pi.x;
			ySum += Pi.y;
			xxSum += Pi.x * Pi.x;
			xySum += Pi.x * Pi.y;
		}
	}

	double denom= (n * xxSum - xSum * xSum);

	if (denom == 0.){   //Should not occur after swapping!!!
		//AfxMessageBox("Vertical (or zero length) line");
		InputStroke.Segment.TipBegin.x = First.x;
		InputStroke.Segment.TipBegin.y = First.y;
		InputStroke.Segment.TipEnd.x = Last.x;
		InputStroke.Segment.TipEnd.y = Last.y;
	}
	else{
		//Regression line
		double slope = (n * xySum - xSum * ySum) / denom;
		double intercept = (ySum - slope * xSum) / n;

		//if(proneToVertical){
		//	//Un-swap before calculating endpoints
		//	slope= 1/slope;
		//	intercept= -intercept/slope;
		//}

		//Calculate endpoints of the regression line
		if (slope != 0.){
			double slopeP = -1/slope;
			double interceptFirst = First.y - slopeP*First.x;
			InputStroke.Segment.TipBegin.x = (interceptFirst - intercept) / (slope - slopeP);
			InputStroke.Segment.TipBegin.y = slope * InputStroke.Segment.TipBegin.x + intercept;

			double interceptLast = Last.y - slopeP*Last.x;
			InputStroke.Segment.TipEnd.x = (interceptLast - intercept) / (slope - slopeP);
			InputStroke.Segment.TipEnd.y = slope * InputStroke.Segment.TipEnd.x + intercept;
		}
		else{
			//AfxMessageBox("Horizontal line");
			InputStroke.Segment.TipBegin.x = First.x;
			InputStroke.Segment.TipBegin.y = First.y;
			InputStroke.Segment.TipEnd.x = Last.x;
			InputStroke.Segment.TipEnd.y = Last.y;
		}
	}

	if(proneToVertical){
		//Un-swap endpoint coordinates
		std::swap(InputStroke.Segment.TipBegin.x, InputStroke.Segment.TipBegin.y);
		std::swap(InputStroke.Segment.TipEnd.x, InputStroke.Segment.TipEnd.y);
	}
}


void CFitLine::FindTips(CStroke &InputStroke, 
						bool RemoveTails, long MaxTrimPointsInTail , double MinAngleInTail)
//Finds the tips of the stroke, 
//and stores them as InputStroke.Segment.TipBegin and InputStroke.Segment.TipEnd

//In principle, the tips of the stroke should be their endpoints.
//However, as a result of unnoticed hand movements while pen down or pen up,
//some users tend to produce undesired "tails" at the endpoints of the strokes
//To prevent failures derived from those incorrect tips, 
//strokes are tidied up here to ignore the tails

//Tips may be corrected in two ways:
//	- If a corner is found near the tip, the corner replaces the tip as the endpoint of the line

//	- Otherwise, if neighboring points define segments 
//	  with steep angles to the segment joining the vertices, 
//	  then those neighbors replace the tips

//		The procedure is as follows:
//		   - Line segment connecting stroke points one and two is compared against
//		     the line connecting second and last points of the stroke.
//		   - If the angle between both lines is smaller than a threshold,
//		     the first point of the stroke is accepted as the first endpoint
//		   - Otherwise, it is replaced by the second point, which is then checked
//		   - The procedure is repeated until the prescribed number of points is checked,
//		     or one of them does not define a bend line

//		   - The procedure is repeated for the other endpoint
{
	long numPoints= InputStroke.GetNumPoints();

	//Initialize the tips as the first and last points of the stroke
	InputStroke.Segment.TipBegin= InputStroke.GetPoint(0).GetPosition();
	InputStroke.Segment.TipEnd= InputStroke.GetPoint(numPoints-1).GetPosition();

	//Recalculate tips by removing approximate tails
	if(RemoveTails){
		long MaxTrim= min(MaxTrimPointsInTail, (numPoints-2)/2);  //Avoid trimming strokes with few points
		if(MaxTrim > 0){
			double tailThreshold= 180.0 - MinAngleInTail;
			tailThreshold= DEGREES_TO_RADIANS(tailThreshold);

			for (long i= MaxTrim; i>0; i--){
				POINT2D NewBegin= InputStroke.GetPoint(i).GetPosition();
				if(GetAngle2D_PI(InputStroke.Segment.TipBegin, NewBegin, InputStroke.Segment.TipEnd) < tailThreshold){
					InputStroke.Segment.TipBegin= NewBegin;
					break;
				}
			}

			for (long i= MaxTrim; i>0; i--){
				POINT2D NewEnd= InputStroke.GetPoint(numPoints-1-i).GetPosition();
				if(GetAngle2D_PI(InputStroke.Segment.TipBegin, NewEnd, InputStroke.Segment.TipEnd) < tailThreshold){
					InputStroke.Segment.TipEnd= NewEnd;
					break;
				}
			}
		}
	}
}


double CFitLine::Obliqueness(CStroke InputStroke)
//Returns the value of obliqueness
//The output is the obliqueness, in the range [0, 1]
//		Obliq is 0 for slope 0 (horizontal or vertical lines)
//		Obliq is 1 for slope= PI/4.
{
	POINT2D ini= InputStroke.GetPoint(0).GetPosition();
	POINT2D end= InputStroke.GetPoint(InputStroke.GetNumPoints()-1).GetPosition();
	if(MODULE_2D(ini, end) == 0.)
		return 0.;

	double slope= std::fabs(atan2(end.y-ini.y, end.x-ini.x));  //return angle in the range [0, PI]

	if (slope>(NUMBER_PI/4))
		slope= (NUMBER_PI/2)-slope;

	return slope/(NUMBER_PI/4);
}


long CFitLine::SmoothStroke(CStroke &Stroke,
							  double ToleranceWidth, double widthInitial)
//Returns a "smoothed" stroke
//Smoothing consists in removing odd points of the stroke

//Also returns the number of smoothing stages

//The goal is removing high frequency oscillations
//As they are small, repeatedly removing odd stroke points makes them eventually dissapear

//Smoothing ends when the smoothed stroke:
//	1. does contain no more corners, or
//	2. becomes significantly narrow than the original one

//Odd point that are too distant from ther neighbors are not removed
//First and last points are not removed
{
	long NumSmoothingSteps= 0;

	Stroke.FindCorners(StrokeCornersApproach);
	if(Stroke.NumCorners() <= 0)
		return NumSmoothingSteps;

	while(true){
		CStroke TempStroke= Stroke;
		if(!TempStroke.Filter(2*ToleranceWidth))    // Tuning Parameter
			break;  //No more filtering allowed
		NumSmoothingSteps++;

		RECTANGLE2D boundsF= TempStroke.GetBounds();
		double widthSmoothed= boundsF.top-boundsF.bottom;

		if(std::fabs(widthInitial-widthSmoothed) >= ToleranceWidth)    // Tuning Parameter
			break;  // Already significantly marrow than the original one
		else{
			Stroke= TempStroke;
			if(Stroke.FindCorners(StrokeCornersApproach) <= 0)
				break;  //No more corners to smooth
		}
	}

	return NumSmoothingSteps;
}


void CFitLine::GetStraightFit(CStroke &InputStroke)
// Fit the stroke into a segment defined by the endpoints of the stroke

//Straight line is calculed as one of the folloging segments:
//	- contained in the regression line, 
//	  and delimited by the points of the regression line closer to the bounding box

//	- connecting the tips of the stroke
{
	if(FitLineApproach == 0){
		//Fit line to the regression line of the points of the stroke
		GetRegressionFit(InputStroke);
	}
	else if(FitLineApproach == 1){
		//Get the line connecting the tips of the stroke
		//(Tails are refined depending on the configuration)
		FindTips(InputStroke, RemoveTails, MaxTrimPointsInTail, MinAngleInTail);
	}
	else {
		AfxMessageBox("The currently selected approach to fit lines is not implemented");
	}

	//Update stroke parameters
	InputStroke.Segment.Slope180= GetAngle2D_PI(InputStroke.Segment.TipBegin, InputStroke.Segment.TipEnd);  //Slope of the segment that fits the stroke
	InputStroke.Segment.Length= MODULE_2D(InputStroke.Segment.TipBegin, InputStroke.Segment.TipEnd);  //Length of the segment that fits the stroke
}


double CFitLine::GetMeritLine(CStroke &Stroke)
//Returns a figure of merit that measures the likeliness of the full stroke
//to be accepted as a single straight line defined by "Stroke.Segment"

//The merit equals 1 if the shape is an straight line
//that connects both tips of the stroke, while
//equals 0 when the shape is highly non-straight

//Stroke is accepted as representing a straight line if:
//		1. Fits into a minimum tolerance band,
//or, alternatively:
//		2. Its smoothed shape fits into a maximum (or "relaxed") tolerance band.

//If stroke is smoothed to remove oscillations,
//then the merit of the smoothed stroke is reduced by a SmoothingPenalty

//Note: Stroke is rotated to simplify calculations
{
	double MeritLine= 1.0;

	//Rotate the stroke
	//-----------------
	POINT2D Centroid((Stroke.Segment.TipBegin.x+Stroke.Segment.TipEnd.x)/2. ,
					 (Stroke.Segment.TipBegin.y+Stroke.Segment.TipEnd.y)/2.);

	CStroke RotatedStroke= Stroke;
	RotatedStroke.Rotate(Centroid, -Stroke.Segment.Slope180);

	//Check the minimum tolerance criterion
	//-------------------------------------
	RECTANGLE2D bounds= RotatedStroke.GetBounds();
	double Tol= bounds.top-bounds.bottom;
	double obliq= Obliqueness(Stroke);
	double COB= (sin((obliq+0.125)*(4*NUMBER_PI/3))/20)+0.95;  // Coefficient of OBliqueness
	double Tolcob= Tol*COB;

	double TolMin= LineTolMin * (bounds.right-bounds.left);

	if(Tolcob < TolMin)
		return MeritLine;

	//Smooth oscillations
	//-------------------
	CStroke SmoothedStroke= RotatedStroke;
	double TolSmooth= Stroke.GetLength()/100;    // Tuning Parameters
	long NumSmoothingSteps= SmoothStroke(SmoothedStroke, TolSmooth, Tol);

	//Check the maximum (relaxed) tolerance criterion
	//-----------------------------------------------
	bounds= SmoothedStroke.GetBounds();
	Tol= bounds.top-bounds.bottom;
	//COB= (sin((obliq+0.125)*(4*NUMBER_PI/3))/20)+0.95;  // Coefficient of OBliqueness
	Tolcob= Tol*COB;

	double TolMax= LineTolMax * (bounds.right-bounds.left);

	//if(Tolcob < TolMin)
	//	MeritLine= 1.0;
	//else if(Tolcob > TolMax)
	if(Tolcob > TolMax)
		MeritLine= 0.0;
	else
		MeritLine= 1.0 - (Tolcob-TolMin)/(TolMax-TolMin);

	//Reduce merit for oscillating lines
	//----------------------------------
	MeritLine= max(0.0, (MeritLine - LineSmoothingPenalty*NumSmoothingSteps));     // Tuning Parameter

	return MeritLine;
}


//---------------------------------------------------------------------------

//CFitEllipse::CFitEllipse()
//{
//	ArcTolMin= 0.010;
//	ArcTolMax= 0.100;
//	ArcSmoothingPenalty= 0.05;
//
//	StrokeCornersApproach = 0;   // 0= automatic, 1= IStraw, 2= Silding Strips;
//	FitEllipseApproach= 0; // 0= automatic, 1= 5P, 2= DIR, 3= GEF
//	CheckQuality5P= true;
//	CheckQualityDIR= true;
//	ResampleStrokesDIR= true;
//	NumResampledPoints= 20;
//	ResampleStrokesGEF= true;
//	NumResampledPointsGEF= 20;
//	DisplayColors= false;
//}


CFitEllipse::CFitEllipse(double ATMin, double ATMax, double SPenalty,
						 long SCApproach,
						 long FEApproach,
						 bool Quality5P, bool QualityDIR,
						 bool RStrokesDIR, long NumP,
						 bool RStrokesGEF, long NumPGEF, bool DColors)
//Initialize with input parameters
: ArcTolMin(ATMin),
  ArcTolMax (ATMax),
  ArcSmoothingPenalty (SPenalty),
  StrokeCornersApproach (SCApproach),
  FitEllipseApproach (FEApproach),
  CheckQuality5P (Quality5P),
  CheckQualityDIR (QualityDIR),
  ResampleStrokesDIR (RStrokesDIR),
  NumResampledPoints (NumP),
  ResampleStrokesGEF (RStrokesGEF),
  NumResampledPointsGEF (NumPGEF),
  DisplayColors (DColors)
{
}


CFitEllipse::~CFitEllipse()
{
}


bool CFitEllipse::IsEllipse(CConicParams phi)
//The input is the set of parameters of a general quadratic curve
//phi (a, b, c, d, e, f),
//where ax^2+bxy+cy^2+dx+ey+f=0

//Returns true if the parameters depict and ellipse, false otherwise
{
	//Determinant |a b/2 d/2; b/2 c e/2; d/2 e/2 f|
	double Delta = phi.a*phi.c*phi.f - phi.b*phi.b*phi.f/4 +phi.b*phi.d*phi.e/4 - phi.c*phi.d*phi.d/4 - phi.a*phi.e*phi.e/4;
	double J = phi.a*phi.c - phi.b*phi.b/4; //|a b/2; b/2 c|

	//Degenerate conic iff Delta==0
	//Hyperbola iff J<0
	//Parabola iff J==0
	//Ellipse iff J>0
	//Imaginary ellipse iff Delta*(a+c)>0

	return(Delta!=0 && J>0 && Delta*(phi.a+phi.c)<0);
}


bool CFitEllipse::FivePointsConic(double p0[3], double p1[3], double p2[3],
								  double p3[3], double p4[3], 
								  CConicParams &phi)

// Code to find the equation of a conic */
//               Tom Davis              */
//             April 12, 1996           */
// http://mathforum.org/library/drmath/view/51735.html

// takes five points in homogeneous coordinates, and returns the
// coefficients of a general conic equation in a, b, c, ..., f:
//
// a*x*x + b*x*y + c*y*y + d*x + e*y + f = 0.
//
// The routine returns 1 on success; 0 otherwise.  (It can fail, for
// example, if there are duplicate points.
//
// Typically, the points will be finite, in which case the third (w)
// coordinate for all the input vectors will be 1, although the code
// deals cleanly with points at infinity.
//
// For example, to find the equation of the conic passing through
// (5, 0), (-5, 0), (3, 2), (3, -2), and (-3, 2), set:
//
// p0[0] =  5, p0[1] =  0, p0[2] = 1,
// p1[0] = -5, p1[1] =  0, p1[2] = 1,
// p2[0] =  3, p2[1] =  2, p2[2] = 1,
// p3[0] =  3, p3[1] = -2, p3[2] = 1,
// p4[0] = -3, p4[1] =  2, p4[2] = 1.
//
// But if you want the equation of the hyperbola that is tangent to the
// line 2x=y at infinity, simply make one of the points be the point at
// infinity along that line, for example:
//
// p0[0] = 1, p0[1] = 2, p0[2] = 0.
{
	double L0[3], L1[3], L2[3], L3[3], Q[3];
	CROSS(p0, p1, L0)
	CROSS(p1, p2, L1)
	CROSS(p2, p3, L2)
	CROSS(p3, p4, L3)
	CROSS(L0, L3, Q)

	double A = Q[0];double B = Q[1];double C = Q[2];
	double a1 = L1[0];double b1 = L1[1];double c1 = L1[2];
	double a2 = L2[0];double b2 = L2[1];double c2 = L2[2];
	double x0 = p0[0];double y0 = p0[1];double w0 = p0[2];
	double x4 = p4[0];double y4 = p4[1];double w4 = p4[2];

	double aa = -A*a1*a2*y4*w0
				+A*a1*a2*w4*y0
				-B*b1*a2*y4*w0
				-B*c1*a2*w4*w0
				+B*a1*b2*w4*y0
				+B*a1*c2*w4*w0
				+C*b1*a2*y4*y0
				+C*c1*a2*w4*y0
				-C*a1*b2*y4*y0
				-C*a1*c2*y4*w0;

	double cc =  A*c1*b2*w4*w0
				+A*a1*b2*x4*w0
				-A*b1*c2*w4*w0
				-A*b1*a2*w4*x0
				+B*b1*b2*x4*w0
				-B*b1*b2*w4*x0
				+C*b1*c2*x4*w0
				+C*b1*a2*x4*x0
				-C*c1*b2*w4*x0
				-C*a1*b2*x4*x0;

	double ff =  A*c1*a2*y4*x0
				+A*c1*b2*y4*y0
				-A*a1*c2*x4*y0
				-A*b1*c2*y4*y0
				-B*c1*a2*x4*x0
				-B*c1*b2*x4*y0
				+B*a1*c2*x4*x0
				+B*b1*c2*y4*x0
				-C*c1*c2*x4*y0
				+C*c1*c2*y4*x0;

	double bb =  A*c1*a2*w4*w0
				+A*a1*a2*x4*w0
				-A*a1*b2*y4*w0
				-A*a1*c2*w4*w0
				-A*a1*a2*w4*x0
				+A*b1*a2*w4*y0
				+B*b1*a2*x4*w0
				-B*b1*b2*y4*w0
				-B*c1*b2*w4*w0
				-B*a1*b2*w4*x0
				+B*b1*b2*w4*y0
				+B*b1*c2*w4*w0
				-C*b1*c2*y4*w0
				-C*b1*a2*x4*y0
				-C*b1*a2*y4*x0
				-C*c1*a2*w4*x0
				+C*c1*b2*w4*y0
				+C*a1*b2*x4*y0
				+C*a1*b2*y4*x0
				+C*a1*c2*x4*w0;

	double dd = -A*c1*a2*y4*w0
				+A*a1*a2*y4*x0
				+A*a1*b2*y4*y0
				+A*a1*c2*w4*y0
				-A*a1*a2*x4*y0
				-A*b1*a2*y4*y0
				+B*b1*a2*y4*x0
				+B*c1*a2*w4*x0
				+B*c1*a2*x4*w0
				+B*c1*b2*w4*y0
				-B*a1*b2*x4*y0
				-B*a1*c2*w4*x0
				-B*a1*c2*x4*w0
				-B*b1*c2*y4*w0
				+C*b1*c2*y4*y0
				+C*c1*c2*w4*y0
				-C*c1*a2*x4*y0
				-C*c1*b2*y4*y0
				-C*c1*c2*y4*w0
				+C*a1*c2*y4*x0;

	double ee = -A*c1*a2*w4*x0
				-A*c1*b2*y4*w0
				-A*c1*b2*w4*y0
				-A*a1*b2*x4*y0
				+A*a1*c2*x4*w0
				+A*b1*c2*y4*w0
				+A*b1*c2*w4*y0
				+A*b1*a2*y4*x0
				-B*b1*a2*x4*x0
				-B*b1*b2*x4*y0
				+B*c1*b2*x4*w0
				+B*a1*b2*x4*x0
				+B*b1*b2*y4*x0
				-B*b1*c2*w4*x0
				-C*b1*c2*x4*y0
				+C*c1*c2*x4*w0
				+C*c1*a2*x4*x0
				+C*c1*b2*y4*x0
				-C*c1*c2*w4*x0
				-C*a1*c2*x4*x0;

	if (aa != 0.0) {
		bb /= aa; cc /= aa; dd /= aa; ee /= aa; ff /= aa; aa = 1.0;
	}
	else if (bb != 0.0) {
		cc /= bb; dd /= bb; ee /= bb; ff /= bb; bb = 1.0;
	}
	else if (cc != 0.0) {
		dd /= cc; ee /= cc; ff /= cc; cc = 1.0;
	}
	else if (dd != 0.0) {
		ee /= dd; ff /= dd; dd = 1.0;
	}
	else if (ee != 0.0) {
		ff /= ee; ee = 1.0;
	}
	else {
		return false;
	}

	phi.SetParams(aa, bb, cc, dd, ee, ff);

	return IsEllipse(phi);
}


bool CFitEllipse::FitEllipse5P(CStroke InputStroke, CConicParams &phi)
//Returns the ellipse that fits the two endpoints plus trhee intermediate points

//Use the first endpoints plus four intermediate points
//if both endpoints are too close
{
	long n= InputStroke.GetNumPoints()-1;
	POINT2D P0, P1, P2, P3, P4;

	//Select five equally spaced points
	//---------------------------------
	P0= InputStroke.GetPoint(0).GetPosition();
	P4= InputStroke.GetPoint(n).GetPosition();

	if(MODULE_2D(P0, P4) < 0.1*InputStroke.GetLength())
		//Use four intermediate points if endpoints are too close to each other
		InputStroke.Resample(6);
	else
		InputStroke.Resample(5);
	//Select the first, the last and three equally spaced points
	//P0= InputStroke.GetPoint(0).GetPosition();
	P1= InputStroke.GetPoint(1).GetPosition();
	P2= InputStroke.GetPoint(2).GetPosition();
	P3= InputStroke.GetPoint(3).GetPosition();
	P4= InputStroke.GetPoint(4).GetPosition();

	//Fit the ellipse passing through five points
	//-------------------------------------------
	double p0[3], p1[3], p2[3], p3[3], p4[3];
	p0[0] = P0.x; p0[1] = P0.y; p0[2] = 1;
	p1[0] = P1.x; p1[1] = P1.y; p1[2] = 1;
	p2[0] = P2.x; p2[1] = P2.y; p2[2] = 1;
	p3[0] = P3.x; p3[1] = P3.y; p3[2] = 1;
	p4[0] = P4.x; p4[1] = P4.y; p4[2] = 1;

	return FivePointsConic(p0, p1, p2, p3, p4, phi);
}


bool CFitEllipse::FitEllipseDIR(CStroke InputStroke, CConicParams &phi)
//Returns the coefficients of the ellipse that best fits the points of InputStroke

//Returns true if the stroke fits an ellipse
//The output parameters belong to the general quadratic curve ax^2+bxy+cy^2+dx+ey+f=0

//Note that all data points are assumed to belong to one single ellipse

// The function follows the approach by Fitzgibbon, as implemented by:
//Radim Halir , Jan Flusser. (1998). Numerically Stable Direct Least Squares Fitting Of Ellipses. WSCG98

// A C# implementation can be found at:
//http://skotagiri.wordpress.com/2010/06/19/c-implementation-for-fitting-an-ellipse-for-a-set-of-points/

// Warning: This fit is somewhat biased toward smaller ellipses
{
	int numPoints = (long)InputStroke.GetNumPoints();

	//C1= First quarter of constraint matrix
	std::vector<std::vector<double>> C1(3, std::vector<double>(3, 0));
	C1[0][0] = 0;     C1[0][1] = 0;     C1[0][2] = 0.5;
	C1[1][0] = 0;     C1[1][1] = -1;    C1[1][2] = 0;
	C1[2][0] = 0.5;   C1[2][1] = 0;     C1[2][2] = 0;

	//2 D1 = quadratic part of the design matrix
	//3 D2 = linear part of the design matrix
	std::vector<std::vector<double>> D1(numPoints, std::vector<double>(3, 0));
	std::vector<std::vector<double>> D2(numPoints, std::vector<double>(3, 0));

	for (long xx=0; xx<numPoints; xx++){
		POINT2D pp= InputStroke.GetPoint(xx).GetPosition();

		D1[xx][0] = pp.x * pp.x;
		D1[xx][1] = pp.x * pp.y;
		D1[xx][2] = pp.y * pp.y;

		D2[xx][0] = pp.x;
		D2[xx][1] = pp.y;
		D2[xx][2] = 1;
	}

	//Scatter matrix
	//Quadratic part of the scatter matrix
	std::vector<std::vector<double>> S1(3, std::vector<double>(3, 0));
	S1= MProduct(MTranspose(D1), D1);

	//Combined part of the scatter matrix
	std::vector<std::vector<double>> S2(3, std::vector<double>(3, 0));
	S2= MProduct(MTranspose(D1), D2);

	//Linear part of the scatter matrix
	std::vector<std::vector<double>> S3(3, std::vector<double>(3, 0));
	S3= MProduct(MTranspose(D2), D2);

	//T = for getting a2 from a1
	std::vector<std::vector<double>> T(3, std::vector<double>(3, 0));
	T= MScale(MProduct(MInverse(S3), MTranspose(S2)), -1);

	//M = reduced scatter matrix
	std::vector<std::vector<double>> M(3, std::vector<double>(3, 0));
	M= MAdd(S1, MProduct(S2, T));

	//9 M = [M(3, :) ./ 2; - M(2, :); M(1, :) ./ 2]; // premultiply by inv(C1)
	////M = C1 * M;
	M= MProduct(MInverse(C1), M);

	//Find the appropriate eigenvector a1 of the matrix M
	std::vector<std::complex<double>> eigenvalues(3, (1.0,0.0));
	std::vector<std::vector<double>> EigenVectors(3, std::vector<double>(3, 0));
	Eigen eigen;
	if(!eigen.UnsymEigen(M, eigenvalues, EigenVectors))
		return false;   //Unable to find eigenvalues (probably the stroke is a perfect straigth line)

	//Save eigenvectors in complex format
	std::vector<std::vector<std::complex<double>>> cev(3, std::vector<std::complex<double>>(3, (0.0,0.0)));

	for (long i= 0; i<3; i++){
		if(std::imag(eigenvalues[i]) != 0){
			//Complex eigenvalues of a real matrix are in conjugate pairs
			for (long j= 0; j<3; j++)
				cev[i][j]= std::complex<double>(EigenVectors[j][i], EigenVectors[j][i+1]);

			for (long j= 0; j<3; j++)
				cev[i][j]/= cev[i][2];

			for (long j= 0; j<3; j++)
				cev[i+1][j]= std::conj(cev[i][j]);
			i++;
		}
		else{
			for (long j= 0; j<3; j++)
				cev[i][j]= std::complex<double>(EigenVectors[j][i], 0);

			for (long j= 0; j<3; j++)
				cev[i][j]/= cev[i][2];
		}
	}

	//11 cond = 4 * evec(1, :) .* evec(3, :) - evec(2, :) .ˆ 2; // evaluate a’Ca
	//12 a1 = evec(:, find(cond > 0)); // eigenvector for min. pos. eigenvalue
	std::vector<std::vector<double>> a1(3, std::vector<double>(1, 0));
	for (int xx = 0; xx < 3; xx++){
		std::complex<double> condition = std::complex<double> (4,0) * cev[xx][0] * cev[xx][2] - cev[xx][1] * cev[xx][1];
		if (std::imag(condition) == 0 && std::real(condition) > 0){
			// Solution is found
			for (int yy = 0; yy < 3; yy++){
				a1[yy][0] = std::real(cev[xx][yy]);
			}
		}
	}

	//13 a2 = T * a1; // ellipse coefficients
	std::vector<std::vector<double>> a2(3, std::vector<double>(1, 0));
	a2= MProduct(T, a1);

	//Ellipse coefficients of general form
	phi.SetParams(a1[0][0], a1[1][0], a1[2][0], a2[0][0], a2[1][0], a2[2][0]);

	return IsEllipse(phi);
}


void CFitEllipse::normalise2dpts(std::vector<std::vector<double>> &pts,
								 std::vector<std::vector<double>> &T)
// Translates and normalises a set of 2D homogeneous points 
// so that their centroid is at the origin and their mean distance from 
// the origin is sqrt(2)
// This process typically improves the conditioning of any equations
//  used to solve homographies, fundamental matrices etc.

// Argument:
//   pts -  3xN array of 2D homogeneous coordinates

// Returns:
//   T   -  The 3x3 transformation matrix, newpts = T*pts

//Based on
// Peter Kovesi
// School of Computer Science & Software Engineering
// The University of Western Australia
// http://www.csse.uwa.edu.au/~pk
{
	// Centroid
	double c0= 0.;
	double c1= 0.;
	long NumPts= pts[0].size();
	for(long i=0; i<NumPts; i++){
		c0+= pts[0][i];
		c1+= pts[1][i];
	}

	c0 /= (double)NumPts;
	c1 /= (double)NumPts;

	// Scale so that the mean distance from the centroid to a point equals sqrt(2)
	double meandist= 0.;
	for(long i=0; i<NumPts; i++){
		double pts0i= pts[0][i] - c0;
		double pts1i= pts[1][i] - c1;
		meandist+= sqrt(pts0i*pts0i + pts1i*pts1i);
	}
	meandist/= (double)NumPts;
	double scale = sqrt(2.)/meandist;

	T[0][0] = scale;   T[0][1] = 0.;      T[0][2] = -scale*c0;
	T[1][0] = 0.;      T[1][1] = scale;   T[1][2] = -scale*c1;
	T[2][0] = 0.;      T[2][1] = 0.;      T[2][2] = 1.;
}


void CFitEllipse::LevenbergMarquardtStep(SzpakData &S)
//   Function: levenbergMarquardtStep
//
//   This function is used in the main loop of guaranteedEllipseFit in the process
//   of minimizing an approximate maximum likelihood cost function of an
//   ellipse fit to data.  It computes an update for the parameters
//   representing the ellipse, using the method of Levenberg-Marquardt for
//   non-linear optimisation. 
//   See: http://en.wikipedia.org/wiki/Levenberg//E2//80//93Marquardt_algorithm
//
//   Parameters:
//      S a data structure containing various parameters
//        needed for the optimisation process
//
//   Returns: 
//     the same data structure S, except that relevant fields have been updated
//
//  Based on a matlab code by Zygmunt L. Szpak (c) 2012
{
	// extract variables from data structure
	std::vector<std::vector<double>> t(6, std::vector<double>(1, 0));
	for(long i= 0; i<6; i++)
		t[i][0]= S.t[i][S.k];
	std::vector<std::vector<double>> delta(6, std::vector<double>(1, 0));
	for(long i= 0; i<6; i++)
		delta[i][0]= S.delta[i][S.k];
	double current_cost = S.cost[0][S.k];

	// compute two potential updates for theta based on different weightings of
	// the identity matrix.
	//-------------------------------------------------------------------------

	std::vector<std::vector<double>> jacob;
	jacob= MProduct(MTranspose(MConcatenateRows(S.jacobian_matrix, S.jacobian_matrix_barrier)) , S.r);
	std::vector<std::vector<double>> MtFt;
	MtFt= MProduct(MTranspose(t), MProduct(S.F, t));
	double tFt= MtFt[0][0];

	// We solve for the new update direction in a numerically careful manner
	// If we didn't worry about numerical stability then we would compute 
	// the first new search direction like this:

	std::vector<std::vector<double>> update_a;
	// update_a = MScale(MLeftDivide(MAdd(H, MScale(S.I, S.lambda)), jacob), -1);

	// But close to the barrier between ellipses and hyperbolas we may
	// experience numerical conditioning problems due to the nature of the
	// barrier term itself.  Hence we perform the calculation in a
	// numerically more stable way with 

	std::vector<std::vector<double>> Z00;
	Z00= MAdd(MProduct(MTranspose(S.jacobian_matrix), S.jacobian_matrix), MScale(S.I, S.lambda));
	std::vector<std::vector<double>> Z01;
	Z01= MScale(MProduct(MTranspose(S.jacobian_matrix_barrier), S.jacobian_matrix_barrier), pow(tFt, 4));
	std::vector<std::vector<double>> Z10;
	Z10= S.I;
	std::vector<std::vector<double>> Z11;
	Z11= MScale(S.I, -pow(tFt, 4));
	std::vector<std::vector<double>> Z_a;
	Z_a= MConcatenateRows(MConcatenateColumns(Z00, Z01), MConcatenateColumns(Z10, Z11));

	std::vector<std::vector<double>> zz_a;
	std::vector<std::vector<double>> zeros(6, std::vector<double>(1, 0));
	zz_a= MScale(MConcatenateRows(jacob, zeros), -1);

	update_a = MLeftDivide(Z_a, zz_a);
	// drop the nuisance parameter components
	update_a.resize(6);

	// In a similar fashion, the second potential search direction could be
	// computed like this:

	std::vector<std::vector<double>> update_b;
	// update_b = MScale(MLeftDivide(MAdd(H, MScale(S.I, lambda/v)), jacob), -1);

	// but instead we computed it with
	//std::vector<std::vector<double>> Z00;
	Z00= MAdd(MProduct(MTranspose(S.jacobian_matrix), S.jacobian_matrix), MScale(S.I, S.lambda/S.damping_multiplier));
	//std::vector<std::vector<double>> Z01;
	Z01= MScale(MProduct(MTranspose(S.jacobian_matrix_barrier), S.jacobian_matrix_barrier), pow(tFt, 4));
	//std::vector<std::vector<double>> Z10;
	Z10= S.I;
	//std::vector<std::vector<double>> Z11;
	Z11= MScale(S.I, -pow(tFt, 4));
	std::vector<std::vector<double>> Z_b;
	Z_b= MConcatenateRows(MConcatenateColumns(Z00, Z01), MConcatenateColumns(Z10, Z11));

	std::vector<std::vector<double>> zz_b;
	//std::vector<std::vector<double>> zeros(6, std::vector<double>(1, 0));
	zz_b= MScale(MConcatenateRows(jacob, zeros), -1);

	update_b = MLeftDivide(Z_b, zz_b);
	// drop the nuisance parameter components
	update_b.resize(6);

	// the potential new parameters are then
	std::vector<std::vector<double>> t_potential_a(6, std::vector<double>(1, 0));
	t_potential_a = MAdd(t, update_a);
	std::vector<std::vector<double>> t_potential_b(6, std::vector<double>(1, 0));
	t_potential_b = MAdd(t, update_b);

	// compute new residuals and costs based on these updates
	//-------------------------------------------------------

	// residuals computed on data points
	double cost_a = 0;
	double cost_b = 0;
	for(long i = 0; i<S.numberOfPoints; i++){
		// transformed data point
		std::vector<std::vector<double>> ux_j(6, std::vector<double>(1, 0));
		ux_j[0][0]= S.data_points[0][i]*S.data_points[0][i];
		ux_j[1][0]= S.data_points[0][i]*S.data_points[1][i];
		ux_j[2][0]= S.data_points[1][i]*S.data_points[1][i];
		ux_j[3][0]= S.data_points[0][i];
		ux_j[4][0]= S.data_points[1][i];
		ux_j[5][0]= 1.;

		// derivative of transformed data point
		std::vector<std::vector<double>> dux_j(6, std::vector<double>(2, 0));
		dux_j[0][0]= 2*S.data_points[0][i];
		dux_j[1][0]= S.data_points[1][i];
		dux_j[2][0]= 0.;
		dux_j[3][0]= 1.;
		dux_j[4][0]= 0.;
		dux_j[5][0]= 0.;
		dux_j[0][1]= 0;
		dux_j[1][1]= S.data_points[0][i];
		dux_j[2][1]= 2*S.data_points[1][i];
		dux_j[3][1]= 0.;
		dux_j[4][1]= 1.;
		dux_j[5][1]= 0.;

		// outer product
		std::vector<std::vector<double>> A;
		A = MProduct(ux_j, MTranspose(ux_j));

		// use identity covs
		std::vector<std::vector<double>> B;
		B = MProduct(dux_j, MTranspose(dux_j));

		std::vector<std::vector<double>> Mt_aBt_a;
		Mt_aBt_a= MProduct(MTranspose(t_potential_a), MProduct(B, t_potential_a));
		double t_aBt_a= Mt_aBt_a[0][0];
		std::vector<std::vector<double>> Mt_aAt_a;
		Mt_aAt_a= MProduct(MTranspose(t_potential_a), MProduct(A, t_potential_a));
		double t_aAt_a= Mt_aAt_a[0][0];

		std::vector<std::vector<double>> Mt_bBt_b;
		Mt_bBt_b= MProduct(MTranspose(t_potential_b), MProduct(B, t_potential_b));
		double t_bBt_b= Mt_bBt_b[0][0];
		std::vector<std::vector<double>> Mt_bAt_b;
		Mt_bAt_b= MProduct(MTranspose(t_potential_b), MProduct(A, t_potential_b));
		double t_bAt_b= Mt_bAt_b[0][0];

		// AML cost for i'th data point
		cost_a = cost_a +  t_aAt_a/t_aBt_a ;
		cost_b = cost_b +  t_bAt_b/t_bBt_b ;
	}

	// Barrier term
	std::vector<std::vector<double>> Mt_aIt_a;
	Mt_aIt_a= MProduct(MTranspose(t_potential_a), MProduct(S.I, t_potential_a));
	double t_aIt_a= Mt_aIt_a[0][0];
	std::vector<std::vector<double>> Mt_aFt_a;
	Mt_aFt_a= MProduct(MTranspose(t_potential_a), MProduct(S.F, t_potential_a));
	double t_aFt_a= Mt_aFt_a[0][0];

	std::vector<std::vector<double>> Mt_bIt_b;
	Mt_bIt_b= MProduct(MTranspose(t_potential_b), MProduct(S.I, t_potential_b));
	double t_bIt_b= Mt_bIt_b[0][0];
	std::vector<std::vector<double>> Mt_bFt_b;
	Mt_bFt_b= MProduct(MTranspose(t_potential_b), MProduct(S.F, t_potential_b));
	double t_bFt_b= Mt_bFt_b[0][0];

	// add the barrier term
	cost_a = cost_a + pow((S.alpha*(t_aIt_a/t_aFt_a)), 2);
	cost_b = cost_b + pow((S.alpha*(t_bIt_b/t_bFt_b)), 2);

	// determine appropriate damping and if possible select an update
	if (cost_a >= current_cost && cost_b >= current_cost){
		// neither update reduced the cost
		S.theta_updated = false;
		// no change in the cost
		S.cost[0][S.k+1] = current_cost;
		// no change in parameters
		for(long i= 0; i<6; i++)
			S.t[i][S.k+1]= t[i][0];
		// no changes in step direction
		for(long i= 0; i<6; i++)
			S.delta[i][S.k+1]= delta[i][0];
		// next iteration add more Identity matrix
		S.lambda = S.lambda * S.damping_multiplier;
	}
	else if (cost_b < current_cost){
		// update 'b' reduced the cost function
		S.theta_updated = true;
		// store the new cost
		S.cost[0][S.k+1] = cost_b;
		// choose update 'b'
		double normt_b= norm_2(t_potential_b);
		for(long i= 0; i<6; i++)
			S.t[i][S.k+1]= t_potential_b[i][0]/normt_b;
		// store the step direction
		for(long i= 0; i<6; i++)
			S.delta[i][S.k+1]= update_b[i][0];
		// next iteration add less Identity matrix
		S.lambda = S.lambda / S.damping_multiplier;
	}
	else{
		// update 'a' reduced the cost function
		S.theta_updated = true;
		// store the new cost
		S.cost[0][S.k+1] = cost_a;
		// choose update 'a'
		double normt_a= norm_2(t_potential_a);
		for(long i= 0; i<6; i++)
			S.t[i][S.k+1]= t_potential_a[i][0]/normt_a;
		// store the step direction
		for(long i= 0; i<6; i++)
			S.delta[i][S.k+1]= update_a[i][0];
		// keep the same damping for the next iteration
		//S.lambda = S.lambda;
	}
}


void CFitEllipse::LineSearchStep(SzpakData &S)
//   Function: lineSearchStep
//
//   This function computes an update for the parameters
//   representing the ellipse, using the pseudo-inverse of a Gauss-Newton
//   approximation of the Hessian matrix. It then performs an inexact
//   line search to determine how far to move along the update direction
//   so that the resulting fit is still an ellipse
//
//   Parameters:
//      S a data structure containing various parameters
//        needed for the optimisation process
//
//   Returns: 
//     the same data structure S, except that relevant fields have been updated
//
//  Based on a matlab code by Zygmunt L. Szpak (c) 2012
{
	// extract local variables from data structure
	//--------------------------------------------
	std::vector<std::vector<double>> t(6, std::vector<double>(1, 0));
	for(long i= 0; i<6; i++)
		t[i][0]= S.t[i][S.k];
	std::vector<std::vector<double>> delta(6, std::vector<double>(1, 0));
	for(long i= 0; i<6; i++)
		delta[i][0]= S.delta[i][S.k];
	double current_cost = S.cost[0][S.k];

	// jacobian (vector), r(t)'d/dtheta[r(t)]
	std::vector<std::vector<double>> jacob;
	jacob= MProduct(MTranspose(MConcatenateRows(S.jacobian_matrix, S.jacobian_matrix_barrier)) , S.r);
	std::vector<std::vector<double>> MtFt;
	MtFt= MProduct(MTranspose(t), MProduct(S.F, t));
	double tFt= MtFt[0][0];

	// We solve for the new update direction in a numerically careful manner
	// If we didn't worry about numerical stability then we would compute 
	// the first new search direction like this:

	std::vector<std::vector<double>> update;
	// update = MScale(MProduct(MPseudoInverse(H), jacob), -1);

	// But close to the barrier between ellipses and hyperbolas we may
	// experience numerical conditioning problems due to the nature of the
	// barrier term itself.  Hence we perform the calculation in a
	// numerically more stable way with 

	std::vector<std::vector<double>> Z00;
	Z00= MAdd(MProduct(MTranspose(S.jacobian_matrix), S.jacobian_matrix), MScale(S.I, S.lambda));
	std::vector<std::vector<double>> Z01;
	Z01= MScale(MProduct(MTranspose(S.jacobian_matrix_barrier), S.jacobian_matrix_barrier), pow(tFt, 4));
	std::vector<std::vector<double>> Z10;
	Z10= S.I;
	std::vector<std::vector<double>> Z11;
	Z11= MScale(S.I, -pow(tFt, 4));
	std::vector<std::vector<double>> Z;
	Z= MConcatenateRows(MConcatenateColumns(Z00, Z01), MConcatenateColumns(Z10, Z11));

	std::vector<std::vector<double>> zz;
	std::vector<std::vector<double>> zeros(6, std::vector<double>(1, 0));
	zz= MScale(MConcatenateRows(jacob, zeros), -1);

	//Moore-Penrose inverse (pseudoinverse) of Z
	//update = pinv(Z,1e-20)*zz;
	update = MProduct(MPseudoInverse(Z, 1e-20), zz);
	// drop the nuisance parameter components
	update.resize(6);

	// there is no repeat...until construct so we use a while-do
	double frac = 0.5;
	double cost;
	std::vector<std::vector<double>> t_potential(6, std::vector<double>(1, 0));

	while (true){
		// compute potential update
		t_potential = MAdd(t, MScale(update, frac));
		delta = MScale(update, frac);
		// halve the step-size
		frac = frac/2.;
		// compute new residuals on data points
		cost = 0.;
		for(long i = 0; i<S.numberOfPoints; i++){
			// transformed data point
			std::vector<std::vector<double>> ux_j(6, std::vector<double>(1, 0));
			ux_j[0][0]= S.data_points[0][i]*S.data_points[0][i];
			ux_j[1][0]= S.data_points[0][i]*S.data_points[1][i];
			ux_j[2][0]= S.data_points[1][i]*S.data_points[1][i];
			ux_j[3][0]= S.data_points[0][i];
			ux_j[4][0]= S.data_points[1][i];
			ux_j[5][0]= 1.;

			// derivative of transformed data point
			std::vector<std::vector<double>> dux_j(6, std::vector<double>(2, 0));
			dux_j[0][0]= 2*S.data_points[0][i];
			dux_j[1][0]= S.data_points[1][i];
			dux_j[2][0]= 0.;
			dux_j[3][0]= 1.;
			dux_j[4][0]= 0.;
			dux_j[5][0]= 0.;
			dux_j[0][1]= 0;
			dux_j[1][1]= S.data_points[0][i];
			dux_j[2][1]= 2*S.data_points[1][i];
			dux_j[3][1]= 0.;
			dux_j[4][1]= 1.;
			dux_j[5][1]= 0.;

			// outer product
			std::vector<std::vector<double>> A;
			A = MProduct(ux_j, MTranspose(ux_j));

			// use identity covs
			std::vector<std::vector<double>> B;
			B = MProduct(dux_j, MTranspose(dux_j));

			std::vector<std::vector<double>> MtBt;
			MtBt= MProduct(MTranspose(t_potential), MProduct(B, t_potential));
			double tBt= MtBt[0][0];
			std::vector<std::vector<double>> MtAt;
			MtAt= MProduct(MTranspose(t_potential), MProduct(A, t_potential));
			double tAt= MtAt[0][0];

			// AML cost for i'th data point
			cost = cost + tAt/tBt;
		}

		// Barrier term
		std::vector<std::vector<double>> MtIt;
		MtIt= MProduct(MTranspose(t_potential), MProduct(S.I, t_potential));
		double tIt= MtIt[0][0];
		std::vector<std::vector<double>> MtFt;
		MtFt= MProduct(MTranspose(t_potential), MProduct(S.F, t_potential));
		double tFt= MtFt[0][0];

		// add the barrier term
		cost = cost + pow((S.alpha*(tIt/tFt)), 2);

		// check to see if cost function was sufficiently decreased, and whether
		// the estimate is still an ellipse. Additonally, if the step size
		// becomes too small we stop. 
		if ((tFt > 0 && (cost < (1-frac*S.gamma)*current_cost))  ||
			norm_2(delta) < S.tolDelta)
			break;
	}

	//Save output parameters
	S.theta_updated = true;
	double normt= norm_2(t_potential);
	for(long i= 0; i<6; i++)
		S.t[i][S.k+1]= t_potential[i][0]/normt;

	for(long i= 0; i<6; i++)
		S.delta[i][S.k+1]= delta[i][0];

	S.cost[0][S.k+1] = cost;
}


void CFitEllipse::guaranteedEllipseFit(long numStroke, long numSubStroke,
									   std::vector<double> &theta,
									   std::vector<std::vector<double>> pts)
//   Function: guaranteedEllipseFit
//
//   This function implements the ellipse fitting algorithm described in
//   Z.Szpak, W. Chojnacki and A. van den Hengel
//   "Guaranteed Ellipse Fitting with the Sampson Distance"
//   Proc. 12th European Conference on Computer Vision. ECCV 
//   Firenze,Italy, oct, 2012
//
//   Parameters:
//      theta an initial seed for parameters [a b c d e f] associated with
//            the equation   a x^2 + b x y + c y^2 + d x + e y + f = 0
//      pts   a 2xN matrix where N is the number of data points
//
//   Returns: 
//     a length-6 vector [a b c d e f] representing the parameters of the equation
//     a x^2 + b x y + c y^2 + d x + e y + f = 0
//     with the additional result that b^2 - 4 a c < 0.
//
//  Based on a matlab code by Zygmunt L. Szpak (c) 2012
{
	// ----------------------------------------------------------------
	// Open the Dialog that shows the progress, and allows cancellation
	CDialogProgress Progress;
	Progress.Create(IDD_DIALOG_Progress);
	CTime HoraInicial= CTime::GetCurrentTime();

	CString text;
	if(numStroke < 0)
		text.Format("Fitting current elliptic arc by GEF...");
	else if(numSubStroke > 0)
		text.Format("Fitting elliptic arc %i (subarc %i) by GEF...", numStroke, numSubStroke);
	else
		text.Format("Fitting elliptic arc %i by GEF...", numStroke);
	Progress.Message(text);
	Progress.RangeProgress(0, 100);
	Progress.UpdateProgress(0);
	// ----------------------------------------------------------------

	//Save input parameters vector as a local parameters matrix
	std::vector<std::vector<double>> t(6, std::vector<double>(1, 0));
	for(long i= 0; i<6; i++)
		t[i][0]= theta[i];

	// various variable initialisations
	//---------------------------------
	SzpakData S; //Define data structure

	// primary loop variable
	bool keep_going = true;
	// determines whether we use LineSearchStep or LevenbergMarquadtStep
	S.use_pseudoinverse = false;
	// in some case a LevenbergMarquardtStep does not decrease the cost
	// function and so the parameters (theta) are not updated
	S.theta_updated = false;
	// damping parameter in LevenbergMarquadtStep
	S.lambda = 0.01;
	// loop counter (matlab arrays start at index 1, not index 0)
	S.k = 0;
	// used to modify the tradeoff between gradient descent and hessian based
	// descent in LevenbergMarquadtStep
	S.damping_multiplier = 1.2;
	// used in LineSearchStep
	S.gamma = 0.00005;
	// number of data points
	S.numberOfPoints = pts[0].size();
	// barrier term that forces parameters to stay in elliptic region

	S.F.assign(6, std::vector<double>());
	for(long i= 0; i<6; i++)
		S.F[i].assign(6, 0.);
	S.F[0][0]= 0; S.F[0][1]= 0; S.F[0][2]= 2;
	S.F[1][0]= 0; S.F[1][1]=-1; S.F[1][2]= 0;
	S.F[2][0]= 2; S.F[2][1]= 0; S.F[2][2]= 0;

	S.I.assign(6, std::vector<double>());
	for(long i= 0; i<6; i++){
		S.I[i].assign(6, 0.);
		S.I[i][i]= 1.;
	}

	// homotopy parameters that weighs contribution of barrier term
	S.alpha = 1e-3;
	// data points that we are going to fit an ellipse to
	S.data_points = pts;

	// various parameters that determine stopping criteria
	//----------------------------------------------------
	// maximum loop iterations 
	long maxIter = 100;
	// step-size tolerance
	S.tolDelta = 1e-7;
	// cost tolerance
	S.tolCost = 1e-7;
	// parameter tolerance
	S.tolTheta = 1e-7;

	// various initial memory allocations
	//-----------------------------------
	// allocate space for cost of each iteration
	S.cost.assign(1, std::vector<double>());
	for(long i= 0; i<1; i++)
		S.cost[i].assign(maxIter, 0.);

	// allocate space for the parameters of each iteration
	S.t.assign(6, std::vector<double>());
	for(long i= 0; i<6; i++)
		S.t[i].assign(maxIter, 0.);

	// allocate space for the parameter direction of each iteration
	S.delta.assign(6, std::vector<double>());
	for(long i= 0; i<6; i++)
		S.delta[i].assign(maxIter, 0.);

	// make parameter vector a unit norm vector for numerical stability
	double normt= norm_2(MTranspose(t)[0]);  //Argument must be a vector
	for(long i= 0; i<6; i++)
		t[i][0] = t[i][0]/normt;

	//////// store the parameters associated with the first iteration
	for(long i= 0; i<6; i++)
		S.t[i][S.k]= t[i][0];

	// start with some random search direction (here we choose all 1's)
	// we can initialise with anything we want, so long as the norm of the
	// vector is not smaller than tolDeta. The initial search direction 
	// is not used in any way in the algorithm. 
	for(long i= 0; i<6; i++)
		S.delta[i][S.k]= 1.;

	// main estimation loop
	bool State= true;
	while (State && keep_going && S.k < maxIter-1){
		// allocate space for residuals (+1 to store barrier term) 
		S.r.assign(S.numberOfPoints+1, std::vector<double>());
		for(long i= 0; i<S.numberOfPoints+1; i++)
			S.r[i].assign(1, 0.);

		// allocate space for the jacobian matrix based on AML component
		S.jacobian_matrix.assign(S.numberOfPoints, std::vector<double>());
		for(long i= 0; i<S.numberOfPoints; i++)
			S.jacobian_matrix[i].assign(6, 0.);

		// allocate space for the jacobian matrix based on Barrier component
		S.jacobian_matrix_barrier.assign(1, std::vector<double>());
		for(long i= 0; i<1; i++)
			S.jacobian_matrix_barrier[i].assign(6, 0.);

		// grab the current parameter estimates
		for(long i= 0; i<6; i++)
			t[i][0]= S.t[i][S.k];

		// residuals computed on data points
		for(long i = 0; i<S.numberOfPoints; i++){
			// transformed data point
			std::vector<std::vector<double>> ux_j(6, std::vector<double>(1, 0));
			ux_j[0][0]= S.data_points[0][i]*S.data_points[0][i];
			ux_j[1][0]= S.data_points[0][i]*S.data_points[1][i];
			ux_j[2][0]= S.data_points[1][i]*S.data_points[1][i];
			ux_j[3][0]= S.data_points[0][i];
			ux_j[4][0]= S.data_points[1][i];
			ux_j[5][0]= 1.;

			// derivative of transformed data point
			std::vector<std::vector<double>> dux_j(6, std::vector<double>(2, 0));
			dux_j[0][0]= 2*S.data_points[0][i];
			dux_j[1][0]= S.data_points[1][i];
			dux_j[2][0]= 0.;
			dux_j[3][0]= 1.;
			dux_j[4][0]= 0.;
			dux_j[5][0]= 0.;
			dux_j[0][1]= 0;
			dux_j[1][1]= S.data_points[0][i];
			dux_j[2][1]= 2*S.data_points[1][i];
			dux_j[3][1]= 0.;
			dux_j[4][1]= 1.;
			dux_j[5][1]= 0.;

			// outer product
			std::vector<std::vector<double>> A(6, std::vector<double>(6, 0));
			A= MProduct(ux_j, MTranspose(ux_j));

			// use identity covs
			std::vector<std::vector<double>> B(6, std::vector<double>(6, 0));
			B= MProduct(dux_j, MTranspose(dux_j));

			std::vector<std::vector<double>> MtBt;
			MtBt= MProduct(MTranspose(t), MProduct(B, t));
			double tBt= MtBt[0][0];
			std::vector<std::vector<double>> MtAt;
			MtAt= MProduct(MTranspose(t), MProduct(A, t));
			double tAt= MtAt[0][0];

			// AML cost for i'th data point
			S.r[i][0] = sqrt(tAt/tBt);

			// derivative AML component
			std::vector<std::vector<double>> M;
			M= MScale(A, 1./tBt);
			std::vector<std::vector<double>> Xbits;
			Xbits= MScale(B, tAt/(tBt*tBt));
			std::vector<std::vector<double>> X;
			X= MSubtract(M, Xbits);

			// gradient for AML cost function 
			std::vector<std::vector<double>> grad;
			grad= MScale(MProduct(X, t), 1./sqrt(tAt/tBt));
			// build up jacobian matrix
			//S.jacobian_matrix[i]= grad;
			for(long j= 0; j<6; j++)
				S.jacobian_matrix[i][j]= grad[j][0];
		}

		// Barrier term
		std::vector<std::vector<double>> MtIt;
		MtIt= MProduct(MTranspose(t), MProduct(S.I, t));
		double tIt= MtIt[0][0];
		std::vector<std::vector<double>> MtFt;
		MtFt= MProduct(MTranspose(t), MProduct(S.F, t));
		double tFt= MtFt[0][0];

		// add the penalty term
		S.r[S.numberOfPoints][0] = S.alpha*(tIt/tFt);

		// Derivative barrier component
		std::vector<std::vector<double>> N;
		N = MScale(S.I, 1./tFt);
		std::vector<std::vector<double>> Ybits;
		Ybits = MScale(S.F, tIt/(tFt*tFt));
		std::vector<std::vector<double>> Y;
		Y = MSubtract(N, Ybits);
		std::vector<std::vector<double>> grad_penalty;
		grad_penalty= MScale(MProduct(Y, t), 2*S.alpha);
		for(long j= 0; j<6; j++)
			S.jacobian_matrix_barrier[0][j]= grad_penalty[j][0];

		// Jacobian matrix after combining AML and barrier terms
		S.jacobian_matrix_full = MConcatenateRows(S.jacobian_matrix, S.jacobian_matrix_barrier);

		// approximate Hessian matrix
		S.H = MProduct(MTranspose(S.jacobian_matrix_full), S.jacobian_matrix_full);

		// sum of squares cost for the current iteration
		//struct.cost(k) = 0.5*(struct.r'*struct.r);
		std::vector<std::vector<double>> Mcost;
		Mcost = MProduct(MTranspose(S.r), S.r);
		S.cost[0][S.k] = Mcost[0][0];

		// If we haven't overshot the barrier term
		// then we use LevenbergMarquadt step
		if (!S.use_pseudoinverse){
			LevenbergMarquardtStep(S);
		}
		else{
			LineSearchStep(S);
		}

		// Check if the latest update overshot the barrier term 
		std::vector<std::vector<double>> tk1(6, std::vector<double>(6, 0));
		for(long i= 0; i<(long)t.size(); i++)
			tk1[i][0]= S.t[i][S.k+1];
		//std::vector<std::vector<double>> MtFt;
		MtFt= MProduct(MTranspose(tk1), MProduct(S.F, tk1));

		if (MtFt[0][0] <= 0){
			// from now onwards we will only use lineSearchStep to ensure
			// that we do not overshoot the barrier 
			S.use_pseudoinverse = true;
			S.lambda = 0;

			//Revert to previous iterate which was an ellipse
			for(long i= 0; i<6; i++)
				S.t[i][S.k+1] = S.t[i][S.k];

			if (S.k > 1){
				for(long i= 0; i<6; i++)
					S.t[i][S.k] = S.t[i][S.k-1];
			}
		}
		else{
			// Check for various stopping criteria to end the main loop
			std::vector<double> tkp(6, 0);
			for(long i= 0; i<6; i++)
				tkp[i] = S.t[i][S.k+1] + S.t[i][S.k];
			double n1= norm_2(tkp);

			std::vector<double> tkm(6, 0);
			for(long i= 0; i<6; i++)
				tkm[i] = S.t[i][S.k+1] - S.t[i][S.k];
			double n2= norm_2(tkm);

			std::vector<double> d(6, 0);
			for(long i= 0; i<6; i++)
				d[i] = S.delta[i][S.k+1];
			double n3= norm_2(d);

			if (min(n1, n2) < S.tolTheta && S.theta_updated)
				keep_going = false;
			else if (std::fabs(S.cost[0][S.k] - S.cost[0][S.k+1]) < S.tolCost && S.theta_updated)
				keep_going = false;
			else if (n3 < S.tolDelta && S.theta_updated)
				keep_going = false;
		}

		S.k = S.k + 1;

		// ----------------------------------------------------------------
		State= Progress.UpdateProgress((long)(100.*S.k/(double)maxIter));
		// ----------------------------------------------------------------
	}

	//Output current theta
	for(long i= 0; i<6; i++)
		theta[i] = S.t[i][S.k];

	double normtheta= norm_2(theta);
	for(long i= 0; i<6; i++)
		theta[i] = theta[i]/normtheta;

	// ----------------------------------------------------------------
	Progress.DestroyWindow();
	// ----------------------------------------------------------------
}


bool CFitEllipse::FitEllipseGEF(CStroke InputStroke, long numStroke, long numSubStroke, CConicParams &phi)
//   Function: compute_guaranteedellipse_estimates

//   This function is a wrapper for an approximate maximum likelihood guaranteed
//   ellipse fit due to

//   Z.Szpak, W. Chojnacki and A. van den Hengel
//   "Guaranteed Ellipse Fitting with the Sampson Distance"
//   Proc. 12th European Conference on Computer Vision. ECCV 
//   Firenze,Italy, oct, 2012

//   It first shifts all the data points to a new coordinate system so that
//   the origin of the coordinate system is at the center of the data points, 
//   and then scales all data points so that they lie more or less within
//   a unit box.
//   It is within this transformed coordinate system that the 
//   ellipse is estimated.
//   Since the approximate maximum likelihood method
//   is an iterative scheme, it requires an initial seed for the parameters.
//   The seed is taken to be the direct ellipse fit due to 

//   R. Halif and J. Flusser
//   "Numerically stable direct least squares fitting of ellipses"
//   Proc. 6th International Conference in Central Europe on Computer Graphics and Visualization. WSCG '98
//   Czech Republic,125--132, feb, 1998
//
//   Their work is a numerically stable version of

//   A. W. Fitzgibbon, M. Pilu, R. B. Fisher
//   "Direct Least Squares Fitting of Ellipses"
//   IEEE Trans. PAMI, Vol. 21, pages 476-480 (1999)

//   The final ellipse parameters estimates are transformed 
//   back to the original data space. 

//   Parameters:
//      pts  a 3xN matrix where N is the number of data points

//   Returns: 
//     a length-6 vector [a b c d e f] representing the parameters of the equation
//     a x^2 + b x y + c y^2 + d x + e y + f = 0
//     with the additional result that b^2 - 4 a c < 0.

//  Based on a matlab code by Zygmunt L. Szpak (c) 2012
{
	// Data points in homogeneous coordinates
	long n = InputStroke.GetNumPoints();
	std::vector<std::vector<double>> pts(3, std::vector<double>(n, 0));

	for (long i=0; i<n; i++){
		POINT2D pp= InputStroke.GetPoint(i).GetPosition();

		pts[0][i] = pp.x;
		pts[1][i] = pp.y;
		pts[2][i] = 1.0;
	}

	// hartley normalise the data
	std::vector<std::vector<double>> T(3, std::vector<double>(3, 0));
	normalise2dpts(pts, T);

	pts= MProduct(T, pts);

	//Seed parameters
	//DIR fit should not be re-calculated here,
	//since the input parameters already contain a DIR fit
	//But this approach requires normalised data
	CStroke StrokeNorm= InputStroke;
	for (long i=0; i<n; i++){
		CStrokePoint pp;

		pp.SetPosition(pts[0][i], pts[1][i]);
		pp.SetTime(InputStroke.GetPoint(i).GetTime());
		StrokeNorm.SetPoint(i, pp);
	}

	if(!FitEllipseDIR(StrokeNorm, phi)){
		if(FitEllipse5P(StrokeNorm, phi))
			phi.b= -phi.b;
		else{
			return false;
		}
	}

	std::vector<double> theta(6,0);
	phi.GetParams(theta[0], theta[1], theta[2], theta[3], theta[4], theta[5]);

	double normtheta= norm_2(theta);
	for(long i= 0; i<6; i++)
		theta[i] = theta[i]/normtheta;

	//Fitting
	//WARNING: Time consuming tasks are in the main loop of guaranteedEllipseFit!
	guaranteedEllipseFit(numStroke, numSubStroke, theta, pts);

	//Denormalise C
	std::vector<std::vector<double>> C(3, std::vector<double>(3, 0));
	C[0][0] = theta[0];      C[0][1] = theta[1]/2.;   C[0][2] = theta[3]/2.;
	C[1][0] = theta[1]/2.;   C[1][1] = theta[2];      C[1][2] = theta[4]/2.;
	C[2][0] = theta[3]/2.;   C[2][1] = theta[4]/2.;   C[2][2] = theta[5];

	C= MProduct(MTranspose(T), MProduct(C, T));

	theta[0] = C[0][0];
	theta[1] = C[0][1]*2;
	theta[2] = C[1][1];
	theta[3] = C[0][2]*2;
	theta[4] = C[1][2]*2;
	theta[5] = C[2][2];
	normtheta= norm_2(theta);
	for(long i= 0; i<6; i++)
		theta[i] = theta[i]/normtheta;

	phi.SetParams(theta[0], theta[1], theta[2], theta[3], theta[4], theta[5]);

	return true;
}


double CFitEllipse::DistancePointEllipse(const double e[2], const double y[2], double x[2])
// The function returns the distance from
// the query point to the ellipse.
// It also computes the ellipse point (x0,x1)
// in the first quadrant that is closest to (y0,y1).

// The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1 with e0 >= e1.
// The query point is (y0,y1) with y0 >= 0 and y1 >= 0. 
{
	double distance= 0.;

	if (y[1] > 0.){
		if (y[0] > 0.){
			// Bisect to compute the root of F(t) for t >= -e1*e1.
			double esqr[2] = { e[0]*e[0], e[1]*e[1] };
			double ey[2] = { e[0]*y[0], e[1]*y[1] };
			double t0 = -esqr[1] + ey[1];
			double t1 = -esqr[1] + sqrt(ey[0]*ey[0] + ey[1]*ey[1]);
			double t = t0;
			const int imax = 2*std::numeric_limits<double>::max_exponent;
			for (int i = 0; i < imax; ++i){
				t = 0.5*(t0 + t1);
				if (t == t0 || t == t1)
					break;

				double r[2] = { ey[0]/(t + esqr[0]), ey[1]/(t + esqr[1]) };
				double f = r[0]*r[0] + r[1]*r[1] - 1.;
				if (f > 0.)
					t0 = t;
				else if (f < 0.)
					t1 = t;
				else
					break;
			}

			x[0] = esqr[0]*y[0]/(t + esqr[0]);
			x[1] = esqr[1]*y[1]/(t + esqr[1]);
			double d[2] = { x[0] - y[0], x[1] - y[1] };
			distance = sqrt(d[0]*d[0] + d[1]*d[1]);
		}
		else{ // y0 == 0
			x[0] = 0.;
			x[1] = e[1];
			distance = std::fabs(y[1] - e[1]);
		}
	}
	else{ // y1 == 0
		double denom0 = e[0]*e[0] - e[1]*e[1];
		double e0y0 = e[0]*y[0];
		if (e0y0 < denom0){
			// y0 is inside the subinterval.
			double x0de0 = e0y0/denom0;
			double x0de0sqr = x0de0*x0de0;
			x[0] = e[0]*x0de0;
			x[1] = e[1]*sqrt(std::fabs(1. - x0de0sqr));
			double d0 = x[0] - y[0];
			distance = sqrt(d0*d0 + x[1]*x[1]);
		}
		else{
			// y0 is outside the subinterval. The closest ellipse point has
			// x1 == 0 and is on the domain-boundary interval (x0/e0)^2 = 1.
			x[0] = e[0];
			x[1] = 0.;
			distance = std::fabs(y[0] - e[0]);
		}
	}
	return distance;
}


POINT2D CFitEllipse::ClosestPointInEllipse(double Ra, double Rb, POINT2D P)
//The function finds the point of the ellipse (defined by its radius Ra and Rb),
//which is closest to the point defined by P

//The ellipse is centred in the origin and with its main axis horizontal

//The distance between some point and an ellipse is a transcendental equation
//Root finding techniques are required

// The function returns the distance from the query point to the ellipse.
// It also computes the ellipse point (x0,x1) that is closest to (y0,y1).
//(This point is the one whose normal passes through P)

//This function reflects components of (y0; y1) as needed
//to place the query point in the first quadrant,
//sorts the (e0; e1) values to be correctly ordered,
//and then calls DistancePointEllipseSpecial.
//The returned (x0; x1) is postprocessed
//to match the quadrant of the query point.

// The ellipse is (x0/e0)^2 + (x1/e1)^2 = 1. The query point is (y0,y1).

//This function is a modified version of "DistancePointEllipse", provided by:
	// Geometric Tools, LLC
	// Copyright (c) 1998-2013
	// Distributed under the Boost Software License, Version 1.0.
	// http://www.boost.org/LICENSE_1_0.txt
	// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
	//
	// File Version: 5.6.3 (2013/01/03)

	//D. Eberly.
	//Internet publication: "Distance from a point to an ellipse in 2D" (2004) 
	//Geometric Tools, LLC, www.geometrictools.com
	//Book publication: "3D Game Engine Design", 2nd edition.
	//Morgan Kaufmann Publishers, San Francisco, CA, 2007.
	//(see Section 14.13.1)
{
	//Save data in internal format
	double e[2];
	e[0]= Ra;
	e[1]= Rb;

	double y[2];
	y[0]= P.x;
	y[1]= P.y;

	//Determine reflections for y to the first quadrant
	bool reflect[2];
	for (long i = 0; i < 2; ++i){
		reflect[i] = (y[i] < 0.);
	}

	//Determine the axis order for decreasing extents
	int permute[2];
	if (e[0] < e[1]){
		permute[0] = 1; permute[1] = 0;
	}
	else{
		permute[0] = 0; permute[1] = 1;
	}

	int invpermute[2];
	for (long i = 0; i < 2; ++i){
		invpermute[permute[i]] = i;
	}

	double locE[2], locY[2];
	for (long i = 0; i < 2; ++i){
		long j = permute[i];
		locE[i] = e[j];
		locY[i] = y[j];
		if (reflect[j]){
			locY[i] = -locY[i];
		}
	}

	double locX[2];
	double distance = DistancePointEllipse(locE, locY, locX);

	//Restore the axis order and reflections
	double x[2];
	for (long i = 0; i < 2; ++i){
		long j = invpermute[i];
		if (reflect[i]){
			locX[j] = -locX[j];
		}
		x[i] = locX[j];
	}

	POINT2D T(x[0], x[1]);

	return T;
}


void CFitEllipse::GetEndPoints(CStroke &Stroke)
//The function finds the points of the ellipse
//that are closer to the endpoints of the stroke

//The function also discriminates the arc that best fits the stroke

//Adds the polar angles of both endpoints into Arc in counterclock sequence
//	StartAngle < EndAngle

//WARNING! All those parameters are calculated in the "ellipse system"
{
	//Get the stroke endpoints and the midpoint
	POINT2D P0= Stroke.GetPoint(0).GetPosition();
	POINT2D P1= Stroke.GetPoint(Stroke.GetNumPoints()-1).GetPosition();
	POINT2D PM= Stroke.GetPoint(Stroke.GetNumPoints()/2).GetPosition();

	//Transform to "ellipse system"
	POINT2D Centre(Stroke.Arc.Centre_x, Stroke.Arc.Centre_y);
	P0= P0 - Centre;
	P1= P1 - Centre;
	P0= P0.Rotate(-Stroke.Arc.Rotation);
	P1= P1.Rotate(-Stroke.Arc.Rotation);

	PM= PM - Centre;
	PM= PM.Rotate(-Stroke.Arc.Rotation);

	//Find the closest points in the ellipse
	POINT2D T0= ClosestPointInEllipse(Stroke.Arc.Radius_a, Stroke.Arc.Radius_b, P0);
	POINT2D T1= ClosestPointInEllipse(Stroke.Arc.Radius_a, Stroke.Arc.Radius_b, P1);
	Stroke.Arc.GapEnds= max(MODULE_2D(T0, P0), MODULE_2D(T1, P1));

	POINT2D TM= ClosestPointInEllipse(Stroke.Arc.Radius_a, Stroke.Arc.Radius_b, PM);

	//Calculate their polar angles
	double A0= acos(T0.x/Stroke.Arc.Radius_a);
	if(T0.y<0) A0= 2*NUMBER_PI-A0;
	double A1= acos(T1.x/Stroke.Arc.Radius_a);
	if(T1.y<0) A1= 2*NUMBER_PI-A1;
	double AM= acos(TM.x/Stroke.Arc.Radius_a);
	if(TM.y<0) AM= 2*NUMBER_PI-AM;

	//Sort from minor to major angle
	double AA0= min(A0, A1);
	double AA1= max(A0, A1);

	if(AA0 < AM && AM < AA1){
		Stroke.Arc.StartAngle= AA0; //Start angle is in the range [0, 2*PI)
		Stroke.Arc.EndAngle= AA1;
	}
	else{
		Stroke.Arc.StartAngle= AA1;
		Stroke.Arc.EndAngle= AA0 + 2*NUMBER_PI;  //End angle is in the range (0, 4*PI]
	}

	Stroke.Arc.StartAngle+= Stroke.Arc.Rotation;
	Stroke.Arc.EndAngle+= Stroke.Arc.Rotation;
	if(Stroke.Arc.StartAngle < 0){
		Stroke.Arc.StartAngle+= 2*NUMBER_PI;
		Stroke.Arc.EndAngle+= 2*NUMBER_PI;
	}

	/////////////////////////////////////////////////////////
	//Save to display
	//T0= T0.Rotate(Stroke.Arc.Rotation) + Centre;
	//TM= TM.Rotate(Stroke.Arc.Rotation) + Centre;
	//T1= T1.Rotate(Stroke.Arc.Rotation) + Centre;

	//Stroke.Arc.T0_x= T0.x;
	//Stroke.Arc.T0_y= T0.y;
	//Stroke.Arc.TM_x= TM.x;
	//Stroke.Arc.TM_y= TM.y;
	//Stroke.Arc.T1_x= T1.x;
	//Stroke.Arc.T1_y= T1.y;
	/////////////////////////////////////////////////////////
}


bool CFitEllipse::GetArc(CConicParams phi, CStroke &Stroke)
//The input is the general quadratic curve ax^2+bxy+cy^2+dx+ey+f=0
//intended at representing an ellipse

//The output is the canonical form of the elliptic arc, which includes:
//	the centre (Arc.Centre_x, Arc.Centre_y)
//	the semi-axes lengths (Arc.Radius_a, Arc.Radius_b)
//	the counterclockwise angle of rotation from the x-axis to the major axis (Arc.Rotation)
//	The foci (Arc.Focus1_x, Arc.Focus1_y), (Arc.Focus2_x, Arc.Focus2_y)
//	The endpoints (the points of the ellipse closer to the endpoints of the stroke)

//Returns false if any parameter is invalid
{
	//Center of the ellipse
	//---------------------
	double J = phi.a*phi.c - phi.b*phi.b/4; //|a b/2; b/2 c|

	Stroke.Arc.Centre_x = (phi.b*phi.e-2*phi.c*phi.d)/(4*J);
	Stroke.Arc.Centre_y = (phi.b*phi.d-2*phi.a*phi.e)/(4*J);

	//Radius and rotation
	//-------------------
	if(phi.a==phi.c && phi.b==0.){
		//Is a circle
		Stroke.Arc.Radius_a = sqrt((phi.d*phi.d+phi.e*phi.e)/(4*phi.a*phi.a) - phi.f/phi.a);
		Stroke.Arc.Radius_b = Stroke.Arc.Radius_a;
		Stroke.Arc.Rotation= 0.;
	}
	else{
		//Semi-axes lengths
		double aa1= (phi.a*phi.e*phi.e+phi.c*phi.d*phi.d+phi.f*phi.b*phi.b-phi.b*phi.d*phi.e)/2. - 2*phi.a*phi.c*phi.f;
		double aa3= sqrt((phi.a-phi.c)*(phi.a-phi.c)+phi.b*phi.b);

		Stroke.Arc.Radius_a = sqrt(aa1/(J*(phi.a+phi.c-aa3)));
		Stroke.Arc.Radius_b = sqrt(aa1/(J*(phi.a+phi.c+aa3)));

		//Counterclockwise angle of rotation from the x-axis to the major axis of the ellipse
		if(phi.b==0){
			if(phi.a<phi.c)
				Stroke.Arc.Rotation= 0.;
			else
				Stroke.Arc.Rotation= NUMBER_PI/2;
		}
		else{
			if(phi.a<phi.c)
				Stroke.Arc.Rotation= atan(phi.b/(phi.a-phi.c))/2;
			else
				Stroke.Arc.Rotation= NUMBER_PI/2 + atan(phi.b/(phi.a-phi.c))/2;
		}
		//Stroke.Arc.Rotation= atan2(phi.b, (phi.a-phi.c))/2;

		if(Stroke.Arc.Rotation < 0) Stroke.Arc.Rotation+= NUMBER_PI;
	}

	//Check validity of the canonical form
	if(_isnan((float)Stroke.Arc.Radius_a)  ||
	   _isnan((float)Stroke.Arc.Radius_b) ||
	   _isnan((float)Stroke.Arc.Rotation))
		return false;

	//Foci
	//----
	double cc= sqrt(Stroke.Arc.Radius_a*Stroke.Arc.Radius_a - Stroke.Arc.Radius_b*Stroke.Arc.Radius_b);
	double ccx= cc * cos(Stroke.Arc.Rotation);
	double ccy= cc * sin(Stroke.Arc.Rotation);
	Stroke.Arc.Focus1_x= Stroke.Arc.Centre_x + ccx;
	Stroke.Arc.Focus1_y= Stroke.Arc.Centre_y + ccy;
	Stroke.Arc.Focus2_x= Stroke.Arc.Centre_x - ccx;
	Stroke.Arc.Focus2_y= Stroke.Arc.Centre_y - ccy;

	//Endpoints
	//---------
	GetEndPoints(Stroke);

	return true;
}


void CFitEllipse::GetStrokeMetrics(CStroke Stroke, CStrokeMetrics &SM)
//Returns metrics of the troke

//		BoxLength measures the length of the minimum box
//		that contains the stroke

//		Sagitta measures the width of the minimum box
//		that contains the stroke

//		ShortStroke is set to true if the stroke defines a "short" elliptic arc
//		(arc encompassing an angle shorter than 180 degrees)
//		This indirectly measured through the "Gap", which is the maximum distance between
//		every endpoint and the closest side of the bounding box

//		FlatStroke is set to true if the curvature is small (the arc is "flat")
//		Stroke is considered flat if the ratio between its sagitta and its chord
//		is less than FLATNESS

//		ShortAndFlat is set to true if ShortStroke and FlatStroke are true,
//		and false otherwise

//Those metrics are used to check if the arc must be fitted again
{
	//Rotate the stroke
	//-----------------
	CStroke RotatedStroke= Stroke;
	RotatedStroke.Rotate(Stroke.GetStrokeCentroid(), -Stroke.GetSlope180());

	//Stroke metrics
	//--------------
	RECTANGLE2D bounds= RotatedStroke.GetBounds();
	SM.BoxLength= std::fabs(bounds.right-bounds.left);

	SM.Sagitta= bounds.top-bounds.bottom;
	POINT2D F= RotatedStroke.GetPoint(0).GetPosition();
	POINT2D L= RotatedStroke.GetPoint(RotatedStroke.GetNumPoints()-1).GetPosition();

	double Chord= MODULE_2D(F, L);
	double FLATNESS= 0.2;
	bool FlatStroke= (SM.Sagitta < FLATNESS*Chord);

	double Gap= max(std::fabs(min(F.x, L.x) - bounds.left), std::fabs(bounds.right - max(F.x, L.x)));
	double TOLERANCE= 0.1*max(Chord/2., SM.Sagitta);
	SM.ShortStroke= (Gap < TOLERANCE);

	SM.ShortAndFlat= (SM.ShortStroke && FlatStroke);
}


bool CFitEllipse::GetConvexHull(CStroke &Stroke)
//Calculates the Convex Hull of the Stroke

//Returns true if the resulting stroke still may depict an elliptic arc
//(i.e. contains five or more points)
{
	CStroke Hull= Stroke;
	Hull.TurnIntoConvexHull();
	if(Hull.GetNumPoints() < 5)
		//Convex Hull is not a valid stroke
		return false;

	Stroke= Hull;
	return true;
}


bool CFitEllipse::IsShortArc(CArc Arc)
//Returns true if Arc encompasses and angle smaller than 180 degrees
{
	if(std::fabs(Arc.EndAngle-Arc.StartAngle) < NUMBER_PI)
		return true;
	else
		return false;
}


bool CFitEllipse::IsArcOverflow(CStroke Stroke,
								double BoxLength, double Sagitta)
//Returns true if the arc exceeds (overflows) the bounding box of the stroke

//WARNING! This flag is only valid if the stoke depicts a long arc
{
	double TOLOVERFLOW= 1.15; //Tolerance is set to 15% of the size of the bounding box

	double StrokeRotation= Stroke.GetSlope180();
	double delta= std::fabs(StrokeRotation-Stroke.Arc.Rotation);

	if((delta < NUMBER_PI/8. || delta > 7./8.*NUMBER_PI)){  //Parallel
		if((2*Stroke.Arc.Radius_a > TOLOVERFLOW*BoxLength) ||
		    (Stroke.Arc.Radius_b > TOLOVERFLOW*Sagitta))  //Only valid for long arcs
			return true;
	}

	else if((delta > 3./8.*NUMBER_PI && delta < 5./8.*NUMBER_PI)){  //Perpendicular
		if((2*Stroke.Arc.Radius_b > TOLOVERFLOW*BoxLength) ||
		    (Stroke.Arc.Radius_a > TOLOVERFLOW*Sagitta))  //Only valid for long arcs
			return true;
	}

	//else if(Arc.Radius_a > sqrt(BoxLength*BoxLength + Sagitta*Sagitta))
	else if(sqrt(Stroke.Arc.Radius_a*Stroke.Arc.Radius_a + Stroke.Arc.Radius_b*Stroke.Arc.Radius_b) >
			sqrt(BoxLength*BoxLength/4 + Sagitta*Sagitta)*TOLOVERFLOW)
		return true;

	return false;
}


bool CFitEllipse::IsGapEnds(CStroke Stroke)
//Returns true if at least one of the endpoints of the stroke is too distant from the arc
{
	double MAXGAP= 0.05; //Gap threshold (% of the length)

	if(Stroke.Arc.GapEnds > Stroke.GetLength(10)*MAXGAP)
		return true;

	return false;
}


bool CFitEllipse::IsArcUnderflow(CStroke Stroke,
								double BoxLength, double Sagitta)
//Returns true if the stroke exceeds the bounding box of the arc
//(Detects the bias of DIR approach towards smaller ellipses)

//WARNING! This flag is only valid if the stoke depicts a short arc
{
	double TOLUNDERFLOW= 0.85;  //Tolerance is set to 15% of the size of the bounding box

	double StrokeRotation= Stroke.GetSlope180();
	double delta= std::fabs(StrokeRotation-Stroke.Arc.Rotation);

	if((delta < NUMBER_PI/8. || delta > 7./8.*NUMBER_PI)){  //Parallel
		if((2*Stroke.Arc.Radius_a < TOLUNDERFLOW*BoxLength) ||
		    (Stroke.Arc.Radius_b < TOLUNDERFLOW*Sagitta))  //Only valid for short arcs
			return true;
	}

	else if((delta > 3./8.*NUMBER_PI && delta < 5./8.*NUMBER_PI)){  //Perpendicular
		if((2*Stroke.Arc.Radius_b < TOLUNDERFLOW*BoxLength) ||
		   (Stroke.Arc.Radius_a < TOLUNDERFLOW*Sagitta))  //Only valid for short arcs
			return true;
	}

	//else if(Arc.Radius_a < sqrt(BoxLength*BoxLength + Sagitta*Sagitta))
	else if(sqrt(Stroke.Arc.Radius_a*Stroke.Arc.Radius_a + Stroke.Arc.Radius_b*Stroke.Arc.Radius_b) <
			sqrt(BoxLength*BoxLength/4 + Sagitta*Sagitta)*TOLUNDERFLOW)
		return true;

	return false;
}


bool CFitEllipse::Fit5P(CStroke &Stroke,
						bool CheckQuality, CStrokeMetrics SM)
//Returns the parameters of the ellipse arc that best fit the Stroke
//Uses the approach Five-points (5P)
//Alternatively uses 5P/CH (5P Convex Hull)
//Returns false if a valid fit cannnot be found

//If flag CheckQuality is true, the function checks to detect poor quality fits
//If so, tries to fit its convex hull,
//and returns false if such poor quality fits persist

//5P is considerd poor if:
//	The stroke is short and flat, and the arc is not short (does not cover a small angle)
//	The arc is greater than (overflows) the bounding box of the stroke
//	At least one of the endpoints of the stroke is too distant from the arc
//	The arc is smaller than (underflows) the boundig box

//On return, stroke parameters are updated

{
	//Get a 5P fit
	bool ConvexHull= false;
	CConicParams phi;

	if(!FitEllipse5P(Stroke, phi)){
		if(!GetConvexHull(Stroke))
			return false;
		ConvexHull= true;

		if(!FitEllipse5P(Stroke, phi))
			return false;
	}

	if(!GetArc(phi, Stroke)){
		if(ConvexHull)
			return false;
		else{
			if(!GetConvexHull(Stroke))
				return false;
			ConvexHull= true;

			if(!FitEllipse5P(Stroke, phi) ||
			   !GetArc(phi, Stroke))
				return false;
		}
	}

	//Check if fit is poor
	if(CheckQuality){
		if(DisplayColors){
			Stroke.Arc.ColorR= 255; Stroke.Arc.ColorG= 0; Stroke.Arc.ColorB= 0;
		}

		bool ShortArc= IsShortArc(Stroke.Arc);
		if((SM.ShortAndFlat && !ShortArc) ||
		   (!SM.ShortStroke && ShortArc) ||
		   //(SM.ShortStroke && !ShortArc) ||
		   (!ShortArc && IsArcOverflow(Stroke, SM.BoxLength, SM.Sagitta))){
			if(ConvexHull)
				return false;
			else{
				if(!GetConvexHull(Stroke))
					return false;
				ConvexHull= true;

				if(!FitEllipse5P(Stroke, phi) ||
				   !GetArc(phi, Stroke))
					return false;
				else{
					ShortArc= IsShortArc(Stroke.Arc);
					if((SM.ShortAndFlat && !ShortArc) ||
					   (!SM.ShortStroke && ShortArc) ||
					   //(SM.ShortStroke && !ShortArc) ||
					   (!ShortArc && IsArcOverflow(Stroke, SM.BoxLength, SM.Sagitta)))
					return false;
				}
			}
		}
	}

	//Assign color to display the arc
	if(DisplayColors){
		if(ConvexHull){
			Stroke.Arc.ColorR= 0; Stroke.Arc.ColorG= 255; Stroke.Arc.ColorB= 155;
		}
		else{
			Stroke.Arc.ColorR= 0; Stroke.Arc.ColorG= 155; Stroke.Arc.ColorB= 0;
		}
	}

	return true;
}


bool CFitEllipse::FitDIR(CStroke &Stroke,
						 bool CheckQuality, CStrokeMetrics SM)
//Returns the parameters of the ellipse arc that best fit the Stroke
//Uses the approach by Fitzgibbon et al. (DIR)
//Alternatively uses DIR/CH (DIR Convex Hull)
//Returns false if a valid fit cannnot be found

//If flag CheckQuality is true, the function checks to detect poor quality fits
//If so, tries to fit its convex hull,
//and returns false if such poor quality fits persist

//DIR is considered poor if:
//	The stroke is short and flat, and the arc is not short (does not cover a small angle)
//	The arc is greater than (overflows) the bounding box of the stroke
//	At least one of the endpoints of the stroke is too distant from the arc
//	The arc is smaller than (underflows) the boundig box

//On return, stroke parameters are updated

{
	//Get a DIR fit
	bool ConvexHull= false;
	CConicParams phi;

	if(!FitEllipseDIR(Stroke, phi)){
		if(!GetConvexHull(Stroke))
			return false;
		ConvexHull= true;

		if(!FitEllipseDIR(Stroke, phi))
			return false;
	}

	if(!GetArc(phi, Stroke)){
		if(ConvexHull)
			return false;
		else{
			if(!GetConvexHull(Stroke))
				return false;
			ConvexHull= true;

			if(!FitEllipseDIR(Stroke, phi) ||
			   !GetArc(phi, Stroke))
				return false;
		}
	}

	//Check if fit is poor
	if(CheckQuality){
		if(DisplayColors){
			Stroke.Arc.ColorR= 255; Stroke.Arc.ColorG= 0; Stroke.Arc.ColorB= 0;
		}

		bool ShortArc= IsShortArc(Stroke.Arc);

		if((SM.ShortAndFlat && !ShortArc) ||
		   (!SM.ShortStroke && ShortArc) ||
		   IsGapEnds(Stroke) ||
		   (SM.ShortAndFlat && IsArcUnderflow(Stroke, SM.BoxLength, SM.Sagitta)) ||
		   (!ShortArc && IsArcOverflow(Stroke, SM.BoxLength, SM.Sagitta))){

			if(ConvexHull)
				return false;
			else{
				if(!GetConvexHull(Stroke))
					return false;
				ConvexHull= true;

				if(!FitEllipseDIR(Stroke, phi) ||
				   !GetArc(phi, Stroke))
					return false;
				else{
					ShortArc= IsShortArc(Stroke.Arc);

					if((SM.ShortAndFlat && !ShortArc) ||
					   (!SM.ShortStroke && ShortArc) ||
					   IsGapEnds(Stroke) ||
					   (SM.ShortAndFlat && IsArcUnderflow(Stroke, SM.BoxLength, SM.Sagitta)) ||
					   (!ShortArc && IsArcOverflow(Stroke, SM.BoxLength, SM.Sagitta)))

					   return false;
				}
			}
		}
	}

	//Assign color to display the arc
	if(DisplayColors){
		if(ConvexHull){
			Stroke.Arc.ColorR= 0; Stroke.Arc.ColorG= 155; Stroke.Arc.ColorB= 255;
		}
		else{
			Stroke.Arc.ColorR= 0; Stroke.Arc.ColorG= 0; Stroke.Arc.ColorB= 255;
		}
	}

	return true;
}


bool CFitEllipse::FastFit(CStroke &Stroke, CStrokeMetrics SM)
//Calculates a fast fit automatically
//The approach seeks for an agreement between calculation time and precision

//5P is calculated first, as it is the faster
//If 5P fit is not valid, DIR approach is launched

//Valid fits may be poor quality
//Hence, the function evaluates some metrics to determine the quality of the fit
//and recalculates if needed

//On return, stroke parameters are updated
{
	if(Fit5P(Stroke, true, SM))
		return true;

	else{
		if(ResampleStrokesDIR){
			//Fit the resampled stroke
			CStroke S= Stroke;
			S.Resample(NumResampledPoints);
			if(FitDIR(S, true, SM))
				return true;
			else
				//Alternatively, try to fit the original stoke
				if(FitDIR(Stroke, true, SM)){
					if(DisplayColors){
						Stroke.Arc.ColorR= 0; Stroke.Arc.ColorG= 225; Stroke.Arc.ColorB= 255;
					}
					return true;
				}
				else
					return false;
		}

		else
			//Fit the original stoke
			return (FitDIR(Stroke, true, SM));
	}
}


bool CFitEllipse::FitGEF(CStroke &Stroke, long numStroke, long numSubStroke)
//Returns the parameters of the ellipse arc that best fit the Stroke
//Uses the approach by Szpack et al.(GEF)
//Returns false if a valid fit cannnot be found

//On return, stroke parameters are updated

{
	if(ResampleStrokesGEF)
		Stroke.Resample(NumResampledPointsGEF);

	CConicParams phi;

	if(!FitEllipseGEF(Stroke, numStroke, numSubStroke, phi))
		return false;

	if(!GetArc(phi, Stroke))
		return false;

	if(DisplayColors){
		if(ResampleStrokesGEF){
			Stroke.Arc.ColorR= 255; Stroke.Arc.ColorG= 0; Stroke.Arc.ColorB= 255;
		}
		else{
			Stroke.Arc.ColorR= 155; Stroke.Arc.ColorG= 0; Stroke.Arc.ColorB= 155;
		}
	}

	return true;
}


bool CFitEllipse::FitEllipseArc(CStroke &Stroke, long numStroke, long numSubStroke)
//Returns the parameters of the ellipse arc that best fit the Stroke

//Uses four different approaches:
//	Automatic
//	Five-points (5P)
//	Fitzgibbon (DIR)
//	Szpack (GEF)

//Returns false if a valid fit cannnot be found

//On return, stroke parameters are updated

{
	if(!DisplayColors){
		Stroke.Arc.ColorR= 0; Stroke.Arc.ColorG= 0; Stroke.Arc.ColorB= 0;
	}

	switch (FitEllipseApproach){
		case 0:{  //Automatic
			CStrokeMetrics SM;
			GetStrokeMetrics(Stroke, SM);

			//Automatic fit sequence
			if(FastFit(Stroke, SM))
				return true;
			else if(FitGEF(Stroke, numStroke, numSubStroke))
				return true;
			else
				return false;
		}
		break;

		case 1:{  //5P, Five-points
			CStrokeMetrics SM;
			if(CheckQuality5P)
				GetStrokeMetrics(Stroke, SM);
			if(!Fit5P(Stroke, CheckQuality5P, SM))
				return false;
		}
		break;

		case 2:{  //DIR, Fitzgibbon
			if(ResampleStrokesDIR)
				Stroke.Resample(NumResampledPoints);  //Resample to reduce calculation time

			CStrokeMetrics SM;
			if(CheckQualityDIR)
				GetStrokeMetrics(Stroke, SM);
			if(!FitDIR(Stroke, CheckQualityDIR, SM))
				return false;
		}
		break;

		case 3:{  //GEF, Szpack
			if(!FitGEF(Stroke, numStroke, numSubStroke))
				return false;
		}
		break;
	}//end switch

	return true;
}


double CFitEllipse::GetStrokeArcWidth(CStroke &Stroke)
//Returns the width of the bounding strip containing the stroke
//Returns the outside band width (t_Out) and the inside band width (t_In)

//The stroke is compared against the elliptic arc that better fits it

//At this end, it is assumed that an ellipse
//is a circle that has been stretched in one direction
//Hence, the stroke is rotated and scaled
//using the same factors that should convert the fitted elliptic arc
//into a unit radius circle

//Then the points of the transformed stroke more distant from the unit radius
//are considered to be the most distant from the ellipse
{
	CStroke S= Stroke;

	//Transform the stroke into "circle system"
	POINT2D centre (Stroke.Arc.Centre_x, Stroke.Arc.Centre_y);
	S.Rotate(centre, -Stroke.Arc.Rotation);
	POINT2D scale (1/Stroke.Arc.Radius_a, 1/Stroke.Arc.Radius_b);
	S.Scale(centre, scale);

	//Find the point of the stroke more distant from the ellipse
	double maxdistOut= 0., maxdistIn= 0.;
	long numworstOut= 0, numworstIn= 0;

	for (long i=0; i<(long)Stroke.GetNumPoints(); i++){
		POINT2D pp= S.GetPoint(i).GetPosition();
		pp.x-= Stroke.Arc.Centre_x;
		pp.y-= Stroke.Arc.Centre_y;
		double currentdist= sqrt(pp.x*pp.x + pp.y*pp.y) -1;

		if(currentdist > maxdistOut){
			maxdistOut= currentdist;
			numworstOut= i;
		}
		if(currentdist < maxdistIn){
			maxdistIn= currentdist;
			numworstIn= i;
		}
	}

	//Get the distance from the worst outside point of the stroke to the ellipse
	POINT2D pw= Stroke.GetPoint(numworstOut).GetPosition();
	Stroke.worstOut_x= pw.x;   //Used for display
	Stroke.worstOut_y= pw.y;   //Used for display

	//Transform to "ellipse system"
	pw= pw - centre;
	pw= pw.Rotate(-Stroke.Arc.Rotation);

	POINT2D pe= ClosestPointInEllipse(Stroke.Arc.Radius_a, Stroke.Arc.Radius_b, pw);
	Stroke.t_Out= MODULE_2D(pw, pe);

	//Get the distance from the worst inside point of the stroke to the ellipse
	pw= Stroke.GetPoint(numworstIn).GetPosition();
	Stroke.worstIn_x= pw.x;     //Used for display
	Stroke.worstIn_y= pw.y;     //Used for display

	//Transform to "ellipse system"
	pw= pw - centre;
	pw= pw.Rotate(-Stroke.Arc.Rotation);

	pe= ClosestPointInEllipse(Stroke.Arc.Radius_a, Stroke.Arc.Radius_b, pw);
	Stroke.t_In= MODULE_2D(pw, pe);

	return (Stroke.t_Out + Stroke.t_In);
}


long CFitEllipse::SmoothStroke(CStroke &Stroke,
							   double ToleranceWidth,
							   double widthInitial)
//Returns a "smoothed" stroke
//Smoothing consists in removing odd point of the stroke

//Also returns the number of smoothing stages

//The goal is removing high frequency oscillations
//As they are small, repeatedly removing odd stroke points makes them eventually dissapear

//Smoothing ends when the smoothed stroke:
//	1. does contain no more corners, or
//	2. becomes significantly narrow than the original one

//Odd point that are too distant from ther neighbors are not removed
//First and last points are not removed
{
	long NumSmoothingSteps= 0;

	Stroke.FindCorners(StrokeCornersApproach);
	if(Stroke.NumCorners() <= 0)
		return NumSmoothingSteps;

	while(true){
		CStroke TempStroke= Stroke;
		if(!TempStroke.Filter(2*ToleranceWidth))    // Tuning Parameter
			break;  //No more filtering allowed
		NumSmoothingSteps++;

		double widthSmoothed= GetStrokeArcWidth(TempStroke);

		if(std::fabs(widthInitial-widthSmoothed) >= ToleranceWidth)    // Tuning Parameter
			break;  // Already significantly marrow than the original one
		else{
			Stroke= TempStroke;
			if(Stroke.FindCorners(StrokeCornersApproach) <= 0)
				break;  //No more corners to smooth
		}
	}

	return NumSmoothingSteps;
}


double CFitEllipse::GetFitAndMeritEllipse(CStroke &Stroke, long numStroke, long numSubStroke)
//Returns the parameters of the elliptic arc:
//	the centre
//	the semi-axes lengths
//	the counterclockwise angle of rotation from the x-axis to the major axis
//	the foci

//Returns the merit of the stroke as an elliptic arc as well

//Display information is also returned in the Stroke (worstOut and worstIn)

//Sparse strokes (those with less than five points) are trivially labelled as non-elliptic
//as they represent polygonal lines, not arcs

//The merit equals 1 if the shape is an ellipse,
//and equals 0 when the shape is highly non-elliptical

//Stroke is accepted as representing an elliptic arc if:
//		1. Fits into a minimum tolerance band,
//or, alternatively:
//		2. Its smoothed shape fits into a maximum (or "relaxed") tolerance band.

//3.Stroke is smoothed to remove oscillations,
//But the merit of the smoothed stroke is reduced by ArcSmoothingPenalty

//Parameters numStroke and numSubStroke are used to monitor the detection of arcs
{
	//Reject sparse strokes as candidate to depict elliptic arcs
	//----------------------------------------------------------
	if(Stroke.GetNumPoints() < 5)   //Tuning parameters
		return 0.0;

	//Fit stroke into a temptative ellipse
	//------------------------------------
	if(!FitEllipseArc(Stroke, numStroke, numSubStroke))
		return 0.0;

	//Check the minimum tolerance criterion
	//-------------------------------------
	double MeritArc= 1.0;
	double Tol= GetStrokeArcWidth(Stroke);

	double TolMin= ArcTolMin * Stroke.GetLength(10);

	if(Tol < TolMin)
		return MeritArc;

	//Smooth oscillations
	//-------------------
	CStroke SmoothedStroke= Stroke;
	double TolSmooth= Stroke.GetLength(10)/100;    // Tuning Parameters
	long NumSmoothingSteps= SmoothStroke(SmoothedStroke, TolSmooth, Tol);

	//Check the maximum (relaxed) tolerance criterion
	//-----------------------------------------------
	Tol= GetStrokeArcWidth(SmoothedStroke);

	double TolMax= ArcTolMax * SmoothedStroke.GetLength(10);

	//Assign merit
	if(Tol < TolMin)
		MeritArc= 1.0;
	else if(Tol > TolMax)
		MeritArc= 0.0;
	else
		MeritArc= 1.0 - (Tol-TolMin)/(TolMax-TolMin);

	//Reduce merit for oscillating arcs
	//---------------------------------
	MeritArc= max(0.0, (MeritArc - ArcSmoothingPenalty*NumSmoothingSteps));     // Tuning Parameter

	return MeritArc;
}



//---------------------------------------------------------------------------


//CFittingStrokes::CFittingStrokes()
//{
//	FitLineApproach= 0;       //0= Regression line, 1= Connect tips
//	MinMeritForLine= 0.5;     //Acceptance criteria for fitting straigth lines under REFER approach
//
//	FitEllipseApproach= 0;    //0= Automatic
//	MinMeritForArc= 0.5;      //Acceptance criteria for fitting elliptic arcs under REFER approach
//}


CFittingStrokes::CFittingStrokes(long FitLApproach,
								 double MinMeritLine,
								 long FitAApproach,
								 double MinMeritArc)
:
FitLineApproach (FitLApproach),
MinMeritForLine (MinMeritLine),
FitEllipseApproach (FitAApproach),
MinMeritForArc (MinMeritArc)
{
}


CFittingStrokes::~CFittingStrokes()
{
}


bool CFittingStrokes::FitStroke_CALI(CStroke &InputStroke)
//Returns the merit of the stroke to be a straight line

//Returns 1 if the stroke fits a straight segment
//Returns 0 otherwise

//WARNING! Only straight lines are detected!

//Uses CALI library:
//		Manuel J. Fonseca, Cesar Pimentel and Joaquim A. Jorge
//		CALI: An Online Scribble Recognizer for Calligraphic Interface
//		AAAI Technical Report SS-02-08. Compilation copyright © 2002, AAAI (www.aaai.org)
{
	//Initialization of merits
	InputStroke.MeritLine = 0;
	InputStroke.MeritArc = 0;

	//Start CALI library
	CIRecognizer *CaliRecognizer;
	CaliRecognizer = new CIRecognizer();

	//Define a new CALI stroke
	CIStroke *CALIStroke;      // A Stroke is the set of points from pen-down to pen-up
	CALIStroke = new CIStroke();

	//Copy the InputStroke into the CALI stroke
	for (long ipoint=0; ipoint<(long)InputStroke.GetNumPoints(); ipoint++){
		//Add the point to the current CALI stroke
		CStrokePoint PointI= InputStroke.GetPoint(ipoint);
		POINT2D c= PointI.GetPosition();

		CALIStroke->addPoint((int)c.x, (int)c.y);
	}

	//Define a new scribble that contains just the current stroke
	CIScribble *CALIScribble;  // A Scribble is a sequence of strokes collected within the timeout value
	CALIScribble = new CIScribble();    
	CALIScribble->addStroke(CALIStroke);

	//CALI recognizes the scribble
	CIList<CIGesture *>* gest=CaliRecognizer->recognize(CALIScribble);

	if (!strcmp("Shape",(*gest)[0]->getGestureType())){
		char *name = (*gest)[0]->getName();
		if(!strcmp("Line", name)){
			InputStroke.MeritLine = 1;
		}

		//CALI
	}

	//Stop CALI library
	if (gest) delete gest;
	delete CALIScribble;
	delete CALIStroke;
	delete CaliRecognizer;

	//Fit the stroke into a segment
	CFitLine FL(ReferApp.Ve.FitLineApproach,
				ReferApp.Ve.RemoveTails, ReferApp.Ve.MaxTrimPointsInTail, ReferApp.Ve.MinAngleInTail,
				ReferApp.Ve.LineTolMin, ReferApp.Ve.LineTolMax, ReferApp.Ve.LineSmoothingPenalty);

	FL.GetStraightFit(InputStroke);  //REFER fits the stroke into a line

	if(InputStroke.MeritLine == 1){
		return true;
	}
	return false;
}


void CFittingStrokes::FitStroke_REFER(CStroke &InputStroke,
									  long numStroke, long numSubStroke)   //Used to trace the fitting arcs progression
//Evaluates the merits of fitting the stroke to different types of lines:

//	- First evaluates the merit of the stroke as a straight line
//	  (The segment instance with the best merit is returned as well)

//	- If such a merit is less than 1, then, merit of the stroke as an arc is also evaluated
//	  (The arc instance with the best merit is returned as well)

//Uses REFER vectorization, which is based on:
//	Plumed R., Company P., Varley P.A.C.
//	Human-like Recognition of Straight Lines in Sketched Strokes
//	CEIG 2013, Spanish Computer Graphics Conference. pp. 57-66. ISBN 978-84-695-8333-3.

//	Plumed R., Company P., Varley P.A.C. (2015) 
//	A new approach for perceptually-based fitting strokes into straight segments. 
//	CEIG 2015, XXV Spanish Computer Graphics Conference, pp. 81-89.
{
	//Initialization of merits
	InputStroke.MeritLine= 0;
	InputStroke.MeritArc= 0;

	//Evaluate the merit as a straight line
	//(Returns the tips of the line as well)
	if(ReferApp.Ve.VectorizeLines){
		CFitLine FL(ReferApp.Ve.FitLineApproach,                                                          //Fit parameters
					ReferApp.Ve.RemoveTails, ReferApp.Ve.MaxTrimPointsInTail, ReferApp.Ve.MinAngleInTail,
					ReferApp.Ve.LineTolMin, ReferApp.Ve.LineTolMax, ReferApp.Ve.LineSmoothingPenalty);   //Merit parameters

		//Fit the stroke into a segment
		FL.GetStraightFit(InputStroke);

		//Calculate the merit
		InputStroke.MeritLine= FL.GetMeritLine(InputStroke);
	}

	//Returns the parameters of the elliptic arc
	//and evaluates the merit as an elliptic arc as well
	if (ReferApp.Ve.VectorizeArcs && InputStroke.MeritLine != 1){
		CFitEllipse FE(ReferApp.Ve.ArcTolMin, ReferApp.Ve.ArcTolMax,
					   ReferApp.Ve.ArcSmoothingPenalty,
					   ReferApp.Ve.StrokeCornersApproach,
					   ReferApp.Ve.FitEllipseApproach,
					   ReferApp.Ve.CheckQuality5P,
					   ReferApp.Ve.CheckQualityDIR,
					   ReferApp.Ve.ResampleStrokesDIR,
					   ReferApp.Ve.NumResampledPoints,
					   ReferApp.Ve.ResampleStrokesGEF,
					   ReferApp.Ve.NumResampledPointsGEF,
					   ReferApp.Ve.DisplayColors);

		InputStroke.MeritArc= FE.GetFitAndMeritEllipse(InputStroke, numStroke, numSubStroke);
	}
}


void CFittingStrokes::FitStroke(CStroke &InputStroke,
								long numStroke, long numSubStroke)
//Calculates the best fits for the stroke InputStroke
//The fits are returned into a fitting, which contains:
//		-Segment, for straigth fit
//		-Arc, for elliptic fit

//Evaluates the merits of fitting the stroke to different types of lines
//The merits are returned in:
//		-InputStroke.MeritLine, for straigth fit
//		-InputStroke.MeritArc, for elliptic fit


//Arguments numStroke and numSubStroke are passed for traceability in case of error
{
	switch (ReferApp.Ve.RecognitionApproach){
		case REFER:{
			FitStroke_REFER(InputStroke, 
							numStroke, numSubStroke);    //Used to trace the fitting arcs progression
		}
		break;

		case CALI:{
			FitStroke_CALI(InputStroke);
		}
		break;

		AfxMessageBox("Selected vectorization approach is not implemented");
	}
}


void CFittingStrokes::FitSketch(CDB_Sketch *WorkingSketch)
// Uses the sketch database to fit a Vectorized Line-Drawing

//"WorkingSketch" is the input and output database:
//	-on return, the merits of the lines fitted to their strokes are updated
{
	//Loop to fit all the strokes
	//---------------------------
	for (long i= 0; i<WorkingSketch->GetNumStrokes(); i++){
		CStroke strokei= WorkingSketch->GetStroke(i);
		FitStroke(strokei, i, 0);
		WorkingSketch->SetStroke(i, strokei);  //Update the fitting in the database
	}
}


//---------------------------------------------------------------------------


CSegmentStrokes::CSegmentStrokes()
{
}


CSegmentStrokes::~CSegmentStrokes()
{
}


bool CSegmentStrokes::GetSegmentedStrokes(std::vector<CStroke> &Stroke)
//Substrokes detected while calculating corners are segmented and stored separately

//The approach assumes that every corner is a candidate endpoint of two attached sub-strokes

//Corners must have been previously calculated and stored as a cue of the stroke
{
	int num_corners= Stroke[0].NumCorners();

	if (num_corners <= 0)
		return false;

	//Segment substrokes
	for (long i= 0; i <= num_corners; i++){
		Stroke.push_back(Stroke[0].GetSubStroke(Stroke[0].StrokeCorners[i], Stroke[0].StrokeCorners[i+1]));

		long ii= Stroke.size()-1;
		Stroke[ii].StrokeCorners.push_back(0);
		Stroke[ii].StrokeCorners.push_back(Stroke[0].StrokeCorners[i+1] - Stroke[0].StrokeCorners[i]);
	}

	return true;
}


void CSegmentStrokes::SegmentSketch(CDB_Sketch *WorkingSketch)
// Uses the sketch database to segment, and fit, poly-strokes

//"WorkingSketch" is the input and output database:
//	-on return, each segment of the segmented strokes is saved as a separate stroke
{
	//Loop to label fitted strokes
	//----------------------------
	long NumStrokes= WorkingSketch->GetNumStrokes();
	std::vector<std::vector<long>> Fitted(NumStrokes, std::vector<long>(1, 0));
	long NumLines= 0;

	for (long i= 0; i<NumStrokes; i++){
		CStroke strokei= WorkingSketch->GetStroke(i);
		if(strokei.MeritLine >= ReferApp.Ve.MinMeritForLine || 
		   strokei.MeritArc >= ReferApp.Ve.MinMeritForArc){
			NumLines++;
			Fitted[i][0]= 1;
		}
		else{
			//Recalculate corners of the stroke
			strokei.FindCorners(ReferApp.Ve.StrokeCornersApproach);
			WorkingSketch->SetStroke(i, strokei);
		}
	}

	//Check if fitting is complete
	if(NumLines == NumStrokes){
		return;
	}

	//Try to fit non-fitted strokes after segmenting them
	//---------------------------------------------------
	CArc Arc;

	CFittingStrokes F(ReferApp.Ve.FitLineApproach,
					  ReferApp.Ve.MinMeritForLine,
					  ReferApp.Ve.FitEllipseApproach,
					  ReferApp.Ve.MinMeritForArc);

	for (long i= NumStrokes-1; i>=0; i--){   //Segment from last to first, to prevent renumberings in the numeration of remaining changes
		if(Fitted[i][0] == 0){  //The stroke failed to be fitted
			std::vector<CStroke> StrokeI;
			StrokeI.push_back(WorkingSketch->GetStroke(i));

			if(GetSegmentedStrokes(StrokeI)){
				for(long j= 1; j<(long)StrokeI.size(); j++){
					//Check whether the sub-stroke fits a line
					F.FitStroke(StrokeI[j], i, j);

					if(StrokeI[j].MeritLine >= ReferApp.Ve.MinMeritForLine || 
					   StrokeI[j].MeritArc >= ReferApp.Ve.MinMeritForArc){
						Fitted[i][0]++;
						Fitted[i].push_back(1);
					}
					else{
						Fitted[i].push_back(0);
					}
				}

				//Update sketch if fitting was (fully or partly) achieved on segmented stroke
				//if (Fitted[i][0] == StrokeI.size()){  Segment only if a full segmentation was found
				if (Fitted[i][0] > 0){

					long next= 0;
					for(long j= 1; j<(long)StrokeI.size(); j++){
						if (Fitted[i][j] != 0){
							WorkingSketch->InsertStroke(i+next, StrokeI[j]);
							next++;
						}
					}
					WorkingSketch->erase(i);
				}
			}
		}
	}
}


//---------------------------------------------------------------------------


void GetNonCollinearValences(long NumStroke, double IsolatedTipThreshold, double MaxAngle,
							 std::vector<SEGMENT2D> Segments,
							 long &valence0, long &valence1)
//Calculates and returns the "valence" of both endpoints of stroke NumStroke

//The valence of one endpoint measures its "closeness" to other endpoints
//It measures the number of strokes which have
//one of their endpoints close to the current endpoint of the current stroke

//Nearly collinear strokes are excluded in the calculation of valences

//Closeness is measured as a threshold calculated relative to the size of the stroke

//Returns valences of both tips

//WARNING: this function differs from the valences calculated in the line-drawing, 
//where vertices are already merged.
//Here, valences are estimated by assuming that close tips are candidate to define common vertices
{
	valence0= valence1= 0;

	//Calculate the threshold used to consider two endpoints
	//as belonging to the same vertex
	double threshold= MODULE_2D(Segments[NumStroke].TipBegin, Segments[NumStroke].TipEnd) * IsolatedTipThreshold;   //Size of the stroke

	//Loop to search all endpoints close to any of the tips
	for (long istroke= 0; istroke<(long)Segments.size(); istroke++){
		if(istroke==NumStroke) continue;

		//Only non-collinear strokes are considered
		//(being nearly parallel would mean being nearly collinear,
		//as they are suposed to be connected to a common tip)

		//Check that strokes are not nearly parallel to each other
		//////if(!NearlyParallel(Segments[NumStroke].TipBegin, Segments[NumStroke].TipEnd, 
		//////				   Segments[istroke].TipBegin, Segments[istroke].TipEnd, MaxAngle)){
		double DeltaAngle= std::fabs(Segments[NumStroke].Slope180 - Segments[istroke].Slope180);
		if(DeltaAngle>MaxAngle && std::fabs(DeltaAngle-NUMBER_PI)>MaxAngle){

			if(MODULE_2D(Segments[istroke].TipBegin, Segments[NumStroke].TipBegin) <= threshold)
				valence0++;
			if(MODULE_2D(Segments[istroke].TipBegin, Segments[NumStroke].TipEnd) <= threshold)
				valence1++;

			if(MODULE_2D(Segments[istroke].TipEnd, Segments[NumStroke].TipBegin) <= threshold)
				valence0++;
			if(MODULE_2D(Segments[istroke].TipEnd, Segments[NumStroke].TipEnd) <= threshold)
				valence1++;
		}
	}
}


//----------------------------------------------------------------------------------


COvertracedStroke::COvertracedStroke()
{
	//Overtraced lines parameters
	//MinOvertraceOverlap= 0.00;  // % of the minimum leght of the compared dashes
	//MaxOvertraceAngle= DEGREES_TO_RADIANS(15);
	//MaxOvertraceOffset= 0.05; //% of the minimum leght of the compared dashes
	//IncreaseOvertraceTolerances= 0.50;  //times for shorter strokes

	//OvertraceLineTolMin= 0.07;
	//OvertraceLineTolMax= 0.14;
	//OvertraceLineSmoothPenalty= 0.10;
	//OvertraceMinMeritLine= 0.25; //Assigned as MinMeritForLine/2;

	MinOvertraceOverlap= ReferApp.Ve.MinOvertraceOverlap;
	MaxOvertraceAngle= ReferApp.Ve.MaxOvertraceAngle;
	MaxOvertraceOffset= ReferApp.Ve.MaxOvertraceOffset;
	IncreaseOvertraceTolerances= ReferApp.Ve.IncreaseOvertraceTolerances;

	OvertraceLineTolMin= ReferApp.Ve.OvertraceLineTolMin;
	OvertraceLineTolMax= ReferApp.Ve.OvertraceLineTolMax;
	OvertraceLineSmoothPenalty= ReferApp.Ve.OvertraceLineSmoothPenalty;
	OvertraceMinMeritLine= ReferApp.Ve.OvertraceMinMeritLine;
}


COvertracedStroke::~COvertracedStroke()
{
}


bool COvertracedStroke::NearlyCollinearOvertraces(POINT2D A0, POINT2D A1, POINT2D B0, POINT2D B1, 
												  double ThresholdAngle, double ThresholdOffset,
												  bool alignment, bool offsetSegment)
//Returns true if two segments are collinear, within angular and offset "thresholds"
//A0 and A1 are the endpoints of the first segment
//B0 and B1 are the endpoints of the second segment

//The ThresholdAngle is the maximum angle difference (in radians)
//between each segment and the line that connects their midpoints

//Two flags control the behavior of the function
//(The first is suitable for consecutive lines, while the second is better for overlapping lines):

//The boolean flag "alignment" controls whether the angular alignment between the segments
//and the line defined by their midpoints must be checked as well
//(This is useful for short and consecutive "nearly" aligned strokes, like in dashed lines)

//The boolean flag "offsetSegment" controls whether the offset must be measured
//between each segment and the line that contains the other segment (tip to line if flag is false),
//or between segments (tip to segment if flag is true)
//(The former is useful for consecutive lines, while the latter is better for overlapping lines)
{
	//Check orientations
	//(Return false if differences between both orientations are over the threshold)
	double difAngle= GetAngle2D_PI(A0, A1, B0, B1);
	difAngle= min(difAngle, std::fabs(difAngle-NUMBER_PI)); //angle in the range [0, PI/2]
	if(difAngle > ThresholdAngle)
		return false;

	//Check alignement between each segment and the line that connects their midpoints
	if(alignment){
		POINT2D MA, MB;
		MA.assign((A0.x+A1.x)/2., (A0.y+A1.y)/2.);
		MB.assign((B0.x+B1.x)/2., (B0.y+B1.y)/2.);
		double angleA= min(GetAngle2D_PI(A0, MA, MB), GetAngle2D_PI(A1, MA, MB));
		double angleB= min(GetAngle2D_PI(B0, MB, MA), GetAngle2D_PI(B1, MB, MA));

		if (angleA > ThresholdAngle || angleB > ThresholdAngle)
			return false;
	}

	double Offset= min(MODULE_2D(A0,A1), MODULE_2D(B0,B1)) * ThresholdOffset; //% of the legth of the shortest segment

	//Check if segments are nearly coincident
	if((MODULE_2D(A0,B0)<Offset && MODULE_2D(A1,B1)<Offset) ||
	   (MODULE_2D(A0,B1)<Offset && MODULE_2D(A1,B0)<Offset))
		return true; //Lines nearly overlap (are trivially collinear)

	//Check offset 
	//(closest point of each segment is not too distant from the line defined by the other segment)
	if(Offset > 0.){
		double minDist;
		if(offsetSegment){
			//Measure the minimum offset between segments
			POINT2D Xvoid;
			minDist= min(min(GetDistancePointIntoSegment(B0.x, B0.y, A0.x, A0.y, A1.x, A1.y, Xvoid.x, Xvoid.y),
							 GetDistancePointIntoSegment(B1.x, B1.y, A0.x, A0.y, A1.x, A1.y, Xvoid.x, Xvoid.y)),
						 min(GetDistancePointIntoSegment(A0.x, A0.y, B0.x, B0.y, B1.x, B1.y, Xvoid.x, Xvoid.y),
							 GetDistancePointIntoSegment(A1.x, A1.y, B0.x, B0.y, B1.x, B1.y, Xvoid.x, Xvoid.y)));
		}
		else{ //offsetLine
			//Measure the minimum offset between lines that contain segments
			minDist= min(min(GetDistancePointToLine(B0, A0, A1),
							 GetDistancePointToLine(B1, A0, A1)),
						 min(GetDistancePointToLine(A0, B0, B1),
							 GetDistancePointToLine(A1, B0, B1)));
		}

		return (minDist < Offset);
	}
	else{
		return true;
	}
}


void COvertracedStroke::GetCandidateOvertraces(std::vector<double> MeritOvertracedLine,
											   std::vector<bool> &visitable, long &NumVisitable)
//Candidate overtraces must be reasonably straight
//(Flags of visitable strokes are updated to remove non candidate strokes)
{
	//Discard non-straight strokes as non-candidate overtraces
	for (long i= 0; i<(long)visitable.size(); i++){
		if(MeritOvertracedLine[i] < OvertraceMinMeritLine){
			visitable[i]= false;
			NumVisitable--;
		}
	}
}


void COvertracedStroke::UpdateCandidateOvertraces(std::vector<double> length,
												  std::vector<bool> &visitableCurrent,
												  std::vector<double> &IncreasedTol)
//Tolerances used to check the elegibility of strokes as valid overtraces
//are recalculated after each overtraced line is identified
{
	long NumStrokes= length.size();

	//Calculate the average length of candidate overtraces
	double Lmin, Lmax;
	bool first= true;
	for (long i= 0; i<NumStrokes; i++){
		if(visitableCurrent[i]){
			if(first){
				Lmin= length[i];
				Lmax= length[i];
				first= false;
			}
			else{
				if(length[i] < Lmin) Lmin= length[i];
				if(length[i] > Lmax) Lmax= length[i];
			}
		}
	}

	//Calculate customized tolerances relative to lengths
	for (long i= 0; i<NumStrokes; i++){
		if(visitableCurrent[i]){
			IncreasedTol[i]= 1. + IncreaseOvertraceTolerances * 
							(1.-(std::fabs(length[i]-Lmin)/(Lmax-Lmin)));
		}
	}
}


bool COvertracedStroke::OverlappedToChain(POINT2D First, POINT2D Last, SEGMENT2D New,
										  double &overlap,
										  POINT2D &FirstI, POINT2D &LastI)
//Returns true if segments First-Last and New overlap more than a minimum threshold

//Threshold "MinOvertraceOverlap" is defined as an external parameter)

//Returns the updated First and Last points as well
{
	//Rotate both segments to make First-Last horizontal 
	//(thus making New "nearly" horizontal, as it must be "nearly" collinear)
	//-----------------------------------------------------------------------
	double angleFL= atan2((Last.y-First.y), (Last.x-First.x));   //Angle in the range [-PI, PI]
	double cosFL= cos(-angleFL);
	double sinFL= sin(-angleFL);
	POINT2D FirstR;
	FirstR.x= First.x*cosFL - First.y*sinFL;
	//FirstR.y= First.x*sinFL + First.y*cosFL;
	POINT2D LastR;
	LastR.x= Last.x*cosFL - Last.y*sinFL;
	//LastR.y= Last.x*sinFL + Last.y*cosFL;

	POINT2D NewBeginR;
	NewBeginR.x= New.TipBegin.x*cosFL - New.TipBegin.y*sinFL;
	//NewBeginR.y= New.TipBegin.x*sinFL + New.TipBegin.y*cosFL;
	POINT2D NewEndR;
	NewEndR.x= New.TipEnd.x*cosFL - New.TipEnd.y*sinFL;
	//NewEndR.y= New.TipEnd.x*sinFL + New.TipEnd.y*cosFL;

	//Reorder rotated segments from left to right
	//-------------------------------------------
	bool swappedFL= false;
	if(FirstR.x > LastR.x){ 
		std::swap(FirstR.x, LastR.x);
		//std::swap(FirstR.y, LastR.y);
		swappedFL= true;
	}

	bool swappedNew= false;
	if(NewBeginR.x > NewEndR.x){
		std::swap(NewBeginR.x, NewEndR.x);
		//std::swap(NewBeginR.y, NewEndR.y);
		swappedNew= true;
	}

	//Compare x-coordinates to determine whether segments overlap
	//and update segment First-Last
	//-----------------------------------------------------------
	FirstI= First;
	LastI= Last;

	if(FirstR.x == NewBeginR.x){
		//Both segments begin in parallel

		if(LastR.x < NewEndR.x){
			//Update Last
			if(swappedFL)
				{if(swappedNew) FirstI= New.TipBegin; else FirstI= New.TipEnd;}
			else
				{if(swappedNew) LastI= New.TipBegin; else LastI= New.TipEnd;}

			overlap= abs(LastR.x-FirstR.x);
		}
		else{
			overlap= abs(NewEndR.x-NewBeginR.x);
		}

		return true;
	}

	else if(FirstR.x < NewBeginR.x){
		//Segment FL begins first (segment New is on its right)
		if (NewBeginR.x < LastR.x){
			if(NewEndR.x < LastR.x){
				//New fully overlaps
				overlap= abs(NewEndR.x-NewBeginR.x);
				return true;
			}
			else{
				double MinOverlap = min(abs(LastR.x - FirstR.x), abs(NewEndR.x - NewBeginR.x)) * MinOvertraceOverlap;
				if((LastR.x - NewBeginR.x) >= MinOverlap){
					//New overlaps at least equal than the minimum gap

					//Update Last
					if(swappedFL)
						{if(swappedNew) FirstI= New.TipBegin; else FirstI= New.TipEnd;}
					else
						{if(swappedNew) LastI= New.TipBegin; else LastI= New.TipEnd;}

					overlap= abs(LastR.x - NewBeginR.x);
					return true;
				}

				else{
					//New overlaps less than the minimum gap
					return false;
				}
			}
		}

		else{
			//Do not overlap, as New is to the right of First-Last
			return false;
		}
	}

	else{
		//Segment New begins first
		if (FirstR.x < NewEndR.x){
			if(LastR.x < NewEndR.x){
				//First-Last fully overlaps
				overlap= abs(LastR.x-FirstR.x);
				return true;
			}
			else{
				double MinOverlap = min(abs(LastR.x - FirstR.x), abs(NewEndR.x - NewBeginR.x)) * MinOvertraceOverlap;
				if((NewEndR.x - FirstR.x) >= MinOverlap){
					//First-Last overlaps at least equal than the minimum gap

					//Update First
					if(swappedFL)
						{if(swappedNew) LastI= New.TipEnd; else LastI= New.TipBegin;}
					else
						{if(swappedNew) FirstI= New.TipEnd; else FirstI= New.TipBegin;}

					overlap= abs(NewEndR.x - FirstR.x);
					return true;
				}

				else{
					//First-Last overlaps less than the minimum gap
					return false;
				}
			}
		}

		else{
			//Do not overlap, as New is to the left of First-Last
			return false;
		}
	}
}


void COvertracedStroke::GetNextOvertrace(std::vector<double> IncreasedTol,
										 std::vector<SEGMENT2D> Segments,
										 std::vector<bool> &visitable,
										 std::vector<long> &Chain,
										 POINT2D &First, POINT2D &Last)
//Searches for the next overtrace in a chain of strokes

//Candidate overtraces are strokes which must:
//		-Neither have been previously visited,
//		 nor have been already included in the chain
//		-Have a similar orientation to previous overtraces (within an angle and an offset)
//		-Are overlapped to the the current chain of overtraces defined by tips "First" and "Last"
{
	double overlap, bestOverlap;
	double angle, bestAngle;
	bool candidateOvertrace= false;
	long numStroke;
	POINT2D FirstI, LastI, FirstNew, LastNew;

	//Loop to search for the next overtrace
	for (long i= 0; i<(long)visitable.size(); i++){
		if(visitable[i]){

			if(NearlyCollinearOvertraces(First, Last, Segments[i].TipBegin, Segments[i].TipEnd,
										 IncreasedTol[i]*MaxOvertraceAngle, 
										 IncreasedTol[i]*MaxOvertraceOffset,
										 false, true)){ // Does not check alignment, but checks offset between segments!!

				if(OverlappedToChain(First, Last, Segments[i], overlap, FirstI, LastI)){

					//Save if candidate overtrace is the currently 
					//closer and best aligned to the current chain
					//(Prioritize both closeness and alignment)
					angle= GetAngle2D_PI(First, Last, Segments[i].TipBegin, Segments[i].TipEnd);
					if(!candidateOvertrace || overlap > bestOverlap || angle < bestAngle){
						candidateOvertrace= true;
						bestOverlap= overlap;
						bestAngle= angle;

						numStroke= i;
						FirstNew= FirstI;
						LastNew= LastI;
					}
				}
			}
		}
	}

	//Add selected overtrace and continue recursion
	if(candidateOvertrace){
		//Update Chain
		Chain.push_back(numStroke);
		First= FirstNew;
		Last= LastNew;

		visitable[numStroke]= false;

		//Recursively search next overtrace
		GetNextOvertrace(IncreasedTol, Segments,
						 visitable, Chain, First, Last);
	}
}


void COvertracedStroke::GroupOvertracedLineStrokes(std::vector<std::vector<long>> OvertracedStrokes,
												   std::vector<std::vector<POINT2D>> TipsOvertracedStrokes,
												   CDB_Sketch *Sketch)
//Update sketch database to group overtraced strokes,
//which have been identified previously in "GetOvertracedLineStrokes"

//For every overtraced stroke:
//	- Cluster all the overtraces into a simplified new stroke
//	- Saves the new stroke by replacing the first overtrace
//	- Removes the original strokes containing all overtracess but the first
{
	CFittingStrokes FF(ReferApp.Ve.FitLineApproach,
					   ReferApp.Ve.MinMeritForLine,
					   ReferApp.Ve.FitEllipseApproach,
					   ReferApp.Ve.MinMeritForArc);

	std::vector<long> eraseList;
	CStrokePoint P;

	//Loop to group all the overtraced strokes
	//----------------------------------------
	for (long i= 0; i<(long)OvertracedStrokes.size(); i++){
		CStroke Overtraced;
		//Add the first tip
		P.SetPosition(TipsOvertracedStrokes[i][0].x, TipsOvertracedStrokes[i][0].y);
		Overtraced.AddPoint(P);

		//Accumulate all overtraces of the i-th overtraced stroke
		for (long j= 0; j<(long)OvertracedStrokes[i].size(); j++){
			Overtraced= Overtraced + Sketch->GetStroke(OvertracedStrokes[i][j]);
		}

		//Add the last tip
		P.SetPosition(TipsOvertracedStrokes[i][1].x, TipsOvertracedStrokes[i][1].y);
		Overtraced.AddPoint(P);

		//Calculate the fittings for the set of overtraces
		FF.FitStroke(Overtraced, i, 0);

		//Calculate a simplified version of the accumulated stroke
		CStroke OvertracedSimplified;
		P.SetPosition(Overtraced.Segment.TipBegin.x, 
					  Overtraced.Segment.TipBegin.y);
		OvertracedSimplified.AddPoint(P);
		double Pcx= (Overtraced.Segment.TipBegin.x + Overtraced.Segment.TipEnd.x)/2.;
		double Pcy= (Overtraced.Segment.TipBegin.y + Overtraced.Segment.TipEnd.y)/2.;
		P.SetPosition(Pcx, Pcy);
		OvertracedSimplified.AddPoint(P);
		P.SetPosition(Overtraced.Segment.TipEnd.x, 
					  Overtraced.Segment.TipEnd.y);
		OvertracedSimplified.AddPoint(P);

		//Replace the accumulated stroke by its simplified version
		Overtraced= OvertracedSimplified;
		FF.FitStroke(Overtraced, i, 0);  //

		//Save the accumulated stroke (replacing the first overtrace of the i-th stroke)
		Sketch->SetStroke(OvertracedStrokes[i][0], Overtraced);

		//Annotate overtraces already accumulated (all but the first)
		for (long j= (long)OvertracedStrokes[i].size()-1; j>0; j--){
			eraseList.push_back(std::abs(OvertracedStrokes[i][j]));
		}
	}

	//Remove acummulated overtraced strokes
	sort(eraseList.begin(), eraseList.end());   //Sort to remove backwards
	for (long i= (long)eraseList.size()-1; i>=0; i--){
		Sketch->erase(eraseList[i]);
	}
}


void COvertracedStroke::GetOvertracedLineStrokes(CDB_Sketch *Sketch)
//Uses the fits of the strokes, which must have been calculated in advance,
//to detect chains of strokes candidates to define overtraced line strokes

//Non eligible strokes are marked as non-visitable before starting the recursive search:
//	- Non straight overtraces are discarded

//Chains are calculated recursively, by searching overtraces:
//	- Reasonably aligned to each other.
//	- Reasonably collinear to each other (within an offset threshold).
//	- Overlapping to the current chain
//	  (A minimum overlap is verified)

//Finally, updates sketch database by grouping overtraced strokes
{
	long NumStrokes= Sketch->GetNumStrokes();

	std::vector<std::vector<long>> OvertracedStrokes;
	std::vector<std::vector<POINT2D>> TipsOvertracedStrokes;

	//Recover cues of strokes (calculated while fitting)
	//(this information will be used to parse the strokes)
	//----------------------------------------------------
	std::vector<SEGMENT2D> Segments;
	std::vector<double> MeritOvertracedLine;
	std::vector<double> length(NumStrokes, 0.);   //Stored as a separate vector to reorder from shorter to longer (see "sequence")

	//CFitLine FOVER(ReferApp.Ve.FitLineApproach,
	//			   ReferApp.Ve.RemoveTails, 
	//			   ReferApp.Ve.MaxTrimPointsInTail, 
	//			   ReferApp.Ve.MinAngleInTail,
	CFitLine FOVER(0 , FALSE , 0 , 0. ,            //Parameters to remove tails not required
				   OvertraceLineTolMin,            //Parameters to calculate merits are specific for overtraces
				   OvertraceLineTolMax, 
				   OvertraceLineSmoothPenalty);

	for (long i= 0; i<NumStrokes; i++){
		CStroke iStroke= Sketch->GetStroke(i);
		Segments.push_back(iStroke.Segment);
		//MeritOvertracedLine.push_back(iStroke.MeritLine);
		MeritOvertracedLine.push_back(FOVER.GetMeritLine(iStroke));  //Recalculate the merits of overtraced lines
		length[i]= iStroke.Segment.Length;
	}

	//Create the list of visitable strokes
	std::vector<bool> visitable(NumStrokes, true);
	long NumVisitable= NumStrokes;

	//Discard non candidate strokes (non-straight strokes)
	GetCandidateOvertraces(MeritOvertracedLine, visitable, NumVisitable);

	//Reorder the search sequence, to parse long strokes first
	std::vector<long> sequence;
	for (long i= 0; i<NumStrokes; i++){
		sequence.push_back(i);
	}
	std::vector<double> lengthCopy= length;
	SortMajorFirst(lengthCopy, sequence);
	lengthCopy.clear();

	//Initialize parameters
	std::vector<double> IncreasedTol(NumStrokes, 0.);

	//Loop to recursively merge candidate overlapped strokes
	//------------------------------------------------------
	while(NumVisitable > 1){
		//Update parameters and discard current non candidate strokes
		//(The range of lengths is recalculated each time the set of remaining dashes is reduced)
		std::vector<bool> visitableCurrent= visitable;
		long NumVisitableCurrent= NumVisitable;

		UpdateCandidateOvertraces(length, visitableCurrent, IncreasedTol);

		if(NumVisitableCurrent > 0){
			//Search for the first non-visited candidate overtrace
			long first= 0;
			while(first<NumStrokes && !visitableCurrent[sequence[first]]) first++;

			//Mark current overtrace as visited
			//(to avoid infinite loops if it turns out to be an isolated overtrace)
			visitable[sequence[first]]= false;
			NumVisitable--;
			visitableCurrent[sequence[first]]= false;

			//Start searching for a chain
			std::vector<long> Chain;
			Chain.push_back(sequence[first]);
			POINT2D First= Segments[sequence[first]].TipBegin;
			POINT2D Last= Segments[sequence[first]].TipEnd;

			//Recursively find subsequent overtraces, 
			//and update the tips of the chain
			GetNextOvertrace(IncreasedTol, Segments, 
							 visitableCurrent, Chain, First, Last);

			if((long)Chain.size() > 1){
				//Mark the overtraces as visited lines
				for (long i= 0; i<(long)Chain.size(); i++){
					if(visitable[Chain[i]]){
						visitable[Chain[i]]= false;
						NumVisitable--;
					}
				}
				//Save the chain as an overtraced line
				OvertracedStrokes.push_back(Chain);

				//Save the tips of the chain
				std::vector<POINT2D> Tips;
				Tips.push_back(First);
				Tips.push_back(Last);
				TipsOvertracedStrokes.push_back(Tips);
			}
		}
	}

	//Update working sketch database, by grouping overtraced strokes
	//--------------------------------------------------------------
	GroupOvertracedLineStrokes(OvertracedStrokes, TipsOvertracedStrokes, Sketch);
}



void COvertracedStroke::GetOvertracedArcStrokes(CDB_Sketch *Sketch)

//A bounding box (the smallest rectangle to enclose a fitted curve) 
//is used to test for the curves adjacency for grouping them together.
//If the bounding boxes overlap one another, 
//the sketched strokes associated with the curves will be grouped together

//We find the least square fitting based on the distance between 
//the captured sketching points and the equation of a general conic curve.
//The fitting problem is then reduced to minimizing the conic function:

//	- We obtain the coefficients (a, h, b, g, and f) 
//	  by solving the partial derivatives of E equals to zero.

//	- The central point, major and minor radii, and the rotation angle of an ellipse 
//	  can be obtained from the equation as in [QIN99a].

//The grouped strokes are fitted again

//The overlapping test will be repeated for new fitted curve 
//until there is no more overlapping bounding boxes in the sketch
{
	bool ToBeDone= true;
}


void COvertracedStroke::GetOvertracedStrokes(CDB_Sketch *Sketch)
//Uses the fits of the strokes, which must have been calculated in advance,
//to detect overtraced lines

//As the grouping process only tries to group strokes in the same category:
//  -lines are only grouped with lines, and
//  -curves are only grouped with curves

//Freely based on:
//	Ku D.C., Qin S.F., Wright D.K. (2006) 
//	Interpretation of Overtracing Freehand Sketching for Geometric Shapes
//	WSCG'2006, Conference proceedings ISBN 80-86943-03-8
{
	if (ReferApp.Ve.VectorizeLines)
		GetOvertracedLineStrokes(Sketch);

	if (ReferApp.Ve.VectorizeArcs)
		GetOvertracedArcStrokes(Sketch);
}


//---------------------------------------------------------------------------


CDashedStroke::CDashedStroke()
//Updates parameters while creating the object
{
	//Dashed lines parameters
	//IsolatedTipThreshold= 0.15;  //% of the lenght of the stroke
	//DashSizeMax= 0,5; //times shorter than the longest stroke
	//DashSizeShortRange= 5.00; //times shorter than the average dash
	//DashSizeLongRange= 2.00; //times longer than the average dash
	//DashGapShortRange= 10.0; //times shorter than the average dash
	//DashGapLongRange= 2.0; //times longer than the average dash
	//MaxDashAngle= DEGREES_TO_RADIANS(20);
	//MaxDashOffset= 0.12; //% of the minimum leght of the compared dashes
	//IncreaseDashTolerances= 0.50;  //times for shorter strokes
	//DashBalanceStepAngle= 0.50; //% Balance between Gap and Angle

	//MaxNonStraightDashes= 0.4;  //% of the total amount of dashes in the stroke
	//DashLineTolMin= 0.050; //Commonly more relaxed than LineTolMin (as short strokes are more unprecise);
	//DashLineTolMax= 0.15; //Commonly more relaxed than LineTolMax (as short strokes are more unprecise);
	//DashMinMeritLine= 0.25; //Assigned as MinMeritForLine/2;
	//DashSmoothingPenalty= 0.10;

	IsolatedTipThreshold= ReferApp.Ve.IsolatedTipThreshold;
	DashSizeMax= ReferApp.Ve.DashSizeMax;
	DashSizeShortRange= ReferApp.Ve.DashSizeShortRange;
	DashSizeLongRange= ReferApp.Ve.DashSizeLongRange;
	DashGapShortRange= ReferApp.Ve.DashGapShortRange;
	DashGapLongRange= ReferApp.Ve.DashGapLongRange;
	MaxDashAngle= ReferApp.Ve.MaxDashAngle;
	MaxDashOffset= ReferApp.Ve.MaxDashOffset;
	IncreaseDashTolerances= ReferApp.Ve.IncreaseDashTolerances;
	DashBalanceStepAngle= ReferApp.Ve.DashBalanceStepAngle;

	MaxNonStraightDashes= ReferApp.Ve.MaxNonStraightDashes;
	DashLineTolMin= ReferApp.Ve.DashLineTolMin;
	DashLineTolMax= ReferApp.Ve.DashLineTolMax;
	DashLineSmoothPenalty= ReferApp.Ve.DashLineSmoothPenalty;
	DashMinMeritLine= ReferApp.Ve.DashMinMeritLine;
}


CDashedStroke::~CDashedStroke()
{
}


void CDashedStroke::GetCandidateDashes(CDB_Sketch *Sketch,
									   std::vector<bool> &visitable, long &NumVisitable)
//Candidate dashes must be:
//	- Not too long (as long edges cannot be hidden by small faces)
//	- Isolated (their tips are distant from tips of other neighbor, non-collinear, strokes)

//(Flags of visitable strokes are updated to remove non candidate strokes)
{
	long NumStrokes= Sketch->GetNumStrokes();

	//Determine the parameter to discriminate long strokes
	CStroke stroke0= Sketch->GetStroke(0);
	double maxLength= stroke0.Segment.Length;
	for (long i= 1; i<NumStrokes; i++){
		CStroke strokei= Sketch->GetStroke(i);
		if(strokei.Segment.Length > maxLength) maxLength= strokei.Segment.Length;
	}
	double trimLength= maxLength*DashSizeMax;

	//Discard long strokes as non-candidate dashes
	for (long i= 0; i<NumStrokes; i++){
		CStroke strokei= Sketch->GetStroke(i);
		if(strokei.Segment.Length > trimLength){
			visitable[i]= false;
			NumVisitable--;
		}
	}

	std::vector<SEGMENT2D> Segments;
	for (long istroke= 0; istroke<(long)Sketch->GetNumStrokes(); istroke++){
		CStroke iStroke= Sketch->GetStroke(istroke);
		Segments.push_back(iStroke.Segment);
	}

	//Discard non-isolated strokes
	for (long i= 0; i<NumStrokes; i++){
		if(visitable[i]){
			long valenceTail, valenceHead;
			GetNonCollinearValences(i, IsolatedTipThreshold, MaxDashAngle, 
									Segments,
									valenceTail, valenceHead);

			if(valenceTail > 0 && valenceHead > 0){
				visitable[i]= false;
				NumVisitable--;
			}
		}
	}
}


void CDashedStroke::UpdateCandidateDashes(std::vector<double> length,
										  double &DashGapMin, double &DashGapMax,
										  std::vector<double> &IncreasedTol,
										  std::vector<bool> &visitableCurrent, long &NumVisitableCurrent)
//Lengths of candidate dashes must fit into a range
//(Too long or too short strokes are marked as currently non-visitable
//before starting the search for dashes)

//Tolerances used to check the eligibility of strokes as valid dashes are calculated

//Parameters are recalculated after each dashed line is identified,
//since short strokes are clustered first
//thus leaving only the larger strokes for the following dashed lines
{
	long NumStrokes= length.size();

	//Calculate the average length of candidate dashes
	//and min and max lengths
	double Lmin= 0, Lmax= 0;
	double avgLength= 0.;
	double avgN= 0;
	for (long i= 0; i<NumStrokes; i++){
		if(visitableCurrent[i]){
			if(avgN == 0){
				Lmin= length[i];
				Lmax= length[i];
			}
			else{
				if(length[i] < Lmin) Lmin= length[i];
				if(length[i] > Lmax) Lmax= length[i];
			}
			avgLength+= length[i];
			avgN++;
		}
	}
	avgLength/= avgN;

	//Fix the current length range for chains
	double DashLengthMin, DashLengthMax;
	DashLengthMin= DashLengthMax= DashGapMin= DashGapMax= avgLength;
	DashLengthMin/= DashSizeShortRange;
	DashLengthMax*= DashSizeLongRange;

	DashGapMin/= DashGapShortRange;
	DashGapMax*= DashGapLongRange;

	//Discard strokes out of length range
	for (long i= 0; i<NumStrokes; i++){
		if(visitableCurrent[i]){
			if(length[i]<DashLengthMin || length[i]>DashLengthMax){
				visitableCurrent[i]= false;
				NumVisitableCurrent--;
			}
		}
	}

	//Calculate customized tolerances relative to lengths
	for (long i= 0; i<NumStrokes; i++){
		if(visitableCurrent[i]){
			IncreasedTol[i]= 1. + IncreaseDashTolerances * 
							(1.-(std::fabs(length[i]-Lmin)/(Lmax-Lmin)));
		}
	}
}


bool CDashedStroke::ConsecutiveToChain(POINT2D First, POINT2D Last, SEGMENT2D New,
									   double DashGapMin, double DashGapMax,
									   double DashGapMean, long numDashGaps,
									   double &gap, long &type, double &meritGap)
//Returns true if the segment "New" continues or precedes 
//the segment defined by tips "First" and "Last"
//Segments must be consecutive, not overlapped

//Returns the gap between the consecutive segments

//Returns the type as well:
// type= 0 if overlapped
// type= 1 if Begin is consecutive to First
// type= 2 if End is consecutive to First
// type= 3 if Begin is consecutive to Last
// type= 4 if End is consecutive to Last

//Finally, returns the merit of the gap
{
	double F0= MODULE_2D(First, New.TipBegin);
	double F1= MODULE_2D(First, New.TipEnd);
	double L0= MODULE_2D(Last, New.TipBegin);
	double L1= MODULE_2D(Last, New.TipEnd);

	POINT2D gapBegin, gapEnd;
	type= 0;
	gap= 0.;

	//Detect overlapping strokes, and calculate gap
	//=============================================
	if(min(F0, F1) < min(L0, L1)){
		//The new dash is close to the beginning of the chain
		//---------------------------------------------------
		POINT2D vchain (Last.x - First.x, Last.y - First.y);   //POINT2D is used as a vector here!!

		if(F0 < F1){
			//The beginning of the dash is close to the beginning of the chain
			if(F0 < DashGapMax && F0 > DashGapMin){
				//Neither too distant, nor too close
				POINT2D vdash (New.TipEnd.x - New.TipBegin.x, New.TipEnd.y - New.TipBegin.y);
				POINT2D vgap (New.TipBegin.x - First.x, New.TipBegin.y - First.y);

				if(DOT_PRODUCT_2D(vchain, vdash) < 0. &&
				   DOT_PRODUCT_2D(vchain, vgap) < 0.){
					//Consecutive and not overlapping
					gap= F0;
					type= 1; //Begin is consecutive to First
					gapBegin= First;
					gapEnd= New.TipBegin;
				}
			}
		}

		else{
			//The end of the dash is close to the beginning of the chain
			if(F1 < DashGapMax && F1 > DashGapMin){
				//Neither too distant, nor too close
				POINT2D vdash (New.TipBegin.x - New.TipEnd.x, New.TipBegin.y - New.TipEnd.y);
				POINT2D vgap (New.TipEnd.x - First.x, New.TipEnd.y - First.y);

				if(DOT_PRODUCT_2D(vchain, vdash) < 0. &&
				   DOT_PRODUCT_2D(vchain, vgap) < 0.){
					//Consecutive and not overlapping
					gap= F1;
					type= 2; //End is consecutive to First
					gapBegin= First;
					gapEnd= New.TipEnd;
				}
			}
		}
	}

	else{
		//The new dash is close to the end of the chain
		//---------------------------------------------
		POINT2D vchain (First.x - Last.x, First.y - Last.y);

		if(L0 < L1){
			//The beginning of the dash is close to the end of the chain
			if(L0 < DashGapMax && L0 > DashGapMin){
				//Neither too distant, nor too close
				POINT2D vdash (New.TipEnd.x - New.TipBegin.x, New.TipEnd.y - New.TipBegin.y);
				POINT2D vgap (New.TipBegin.x - Last.x, New.TipBegin.y - Last.y);

				if(DOT_PRODUCT_2D(vchain, vdash) < 0. &&
				   DOT_PRODUCT_2D(vchain, vgap) < 0.){
					//Consecutive and not overlapping
					gap= L0;
					type= 3; //Begin is consecutive to Last
					gapBegin= Last;
					gapEnd= New.TipBegin;
				}
			}
		}

		else{
			//The end of the dash is close to the end of the chain
			if(L1 < DashGapMax && L1 > DashGapMin){
				//Neither too distant, nor too close
				POINT2D vdash (New.TipBegin.x - New.TipEnd.x, New.TipBegin.y - New.TipEnd.y);
				POINT2D vgap (New.TipEnd.x - Last.x, New.TipEnd.y - Last.y);

				if(DOT_PRODUCT_2D(vchain, vdash) < 0. &&
				   DOT_PRODUCT_2D(vchain, vgap) < 0.){
					//Consecutive and not overlapping
					gap= L1;
					type= 4; //End is consecutive to Last
					gapBegin= Last;
					gapEnd= New.TipEnd;
				}
			}
		}
	}

	if(type == 0){
		//Overlapping
		return false;
	}

	//Calculate the merit of the gap
	if(gap < DashGapMin || gap > DashGapMax){
		meritGap= 0.;
	}
	else{
		double averageGap;
		if(numDashGaps > 0){
			averageGap= DashGapMean/numDashGaps;
		}
		else{
			averageGap= 0.5 * MODULE_2D(First, Last);    // WARNING: we are assuming than the default average gap is smaller (1/2) that the smaller dash!!!
		}

		if(gap <= averageGap){
			meritGap= 1. - (averageGap-gap)/(averageGap-DashGapMin);
		}
		else{
			meritGap= 1. - (gap - averageGap)/(DashGapMax-averageGap);
		}
	}

	return true;
}


bool CDashedStroke::NearlyCollinearDashes(POINT2D First, POINT2D Last, POINT2D NewBegin, POINT2D NewEnd, 
										  double ThresholdAngle, double ThresholdStep,
										  long type, double &meritAngle, double &meritOffset)
//Returns true if two segments are collinear, within angular and offset "thresholds"
//First and Last are the endpoints of the first segment
//NewBegin and NewEnd are the endpoints of the second segment

//The angle between the set of previous dashes (first segment) and the new dash (second segment)
//must be minor than a ThresholdAngle (in radians)

//The offset measured as the mean distance 
//between each segment and the line that contains the other segment
//must be minor than a ThresholdOffset

//The function returns the orientation and offset merits
{
	//Check relative orientations between dashes
	//------------------------------------------
	double difAngle= GetAngle2D_PI(First, Last, NewBegin, NewEnd);
	difAngle= min(difAngle, std::fabs(difAngle-NUMBER_PI)); //angle in the range [0, PI/2]

	meritAngle= 1. - min((difAngle/(ThresholdAngle)) , 1.);

	//Return false if differences between both orientations are over the threshold
	if(difAngle > ThresholdAngle)
		return false;

	//Check offset
	//------------
	POINT2D gapBegin, gapEnd;
	if(type == 1){ //Begin is consecutive to First
		gapBegin= First;
		gapEnd= NewBegin;
	}
	else if(type == 2){ //End is consecutive to First
		gapBegin= First;
		gapEnd= NewEnd;
	}
	else if(type == 3){ //Begin is consecutive to Last
		gapBegin= Last;
		gapEnd= NewBegin;
	}
	else if(type == 4){ //End is consecutive to Last
		gapBegin= Last;
		gapEnd= NewEnd;
	}
	else{
		return false;
	}

	//Orientation between gap and previous dashes
	double angleGapToPrevious= GetAngle2D_PI(gapBegin, gapEnd, First, Last);
	angleGapToPrevious= min(angleGapToPrevious, std::fabs(angleGapToPrevious-NUMBER_PI)); //angle in the range [0, PI/2]

	//Orientation between gap and new dash
	double angleGapToNew= GetAngle2D_PI(gapBegin, gapEnd, NewBegin, NewEnd);
	angleGapToNew= min(angleGapToNew, std::fabs(angleGapToNew-NUMBER_PI)); //angle in the range [0, PI/2]

	double meritOffsetAngle= 1. - min((max(angleGapToPrevious, angleGapToNew)/(ThresholdAngle)) , 1.);

	//Step between lines that contain dashes
	double OffsetStep= min(MODULE_2D(First, Last), MODULE_2D(NewBegin, NewEnd)) * ThresholdStep; //% of the length of the shortest segment
	double step= max(GetDistancePointToLine(gapEnd, First, Last), 
					 GetDistancePointToLine(gapBegin, NewBegin, NewEnd));
	double meritOffsetStep= 1. - min(abs(OffsetStep-step) , OffsetStep)/OffsetStep;

	meritOffset= (meritOffsetAngle + meritOffsetStep)/2.;

	return (meritOffsetAngle >0 && meritOffsetStep >0);




	//return (step < Offset);

	//(Points of each segment are not too distant from the line defined by the other segment)
	double meanDist= (GetDistancePointToLine(NewBegin, First, Last) +
					  GetDistancePointToLine(NewEnd, First, Last) +
					  GetDistancePointToLine(First, NewBegin, NewEnd) +
					  GetDistancePointToLine(Last, NewBegin, NewEnd)) /4.;
////////////////////////////////////////////////////////////////
	double NB= GetDistancePointToLine(NewBegin, First, Last);
	double NE= GetDistancePointToLine(NewEnd, First, Last);
	double F= GetDistancePointToLine(First, NewBegin, NewEnd);
	double L= GetDistancePointToLine(Last, NewBegin, NewEnd);

	double mean = (NB + NE + F + L)/4.;
	double variance = ((NB-mean)*(NB-mean) + (NE-mean)*(NE-mean) +(F-mean)*(F-mean) +(L-mean)*(L-mean)) / 4.;
	double standardDeviation = sqrt(variance);
////////////////////////////////////////////////////////////////
//+++++++++++++++++++++
		if(meanDist >= OffsetStep && meritOffset > 0.)
			bool warning= true;
//+++++++++++++++++++++

	return (meanDist < OffsetStep);
}


void CDashedStroke::UpdateChain(SEGMENT2D New, POINT2D &First, POINT2D &Last, 
								long numStroke, long type,
								std::vector<long> &Chain)
//Updates the Current Chain of dashed lines, by adding the stroke numStroke

//According to "type", the stroke is added to the closer tip of the chain
//The corresponding First or Last tip is updated as well

//Vector Chain stores the numbers of the consecutive strokes that define the dashed line
//The numbers of the strokes are incremented in 1, to avoid using 0
//The signs of numbers of the strokes are reversed (thus being negative)
//for strokes linked to the dashed line from head to tail
{
	switch (type){
		case 1:{
			//The beginning of the dash is close to the beginning of the chain
			//Add the reversed stroke New to the beginning of the chain
			First= New.TipEnd;
			Chain.insert(Chain.begin(), -(numStroke+1));
			}
			break;
		case 2:{
			//The end of the dash is close to the beginning of the chain
			//Add stroke New to the beginning of the chain
			First= New.TipBegin;
			Chain.insert(Chain.begin(), (numStroke+1));
			}
			break;
		case 3:{
			//The beginning of the dash is close to the end of the chain
			//Add stroke New to the end of the chain
			Last= New.TipEnd;
			Chain.push_back(numStroke+1);
			}
			break;
		case 4:{
			//The end of the dash is close to the end of the chain
			//Add reversed stroke New to the end of the chain
			Last= New.TipBegin;
			Chain.push_back(-(numStroke+1));
			}
			break;
	}
}


void CDashedStroke::GetNextDash(double DashGapMin, double DashGapMax,
								std::vector<double> IncreasedTol,
								double &DashGapMean, long &numDashGaps,
								std::vector<SEGMENT2D> Segments,
								std::vector<bool> &visitable,
								std::vector<long> &Chain,
								POINT2D &First, POINT2D &Last)
//Searches for the next dash in a chain of strokes

//Candidate dashes are strokes which must:
//		-Neither have been previously visited,
//		 nor have been already included in the chain
//		-Its length must fit in a range
//		 (Too long or too short strokes are marked as visited before starting the search for dashes)

//		-Are consecutive to the "First" or "Last" tips of the current chain of dashes withing a gap range
//		-Have a similar orientation to previous dashes (within an angle and an offset)
{
	double gap;
	long type, bestType;
	double bestGap= DashGapMin;
	double meritAngle, meritGap, meritOffset;
	double bestMeritDash= 0.;
	long numStroke;
	bool candidateDash= false;

	//Loop to search for the next dash
	for (long i= 0; i<(long)visitable.size(); i++){
		//Reject non-visitable dashes
		if(visitable[i]){
			//Reject non-consecutive dashes
			if(ConsecutiveToChain(First, Last, Segments[i], 
								  DashGapMin, DashGapMax, DashGapMean, numDashGaps,
								  gap, type, meritGap)){

				//Reject non-collinear dashes
				if(NearlyCollinearDashes(First, Last, Segments[i].TipBegin, Segments[i].TipEnd, 
										 IncreasedTol[i]*MaxDashAngle, 
										 IncreasedTol[i]*MaxDashOffset, 
										 type, meritAngle, meritOffset)){

					//Save most meritorious candidate
					//(Null merit for offset out of range)
					double meritDash= (meritGap * DashBalanceStepAngle + meritAngle * (1. - DashBalanceStepAngle)) * meritOffset;

					if(!candidateDash || meritDash>bestMeritDash){
						candidateDash= true;
						bestMeritDash= meritDash;
						bestGap= gap;
						bestType= type;
						numStroke= i;
					}
				}
			}
		}
	}

	//Add selected dash and continue recursion
	if(candidateDash){
		UpdateChain(Segments[numStroke], First, Last, numStroke, bestType, Chain);
		visitable[numStroke]= false;

		DashGapMean+= bestGap;
		numDashGaps++;

		//Recursively search next dash
		GetNextDash(DashGapMin, DashGapMax, IncreasedTol, 
					DashGapMean, numDashGaps,
					Segments,
					visitable, Chain, First, Last);
	}
}


void CDashedStroke::GetCandidateChainOfDashes(CDB_Sketch *Sketch,
											  std::vector<std::vector<long>> DashedStrokes,
											  std::vector<SEGMENT2D> &DashedSegments,
											  double &DashGapMin, double &DashGapMax,
											  std::vector<double> &IncreasedTol,
											  std::vector<bool> &visitable, long &NumVisitable)
//Candidate dashed strokes must be:
//	- Isolated (their tips are distant from tips of other neighbor, non-collinear, dashed strokes)

//Parameters required to determine validity of dashed strokes are calculated,
//and flags of visitable dashed strokes are updated
{
	//Calculate segments of dashed strokes
	long NumDashedStrokes= DashedStrokes.size();
	if(NumDashedStrokes == 0) return;

	for (long i= 0; i<NumDashedStrokes; i++){
		CStroke strokeI= Sketch->GetStroke(std::abs(DashedStrokes[i][0])-1);
		POINT2D First;
		if(DashedStrokes[i][0] >= 0)
			First= strokeI.GetPoint(0).GetPosition();
		else
			First= strokeI.GetPoint(strokeI.GetNumPoints()-1).GetPosition();

		CStroke strokeF= Sketch->GetStroke(std::abs(DashedStrokes[i][DashedStrokes[i].size()-1])-1);
		POINT2D Last;
		if(DashedStrokes[i][DashedStrokes[i].size()-1] < 0)
			Last= strokeF.GetPoint(0).GetPosition();
		else
			Last= strokeF.GetPoint(strokeF.GetNumPoints()-1).GetPosition();

		SEGMENT2D dashedStrokeI (First, Last);
		dashedStrokeI.Slope180= GetAngle2D_PI(First, Last);
		dashedStrokeI.Length= MODULE_2D(First, Last);
		DashedSegments.push_back(dashedStrokeI);
	}

	//Calculate length parameters of dashed strokes
	double avgLength= 0.;
	long numDashes= 0;
	double maxLength= 0.;
	for (long i= 0; i<NumDashedStrokes; i++){
		for (long j= 0; j<(long)DashedStrokes[i].size(); j++){
			CStroke strokeIJ= Sketch->GetStroke(std::abs(DashedStrokes[i][j])-1);
			avgLength+= strokeIJ.Segment.Length;
			numDashes++;
			if(j==0 || maxLength<strokeIJ.Segment.Length) maxLength= strokeIJ.Segment.Length; //Max length of strokes of dashed strokes
		}
	}
	avgLength/= numDashes;    //Average length of strokes of dashed strokes

	//Fix the gaps range for all the chains
	//DashGapMin= DashGapMax= avgLength;
	DashGapMin= DashGapMax= maxLength;
	DashGapMin/= DashGapShortRange;
	DashGapMax*= DashGapLongRange;

	//Discard non-isolated dashed strokes
	for (long i= 0; i<NumDashedStrokes; i++){
		if(visitable[i]){
			long valenceI, valenceF;
			GetNonCollinearValences(i, IsolatedTipThreshold, MaxDashAngle, 
									DashedSegments, valenceI, valenceF);

			if(valenceI > 0 && valenceF > 0){
				visitable[i]= false;
				NumVisitable--;
			}
		}
	}

	//Calculate customized tolerances relative to lengths
	double Lmin= DashedSegments[0].Length, Lmax= DashedSegments[0].Length;
	for (long i= 0; i<NumDashedStrokes; i++){
		if(visitable[i]){
			if(DashedSegments[i].Length < Lmin) Lmin= DashedSegments[i].Length;
			if(DashedSegments[i].Length > Lmax) Lmax= DashedSegments[i].Length;
		}
	}

	for (long i= 0; i<NumDashedStrokes; i++){
		if(visitable[i]){
			IncreasedTol[i]= 1. + IncreaseDashTolerances * 
							(1.-(std::fabs(DashedSegments[i].Length-Lmin)/(Lmax-Lmin)));
		}
	}
}


void CDashedStroke::GetNextChainOfDashes(double DashGapMin, double DashGapMax,
										 std::vector<double> IncreasedTol,
										 double &DashGapMean, long &numDashGaps,
										 std::vector<SEGMENT2D> DashedSegments,
										 std::vector<bool> &visitable,
										 std::vector<long> &Chain,
										 POINT2D &First, POINT2D &Last)
//Searches for the next dash in a chain of strokes

//Candidate dashes are strokes which must:
//		-Neither have been previously visited,
//		-nor have been already included in the chain

//		-Are consecutive to the "First" or "Last" tips of the current chain of dashes
//		-Have a similar orientation to previous dashes (within an angle and an offset)
{
	double gap;
	long type, bestType;
	double bestGap= DashGapMin;
	double meritAngle, meritGap, meritOffset;
	double bestMeritDash= 0.;
	long numStroke;
	bool candidateDash= false;

	//Loop to search the next dash
	for (long i= 0; i<(long)visitable.size(); i++){
		if(visitable[i]){
			if(ConsecutiveToChain(First, Last, DashedSegments[i], 
								  DashGapMin, DashGapMax, DashGapMean, numDashGaps,
								  gap, type, meritGap)){

				if(NearlyCollinearDashes(First, Last, DashedSegments[i].TipBegin, DashedSegments[i].TipEnd, 
										 IncreasedTol[i]*MaxDashAngle,
										 IncreasedTol[i]*MaxDashOffset,
										 type, meritAngle, meritOffset)){

					double meritDash= (meritGap * DashBalanceStepAngle + meritAngle * (1. - DashBalanceStepAngle)) * meritOffset;

					if(!candidateDash || meritDash>bestMeritDash){
						candidateDash= true;
						bestMeritDash= meritDash;
						bestGap= gap;
						bestType= type;
						numStroke= i;
					}
				}
			}
		}
	}

	//Add selected dash and continue recursion
	if(candidateDash){
		UpdateChain(DashedSegments[numStroke], First, Last, numStroke, bestType, Chain);
		visitable[numStroke]= true;

		DashGapMean+= bestGap;
		numDashGaps++;

		//Recursively search next chain of dashes
		GetNextChainOfDashes(DashGapMin, DashGapMax, IncreasedTol, 
							 DashGapMean, numDashGaps,
							 DashedSegments,
							 visitable, Chain, First, Last);
	}
}


void CDashedStroke::GroupChainsOfDashes(std::vector<std::vector<long>> DashedStrokes,
										CDB_Sketch *Sketch)
//Update sketch database by grouping dashed strokes,
//which have been identified previously in "DashedStrokes"

//Dashes must have been consecutively stored in "DashedStrokes"
//Besides, their identification number must be positive iff its sense is the same
//as that of the whole dashed stroke (i.e. if they are pointing in the same way),
//and negative otherwise

//The identification numbers of the strokes are increased by one,
//to avoid numbering any of the strokes as "0"

//For every dashed stroke:
//	- Creates a new stroke chaining the tips plus the midpoints of all the dashes
//	  (reversing the sense of each stroke if necessary to align it to the sense of the whole dashed stroke)
//	- Saves the new stroke by replacing the first original dash
//	- Removes the strokes containing all the original dashes but the first
{
	CFittingStrokes FF(ReferApp.Ve.FitLineApproach,
					   ReferApp.Ve.MinMeritForLine,
					   ReferApp.Ve.FitEllipseApproach,
					   ReferApp.Ve.MinMeritForArc);

	std::vector<long> eraseList;

	//Loop to group all the dashed strokes
	//------------------------------------
	for (long i= 0; i<(long)DashedStrokes.size(); i++){
		//Chain all dashes of the i-th stroke
		CStroke ChainedStroke;
		ChainedStroke.Type= DASHED;

		for (long j= 0; j<(long)DashedStrokes[i].size(); j++){
			//Use fitted segment tips
			CStroke strokej= Sketch->GetStroke(std::abs(DashedStrokes[i][j])-1);

			POINT2D F= strokej.Segment.TipBegin;
			CStrokePoint First(F.x, F.y, 0.);
			POINT2D L= strokej.Segment.TipEnd;
			CStrokePoint Last(L.x, L.y, 0.);

			//Swap tips if the dash is reversed
			if(DashedStrokes[i][j] < 0)
				std::swap(First, Last);

			//Add first tip
			ChainedStroke.AddPoint(First);

			//Add middle point
			CStrokePoint Middle;
			Middle.SetPosition((F.x+L.x)/2.,(F.y+L.y)/2.);
			Middle.SetTime(0.);
			ChainedStroke.AddPoint(Middle);

			//Add last tip
			ChainedStroke.AddPoint(Last);
		}

		//Calculate the fittings for the chained stroke
		FF.FitStroke(ChainedStroke, i, 0);

		//Save the chained stroke (replacing its first dash)
		Sketch->SetStroke(std::abs(DashedStrokes[i][0])-1, ChainedStroke);

		//Annotate dashes already chained (all but the first)
		//to remove them later (thus preventing renumberings of the dashes)
		for (long j= (long)DashedStrokes[i].size()-1; j>0; j--){
			eraseList.push_back(std::abs(DashedStrokes[i][j]) - 1);
		}
	}

	//Remove chained dashed strokes
	//-----------------------------
	sort(eraseList.begin(), eraseList.end());   //Sort to remove backwards
	for (long i= (long)eraseList.size()-1; i>=0; i--){
		Sketch->erase(eraseList[i]);
	}
}


void CDashedStroke::ClusterChainsOfDashes(CDB_Sketch *Sketch, std::vector<std::vector<long>> &DashedStrokes)
//Searches groups of chains of dashes by clustering consecutive, isolated and aligned chains of dashes
{
	long NumDashedStrokes= DashedStrokes.size();
	if(NumDashedStrokes <= 1) return;

	//Create the list of visitable dashed strokes
	std::vector<bool> visitable(NumDashedStrokes, true);
	long NumVisitable= NumDashedStrokes;

	//Calculate parameters and discard non candidate strokes
	std::vector<SEGMENT2D> DashedSegments;
	double DashGapMin, DashGapMax;
	std::vector<double> IncreasedTol(NumDashedStrokes, 0.);

	GetCandidateChainOfDashes(Sketch, DashedStrokes, 
							  DashedSegments, 
							  DashGapMin, DashGapMax, IncreasedTol,
							  visitable, NumVisitable);

	//Reorder the search sequence, to parse long dashed strokes first
	std::vector<double> length;
	std::vector<long> sequence;
	for (long i= 0; i<NumDashedStrokes; i++){
		length.push_back(DashedSegments[i].Length);   //Used to reorder the "sequence"
		sequence.push_back(i);
	}
	SortMajorFirst(length, sequence);

	//Loop to merge candidate dashed strokes
	//--------------------------------------
	while(NumVisitable > 1){
		//Search for the first non-visited candidate dashed stroke
		long first= 0;
		while(first<NumDashedStrokes && !visitable[sequence[first]]) first++;   //Prioritize longer dashed strokes

		//Mark current dashed stroke as visited
		//(to avoid infinite loops if it turns out to be an isolated dash)
		visitable[sequence[first]]= false;
		NumVisitable--;

		//Start searching for a chain
		std::vector<long> Chain;
		Chain.push_back(sequence[first]+1);     //Chain stores the numbers of the consecutive strokes that define the dashed line
												//The numbers of the strokes are incremented in 1, to avoid using 0
												//The signs of numbers of the strokes are reversed (thus being negative)
												//for strokes linked to the dashed line from head to tail
		double DashGapMean= 0.;
		long numDashGaps= 0;

		//Recursively find subsequent chains of dashes
		GetNextChainOfDashes(DashGapMin, DashGapMax, IncreasedTol, 
							 DashGapMean, numDashGaps,
							 DashedSegments,
							 visitable, Chain, 
							 DashedSegments[sequence[first]].TipBegin,
							 DashedSegments[sequence[first]].TipEnd);

		if((long)Chain.size() > 1){
			//Update the list of dashed strokes
			for (long i= 0; i<(long)Chain.size(); i++){
				if(visitable[std::abs(Chain[i])-1]){
					visitable[std::abs(Chain[i])-1]= false;
					NumVisitable--;
				}
			}

			//Merge the chains of dashed lines
			for (long j= 1; j<(long)Chain.size(); j++){
				if(Chain[j] > 0){
					for (long k= 0; k<(long)DashedStrokes[std::abs(Chain[j])-1].size(); k++){
						DashedStrokes[std::abs(Chain[0])-1].push_back(DashedStrokes[std::abs(Chain[j])-1][k]);
					}
				}
				else{
					for (long k= (long)DashedStrokes[std::abs(Chain[j])-1].size()-1 ; k>= 0; k--){
						DashedStrokes[std::abs(Chain[0])-1].push_back(DashedStrokes[std::abs(Chain[j])-1][k]);
					}
				}

				//Remove merged dashed stroke
				DashedStrokes.erase(DashedStrokes.begin()+std::abs(Chain[j])-1);
				NumDashedStrokes--;

				visitable.erase(visitable.begin()+ (std::abs(Chain[j])-1));
				if(visitable[std::abs(Chain[j])-1])
					NumVisitable--;

				for (long k= 0; k<(long)Chain.size(); k++){
					if(std::abs(Chain[k]) > std::abs(Chain[j])){
						if(Chain[k] > 0)
							Chain[k]--;
						else
							Chain[k]++;
					}
				}
				Chain.erase(Chain.begin()+j);
			}
		}
	}
}


void CDashedStroke::GetDashedStrokes(CDB_Sketch *Sketch,
									 std::vector<std::vector<long>> &DashedStrokes)
//Uses the fits of the strokes, which must have been calculated in advance,
//to detect chains of strokes candidates to define dashed strokes

//Non eligible strokes are marked as non-visitable before starting the recursive search:
//	- Very long strokes are first discarded as non-candidate dashes
//	  (because dashed lines represent hidden edges,
//	  which can only be hidden by faces delimited by edges larger than the dashes)
//	- Non-isolated strokes are also discarded
//	  (Tips of candidate strokes must be distant from tips of other strokes
//	  that are simultaneously neighbors and non-collinear)

//Chains are calculated recursively, by searching consecutive dashes:
//	- Similar to each other (neither too short nor too long, relative to the average length)
//	- Consecutive or preceding to the current chain
//	  (neither too distant, nor too close to their consecutive or preceding)
//	- Reasonably aligned to each other.

//Finally, candidate dashes must also be:
//	- straight (but not all of them, as a certain quantity of non-straight dashes is tolerated)

//A postprocessing stage clusters consecutive and isolated chains of dashed strokes
//(This is useful to repair dashed stokes interrupted by a defective dash)

//Finally, updates sketch database by grouping dashed strokes into chained strokes

//A future improvement, should distinghish 
//between straight dashed lines and dashed arcs!!!
//This may be done by replacing the constraint of straightness
//by a constant, or "smoth" and "similar", rotation
{
	long NumStrokes= Sketch->GetNumStrokes();
	if(NumStrokes <= 1) return;

	//Recover cues of strokes (calculated while fitting)
	//(this information will be used to parse the strokes)
	//----------------------------------------------------
	std::vector<SEGMENT2D> Segments;
	std::vector<double> MeritDashLine;
	std::vector<double> length(NumStrokes, 0.);   //Stored as a separate vector to reorder from shorter to longer (see "sequence")

	//CFitLine FDASH(ReferApp.Ve.FitLineApproach,
	//			   ReferApp.Ve.RemoveTails,
	//			   ReferApp.Ve.MaxTrimPointsInTail,
	//			   ReferApp.Ve.MinAngleInTail,
	CFitLine FDASH(0 , FALSE , 0 , 0. ,            //Parameters to remove tails not required
				   ReferApp.Ve.DashLineTolMin,     //WARNING: specific parameters used to evaluate dashed lines!
				   ReferApp.Ve.DashLineTolMax,
				   ReferApp.Ve.DashLineSmoothPenalty);

	for (long i= 0; i<NumStrokes; i++){
		CStroke strokei= Sketch->GetStroke(i);
		Segments.push_back(strokei.Segment);
		MeritDashLine.push_back(FDASH.GetMeritLine(strokei));  //Recalculate the merits of dashed lines
		length[i]= strokei.Segment.Length;
	}

	//Create the list of visitable strokes
	std::vector<bool> visitable(NumStrokes, true);
	long NumVisitable= NumStrokes;

	//Discard non candidate strokes (long, or non-isolated)
	GetCandidateDashes(Sketch, visitable, NumVisitable);

	//Reorder the search sequence, to parse short strokes first
	std::vector<long> sequence;
	for (long i= 0; i<NumStrokes; i++){
		sequence.push_back(i);
	}
	std::vector<double> lengthCopy= length;
	SortMinorFirst(lengthCopy, sequence);
	lengthCopy.clear();

	//Initialize parameters
	double DashGapMin, DashGapMax;
	std::vector<double> IncreasedTol(NumStrokes, 0.);

	//Loop to recursively merge candidate dashed strokes
	//--------------------------------------------------
	while(NumVisitable > 1){
		//Update parameters and discard current non candidate strokes
		//(The range of lengths is recalculated each time the set of remaining dashes is reduced)
		std::vector<bool> visitableCurrent= visitable;
		long NumVisitableCurrent= NumVisitable;

		UpdateCandidateDashes(length,
							  DashGapMin, DashGapMax, IncreasedTol,
							  visitableCurrent, NumVisitableCurrent);

		if(NumVisitableCurrent > 0){
			//Search for the first non-visited candidate dash
			long first= 0;
			while(first<NumStrokes && !visitableCurrent[sequence[first]]) first++;   //Prioritize shorter dashes

			//Mark current dash as visited
			//(to avoid infinite loops if it turns out to be an isolated dash)
			visitable[sequence[first]]= false;
			NumVisitable--;
			visitableCurrent[sequence[first]]= false;

			//Start searching for a chain
			std::vector<long> Chain;
			Chain.push_back(sequence[first]+1); //Chain stores the numbers of the consecutive strokes that define the dashed line
												//The numbers of the strokes are incremented in 1, to avoid using 0
												//The signs of numbers of the strokes are reversed (thus being negative)
												//for strokes linked to the dashed line from head to tail
			double DashGapMean= 0.;
			long numDashGaps= 0;

			//Recursively find subsequent dashes
			GetNextDash(DashGapMin, DashGapMax, IncreasedTol,
						DashGapMean, numDashGaps,
						Segments, visitableCurrent, Chain,
						Segments[sequence[first]].TipBegin, Segments[sequence[first]].TipEnd);

			if((long)Chain.size() > 1){
				//Check straightness of dashes in the chain
				long NonStraight= 0;
				for (long i= 0; i<(long)Chain.size(); i++){
					if(MeritDashLine[std::abs(Chain[i])-1] < DashMinMeritLine)
						NonStraight++;
				}

				//Accept chain if its dashes are straight enough
				if(NonStraight <= MaxNonStraightDashes*Chain.size()){
					//Mark the dashes as visited lines
					for (long i= 0; i<(long)Chain.size(); i++){
						if(visitable[std::abs(Chain[i])-1]){
							visitable[std::abs(Chain[i])-1]= false;
							NumVisitable--;
						}
					}

					//Save the chain as a dashed line
					DashedStrokes.push_back(Chain);
				}
			}
		}
		else if(NumVisitable > 1){
			//Search for the first non-visited candidate dash
			long first= 0;
			while(first<NumStrokes && !visitable[sequence[first]]) first++;   //Prioritize shorter dashes

			//Mark current dash as visited
			//(to avoid infinite loops if it turns out to be an isolated dash)
			visitable[sequence[first]]= false;
			NumVisitable--;
		}
	}

	//Clustering consecutive and isolated dashed strokes
	//(To repair dashed stokes interrupted by a defective dash)
	//---------------------------------------------------------
	ClusterChainsOfDashes(Sketch, DashedStrokes);

	//Update working sketch database, by grouping dashed strokes
	//----------------------------------------------------------
	GroupChainsOfDashes(DashedStrokes, Sketch);
}


//--------------------------------------------------------------------------------


void CVectorization::SaveStraightLine(CStroke &InputStroke, SEGMENT2D Segment, CDB_Model *Drawing)
//Saves in the database Drawing the straight line depicted in InputStroke
//The number of the new straight line is saved in the InputStroke
{
	//Link the line-segment to the stroke
	long numLine= Drawing->AddEdgeByVertices(Segment.TipBegin.x, Segment.TipBegin.y, 0, Segment.TipEnd.x, Segment.TipEnd.y, 0);

	//Add the flag for dashed lines
	if(InputStroke.Type == DASHED){
		CEdge NewEdge= Drawing->GetEdge(numLine);
		NewEdge.Dashed= true;
		Drawing->SetEdge(numLine, NewEdge);
	}
}


long CVectorization::SaveLine(CStroke &InputStroke, CDB_Model *Drawing)
//Saves in the database "Drawing" the line depicted in "InputStroke"

//Returns flags indicating the type of line fitted to the stroke:
//	+2 if the line fitted is a straight line with maximum merit
//	+1 if the line fitted is a straight line with low merit
//	 0 if the none line is fitted
//	-1 if the line fitted is an elliptic arc with low merit
//	-2 if the line fitted is an elliptic arc with maximum merit
{
	if (InputStroke.MeritLine == 1){
		SaveStraightLine(InputStroke, InputStroke.Segment, Drawing);
		return +2;
	}

	else{
		if (InputStroke.MeritArc == 1){
			//Save the arc in the database
			//InputStroke.NumArc= Drawing->AddArc(InputStroke.Arc);
			Drawing->AddArc(InputStroke.Arc);
			return -2;
		}

		else if (InputStroke.MeritLine >= ReferApp.Ve.MinMeritForLine){
			SaveStraightLine(InputStroke, InputStroke.Segment, Drawing);
			return +1;
		}

		else if (InputStroke.MeritArc >= ReferApp.Ve.MinMeritForArc){
			//Save the arc in the database
			//InputStroke.NumArc= Drawing->AddArc(InputStroke.Arc);
			Drawing->AddArc(InputStroke.Arc);
			return -1;
		}
	}
	return 0;
}


void CVectorization::ClearVectorize(CReferDoc* pDoc)
//Clear previously vectorized line-drawing
{
	//Erase the corners added to the strokes while in interactive vectorization
	pDoc->DB_InputSketch->EraseSketchCorners();

	//Erase the fits and merits added to the strokes while in interactive vectorization
	pDoc->DB_InputSketch->EraseSketchFitsAndMerits();

	//Reset the batch vectorized sketch (produced by detecting corners, segmenting and grouping)
	pDoc->DB_WorkingSketch->CopyFrom(pDoc->DB_InputSketch);

	//Erase vectorized line drawing
	pDoc->DB_DrawingOn->Clear();
	pDoc->DB_DrawingOff->Clear();

	//Erase model database, which would be updated after vectorizing
	pDoc->DB_Brep_Model->Clear();
}


bool CVectorization::VectorizeStroke(CDB_Model *DrawingOn,
									 CStroke &InputStroke,
									 long numStroke)
//Vectorizes one single stroke

//It is called for online vectorization.
//This is, eager vectorization,
//right after each stroke is depicted
{
	CFittingStrokes F(ReferApp.Ve.FitLineApproach,
					  ReferApp.Ve.MinMeritForLine,
					  ReferApp.Ve.FitEllipseApproach,
					  ReferApp.Ve.MinMeritForArc);

	F.FitStroke(InputStroke, numStroke, 0);
	if(SaveLine(InputStroke, DrawingOn) != 0){
		return true;
	}

	//Try segmenting the stroke, if fitting as as whole was not successful
	else if(ReferApp.Ve.SegmentStrokes){
		bool detected= false;
		CSegmentStrokes S;
		std::vector<CStroke> SegmentedStroke;
		SegmentedStroke.push_back(InputStroke);

		if(S.GetSegmentedStrokes(SegmentedStroke)){
			for(long i= 1; i<(long)SegmentedStroke.size(); i++){
				F.FitStroke(SegmentedStroke[i], numStroke, i);
				if(SaveLine(SegmentedStroke[i], DrawingOn) != 0){
					detected= true; //true if at least one substroke is succesfully vectorized as a line
				}
			}
		}
		return detected;
	}
	return false;
}


void CVectorization::VectorizeSketch(CReferDoc* pDoc)
// Updates vectorization of the whole input sketch

//Vectorization includes:
//	- Removing previous vectorization
//	- Fitting strokes into lines
//	- Segmenting poly-strokes
//	- Grouping overtraced and non-solid strokes

//Note that grouping is done after fitting,
//while segmentation is done while fitting

//The input is:
//	pDoc->DB_InputSketch

//The output are:
//	pDoc->DB_WorkingSketch
//	pDoc->DB_DrawingOn
{
	//Clear previous vectorizations
	ClearVectorize(pDoc);

	if(pDoc->DB_InputSketch== NULL || pDoc->DB_InputSketch->GetNumStrokes() == 0){
		//AfxMessageBox("No sketch to vectorize");
		return;
	}

	//Fit strokes into lines
	//----------------------
	CFittingStrokes F(ReferApp.Ve.FitLineApproach,
					  ReferApp.Ve.MinMeritForLine,
					  ReferApp.Ve.FitEllipseApproach,
					  ReferApp.Ve.MinMeritForArc);

	F.FitSketch(pDoc->DB_WorkingSketch); 

	if(pDoc->DB_WorkingSketch->GetNumStrokes() == 0)
		return;

	//Group overtraced strokes
	//------------------------
	if(ReferApp.Ve.GroupOvertracedStrokes){
		//Search for groups of overtraced strokes
		COvertracedStroke OS;
		OS.GetOvertracedStrokes(pDoc->DB_WorkingSketch);
	}

	//Group non-solid strokes
	//-----------------------
	if(ReferApp.Ve.GroupNonSolidStrokes){
		//Search for groups of dashed strokes
		//(Plus update working sketch database, by grouping dashed strokes)
		std::vector<std::vector<long>> DashedStrokes;
		CDashedStroke DS;
		DS.GetDashedStrokes(pDoc->DB_WorkingSketch, DashedStrokes);

		//Save the list of dashed strokes for visualization purposes
		ReferApp.Ve.DashedStrokes= DashedStrokes;

		//Search for groups of dash-dot strokes (To Be Done!)
		//std::vector<std::vector<long>> DashDotStrokes;
		//CDashDotStroke DDS;
		//DDS.GetDashDotStrokes(fits, DashDotStrokes, pDoc->DB_WorkingSketch);

		//Search for groups of dot strokes (To Be Done!)
		//std::vector<std::vector<long>> DotStrokes;
		//CDotStrokes DoS;
		//DoS.GetDotStrokes(fits, DotStrokes, pDoc->DB_WorkingSketch);
	}

	//Segment poly-strokes
	//--------------------
	if(ReferApp.Ve.SegmentStrokes){
		CSegmentStrokes S;
		S.SegmentSketch(pDoc->DB_WorkingSketch);
	}

	//Use fits previously saved in WorkingSketch to update online line drawing
	//------------------------------------------------------------------------
	long NumStrokes= pDoc->DB_WorkingSketch->GetNumStrokes();
	if(NumStrokes == 0)
		return;

	for (long i= 0; i<NumStrokes; i++){
		CStroke StrokeI= pDoc->DB_WorkingSketch->GetStroke(i);
		SaveLine(StrokeI, pDoc->DB_DrawingOn);
		pDoc->DB_WorkingSketch->SetStroke(i, StrokeI);
	}
}
