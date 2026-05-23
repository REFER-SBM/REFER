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

// InOutOBJ.h

#ifndef InOutOBJ_VISITED
	#define InOutOBJ_VISITED

class InOutOBJ{
		public:
			InOutOBJ();
			~InOutOBJ();

		private:
			int InOutOBJ::digits(long number);

		public:
			bool writeOBJ( const char *file, CDB_Model *pDDBB);
	}; // class InOutOBJ

#endif
