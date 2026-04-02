
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

#if !defined DATACHAMFERSANDROUNDS_DEFINED
	#define DATACHAMFERSANDROUNDS_DEFINED

	class CDataChamfersAndRounds{    //TBD ChamfersAndRounds
		public:
			double ShortEdges;
			double LongEdges;
			double SimilarFacesDistances;
			double SimilarFacesAngles;
			double CollinearEdges;

			long NumChamfers;
			std::vector<long> Chamfers;
			std::vector<double> MeritChamfers;

			long NumRounds;
			std::vector<long> Rounds;
			std::vector<double> MeritRounds;

			CDataChamfersAndRounds();
			~CDataChamfersAndRounds();
	};

#endif // DATACHAMFERSANDROUNDS_DEFINED