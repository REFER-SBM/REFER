
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

#if !defined INFLATIONAXONOMETRIC_DEFINED
#define INFLATIONAXONOMETRIC_DEFINED

	class CInflationAxonometric{
		private:
			void GetSortedEdges(CDB_Model *DDBB,
								long CurrentVertex,
								std::vector<long> &EdgesInVertex);
			long GetReferenceEdge(CDB_Model *DDBB,
								  std::vector<long> EdgesInVertex,
								  std::vector<long> SpanningTree);
			double GetLateralZ(CDB_Model *DDBB,
							   CVertex VectorA, double ModuleA, double Fi,
							   CVertex HeadVertexC, CVertex TailVertexC,
							   long CurrentVertex,
							   long ReferenceEdge,
							   std::vector<double> VariablesZ);
			bool GetTriplet(CDB_Model *DDBB, long CurrentEdge,
							std::vector<long> EdgesInVertex,
							long CentralVertex,
							//double ModuleA, CVertex VectorA,
							double &ModuleB, CVertex &VectorB,
							double &ModuleC, CVertex &VectorC);
			bool SolveLateralVertex(CDB_Model *DDBB,
									long CentralVertex,
									std::vector<long> EdgesInVertex,
									long CurrentEdge,
									std::vector<long> &SpanningTree,
									std::vector<double> &VariablesZ,
									double &NewZ);
			void AddLateralToTree(CDB_Model *DDBB,
								  std::vector<long> &SpanningTree,
								  std::vector<double> &Lengths,
								  bool &First,
								  long CurrentNode,
								  long LateralVertex,
								  double Length);

			void SolveVertex(CDB_Model *DDBB,
							 std::vector<double> &VariablesZ,
							 std::vector<long> &SpanningTree,
							 long &CurrentNode,
							 std::vector<double> &Lengths);

			void GetAxonometricInflation_2001(CDB_Model *DDBB,
											  long VertexCloserToCentre);


		private:
			void GetSons(CDB_Model *DDBB, long father, std::vector<long> &Sons);
			void SortSons(CDB_Model *DDBB, long father,
						  std::vector<long> &Sons,
						  std::vector<double> &DistanceToFather);

			void GetCoordinates(long father, long son, double DistanceToFather,
								CDB_Model *DDBB,
								std::vector<std::vector<double>> &Coords);

			void DFS_VisitSons(long father, std::vector<bool> &visited,
							   CDB_Model *DDBB,
							   std::vector<std::vector<double>> &Coords);

			void BFS_VisitVertices(long first,
								   CDB_Model *DDBB,
								   std::vector<std::vector<double>> &Coords);

			void GetAxonometricInflation_2011(CDB_Model *DDBB, long CentralVertex);


		public:
			long GetCentralNormalonVertex(CDB_Model *DDBB);

			void GetAxonometricInflation(CDB_Model *DDBB);
	};
#endif // INFLATIONAXONOMETRIC_DEFINED