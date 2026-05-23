
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

#if !defined DATAMAINAXES_DEFINED
	#define DATAMAINAXES_DEFINED

	class CDataMainAxes{
		public:
		//Tunning parametes
		int MainAxesApproach;

		bool ProjectiveRule;
		double ToleranceCubicAngles;
		bool MatchingCornersRule;
		bool IsometricRule;
		bool VerticalRule;
		bool HorizontalRule;
		bool FrequencyRule;
		double MeritMatchingCorners;
		double MeritIsometric;
		double MeritVertical;
		double MeritHorizontal;
		double MeritFrequency;
		double ThresholdOverlap;
		double ThresholdAngle;

		std::vector<long> UserMainAxes;

		//Calculated axonometric parameters
		std::vector<double> MainAngles;
		std::vector<double> CoefE;
		std::vector<double> AngleXYZ;    //XOY= AngleXYZ[2], YOZ= AngleXYZ[0], ZOX= AngleXYZ[1]
		long obliqueAngle;

		std::vector<std::vector<double>> M;

		CDataMainAxes();
		~CDataMainAxes();
	};

#endif // DATAMAINAXES_DEFINED