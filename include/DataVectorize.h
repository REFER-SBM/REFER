
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

#if !defined DATAVECTORIZE_DEFINED
	#define DATAVECTORIZE_DEFINED

	class CDataVectorize{
		public:
			//PARAMETERS TO CONTROL VECTORIZATION
			bool ParseGestures;          // Flag to indicate that gestures must be detected

			//Corner cues
			bool RemoveTails;            // Flag to indicate that tails of strokes that represent tips of straigh lines must be trimmed
			long MaxTrimPointsInTail;    // Parameter to indicate the maximum number of points to trim
			double MinAngleInTail;       // Parameter to indicate the minimum angle to consider a bend as a tail

			long StrokeCornersApproach;

			long RecognitionApproach;

			//Straight Line detection
			bool VectorizeLines;
			double LineTolMin;           // Width of the tolerance strip in % of the length
			double LineTolMax;           // Width of the tolerance strip in % of the length
			double LineSmoothingPenalty; // Number of points that fit the regression line in %

			double MinMeritForLine;      // Minimum merit for accepting a strokes as a good straigth line
			long FitLineApproach;        // Approach: 0= Regression line, 1= Connect tips

			//Ellipse arc detection
			bool VectorizeArcs;
			double ArcTolMin;            // Width of the tolerance strip in % of the length
			double ArcTolMax;            // Width of the tolerance strip in % of the length
			double ArcSmoothingPenalty;  // Number of points that fit the regression line in %

			double MinMeritForArc;       // Minimum merit for accepting a strokes as a good straigth line
			long FitEllipseApproach;     // Approach: 0= automatic, 1= Five-points, 2= Fitzgibbon, 3= Szpack
			bool CheckQuality5P;         // Flag to indicate that 5P fit must checked
			bool CheckQualityDIR;        // Flag to indicate that DIR fit must checked
			bool ResampleStrokesDIR;     // Flag to indicate that strokes must be resampled before DIR fitting
			long NumResampledPoints;     // Number of points for the resampled strokes
			bool ResampleStrokesGEF;     // Flag to indicate that strokes must be resampled before GEF fitting
			long NumResampledPointsGEF;  // Number of points for the resampled strokes
			bool DisplayColors;          // Flag to indicate that elliptic arcs must be displayed in colors depending on the way they were fitted

			//PARAMETERS TO CONTROL GROUPING OF STROKES

			//Overtraced strokes
			bool GroupOvertracedStrokes;

			double MinOvertraceOverlap;
			double MaxOvertraceAngle;
			double MaxOvertraceOffset;
			double IncreaseOvertraceTolerances;
			double OvertraceLineTolMin;         // Minimum width of the tolerance strip in % of the length
			double OvertraceLineTolMax;         // Maximum width of the tolerance strip in % of the length
			double OvertraceLineSmoothPenalty;
			double OvertraceMinMeritLine;       // Number of points that fit the regression line in %

			//Dashed strokes
			bool GroupNonSolidStrokes;
			std::vector<std::vector<long>> DashedStrokes;   //stores the numbers of the stokes that constitute dashed strokes (minus sign to reverse, increased by 1 to prevent using 0)

			double IsolatedTipThreshold;
			double DashSizeMax;
			double DashSizeShortRange;
			double DashSizeLongRange;
			double DashGapShortRange;
			double DashGapLongRange;
			double MaxDashAngle;
			double MaxDashOffset;
			double IncreaseDashTolerances;
			double DashBalanceStepAngle;

			double MaxNonStraightDashes;
			double DashLineTolMin;      // Width of the small tolerance strip in % of the length
			double DashLineTolMax;      // Width of the extended tolerance strip in % of the length
			double DashLineSmoothPenalty;
			double DashMinMeritLine;  // Minimum merit to accept each dash as straigth

			//PARAMETERS TO CONTROL SEGMENTATION OF POLY-STROKES
			bool SegmentStrokes;

			CDataVectorize();
			~CDataVectorize();
	};

#endif // DATAVECTORIZE_DEFINED