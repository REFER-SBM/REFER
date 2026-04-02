
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

#include "DataAxonometric.h"

#if !defined DATADIRECTINFLATION_DEFINED
	#define DATADIRECTINFLATION_DEFINED

	enum DIRINFLATION{
		NO_DIRINFLATE= -1,
		AUTOMATIC    =  0,
		AXONOMETRIC  =  1,
		LEVELS       =  2,
		RANDOM       =  3
	};

	class CDataDirectInflation{
		public:
		DIRINFLATION	iPreInfationIfNormalon,
						iPreInfationIfQuasi,
						iPreInfationIfPyramid,
						iPreInfationIfPrism,
						iPreInfationIfUndefinedD,
						iPreInfationIfUndefinedND;

		DIRINFLATION DirectInflationApproach;

		CDataDirectInflation();
		~CDataDirectInflation();
	};

#endif // DATADIRECTINFLATION_DEFINED