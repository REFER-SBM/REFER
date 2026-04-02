
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

#if !defined DATARIBSANDDIVIDERS_DEFINED
	#define DATARIBSANDDIVIDERS_DEFINED

	class CDataRibsAndDividers{
		public:
			double ShortEdges;
			double LongEdges;
			double SimilarFacesDistances;
			double SimilarFacesAngles;
			double CollinearEdges;

			long NumRibs;
			std::vector<long> Ribs;
			std::vector<double> MeritRibs;
			std::vector<std::vector<long>> ContainerRibFaces;


			long NumDividers;
			std::vector<long> Dividers;
			std::vector<double> MeritDividers;
			std::vector<std::vector<long>> ContainerDividerFaces;


			CDataRibsAndDividers();
			~CDataRibsAndDividers();
	};

#endif // DATARIBSANDDIVIDERS_DEFINED