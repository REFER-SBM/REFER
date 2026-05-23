
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

#if !defined DATACSG_DEFINED
	#define DATACSG_DEFINED

	class CDataCSG{

		public:
		bool CSG_Inflation;
		bool CSG_Inflation_Direct;
		bool CSG_Inflation_Optimization;

		double CSG_ThresholdFMerit;
		double CSG_ThresholdDMerit;
		double CSG_ThresholdAngle;
		double CSG_ThresholdOffset;
		double CSG_ThresholdLocation;
		bool CSG_UseMultipleOrigins;

		CDataCSG();
		~CDataCSG();

		void Clear();

};


#endif // DATACSG_DEFINED