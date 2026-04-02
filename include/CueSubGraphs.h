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

#if !defined CUESUBGRAPHS_VISITED
#define CUESUBGRAPHS_VISITED

//WARNING: an embeeded version of this code
//is used to calculate symmetry planes in "CueSymmetry"

	class CCueSubGraphs
	{
		private:
			//Input data
			int VertexCount;
			std::vector <double> VertexX;
			std::vector <double> VertexY;
			//std::vector <double> VertexZ;

			int EdgeCount;
			std::vector <int> EdgeHead;
			std::vector <int> EdgeTail;

			//Output data
			int SubgraphCount;  //Number of subgraphs
			std::vector <long> SubgraphForVertex; //Number of the subgraph where each vertex belongs
			std::vector <long> SubgraphForEdge; //Number of the subgraph where each edge belongs

			//Internal functions
			void InputData(CDB_Model *DDBB);

			void DetermineSubgraphs();

			long majority4 (long a, long b, long c, long d);
			long GetOuterSubgraph();

			std::vector<long> GetAllEdgesInVertex(long iVertex);
			void GetVisitedVerticesAndEdges(long first, std::vector<bool> &visited);
			void GetNextGraph(long first, std::vector<bool> &visited);

			void GetSubGraphs();
			void SaveCurrentSubgraph(long numgraph, std::vector<CDB_Model> &Graphs);

			void SplitEdge(CDB_Model *DDBB, long Position);

		public:
			void GetSubGraphs(CDB_Model *DDBB);
			void GetSubGraphs(CDB_Model *DDBB, std::vector<CDB_Model> &Graphs);

			void CalculateSubgraphs(CDB_Model *DDBB,
									long &OutSubgraphCount,  //Number of subgraphs
									std::vector <long> &OutSubgraphForVertex, //Number of the subgraph where each vertex belongs
									std::vector <long> &OutSubgraphForEdge); //Number of the subgraph where each edge belongs

			bool IsMultiGraph(CDB_Model *DDBB);

			void SplitVertex(CDB_Model *DDBB, long Position);
			void SplitVertex(CDB_Model *DDBB, long Position, std::vector <long> &NewVertices);
			void SplitByCircuit(CDB_Model *DDBB, std::vector<long> Circuit);
	};

#endif // CUESUBGRAPHS_VISITED
