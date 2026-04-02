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

#ifndef VANISHINGPOINTS_VISITED
	#define VANISHINGPOINTS_VISITED

	//Finding Vanishing Points in 2D line drawings
	//--------------------------------------------
	class CCueVanishingPoints{
		private:
			//Data management
			//---------------

			//Input data
			int VertexCount;
			std::vector <POINT2D> Vertex;

			int EdgeCount;
			std::vector <int> EdgeHead;
			std::vector <int> EdgeTail;

			//Polar coordinates reference system
			POINT2D DrawingCentroid;
			void GetDrawingCentroid();
			void InitParams();

			//Parameters
			double BoundingRadius;
			double GetBoundingRadius();
			double InnerRingRadius;       // Ri
			double OuterRingRadius;       // Ro
			double CuttingDistance;       // CD
			double ToleranceCoincident;   //SnapD
			double ToleranceParallel;    //TolCol
			double ToleranceDispersion;   //MinDisp

			//Output data
			long NumGroups;
			std::vector<POINT2D> MeanVP;
			std::vector <std::vector <long>> ParallelEdges;
			std::vector<bool> ParallelGroup;

			void Output_VP();

			//Candidate Vanishing Points
			//--------------------------
			std::vector<POINT2D> CandidateVP;
			std::vector <std::vector <long>> EdgesInVP;

			bool IntersectOutside(POINT2D a1, POINT2D b1,
								  POINT2D a2, POINT2D b2,
								  POINT2D &Int);
			bool IsInRing(POINT2D p);
			void GetCandidateVP();

			//Clustering
			//----------
			std::vector<std::vector<long>> Cluster;

			POINT2D GetClusterCentroid(long numCluster);

			double GetOrientation(POINT2D p);
			void ReorderClustersByOrientation(std::vector<double> &Orientation);
			double GetAngleDifference(double Angle1, double Angle2);
			void GetClusters();

			//Vanishing points
			//----------------
			double GetMedian(long numCluster, POINT2D vp);
			double erfc(double x);
			void GetMeanAndDeviation(long numCluster,
									 std::vector<bool> Outliers,
									 POINT2D &vp,
									 double &meanDist,
									 double &StandardDeviation);
			POINT2D GetRefinedCentroid(long numCluster);
			void GetMeanVP();

			//Groups of edges
			//---------------
			void GetGroupsOfEdges();
			void ReorderClustersBySize();

			//Refine vanishing points
			//-----------------------

			//Intersections
			bool IntersectInside(POINT2D a1, POINT2D b1,
								 POINT2D a2, POINT2D b2);
			bool FindIntersectionsInGroups(long CurrentGroup,
										   std::vector<long> &IntersectedEdges);
			void RemoveTheLessLikelyIntersectedEdge(long CurrentGroup,
													std::vector <long> IntersectedEdges);
			long IntersectExtension(POINT2D a1, POINT2D b1,
									POINT2D a2, POINT2D b2);
			void FilterIntersectedEdges();

			//Groups of parallel lines
			double GetDispersion(long numCluster);
			void GetParallelGroups();

			double GetGroupOrientation(long NumGroup);
			void FilterParallelGroups();

			//Anticonvergence for convergent groups
			bool IntersectDistance(POINT2D a1, POINT2D b1,
								   POINT2D a2, POINT2D b2);
			bool FilterAntiConvergentEdges();

			//Repetitions
			void RemoveVanishingPoints(long CurrentGroup, long RepeatedEdge);
			void FilterRepeatedEdges();

			//Opposite convergence for parallel groups
			void AddEdgeInGroup(long NewEdge, long NumGroup);
			void FilterOppositeConvergence();

			//Isolated edges
			void ResampleIsolatedParallelEdges(std::vector<bool> &IsolatedEdge);
			void ResampleIsolatedConvergentEdges(std::vector<bool> &IsolatedEdge);
			void ResampleIsolatedEdges();

		public:
			CCueVanishingPoints(CDB_Model *DDBB);
			CCueVanishingPoints(int IVertexCount,
								std::vector <POINT2D> IVertex,
								int IEdgeCount,
								std::vector <int> IEdgeHead,
								std::vector <int> IEdgeTail);

			std::vector <std::vector <long>> GetVanishingPoints();
	};
#endif // VANISHINGPOINTS_VISITED
