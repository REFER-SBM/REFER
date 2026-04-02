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

#ifndef DB_MODEL_VISITED
	#define DB_MODEL_VISITED

	//----------------------------------------------------------------------

	// VERTICES
	class CVertex
	// Vertices are the endpoints of edges and connect the edges to each other
	{
		public:
			double x, y, z, merit;
		private:

		public:
			CVertex();
			CVertex(double xx, double yy, double zz);
			CVertex(double xx, double yy, double zz, double merit);
			CVertex(CVertex const &V);
			~CVertex();
			void Clear();
	};

	//----------------------------------------------------------------------

	// VECTORS
	class CVector
	// Vector is an oriented segment pointing from the ogirin (0, 0, 0), 
	// to a triplet of coordiantes (x, y, z)
	{
		public:
			//Components
			double x, y, z;

			//Merit
			double merit;

			//Tips
			CVertex Head, Tail;
		private:

		public:
			CVector();
			CVector(double xx, double yy);
			CVector(double xx, double yy, double zz);
			CVector(double xx, double yy, double zz, double merit);
			CVector(CVertex const &newHead, CVertex const &newTail);
			CVector(CVector const &V);
			~CVector();
			void Clear();
	};

	//----------------------------------------------------------------------
	// SET OF VERTICES
	class CVerticesSet
	{
		private:
			std::vector<CVertex> SetOfVertices;

		public:
			CVerticesSet();
			CVerticesSet(CVerticesSet const &VS);
			~CVerticesSet();
			void Clear();
			long GetSize();
			void AddVertex(CVertex const &Vertex);
			CVertex GetVertex(long Position);
			long GetVertexNum(CVertex const &Vertex);
			void SetVertex(long Position, CVertex const &Vertex);
			void RemoveVertex(long Position);

			void GetBoundingBox(double &Xmin, double &Ymin, double &Zmin,
								double &Xmax, double &Ymax, double &Zmax);
	};


	//----------------------------------------------------------------------
	// EDGES
	class CEdge
	// Edge defined by its two vertices: Tail ---> Head
	// It may contain some attributes and flags:
	//		RGB color codes
	{
		public:
			long Head, Tail;
			int ColorR, ColorG, ColorB;
			bool Dashed;

		public:
			CEdge();
			CEdge(long const &newHead, long const &newTail);
			CEdge(CEdge const &E);
			~CEdge();
	};


	//----------------------------------------------------------------------
	// SET OF EDGES
	class CEdgesSet
	{
		private:
			std::vector<CEdge> SetOfEdges;
		public:
			CEdgesSet();
			CEdgesSet(CEdgesSet const &ES);
			~CEdgesSet();
			void Clear();
			long GetSize();
			void AddEdge(CEdge const &Edge);
			CEdge GetEdge(long Position);
			long GetEdgeNum(CEdge const &Edge);
			void SetEdge(long Position, CEdge const &Edge);
			void RemoveEdge(long Position);
	};

	//----------------------------------------------------------------------
	// ARCS
	class CArc
	// Arc defined by 
	//	the centre (x,y)
	//	the radius (for the particular case of a circle)
	//	the semi-axes lengths (a, b)
	//	the counterclockwise angle of rotation from the x-axis to the major axis
	// It may contain some attributes and flags:
	//		RGB color codes
	{
		public:
			double Centre_x;
			double Centre_y;
			double Radius_a;
			double Radius_b;
			double Rotation;
			double Focus1_x;
			double Focus1_y;
			double Focus2_x;
			double Focus2_y;

			double StartAngle;  //Relative to the local system. Absolute angle is StrarAngle+Rotation
			double EndAngle;  //Relative to the local system. Absolute angle is EndAngle+Rotation
			double GapEnds;

			//double T0_x;
			//double T0_y;
			//double TM_x;
			//double TM_y;
			//double T1_x;
			//double T1_y;

			int ColorR, ColorG, ColorB;

		public:
			CArc();
			CArc(CArc const &A);
			~CArc();

			void clear();
			bool operator==(const CArc& right) const;
	};


	//----------------------------------------------------------------------
	// SET OF ARCS
	class CArcsSet
	{
		private:
			std::vector<CArc> SetOfArcs;
		public:
			CArcsSet();
			CArcsSet(CArcsSet const &AL);
			~CArcsSet();
			void Clear();
			long GetSize();
			void AddArc(CArc const &Arc);
			CArc GetArc(long Position);
			void SetArc(long Position, CArc const &Arc);
			void RemoveArc(long Position);
	};

	//-------------------------------------------------
	// FACES
	class CFace
	// Polygonal face, defined by the list of its edges
	// It may contain information about symmetry axis (look at GetSkewAxes)
	// It may also contain information about triangulation (look at SetFaceTriangulation)
	{
		private:
			std::vector<long> EdgesInFace;  //Edges are saved in ordered sequence
											//The sequence should be CCW after tessellation
			std::vector<long> Axes;
			CVector Normal;
			std::vector<long> Triangulation;
			std::vector<CVector> TriangulationNormals;

		public:
			CFace();
			CFace(CFace const &C);
			~CFace();
			void Clear();

			void AddEdgeInFace(long IndexEdge);
			void SetEdgeInFace(long Position, long IndexEdge);
			void RemoveEdgeInFace(long Position);
			bool SearchEdgeInFace(long IndexEdge, long &Position);
			bool SearchEdgeInFace(long IndexEdge);

			long GetSizeEdgesInFace();
			long GetEdgeInFace(long Position);
			std::vector<long> GetAllEdgesInFace();

			void SetAllEdgesInFace(std::vector<long> &Origin);
			void RemoveAllEdgesInFace();
			void CopyEdgesInFaceTo(CFace &Destination);

			void AddSymAxisInFace(long IndexAxis);
			//void SetSymAxisInFace(long Position, long IndexAxis);
			void RemoveSymAxisInFace(long Position);
			long GetSizeSymAxesInFace();
			long GetSymAxisInFace(long IndexAxis);
			bool SearchSymAxisInFace(long IndexAxis, long &Position);

			void SetNormal(CVector &Normal);
			CVector GetNormal(bool unit);
			void ReverseFace();
			void SetFaceTriangulation(std::vector<long> &Triangles);
			std::vector<long> GetFaceTriangulation();
			void SetFaceTrianglesNormals(std::vector<CVector> &TrianglesNormals);
			std::vector<CVector> GetFaceTrianglesNormals();
	};


	//--------------------------------------------------------------------
	// SET OF FACES
	class CFacesSet{
		private:
			std::vector<CFace> SetOfFaces;

		public:
			CFacesSet();
			CFacesSet(CFacesSet const &LC);
			~CFacesSet();
			void Clear();
			long GetSize();
			void AddFace(CFace const &Face);
			long IsFace(CFace Face);
			CFace GetFace(long Position);
			void SetFace(long Position, CFace const &Face);
			void RemoveFace(long Position);
	};


	//-------------------------------------------------------------------------------
	// MODEL
	class CDB_Model{
		private:
			CVerticesSet CVertices;
			CEdgesSet CEdges;
			CArcsSet CArcs;
			CFacesSet CFaces;

		public:
			CDB_Model();
			CDB_Model(CDB_Model const &DB);
			~CDB_Model();

			void Clear();
			void ClearFaces();
			void ClearVertices();
			void CopyVerticesTo(CDB_Model *Destination);
			void CopyVerticesToVector(std::vector<CVertex> &Vertices);
			void CopyEdgesTo(CDB_Model *Destination);
			void CopyEdgesToVector(std::vector<CEdge> &EdgesVector);
			void CopyFacesTo(CDB_Model *Destination);

			long GetSizeVertices();
			long GetSizeEdges();
			long GetSizeArcs();
			long GetSizeFaces();

			void AddVertex(CVertex const &Vertex);
			long AddEdge(CEdge const &Edge);
			long AddEdgeByVertices(double X1, double Y1, double Z1,
								   double X2, double Y2, double Z2);
			long AddArc(CArc const &Arc);
			void AddFace(CFace const &Face);
			long IsFace(CFace Face);

			CVertex GetVertex(long Position);
			CEdge GetEdge(long Position);

			//Twin vertices and edges//////////////////////////////////
			//Useful functions to find twin vertices and edges 
			//between deconstruction and datum graph
			//double MinVertexDist();
			//long IsNearVertex(CVertex Vertex, double threshold);
			long GetVertexNum(CVertex Vertex);
			long GetEdgeNum(CEdge const &Edge);
			long GetEdgeByVerticesNums(long Vertex1, long Vertex2);
			//End Twin vertices and edges//////////////////////////////

			CArc GetArc(long Position);
			
			CFace GetFace(long Position);
			long GetFaceByVertices(std::vector<long> numVertices);
			long GetFaceByVertices(std::vector<CVertex> Vertices);

			std::vector<long> GetFacesSharingVertex(long vertex);
			std::vector<long> GetFacesSharingEdge(long edge);
			std::vector<std::vector<long>> GetAllFacesSharingEdges();

			long GetSizeEdgesInVertex(long iVertex);
			long GetEdgeInVertex(long iVertex, long ith);
			std::vector<long> GetAllEdgesInVertex(long iVertex);
			std::vector<long> GetAllEdgesNotInVertex(long iVertex);
			bool GetVerticesConnectedToVertex(CVertex Vertex, 
											  std::vector<CVertex> &ConnectedVertices);
			CVertex GetMidPointInEdge(long edge);

			long GetSizeVerticesInFace(long iFace);
			long GetVertexInFace(long iFace, long iVertex);
			std::vector<long> GetAllVerticesInFace(CFace &Cara);
			std::vector<long> GetAllVerticesInFace(long iface);
			std::vector<POINT2D> GetFaceAsPolygon(CFace &Cara);
			std::vector<CVertex> GetAllCoordinateVerticesInFace(long iface);
			std::vector<long> GetAllEdgesInFace(long iface);

			bool IsVertexInFace(CFace &Face, long iVertex);
			bool IsVertexInFace(long iFace, long iVertex);
			bool IsVertex(CVertex Vertex, long &Num);

			bool IsEdgeInFace(long iFace, long iEdge);

			void GetBoundingBox(double &Xmin, double &Ymin, double &Zmin,
								double &Xmax, double &Ymax, double &Zmax);

			double GetLength2D(long IndexEdge);
			double GetLength3D(long IndexEdge);

			double GetDistance2D(long NumVertex1, long NumVertex2);
			double GetDistance2D(CVertex Vertex1, CVertex Vertex2);
			//double GetDistance3D(long NumVertex1, long NumVertex2);

			//double GetDistanceVertexEdge(long Vertex, long Edge);

			double GetAngle2D_PI_half(CVector V1, CVector V2);
			double GetAngle2D_PI_half(long numEdge1, long numEdge2);

			double GetAngle2DX_PI(long IndexEdge, double Threshold);

			double GetAngle2D_PI(CVertex v1, CVertex vP, CVertex v2);
			double GetAngle2D_PI(long numVertex1, long numVertexP, long numVertex2);

			double GetAngle2DX_2PI(long numEdge, long numVertexTail, double Threshold);
			double GetAngle2DX_2PI(CVertex Vertexorigin, CVertex Vertexend);

			/////////////////////////////////////
			double GetAngle2DX_PIPI(CVertex Vertexorigin, CVertex Vertexend); //WARNING: Possible incorrect use for finding Features!!!
			/////////////////////////////////////

			double GetAngle3D_PI(CVertex v1, CVertex vP, CVertex v2);

			double GetAngle3D_PI_half(CVector v1, CVector v2);

			double GetMinAngleInVertex(long iVertex);
			void GetAngularDistribution(std::vector<double> &Angles,
										std::vector<double> &Frequencies,
										bool normalizeFrequencies);

			void SetVertex(long Position, CVertex const &Vertex);
			void SetEdge(long Position, CEdge const &Edge);
			void SetFace(long Position, CFace const &Face);
			void SetFaceAsPolygon(long numFace, std::vector<POINT2D> &FacePolygon);

			void SetAllVertex_XY(std::vector<double> XY);
			void SetAllVertex_Z(std::vector<double> Z);
			void SetAllVertex_Z_ToZero();

			void RemoveVertex(long Position, bool updateEdges);
			void RemoveEdge(long Position, bool clearVertices);
			void RemoveFace(long Position);

			void MergeVertices (long MainVertex, long VertexToMerge);

			//Used by HiddenEdges-------------
			CVerticesSet GetSetOfVertices();
			CEdgesSet GetSetOfEdges();
			void UpdateSetOfVertices(CVerticesSet inVertices);
			void UpdateSetOfEdges(CEdgesSet inEdges);
			//End used by HiddenEdges---------
	};
#endif  //DB_MODEL
