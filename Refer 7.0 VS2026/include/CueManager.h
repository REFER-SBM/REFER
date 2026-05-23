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

#if !defined CUEMANAGER_VISITED
#define CUEMANAGER_VISITED

class CCueManager
{
	private:
		bool DetectValences,
			DetectParallelEdges,
			DetectVanishingPoints,
			DetectGraphs,
			DetectPerimeter,
			DetectLabels,
			DetectMainAxes,
			DetectFaces,
			DetectTypology,
			DetectHiddenEdges,
			DetectSymmetry,
			DetectLevels;

	public:
		CCueManager();
		CCueManager(bool Set_DetectValences,
					bool Set_DetectParallelEdges,
					bool Set_DetectVanishingPoints,
					bool Set_DetectGraphs,
					bool Set_DetectPerimeter,
					bool Set_DetectLabels,
					bool Set_DetectMainAxes,
					bool Set_DetectFaces,
					bool Set_DetectTypology,
					bool Set_DetectHiddenEdges,
					bool Set_DetectSymmetry,
					bool Set_DetectLevels);
		~CCueManager();

		void FindCues(CDB_Model *DDBB);
		void EraseCues();
};
#endif // CUEMANAGER_VISITED
