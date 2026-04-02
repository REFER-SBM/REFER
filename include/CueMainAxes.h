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

#if !defined CUEMAINAXES_DEFINED
#define CUEMAINAXES_DEFINED 

	class CCueMainAxes
	{
		private:
			void GetTriplets(std::vector<double> Angles,
							 std::vector<double> Frequencies,
							 std::vector<std::vector<double>> &Triplets,
							 std::vector<double> &Merits,
							 std::vector<double> &TotalFrequency);
			void AutomaticDetect(CDB_Model *DDBB,
								 std::vector<double> Angles,
								 std::vector<double> Frequencies);
			void GetBestAngles(CDB_Model *DDBB);
			void ReorderMainAngles();

			bool GetTransformationMatrix(std::vector<std::vector<double>> &M);

			bool GetAxonometricParams(CDB_Model *DDBB);

		public:
			void GetMainAxes(CDB_Model *DDBB);

			void TransformToMainAxes(CDB_Model *DDBB, bool Translate);
			void TransformFromMainAxes(CDB_Model *DDBB, bool Translate);
	};
#endif // CUEMAINAXES_DEFINED
