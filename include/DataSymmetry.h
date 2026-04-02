
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

#if !defined DATASYMMETRY_DEFINED
	#define DATASYMMETRY_DEFINED

	class CDataSymmetry{
		public:

		bool DetectNormalCrossing;
		double MaxParallelCrossing;
		double MinParallelCrossing;
		double MeritWeight_1;

		bool DetectDistorted;
		double MinSymDistortion;
		double MaxSymDistortion;
		double MaxParallelFaceSymLines;
		double MinParallelFaceSymLines;
		double MeritWeight_2;

		bool SymmetricalEdges;
		double MaxParallelEdgeSymLines;
		double MinParallelEdgeSymLines;
		double MeritWeight_3;

		bool PrioritizeVertical;
		double minVerticalThreshold;
		double maxVerticalThreshold;
		double MeritWeight_4;

		bool RemoveBadMerit;
		double MinMerit;

		std::vector<std::vector<long>> FaceAxesU;
		std::vector<std::vector<long>> FaceAxesV;

		std::vector<std::vector<std::vector<long>>> SymPlanes;
		std::vector<std::vector<double>> SymMerits;

		CDataSymmetry();
		~CDataSymmetry();
	};

#endif // DATASYMMETRY_DEFINED