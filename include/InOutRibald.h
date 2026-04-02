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

#ifndef INOUTRIBALD_VISITED
	#define INOUTRIBALD_VISITED

	class InOutRibald {
		public:
			InOutRibald();
			~InOutRibald();

			bool readRibald( const char *file, CDB_Model *pDDBB);
			int digits(long number);
			bool writeRibald( const char *file, CDB_Model *pDDBB);
	};

#endif