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

#if !defined TYPOLOGY_DEFINED
	#define TYPOLOGY_DEFINED

	class CCueTypology
	{
		private:
			bool IsPyramid(CDB_Model *DDBB);

			void GetNormalonEdges(CDB_Model *DDBB);
			double IsNormalon(CDB_Model *DDBB);

			double IsQuasiNormalon(CDB_Model *DDBB,
								   std::vector<double> &VariablesZ_Aux);

			bool IsPrismTopology(CDB_Model *DDBB, long &Base0, long &Base1);
			double IsPrismGeometry(CDB_Model *DDBB, long Base0, long Base1);
			double IsPrismGeometry(CDB_Model *DDBB);

		public:
			double IsPrism(CDB_Model *DDBB, long &Base0, long &Base1);

			TYPOLOGY GetTypology(CDB_Model *DDBB);
	};
#endif // TYPOLOGY_DEFINED