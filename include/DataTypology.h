
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

#if !defined DATATYPOLOGY_DEFINED
	#define DATATYPOLOGY_DEFINED

	enum TYPOLOGY{
		Undefined,
		Normalon,
		Pyramidal,
		Prismatic,
		QuasiNormalon,
		Undefined_D,     //Undefined,but with a dominant direction (many edges parallel to each other)
		Undefined_ND
	};

	class CDataTypology{
		public:
			TYPOLOGY Typology;

			double dFrequencyPrismaticObjects;
			double dFrequencyUndefinedObjects_D;

			double ThresholdForParallel;
			double ThresholdForArea;

		CDataTypology();
		~CDataTypology();
	};

#endif // DATATYPOLOGY_DEFINED