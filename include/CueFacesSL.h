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

#include "CueFacesSL_Optimization_Rb.h"


#ifndef FACES_SL_VISITED
	#define FACES_SL_VISITED
	//Shpitalni and Lipson's approach (modified)
	//------------------------------------------
	class ffaces {
		public:
			unsigned int recorrido;
			unsigned int bondad;
			unsigned int total;
	};

	class coord {
		public:
			double x;
			double y;
			double z;
	};

	class presen {
		public:
			char elemento;
			char activo;
	};

	class CFacesSL{
		private:
			bool IsInternalToFace(std::vector<long> vertices,
								  double PointX, double PointY, CDB_Model *DDBB);

			void GetTriangularFaces(CFacesSet &CarasTriangulares, CDB_Model *DDBB);


			void GenFace(CDB_Model* DDBB,
						 std::stack<long> pila_vertice,
						 std::stack<long> pila_arista,
						 std::stack<int>  pila_marca);
			void GenCircuits(long pI, CDB_Model* DDBB, 
							 std::vector<bool> &VisitedVertices,
							 std::vector<bool> &VisitedEdges);
			void FundamentalCircuits(CDB_Model* DDBB, std::vector<long> VisitedEdges);


			void AddCircuit(CFace *Cara, CFace *Cirtuit);
			bool SingleCircuit(CFace circuit, CDB_Model *DDBB);

			bool Intercept(long i, CFace *pCara, CDB_Model *DDBB);
			bool SelfIntersect(CFace *pCara, CDB_Model *DDBB);

			bool InternalEdge(CFace *pCara, CDB_Model *DDBB);
			bool CombinedCircuits(CDB_Model *DDBB, CDialogProgress *pProgress);


			void SortFaces(CDB_Model* pDDBB);
			long sharedEdges(CFace *circ1, CFace *circ2, CDB_Model *DDBB);
			void RemoveOverlappingCircuits(CDB_Model *DDBB);


			void GetIncompatibilities(CDB_Model *DDBB, std::vector<bool> incompatibilities);
			void Rangos_Maximos(std::vector<long> RAsup,
								std::vector<long> RVsup,
								CDB_Model *DDBB);

			//----------CueFacesSL_Optimization---------------
			void pushPere(Rb_node k,Rb_node *cabeza);

			ffaces calcular_f(std::vector<presen> &presencia,
							  long iCara,
							  std::vector<long> RAsup,
							  std::vector<long> RVsup,
							  CDB_Model *DDBB);
			unsigned int insertar_hijos(Rb_node padre,Rb_node Abiertos,long TamCar,
										std::vector<long> RAsup,
										std::vector<long> RVsup,
										std::vector<bool> matrizPere,
										int inicio,
										std::vector<bool> seg,
										CDB_Model *DDBB);

			inline coord prod_vec(coord &l1,coord &l2);
			inline double prod_esc(coord &l1,coord &l2);
			double calcular_w(long cara, CDB_Model *DDBB);
			Rb_node seleccion(Rb_node Cerrados,unsigned long solucion, CDB_Model *DDBB);

			void eliminacion(Rb_node elegido,long TamCar, CDB_Model *DDBB);
			//-------------------------------------------

			void OptimizeFaces(CDB_Model *DDBB, 
							   bool tipo_objeto, bool metodo, bool valor_f);

		public:
			bool Get_Faces_SL(CDB_Model *DDBB,
							  bool tipo_objeto, bool metodo,
							  bool valor_f);
	};
#endif //FACES_SL_VISITED
