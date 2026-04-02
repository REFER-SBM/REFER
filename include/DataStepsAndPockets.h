
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

#if !defined DATASTEPSANDPOCKETS_DEFINED
	#define DATASTEPSANDPOCKETS_DEFINED

	class CDataStepsAndPockets{
		public:
			double ShortEdges;
			double LongEdges;
			double SimilarFacesDistances;
			double SimilarFacesAngles;
			double CollinearEdges;
			double ThresholdAreaBases;

			long NumSteps;
			std::vector<long> Steps;
			std::vector<double> MeritSteps;
			std::vector<long> ContainerStepFaces;
			std::vector<long> StepsBaseFace;

			long NumPockets;
			std::vector<long> Pockets;
			std::vector<double> MeritPockets;
			std::vector<long> ContainerPocketFaces;
			std::vector<long> PocketsBaseFace;
			

			CDataStepsAndPockets();
			~CDataStepsAndPockets();
	};

#endif // DATASTEPSANDPOCKETS_DEFINED