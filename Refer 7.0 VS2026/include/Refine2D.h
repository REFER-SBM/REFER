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

#if !defined REFINE2D_DEFINED
	#define REFINE2D_DEFINED 

	class Junctions2D
	{
		private:

			class Delaunay
			{
				private:
					struct EDGE{int p1, p2;};
					struct TRIANGLE{int p1, p2, p3;};

					bool ReorderPoints(std::vector<POINT2D> &p,
									   std::vector<long> &NumPoint);
					bool CircumCircle(double xp, double yp, double x1, double y1,
									  double x2, double y2, double x3, double y3,
									  double &xc, double &yc, double &);
					void Triangulate(std::vector<POINT2D> point,
									 std::vector<TRIANGLE> &v,
									 int &ntri);

				public:
					bool NeighborTips(CDB_Model *DDBB,
									  std::vector<std::vector<long>> &nt);
			};

			std::vector<long> PriorLine;
			std::vector<double> LengthEdge;

			std::vector<std::vector<long>> Neighbor;
			std::vector<std::vector<long>> semiLines;

			struct SLX{
				std::vector<long> sl;
				std::vector<long> lineX;
				std::vector<POINT2D> pointX;
			};
			std::vector<SLX> sLines;

			std::vector<long> valence;
			std::vector<std::vector<long>> dsLines;

			bool MergeCubicCorners;
			double DistanceMergeCubicCorners;
			bool MergeAlignedCubicCorners;
			double MergeRotationCubicCorners;

			bool MergeCarefulLineTips;
			double maxDistance;
			double maxRotation;
			double ReificationMerit;

			bool MergeCasualLineTips;
			double CasualInTol;
			double CasualOutTol;

			bool ReMergeTriplets;
			double maxDistanceTriplets;
			double EmergentTripletsMerit;

			bool ReMergeDangling;
			double maxDistanceDangling;
			double EmergenceDanglingDeMerit;

			CDB_Model *DDBB;

			long GetCubicCornerJunctions(CDB_Model *DDBB);

			void ResetMerits();
			void ReorderLines();

			void GetAllSemiLines();
			double AvgLength(long numTip1, long numTip2);
			long GetFarTip(long CloseTip, long &line);
			double GetIntersection(double x1, double y1, double x2, double y2,
								   double x3, double y3, double x4, double y4,
								   double maxDist,
								   double &x, double &y);
			double MeritRotation(long NumTip, POINT2D newPoint);
			double MeritTips(long NumTip1, long NumTip2,
							 POINT2D &junction, bool &swapTips);
			void DeleteNeighbor(long numNeighbor);

			void UpdateNeighbors(long NumTip1, long NumTip2,
								 long NumTip22);
			void MergeTips(long NumTip1, long NumTip2,
						   POINT2D junction, 
						   double &merit1, double merit2,
						   bool updateNeighbors,
						   bool updateSemilines);
			long MergeCarefulTips();

			void GetTripletSemiLines();
			void RemoveDanglingSemiLine(long num);
			bool SemiLinesIntersect(double x1, double y1, double x11, double y11,
									double x2, double y2, double x22, double y22,
									double &ta, double &tb, POINT2D &px);
			void GetTripletsIntersections();
			void RenumberTripletTips(long v1, long v2, long v3);
			void MergeTriplet(long numDangling, long numNeighbor,
							  CVertex center, double merit,
							  long line2, long line3);
			long GetTriplets();
			long GetPoorTriplets();
			long mergeCasualTriplets();

			void GetDanglingSemiLines();
			double alwDangling(POINT2D p1, POINT2D p11, long junction);
			double meritCasualDangling(POINT2D p1, POINT2D p11, double length,
									   long junction);
			long mergeCasualDanglingLines();

	public:
			Junctions2D(bool i_MergeCubicCorners,
						double i_DistanceMergeCubicCorners,
						bool i_MergeAlignedCubicCorners,
						double i_MergeRotationCubicCorners,
						bool i_MergeCarefulLineTips,
						double i_maxDistance,
						double i_maxRotation,
						double i_ReificationMerit,
						bool i_MergeCasualLineTips,
						double CasualInTol,
						double CasualOutTol,
						bool i_ReMergeTriplets,
						double i_maxDistanceTriplets,
						double i_EmergentTripletsMerit,
						bool i_ReMergeDangling,
						double i_maxDistanceDangling,
						double i_EmergenceDanglingMerit);
			Junctions2D::~Junctions2D();

			long GetJunctions(CDB_Model *DDBB);
	};


	class CRefine2D
	{
		private:
			std::vector<CVertex> Vertices;
			std::vector<CEdge> Edges;

			void RemoveShortEdges(CDB_Model *DDBB);

			void Split_T_Vertices(CDB_Model *DDBB);

			void MergeCollinearEdges(CDB_Model *DDBB);

			void SplitNonTwoManifoldVertices(CDB_Model *DDBB);
			void SplitNonTwoManifoldEdges(CDB_Model *DDBB);

			//--------OptimizeDrawing2D-----------------------------
			double CalculeMUab(double a, double b, double x);
			void WeightsParallelism(CDB_Model *DDBB,
									std::vector<std::vector<double>> &MatrixParallelism2D,
									double ErrorParallelism);
			void WeightsCollinearity(CDB_Model *DDBB,
									 std::vector<std::vector<double>> &MatrixzCollinearity,
									 double ErrorCollinearity2D);
			void WeightsVerticality(CDB_Model *DDBB,
									 std::vector<double> &LineVerticality,
									 double ErrorVerticality2D);

			double GetCostParallelism(CDB_Model *DDBB,
									  std::vector<double> &Solucion2D,
									  std::vector<std::vector<double>> MatrixParallelism);
			double GetCostCollinearity(CDB_Model *DDBB,
									   std::vector<double> &Solucion2D,
									   std::vector<std::vector<double>> MatrixCollinearity);
			double GetCostVerticality(CDB_Model *DDBB,
									   std::vector<double> &Solucion2D,
									   std::vector<double> LineVerticality);

			void GetIncrements(CDB_Model *DDBB,
							   std::vector<double> &ListaIncrementos,
							   double InitialStep,
							   double FinalStep,
							   double StepReduction);

			void GetNewSolutionHillClimbing(std::vector<double> &Solucion,
											std::vector<double> &NewSolution,
											CDataOptHC* pParams);

			double GetCost(CDB_Model *DDBB,
						   std::vector<double> &Solution,
						   std::vector<double> &EvolutionCost,
						   std::vector<double> &EvolutionXY,
						   bool evolution,
						   std::vector<std::vector<double>> MatrixParallelism,
						   std::vector<double> LineVerticality,
						   std::vector<std::vector<double>> MatrixCollinearity);

			void OptimizeDrawing2D(CDB_Model *DDBB);
			//------------------------------------------------------

		public:
			void GetRefineDrawing(CDB_Model *DDBB, bool off_line);
			void DetectJunctions(std::vector<std::vector<long>> &Vertices,
							  std::vector<std::vector<long>> &Edges);
	};

#endif // JUNCTIONSOFFLINE_DEFINED

