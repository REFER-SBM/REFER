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

#ifndef InOutSTEP_VISITED
	#define InOutSTEP_VISITED

	class CInOutSTEP {

	public:
		CInOutSTEP ();
		~CInOutSTEP ();


		bool WriteHeader(const char *file);
		bool WriteSTEPFile(CDB_CSG *pDB_CSG_Model);

	}; // class CInOutSTEP

#endif