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

#if !defined PERIMETER_DEFINED
	#define PERIMETER_DEFINED


	class CCuePerimeter
	{
		private:
			bool IncludeDanglingEdges;
			double ThresholdIntersection;

			//Input data
			int VertexCount;
			std::vector <POINT2D> Vertex;

			int EdgeCount;
			std::vector <long> EdgeHead;
			std::vector <long> EdgeTail;

			std::vector <std::vector <long>> EdgesInVertex;

			bool InputSubGraph(CDB_Model *DDBB, long NumGraph,
							   std::vector <long> &ReverseMapVertices,
							   std::vector <long> &ReverseMapEdges);
			void GetImplicitData();

			//Internal functions
			long GetFirstVertex();
			bool isDangling(long numEdge, long closeEnd, long &numVisited);
			long GetDanglingEdges(long numVertex);
			bool isTjunction(long numVertex, long numEdge, long otherEdge);
			double GetInnerAngle(POINT2D P1, POINT2D P2, POINT2D P3);
			long GetNextEdge(POINT2D Previous, long Current, long CurrentEdge, bool &dangling);
			std::vector <bool> GetEdgesToCheck(long CurrentEdge);
			bool GetCloserIntersection(POINT2D a1, POINT2D b1, std::vector <bool> CheckEdge,
									   long &InterceptingEdge, POINT2D &Intercept, long &T_vertex);
			void PerimeterByEnvelope(std::vector <long> &ReverseMapVertices,
									 std::vector <long> &ReverseMapEdges,
									 std::vector <long> &PerimeterVertices,
									 std::vector <long> &PerimeterEdges,
									 std::vector <POINT2D> &Tvertex,
									 std::vector <std::vector <long>> &Tedges);

		public:
			void GetPerimeter(CDB_Model *DDBB);
	};
#endif // PERIMETER_DEFINED