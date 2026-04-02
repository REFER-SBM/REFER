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

#ifndef InOutCSG_VISITED
	#define InOutCSG_VISITED

	class InOutCSG {

	public:
		InOutCSG ();
		~InOutCSG ();

		//private
		void writeDatumOrigins(std::ofstream &out,
							   CDB_CSG *pDB_CSG_Model,
							   CDB_Model *DB_DatumGraph);
		void writeDatumPlanes(std::ofstream &out,
							  CDB_CSG *pDB_CSG_Model);

		void writeCrossSection(std::ofstream &out, 
							   CFeature Feature,
							   CDB_CSG *pDB_CSG_Model,
							   CDB_Model *DB_DatumGraph);

		void writeFeatureCSG(std::ofstream &out,
							 long Branch,
							 CFeature Feature,
							 CDB_CSG *pDB_CSG_Model,
							 CDB_Model *DB_DatumGraph);

		//public
		bool writeCSG( const char *file, CDB_CSG *pDB_CSG_Model, CDB_Model *DB_DatumGraph);
	}; // class InOutCSG

#endif
