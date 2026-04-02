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

#ifndef InOutSketch_VISITED
	#define InOutSketch_VISITED

	class CInOutSketch {

	 public:
		 CInOutSketch ();
		 ~CInOutSketch ();

		bool readSketch(const char *file, CDB_Sketch *DB_Sketch);
		//  Reads ref 'file'
		//	Stores some entities in 'DB_InputSketch'
		//  Returns TRUE if ends normally (even if nothing known is found)

		bool writeSketch(const char *file, CDB_Sketch *DB_Sketch);
		// Writes the entities of the database 'DB_InputSketch' into a res file
		// Returns TRUE if the process is sucessful

	}; // class CInOutSketch

#endif
