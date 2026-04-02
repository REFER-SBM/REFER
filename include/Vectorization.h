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

#ifndef _VECTORIZATION_
	#define _VECTORIZATION_


	class CFitLine
	{
		private:
			//Fitting parameters
			long FitLineApproach;
			bool RemoveTails;
			long MaxTrimPointsInTail;
			double MinAngleInTail;
			//Merit parameters
			double LineTolMin;
			double LineTolMax;
			double LineSmoothingPenalty;
			long StrokeCornersApproach;

			void GetRegressionFit(CStroke &InputStroke);
			void FindTips(CStroke &InputStroke,
						  bool RemoveTails, long MaxTrimPointsInTail, double MinAngleInTail);

			double Obliqueness(CStroke InputStroke);
			long SmoothStroke(CStroke &Stroke,
							  double ToleranceWidth, double widthInitial);
		public:
			//CFitLine();
			CFitLine(long FitLApproach,
					 bool Remove_Tails, long Max_Trim_Points_In_Tail, double Min_Angle_In_Tail,  //Required only for FindTips approach
					 double LTMin, double LTMax, double SPenalty);
			CFitLine::~CFitLine();

			void GetStraightFit(CStroke &InputStroke);

			double GetMeritLine(CStroke &Stroke);
	};

	class CFitEllipse
	{
		private:
			double ArcTolMin;
			double ArcTolMax;
			double ArcSmoothingPenalty;

			long StrokeCornersApproach;
			long FitEllipseApproach;
			bool CheckQuality5P;
			bool CheckQualityDIR;
			bool ResampleStrokesDIR;
			long NumResampledPoints;
			bool ResampleStrokesGEF;
			long NumResampledPointsGEF;
			bool DisplayColors;

			class CStrokeMetrics{
				public:
					double BoxLength, Sagitta;
					bool ShortStroke, ShortAndFlat;
					CStrokeMetrics::CStrokeMetrics()
					{
						BoxLength= 0; Sagitta= 0;
						ShortStroke= false; ShortAndFlat= false;
					}
			};

			class CConicParams{
				public:
					double a, b, c, d, e, f;
					CConicParams::CConicParams()
					{
						a= 0; b= 0;c= 0; d= 0; e= 0; f=0;
					}
					CConicParams::CConicParams(double aa, double bb, double cc, double dd, double ee, double ff)
						:a (aa), b (bb), c (cc), d (dd), e (ee), f(ff)
					{
					}
					void CConicParams::SetParams(double aa, double bb, double cc, double dd, double ee, double ff)
					{
						a= aa; b= bb; c= cc; d= dd; e= ee; f= ff;
					}
					void CConicParams::GetParams(double &aa, double &bb, double &cc, double &dd, double &ee, double &ff)
					{
						aa= a; bb= b; cc= c; dd= d; ee= e; ff= f;
					}
			};

			bool IsEllipse(CConicParams phi);
			bool FivePointsConic(double p0[3], double p1[3], double p2[3],
								 double p3[3], double p4[3], 
								 CConicParams &phi);
			bool FitEllipse5P(CStroke InputStroke, CConicParams &phi);
			bool FitEllipseDIR(CStroke InputStroke, CConicParams &phi);

			class SzpakData{
				public:
				bool use_pseudoinverse;
				bool theta_updated;
				double lambda;
				long k;
				double damping_multiplier;
				double gamma;
				long numberOfPoints;
				std::vector<std::vector<double>> F;
				std::vector<std::vector<double>> I;
				double alpha;
				std::vector<std::vector<double>> data_points;

				double tolDelta;
				double tolCost;
				double tolTheta;
				std::vector<std::vector<double>> cost;
				std::vector<std::vector<double>> t;
				std::vector<std::vector<double>> delta;

				std::vector<std::vector<double>> r;
				std::vector<std::vector<double>> jacobian_matrix;
				std::vector<std::vector<double>> jacobian_matrix_barrier;
				std::vector<std::vector<double>> jacobian_matrix_full;
				std::vector<std::vector<double>> H;
			};

			void normalise2dpts(std::vector<std::vector<double>> &pts,
								std::vector<std::vector<double>> &T);
			void LevenbergMarquardtStep(SzpakData &S);
			void LineSearchStep(SzpakData &S);
			void guaranteedEllipseFit(long numStroke, long numSubStroke,
									  std::vector<double> &theta,
									  std::vector<std::vector<double>> pts);
			bool FitEllipseGEF(CStroke InputStroke, long numStroke, long numSubStroke, CConicParams &phi);

			double DistancePointEllipse(const double e[2], const double y[2], double x[2]);
			POINT2D ClosestPointInEllipse(double Ra, double Rb, POINT2D P);
			void GetEndPoints(CStroke &Stroke);
			bool GetArc(CConicParams phi, CStroke &Stroke);

			void GetStrokeMetrics(CStroke Stroke, CStrokeMetrics &SM);
			bool GetConvexHull(CStroke &Stroke);

			bool IsShortArc(CArc Arc);
			bool IsArcOverflow(CStroke Stroke,
							   double BoxLength, double Sagitta);
			bool IsGapEnds(CStroke Stroke);
			bool IsArcUnderflow(CStroke Stroke,
							   double BoxLength, double Sagitta);
			bool Fit5P(CStroke &Stroke,
					   bool CheckQuality, CStrokeMetrics SM);
			bool FitDIR(CStroke &Stroke,
						bool CheckQuality, CStrokeMetrics SM);
			bool FastFit(CStroke &Stroke, CStrokeMetrics SM);
			bool FitGEF(CStroke &Stroke, long numStroke, long numSubStroke);

			bool FitEllipseArc(CStroke &Stroke, long numStroke, long numSubStroke);

			double GetStrokeArcWidth(CStroke &Stroke);
			long SmoothStroke(CStroke &Stroke,
							  double ToleranceWidth,
							  double widthInitial);
		public:
			//CFitEllipse::CFitEllipse();
			CFitEllipse::CFitEllipse(double ATMin, double ATMax, double SPenalty,
									 long SCApproach,
									 long FEApproach,
									 bool Quality5P, bool QualityDIR,
									 bool RStrokes, long NumP,
									 bool RStrokesGEF, long NumPGEF,
									 bool DColors);
			CFitEllipse::~CFitEllipse();

			double GetFitAndMeritEllipse(CStroke &Stroke, long numStroke, long numSubStroke);
	};


	class CFittingStrokes
	{
		private:
			long FitLineApproach;
			double MinMeritForLine;
			long FitEllipseApproach;
			double MinMeritForArc;

			bool FitStroke_CALI(CStroke &InputStroke);
			void FitStroke_REFER(CStroke &InputStroke,
								 long numStroke, long numSubStroke);

		public:
			//CFittingStrokes::CFittingStrokes();
			CFittingStrokes::CFittingStrokes(long FitLApproach,
											 double MinMeritLine,
											 long FitAApproach,
											 double MinMeritArc);
			CFittingStrokes::~CFittingStrokes();

			//OnLine fitting of a single stroke
			void FitStroke(CStroke &InputStroke,
						   long numStroke, long numSubStroke);

			//Offline fitting of the whole sketch
			void FitSketch(CDB_Sketch *WorkingSketch);
	};


	class CSegmentStrokes
	{
		public:
			CSegmentStrokes::CSegmentStrokes();
			CSegmentStrokes::~CSegmentStrokes();
			bool GetSegmentedStrokes(std::vector<CStroke> &Stroke);

			//Offline segmentation of the whole sketch
			void SegmentSketch(CDB_Sketch *WorkingSketch);
	};


	//Auxiliar geometrical function, used for clustering Overtraced and Dashed strokes
	void GetNonCollinearValences(long NumStroke, double IsolatedTipThreshold, double MaxAngle,
								 std::vector<SEGMENT2D> Segments,
								 long &valence0, long &valence1);


	class COvertracedStroke
	{
		private:
			double MinOvertraceOverlap;
			double MaxOvertraceAngle;
			double MaxOvertraceOffset;
			double IncreaseOvertraceTolerances;

			double OvertraceLineTolMin;
			double OvertraceLineTolMax;
			double OvertraceLineSmoothPenalty;
			double OvertraceMinMeritLine;

			bool NearlyCollinearOvertraces(POINT2D A0, POINT2D A1, POINT2D B0, POINT2D B1,
										   double ThresholdAngle, double ThresholdOffset,
										   bool alignment, bool offsetSegment);

			void GetCandidateOvertraces(std::vector<double> MeritOvertracedLine,
										std::vector<bool> &visitable, long &NumVisitable);
			void UpdateCandidateOvertraces(std::vector<double> length,
										   std::vector<bool> &visitableCurrent,
										   std::vector<double> &IncreasedTol);
			bool OverlappedToChain(POINT2D First, POINT2D Last, SEGMENT2D New,
								   double &overlap,
								   POINT2D &FirstI, POINT2D &LastI);
			void GetNextOvertrace(std::vector<double> IncreasedTol,
								  std::vector<SEGMENT2D> Segments,
								  std::vector<bool> &visitable,
								  std::vector<long> &Chain,
								  POINT2D &First, POINT2D &Last);
			void GroupOvertracedLineStrokes(std::vector<std::vector<long>> OvertracedStrokes,
											std::vector<std::vector<POINT2D>> TipsOvertracedStrokes,
											CDB_Sketch *Sketch);

			void GetOvertracedLineStrokes(CDB_Sketch *Sketch);
			void GetOvertracedArcStrokes(CDB_Sketch *Sketch);

		public:
			COvertracedStroke::COvertracedStroke();
			COvertracedStroke::~COvertracedStroke();

			void GetOvertracedStrokes(CDB_Sketch *Sketch);
	};


	class CDashedStroke
	{
		private:
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
			double DashLineTolMin;
			double DashLineTolMax;
			double DashLineSmoothPenalty;
			double DashMinMeritLine;

			void GetCandidateDashes(CDB_Sketch *Sketch,
									std::vector<bool> &visitable, long &NumVisitable);
			void UpdateCandidateDashes(std::vector<double> length,
									   double &DashGapMin, double &DashGapMax,
									   std::vector<double> &IncreasedTol,
									   std::vector<bool> &visitableCurrent, long &NumVisitableCurrent);

			bool ConsecutiveToChain(POINT2D First, POINT2D Last, SEGMENT2D New,
									double DashGapMin, double DashGapMax,
									double DashGapMean, long numDashGaps,
									double &gap, long &type, double &meritGap);
			bool NearlyCollinearDashes(POINT2D A0, POINT2D A1, POINT2D B0, POINT2D B1,
									   double ThresholdAngle, double ThresholdStep,
									   long type, double &meritAngle, double &meritOffset);
			void UpdateChain(SEGMENT2D New, POINT2D &First, POINT2D &Last, 
							 long numStroke, long type,
							 std::vector<long> &Chain);
			void GetNextDash(double DashGapMin, double DashGapMax,
							 std::vector<double> IncreasedTol,
							 double &DashGapMean, long &numDashGaps,
							 std::vector<SEGMENT2D> Segments,
							 std::vector<bool> &visitable,
							 std::vector<long> &Chain,
							 POINT2D &First, POINT2D &Last);

			void GetCandidateChainOfDashes(CDB_Sketch *Sketch,
										   std::vector<std::vector<long>> DashedStrokes,
										   std::vector<SEGMENT2D> &DashedSegments,
										   double &DashGapMin, double &DashGapMax,
										   std::vector<double> &IncreasedTol,
										   std::vector<bool> &visitable, long &NumVisitable);

			void GetNextChainOfDashes(double DashGapMin, double DashGapMax,
									  std::vector<double> IncreasedTol,
									  double &DashGapMean, long &numDashGaps,
									  std::vector<SEGMENT2D> DashedSegments,
									  std::vector<bool> &visitable,
									  std::vector<long> &Chain,
									  POINT2D &First, POINT2D &Last);

			void ClusterChainsOfDashes(CDB_Sketch *Sketch, std::vector<std::vector<long>> &DashedStrokes);
			void GroupChainsOfDashes(std::vector<std::vector<long>> DashedStrokes, CDB_Sketch *Sketch);

		public:
			CDashedStroke::CDashedStroke();
			CDashedStroke::~CDashedStroke();

			void GetDashedStrokes(CDB_Sketch *Sketch,
								  std::vector<std::vector<long>> &DashedStrokes);
	};


	class CVectorization
	{
		private:
			//Update the drawing with fitted lines
			void SaveStraightLine(CStroke &InputStroke, SEGMENT2D Segment, CDB_Model *Drawing);
			long SaveLine(CStroke &InputStroke, CDB_Model *Drawing);

		public:
			void ClearVectorize(CReferDoc* pDoc);

			//Online Vectorization of a single stroke
			bool VectorizeStroke(CDB_Model *DrawingOn,
								 CStroke &InputStroke,
								 long numStroke);

			//Offline vectorization of the whole sketch
			void VectorizeSketch(CReferDoc* pDoc);
	};

#endif  // _VECTORIZATION_
