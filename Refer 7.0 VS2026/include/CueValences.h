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

#if !defined CUE_VALENCES_DEFINED
	#define CUE_VALENCES_DEFINED

	class CCueValences
	{
		private:
			int GetSingleValence(CDB_Model *DDBB, long v);

		public:
			void GetValences(CDB_Model *DDBB);
	};
#endif // CUE_VALENCES_DEFINED