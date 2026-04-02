
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

#if !defined DATARAILSANDSLOTS_DEFINED
	#define DATARAILSANDSLOTS_DEFINED

	class CDataRailsAndSlots{
		public:
			double ShortEdges;
			double LongEdges;
			double SimilarFacesDistances;
			double SimilarFacesAngles;
			double CollinearEdges;
			double ThresholdAreaBases;

			long NumRails;
			std::vector<long> Rails;
			std::vector<double> MeritRails;
			std::vector<long> ContainerRailFaces;
			std::vector<long> BlindRails;

			long NumSlots;
			std::vector<long> Slots;
			std::vector<double> MeritSlots;
			std::vector<long> ContainerSlotFaces;
			std::vector<long> BlindSlots;

			CDataRailsAndSlots();
			~CDataRailsAndSlots();
	};

#endif // DATARAILSANDSLOTS_DEFINED