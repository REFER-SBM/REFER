
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

#if !defined DATAPERIMETER_DEFINED
	#define DATAPERIMETER_DEFINED

	class CDataPerimeter{
		public:
		bool IncludeDanglingEdges;
		double ThresholdIntersection;

		std::vector<std::vector<long>> PerimeterVertices;
		std::vector<std::vector<long>> PerimeterEdges;
		std::vector<std::vector<POINT2D>> Tjunction;
		std::vector<std::vector<std::vector<long>>> Tedges;

		CDataPerimeter();
		~CDataPerimeter();
	};

#endif // DATAPERIMETER_DEFINED