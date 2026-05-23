
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

#if !defined DATASUBGRAPHS_DEFINED
	#define DATASUBGRAPHS_DEFINED

	class CDataSubGraphs{
		public:
		int SubGraphsApproach;

		long NumSubgraphs;                     //Number of subgraphs
		std::vector<long> SubgraphForVertex;   //Number of the subgraph where each vertex belongs
		std::vector<long> SubgraphForEdge;     //Number of the subgraph where each edge belongs

		long Outer;

		CDataSubGraphs();
		~CDataSubGraphs();
	};

#endif // DATASUBGRAPHS_DEFINED