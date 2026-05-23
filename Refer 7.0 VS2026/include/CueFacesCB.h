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

#ifndef FACES_CB_VISITED
	#define FACES_CB_VISITED

	//Courter and Brewer's approach for finding faces
	class CFacesCB{
		private:
			//Utilities
			void GetCycleMatrix(CDB_Model* pDDBB, std::vector<std::vector<long>> &CycleMatrix);
			void GetInteractionMatrix(std::vector<std::vector<long>> &CycleMatrix,
									  std::vector<std::vector<long>> &InteractionMatrix);
			bool OrderCircuit(CDB_Model* pDDBB, std::vector<long> &Circuit);

			void GetSymmetricDifference(std::vector<long> &Circuit1,
										std::vector<long> &Circuit2,
										std::vector<long> &SymmetricDifference);
			void GetFalseFaces(CDB_Model* DDBB,
							   std::vector<long> &ListBadFaces,
							   bool GreaterFirst);
			void VisitedCircuits(CDB_Model *DDBB, 
								 std::vector<bool> &VisitedVertices,
								 std::vector<bool> &VisitedEdges);

			//Stages
			void GetFundamentalCycles(CDB_Model *DDBB, 
									  std::vector<bool> VisitedVertices,
									  std::vector<bool> VisitedEdges);
			void ReduceCyclesToFaces(CDB_Model* DDBB);
			void PostProcessFaces(CDB_Model* DDBB);
			void AddLastFace(CDB_Model* DDBB);
		public:
			bool Get_Faces_CB(CDB_Model *DDBB);
	};

#endif //FACES_CB_VISITED
