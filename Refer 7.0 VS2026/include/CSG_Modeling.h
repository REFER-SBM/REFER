
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

#if !defined CSG_MODELING_DEFINED
#define CSG_MODELING_DEFINED

	//----------------------------------------------------------------------
	class CCSG_Modeling{
		private:
			long CalculeOrigin(CDB_Model *DDBB, long SubGraph);
			void PreCalculeOrigins(CDB_Model *DDBB, 
								   std::vector<long> &Origin);

			void InitDatumGraph(CDB_Model *DDBB, 
								CDB_Model *DB_DatumGraph);

			void AddMidPointToDatumGraph(CDB_Model *DDBB, 
										 CDB_Model *DB_DatumGraph,
										 long corner,
										 CVertex &midV, long &midVert);

			void SaveSymPlanesAsDatum(CDB_Model *DDBB_Deconstruct,
									  CDB_Model *DB_DatumGraph,
									  CDB_CSG *DB_CSG_Model);

			void InflateFeatures(CReferDoc* pDoc);

		public:
			CCSG_Modeling::CCSG_Modeling();
			CCSG_Modeling::~CCSG_Modeling();

			void GetCSG(CReferDoc* pDoc);
	};
#endif // CSG_MODELING_DEFINED