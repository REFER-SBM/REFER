
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

#if !defined DATALEVELS_DEFINED
	#define DATALEVELS_DEFINED

	class CDataLevels{
		public:
			bool IncrementalLevels;
			int SpanningTreeLevels;
			double Coef_TreeUp;
			double Coef_TreeDown;

			std::vector<double> LevelsV;
			std::vector<double> LevelsH;

			std::vector<long> LevelOfVertex;

			std::vector<long> LevelsTreeVertices;

			double VerticalAngleThreshold;

		CDataLevels();
		~CDataLevels();
	};

#endif // DATALEVELS_DEFINED