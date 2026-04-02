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

#if !defined REFINE3D_DEFINED
#define REFINE3D_DEFINED 

//private:
	//void RefineFacePlanarity_Ana(CDB_Model *DDBB);
	void CheckTriplets(CDB_Model *DDBB,
					   std::vector<long> VerticesInFace,
					   std::vector<long> EvaluatedVerticesInFace,
					   std::vector<long>EvaluatedVertices,
					   std::vector<long> &VerticesBestFace,
					   std::vector<double> &CoefBestFace);
	void RefineFaces(CDB_Model *DDBB, std::vector<long> FacesToParse);
	void RefineFacePlanarity(CDB_Model *DDBB);

	void RefineFaceAlignment(CDB_Model *DDBB);

//public:
	void RefineModelBrep(CReferDoc* pDoc);

#endif // REFINE3D_DEFINED

