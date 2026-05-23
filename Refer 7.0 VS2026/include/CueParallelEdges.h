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

#ifndef PARALLELEDGES_VISITED
	#define PARALLELEDGES_VISITED

	class CCueParallelEdges{
		private:
			//Input data
			int VertexCount;
			std::vector <POINT2D> Vertex;

			int EdgeCount;
			std::vector <int> EdgeHead;
			std::vector <int> EdgeTail;

			bool InputModel(CDB_Model *DDBB);

			//Parameters
			//long ReferApp.Cu.PE.ParallelEdgesApproach;
			//double ReferApp.Cu.PE.TolForParallel;

			//Output data
			std::vector <std::vector <long>> ParallelEdges;

			//Internal operations
			double GetAngle_PI(long NumVertex1, long NumVertex2);

			void GetAngularDistribution(std::vector<double> &Angles);
			bool GetADGParallelEdges(std::vector<double> Angles);
			void GetAutomaticParallelEdges();

		public:
			//CCueParallelEdges(int IVertexCount,
			//				  std::vector <POINT2D> IVertex,
			//				  int IEdgeCount,
			//				  std::vector <int> IEdgeHead,
			//				  std::vector <int> IEdgeTail);
			std::vector <std::vector <long>> GetParallelEdges(CDB_Model *DDBB);
	};




#endif // PARALLELEDGES_VISITED
