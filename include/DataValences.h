
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

#if !defined DATAVALENCES_DEFINED
	#define DATAVALENCES_DEFINED

	class CDataValences{
		public:

		bool CollinearAsSingle;
		std::vector<long> Valences;

		CDataValences();
		~CDataValences();
	};

#endif // DATAVALENCES_DEFINED