
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

#include <vector>

#include "Tools_Geometry.h"  // DEGREES_TO_RADIANS
#include "DataVectorize.h"

CDataVectorize::CDataVectorize()
//Parameters to control the vectorization process
{
	ParseGestures = true;        //Flag to indicate that gestures must be detected

	//Vectorization cues
	//------------------
	RemoveTails= true;           //Remove udesired tails in the tips of strokes that depict straight lines
	MaxTrimPointsInTail= 5;      //Remove up to five points in the tail
	MinAngleInTail= 45;          //Remove bended sub-segments rotated more that a minimum angle

	//Recognition parameters
	//----------------------
	RecognitionApproach = 0;    // 0= REFER, 1= CALI;

	//Acceptance criteria for fitting straigth lines under REFER approach
	VectorizeLines= true;
	LineTolMin= 0.035;
	//LineTolMax= 0.070;
	LineTolMax= 0.100;  //Extended tolerance for lines IFF VectorizeArcs is FALSE
	LineSmoothingPenalty= 0.10;
	//MinMeritForLine= 0.5;
	MinMeritForLine= 0.05;    //Reduced merit for lines IFF VectorizeArcs is FALSE
	FitLineApproach= 0;       // 0= Regression line, 1= Connect tips

	//Acceptance criteria for fitting elliptic arcs under REFER approach
	VectorizeArcs= false;
	ArcTolMin= 0.010;
	ArcTolMax= 0.100;
	ArcSmoothingPenalty= 0.05;
	MinMeritForArc= 0.5;
	FitEllipseApproach= 0; // 0= automatic, 1= Five-points, 2= Fitzgibbon, 3= Szpack
	CheckQuality5P =true;
	CheckQualityDIR =true;
	ResampleStrokesDIR= true;
	NumResampledPoints= 20;
	ResampleStrokesGEF= true;
	NumResampledPointsGEF= 20;
	DisplayColors= true;

	//Grouping parameters
	//-------------------
	//Parameters for overtraced lines
	GroupOvertracedStrokes= false;

	MinOvertraceOverlap= 0.00;  // % of the minimum leght of the compared overtraces
	MaxOvertraceAngle= DEGREES_TO_RADIANS(15);
	MaxOvertraceOffset= 0.05; //% of the minimum leght of the compared overtraces
	IncreaseOvertraceTolerances= 0.50; //times for shorter strokes

	OvertraceLineTolMin= 0.07;  //Commonly more relaxed than LineTolMin (as short strokes are more unprecise)
	OvertraceLineTolMax= 0.14;  //Commonly more relaxed than LineTolMax (as short strokes are more unprecise)
	OvertraceLineSmoothPenalty= 0.10;
	OvertraceMinMeritLine= 0.25; // Calculated as MinMeritForLine/2;

	//Parameters for dashed lines
	GroupNonSolidStrokes= false;

	IsolatedTipThreshold= 0.25;  //% of the size of the dashes in the stroke
	DashSizeMax= 0.50; //times shorter than the longest stroke
	DashSizeShortRange= 5.00; //times shorter than the average dash
	DashSizeLongRange= 2.50; //times longer than the average dash
	DashGapShortRange= 10.00; //times shorter than the average dash
	DashGapLongRange= 2.00; //times longer than the average dash
	MaxDashAngle= DEGREES_TO_RADIANS(20);
	MaxDashOffset= 0.15; //% of the minimum leght of the compared dashes
	IncreaseDashTolerances= 0.50;  //times for shorter dashes
	DashBalanceStepAngle= 0.50; // % Balance between Gap and Angle

	MaxNonStraightDashes= 0.40;  //% of the total amount of dashes in the stroke
	DashLineTolMin= 0.050; //Commonly more relaxed than LineTolMin (as short strokes are more unprecise)
	DashLineTolMax= 0.15; //Commonly more relaxed than LineTolMax (as short strokes are more unprecise)
	DashLineSmoothPenalty= 0.10;
	DashMinMeritLine= 0.25; // Calculated as MinMeritForLine/2

	//Segmentation parameters
	//-----------------------
	SegmentStrokes = true;

	StrokeCornersApproach = 0;   // 0= automatic, 1= IStraw, 2= Silding Strips;
};


CDataVectorize::~CDataVectorize(){
};
