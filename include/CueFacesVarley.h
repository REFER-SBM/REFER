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

//Get faces from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C. and Company P.
	//A new algorithm for finding faces in wireframes.
	//Computer-Aided Design 42(4), 279-309

#ifndef _CUE_FACES_VARLEY_DEFINED_
	#define _CUE_FACES_VARLEY_DEFINED_

	//Output data
	//-----------
	class FACE
	{
		public:
			int C; //Number of edges/vertices that define the face
			std::vector <int> V;  //List of vertices that define the face
			std::vector <int> E;  //List of edges that define the face
	};


	//Local data
	//----------
	class TRIANGULAR_LOOP{
		public:
			int E,F,G;
			int Vtotal;
	};


	class QUADRILATERAL_LOOP
	{
		public:
			int E,F,G,H;
	};


	class STRING
	//We define a string to be an ordered sequence of two or more
	//vertices, in which each consecutive pair of vertices is joined by
	//a half-edge. A string may not include the same vertex twice. The
	//minimum string is two vertices joined by a single half-edge. 
	//Each string has a priority associated with it, since some strings are
	//more important than others: they are in a part of the object we
	//particularly want to work on.
	{
		public:
			int Sta;
			int Fin;
			int Pri;
			int Mic;
			std::vector <int> Mid;

		public:
			STRING();
			STRING(const STRING &S);
			~STRING();

			int VertexInString(int Index);
			int TripleExistsInString(int U, int V, int W);
	};


	class THROUGH{
		public:
			int Sta;
			int Mid;
			int Fin;
			int Use;

			THROUGH();
			THROUGH(const THROUGH &T);
			~THROUGH();
	};


	class STRINGS{
		private:
			int ThroughsCanBeMerged(int V, int U, int W);
			bool TripleExistsInAnyString(int U, int V, int W);

		public:
			int Count;
			std::vector <STRING> Strings;
			std::vector <std::vector <THROUGH>> Throughs;
			std::vector <int> ThroughCount;

		public:
			STRINGS();
			STRINGS(const STRINGS &S);
			~STRINGS();

			//bool CompareStrings(STRING A, STRING B);
			void SortStrings();
			void DeleteString(int T, bool Sortnow);
			bool MergeStrings(int S, int T, bool Sortnow, double Devalue);
			int FindStringByVertices(int E, int F);

			void DeleteThrough(int V, int T);
			void MergeThroughs(int V, int U, int W);
			int ThroughsMustBeMerged(int V, int U, int W);

			bool TripleExistsInAnyFace(int U, int V, int W);
			bool StringsCanBeMerged(int T, int S);
	};


	//Finding faces in wireframes
	//---------------------------
	class CFacesVarley{
		private:
			int Inflated;  //Switch between 2D and 3D mode

			//Input data
			//----------
			int VertexCount;
			std::vector <double> VertexX;
			std::vector <double> VertexY;
			//std::vector <double> VertexZ;

			int EdgeCount;
			std::vector <int> EdgeU;
			std::vector <int> EdgeV;

			std::vector <int> EdgeCountForVertex;

			void Input_Varley(CDB_Model *DDBB);

			//Working data
			//------------
			std::vector <TRIANGULAR_LOOP> TriangularLoop;
			int TriangularLoopCount;

			std::vector <QUADRILATERAL_LOOP> QuadrilateralLoop;
			int QuadrilateralLoopCount;

			//Tuneable parameters for string priority
			int InitialPriorityTri;
			int InitialPriorityQuad;
			double PassATurn;

			//Tuneable parameter for parallelism checking
			double tcMp;

			//Subgraphs data
			int SubgraphCount;
			int OuterSubgraph;
			int LeftVertex,RightVertex,TopVertex,BottomVertex;
			std::vector <int> SubgraphForVertex;
			std::vector <int> SubgraphForEdge;

			//Master list of strings
			STRINGS Master;

			//Auxiliar functions
			//------------------
			//Auxiliar for Put_Params_Varley
			void DetermineExtrema();
			void DetermineSubgraphs();
			void GetImplicitData();

			//Common auxiliar functions
			DEGREES AngleFromVertexToVertex (int U, int V);
			int EdgeJoining (int U, int V);

			//Auxiliar for ListParallelQuadrilateralLoops
			int UsedInAQuadrilateralLoop(int E);

			//Auxiliar for IdentifyThroughVertices
			int EdgeOtherEnd (int E, int V);
			int CommonVertex (int E, int F);
			bool UsedInATriangularLoop(int E);
			//bool AllOtherEdgesAreTheSameSide(int V, int E, int F);

			//Auxiliar for ChooseSeedVertex
			int TotalVertexPriority (int V);

			//Auxiliar for MakeFirstMove
			int FindAStringStarting (int V);
			int FindAStringFinishing (int V, int W);
			void MakeAFaceFromAnyTriangle(void);

			//Auxiliar for ForcedMove
			void CreateAFaceFromStrings(STRINGS *Strings, int S, int T);

			//Auxiliar for Mating Values (in MergeArbitraryStrings)
			VECTOR3D *StringEdgeVector3D (VECTOR3D *VS, STRING *S, int n);
			DEGREES StringEdgeAngle2D (STRING *S, int n);
			double FoMAnglesParallel2D (DEGREES A, DEGREES B);
			double FoMParallel3D (VECTOR3D *V1, VECTOR3D *V2);
			double EdgeMatingValue(STRING *S, int i, STRING *T, int j);
			double StringMatingValue(STRING *S, STRING *T);

			//Main flow in Get_Faces_Varley
			//-----------------------------
			void ListExternalTriangularLoops();
			void ListParallelQuadrilateralLoops();
			void IdentifyThroughVertices();
			void CreateInitialStrings();

			int ChooseSeedVertex();
			void MakeFirstMove(int SeedVertex);

			int ForcedMove(STRINGS *Strings);
			bool MergeArbitraryStrings();
			bool CanCompleteAFace(STRINGS *Strings);

			bool EdgeInContour(long NumGraph, long Edge);
			bool FaceIsContour(FACE FaceI);

		public:
			CFacesVarley(CDB_Model *DDBB);
			~CFacesVarley();

			bool Get_Faces_Varley();
			void Output_Faces_Varley(CDB_Model *DDBB);
	};

#endif // _CUE_FACES_VARLEY_DEFINED_
