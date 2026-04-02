
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

#if !defined INFLATIONLEVELS_DEFINED
#define INFLATIONLEVELS_DEFINED

	class CInflationLevels{
		private:
			long ConnectGraphs(CDB_Model *DDBB);
			long GetCentralVertex(CDB_Model *DDBB);

			void GetSons(CDB_Model *DDBB, long father, std::vector<long> &Sons);
			void SortSons(CDB_Model *DDBB, long father,
						  std::vector<long> &Sons,
						  std::vector<double> &DistanceToFather);

			void DFS_VisitSons(long father, std::vector<bool> &visited,
							   CDB_Model *DDBB, double Delta,
							   std::vector<double> &VariablesZ);

			void BFS_VisitVertices(long first,
								   CDB_Model *DDBB, double Delta,
								   std::vector<double> &VariablesZ);

			void GetIncrementalLevels(CDB_Model *DDBB,
									  std::vector<double> &VariablesZ);

			void GetFixedLevels(CDB_Model *DDBB,
								std::vector<double> &VariablesZ);


		public:
			void InflateByLevels(CDB_Model *DDBB);
	};
#endif // INFLATIONLEVELS_DEFINED