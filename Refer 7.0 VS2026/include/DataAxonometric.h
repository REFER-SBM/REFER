
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

#if !defined DATAAXONOMETRIC_DEFINED
	#define DATAAXONOMETRIC_DEFINED

	class CDataAxonometric{
		public:
			long AxonometricApproach;
			bool bAmendCabinet;
			double d90degrees;
			double d135degrees;

			long SpanningTreeAxonometric;
			double CabinetThreshold;
			double CabinetReduction;

			std::vector<long> AxonometricTreeVertices;
			std::vector<long> NormalonEdges;
			std::vector<double> meritNormalonEdges;

		CDataAxonometric();
		~CDataAxonometric();
	};

#endif // DATAAXONOMETRIC_DEFINED