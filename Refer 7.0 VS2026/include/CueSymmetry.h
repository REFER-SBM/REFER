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

#if !defined CUESYMMETRY_VISITED
#define CUESYMMETRY_VISITED


	class CModelSymmetry
	{
		public:
			int VertexCount;
			std::vector <double> VertexX;
			std::vector <double> VertexY;

			int EdgeCount;
			std::vector <long> EdgeU;
			std::vector <long> EdgeV;

			class CFACE
			{
				public:
					int C; //Number of edges/vertices that define the face
					std::vector <long> E;  //List of edges that define the face
					std::vector <long> V;  //List of vertices that define the face

				CFACE();
				~CFACE();
			};

			int FaceCount;
			std::vector <CFACE> Face;

			std::vector <std::vector <long>> EdgesInVertex;
			std::vector <std::vector <long>> FacesInEdge;

		CModelSymmetry();
		CModelSymmetry(const CModelSymmetry& MS);
		~CModelSymmetry();
		void DeleteVertex(long NumVertex);
		void DeleteEdge(long NumEdge);
		void ClearAllFaces();
		double GetFaceSize(long NumFace);
		long GetEdge(long V1, long V2);
	};


	//WARNING: this is an embeeded version of the code
	//used to calculate subgraphs in "CueSubGraphs"
	class CGraphs
	{
		public:
			std::vector <std::vector <long>> PairedVertices;
			std::vector <std::vector <long>> PairedMidPoints;

			int SubgraphCount;
			std::vector <long> SubgraphForVertex;
			std::vector <long> SubgraphForEdge;
			std::vector <long> SubgraphForFace;

		private:
			void SplitEdge(CModelSymmetry &MS, long Position);
			void SplitVertex(CModelSymmetry &MS, long Position);
		public:
			void SplitByCircuit(CModelSymmetry &MS, std::vector<long> Circuit);
			void DetermineSubgraphs(CModelSymmetry MS);
			void DetermineFaceSubgraphs(CModelSymmetry MS);
	};


	class CCueSymmetry
	{
		private:
			//Input data
			//----------
			CModelSymmetry MS_main;  //Full model
			CModelSymmetry MS;       //Current subgraph

			bool bDetectNormalCrossing;
			double MaxParallelCrossing;
			double MinParallelCrossing;
			double MeritWeight_1;

			bool bDetectDistorted;
			double MinSymDistortion;
			double MaxSymDistortion;
			//double MaxParallelFaceSymLines;     !!WARNING: THIS REQUIRES REVISION
			//double MinParallelFaceSymLines;
			double MeritWeight_2;

			bool bSymmetricalEdges;
			double MaxParallelEdgeSymLines;
			double MinParallelEdgeSymLines;
			double MeritWeight_3;

			bool bPrioritizeVertical;
			double minVerticalThreshold;
			double maxVerticalThreshold;
			double MeritWeight_4;

			bool bRemoveBadMerit;
			double MinMerit;

			double IntersectTolInside;
			double IntersectTolParallel;

			double Collinear_angle;
			double Collinear_offset;

			void Input(CDB_Model *DDBB);

			void GetImplicitData();
			void GetSubGraph(CGraphs CG, long NumGraph,
							 std::vector <long> &ReverseMapVertices,
							 std::vector <long> &ReverseMapEdges,
							 std::vector <long> &ReverseMapFaces);

			//Working data
			//------------
			//Lists of endpoints of face axes (skewed symmetry axes used to detect symetry planes)
			//(The two endpoints for i-th axis are stored in FaceAxes[0][i] and FaceAxes[1][i])
			//(If vertices, their indices are stored as positive numbers)
			//(If midpoints, the indices of the edges they belong to are stored
			// switched to negative numbers and displaced by one unit to prevent zero index)
			std::vector <std::vector <long>> FaceAxesU;
			std::vector <std::vector <long>> FaceAxesV;
			std::vector <std::vector <double>> FaceAxesMerit;

			std::vector<long> SymSidesU;  //Initial endpoint of a candidate side
			std::vector<long> SymSidesV;  //Final endpoint of a candidate side
			std::vector<long> SymSidesFace0;  //First face at with the candidate side belongs
			std::vector<long> SymSidesFace1;  //Second face at with the candidate sides belongs
			std::vector<bool> SymSidesCollinear;  //Flag for sides that are collinear to other sides

			std::vector<std::vector<long>> SymPlanesFaces;
			///////////////////////////////////////
			long info_TotalAxes;
			long info_NonBilateralAxes;
			long info_DiscontinuedAxes;
			long info_NonSymmetricEdges;

			long info_Branches;
			long info_BranchesAtInvalidCCR;
			long info_BranchesAtVisitedFaces;
			long info_BranchesAtVisitedCorners;
			long info_ValidBranches;

			long info_TestedCircuits;
			long info_Found;
			long info_RemovedCircuits;

			void PrintInfo();
			///////////////////////////////////////

			//Output data
			//-----------
			std::vector<std::vector<long>> SymPlanes;
			std::vector<double> SymMerits;

			void OutputReset(long FaceCount);
			void Output(long NumGraph,
						std::vector <long> ReverseMapVertices,
						std::vector <long> ReverseMapEdges,
						std::vector <long> ReverseMapFaces);

			//internal functions
			//------------------
			long findSide(POINT2D A1, POINT2D A2, POINT2D P);
			bool SearchInList(std::vector <long> List,
							  long value, long next,
							  long &position, long &delta);
			bool IntersectInside(POINT2D a1, POINT2D b1,
								 POINT2D a2, POINT2D b2,
								 double TolInside,
								 double TolParallel);
			POINT2D GetCorner(long ii);

			void GetPolygonAxes(long NumFace);
			void RemoveAxis(long face, long axis);
			void RemoveNonBilateralAxes(long NumFace);
			void RemoveDiscontinuedMidPointAxes();
			void GetSkewAxes();

			bool AddAxesToSidesList();
			bool AddEdgesToSidesList();

			bool EvenSubgraphsAtCorners(CGraphs CG);
			bool NewPair(std::vector<std::vector<long>> Pairs, long Subgraph0, long Subgraph1);
			bool GetPairs(CGraphs CG, std::vector <std::vector <long>> &Pairs);
			bool EqualSubgraphs(std::vector<long> Circuit);
			bool CircuitIsSymmetryPlane(std::vector<long> Circuit);
			bool IntersectPreviousSides(long U, long V, std::vector<long> Circuit);

			void GetNexts(std::vector<long> &Next,
						  long father, bool FatherIsAxis,
						  std::vector <bool> FacesVisited,
						  std::vector <bool> SymSidesUsed,
						  std::vector<long> Circuit);
			bool VisitNext(std::vector<long> &Circuit,
						   std::vector<long> &CircuitFaces,
						   std::vector <bool> &FacesVisited,
						   std::vector <bool> &SymSidesUsed);
			void GetSymmetryCircuits();

			double MeritCrossingEdges(std::vector<long> Circuit);
			void GetPair(long NumFace, long Current,
						 long &next, long &prev, long &size);
			double Ramp11Merit(double value, double minmerit, double maxmerit);
			double MeritAxes(std::vector<long> Circuit,
							 std::vector<long> CircuitFaces);
			double Ramp01Merit(double value, double minmerit, double maxmerit);
			double MeritSymEdges(std::vector<long> Circuit);
			double MeritVertical(std::vector<long> Circuit);
			double MeritHorizontal(std::vector<long> Circuit);
			void reorder();
			void GetMerits();

		public:
			void GetBilateralSymmetryPlanes(CDB_Model *DDBB);
	};

#endif // CUESYMMETRY_VISITED
