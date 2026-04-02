
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

#if !defined DATAHIDDENEDGES_DEFINED
	#define DATAHIDDENEDGES_DEFINED


	enum LINE_VISIBILITY{
		VISIBLE,
		HIDDEN,
		SEMI_HIDDEN,
		UNDEFINED
	};

	class CDataHiddenEdges{
		public:

		bool HiddenEdgesByDashedLines;
		bool HiddenEdgesRefineDashedLines;
		int HiddenEdgesApproach;
		bool HiddenEdgesAutomatic;

		std::vector<LINE_VISIBILITY> TypesOfEdges;

		CDataHiddenEdges();
		~CDataHiddenEdges();
	};

#endif // DATAHIDDENEDGES_DEFINED