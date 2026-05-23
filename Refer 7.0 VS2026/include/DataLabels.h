
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

#if !defined DATALABELS_DEFINED
	#define DATALABELS_DEFINED

	enum VERTEX_LABEL{
		JunctionIsL, //     'L'
		JunctionIsW, //     'W'
		JunctionIsY, //     'Y'
		JunctionIsT, //     'T'
		JunctionIsT4, //    '4'
		JunctionIsZ, //     'Z'
		JunctionIsX, //     'X'
		JunctionIsK, //     'K'
		JunctionIsKplus, // '#'
		JunctionIsM, //     'M'
		JunctionIs5way, //  '5'
		JunctionIs6way, //  '6'
		JunctionIsZ6, //    '*'
		JunctionIs7way, //  '7'
		JunctionIs8way, //  '8'
		JunctionIsHidden, // 'H'
		JunctionIsUndefined // '?'
	};

	enum EDGE_LABEL{
		BLANK,
		BOUNDARY,
		RBOUNDARY,
		CONVEX,
		CONCAVE
	};

	class CDataLabels{
		public:

		int LabelsApproach;
		int VertexCatalogue;

		std::vector<VERTEX_LABEL> LabelsInVertices;
		std::vector<EDGE_LABEL> LabelsInEdges;

		CDataLabels();
		~CDataLabels();
	};

#endif // DATALABELS_DEFINED