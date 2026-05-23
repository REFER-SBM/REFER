
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

#if !defined DATAVANISHINGPOINTS_DEFINED
	#define DATAVANISHINGPOINTS_DEFINED

	class CDataVanishingPoints{
		public:
		//Parameters to detect vanishing points
		double ClusteringAngle;
		double InRing;
		double OutRing;
		double ToleranceCoincident;
		double ToleranceParallel;
		double ToleranceDispersion;

		//Candidate vanishing points
		std::vector<POINT2D> CandidateVP;

		//Clusters of candidate vanishing points
		POINT2D DrawingCentroid;
		double InnerRingRadius, OuterRingRadius;

		std::vector<std::vector<long>> ClusterOfVPs;

		//Mean vanishing points
		std::vector<POINT2D> MeanVP;
		std::vector <std::vector <long>> ParallelEdges;
		std::vector<bool> ParallelGroup;

		CDataVanishingPoints();
		~CDataVanishingPoints();
	};

#endif // DATAVANISHINGPOINTS_DEFINED