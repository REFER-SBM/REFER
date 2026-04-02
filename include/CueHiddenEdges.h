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

#if !defined HIDDENEDGES_DEFINED
	#define HIDDENEDGES_DEFINED


	class CCueHiddenEdges{
		private:
			class FACE
			{
				public:
					int C; //Number of edges/vertices that define the face
					std::vector <long> V;  //List of vertices that define the face
					std::vector <long> E;  //List of edges that define the face
			};

			//Input data
			//----------
			int VertexCount;
			std::vector <POINT2D> Vertex;

			int EdgeCount;
			std::vector <long> EdgeHead;
			std::vector <long> EdgeTail;

			int FaceCount;
			std::vector <FACE> Face;

			std::vector<long> PerimeterEdges;
			std::vector<long> PerimeterVertices;

			bool InputModel(CDB_Model *DDBB);

			//Working data
			std::vector<LINE_VISIBILITY> Visible;

			//Geometric calculations
			std::vector<long> GetAllEdgesInVertex(long iVertex);
			bool IsInternalToFace(long numFace, double PointX, double PointY);

			void GetInterceptions(long CurrentEdge,
								  std::vector<long> &InterceptedEdges);
			void GetInterceptions(long CurrentEdge,
								  std::vector<long> &InterceptedEdges,
								  std::vector<double> &XIntersections,
								  std::vector<double> &YIntersections);
			void ChunkPartiallyHiddenEdges(CDB_Model *DDBB);

			//Faces variation approach
			bool HiddenEdgeInPerimeter();
			bool VisibleEdgesIntersect();
			bool FaceIsVisible(long numFace);
			bool VisibleVertexInsideVisibleFace();
			bool HiddenEdgeConnectedToVisibleEdges();
			long GetNumHiddenEdges();
			bool GetHiddenByFaceVariations(CDB_Model *DDBB);

			//Edges variation aproach
			void FixPerimeterVisibility();
			std::vector<long> GetFrontFaces();
			void FixFrontFaceVisibility(std::vector<long> numFrontFaces);
			void FixInternalHiddenVertices(std::vector<long> numFrontFaces);

			void FixEdgesBetweenVisibleFaces();
			bool HiddenVertexOutsideVisibleFaces();
			bool HiddenEdgeConnectedToHiddenEdges();
			bool GetHiddenByEdgeVariations(CDB_Model *DDBB);

		public:
			bool GetHiddenEdges(CDB_Model *DDBB);
	};
#endif  // HIDDENEDGES_DEFINED
