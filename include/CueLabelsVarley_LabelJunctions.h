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

//Get labels from a graph-like representation of a polyhedrical shape

//Based on:
	//Varley P.A.C.
	//Automatic Creation of Boundary-Representation Models 
	//from Single Line Drawings, PhD Thesis, University of Wales. 2003

#ifndef _CUE_LABELS_VARLEY_LABELJUNCTIONS_DEFINED
	#define _CUE_LABELS_VARLEY_LABELJUNCTIONS_DEFINED

		void FinaliseWLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label);
		void FinaliseLLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label);
		void FinaliseYLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label);
		void FinaliseTLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label);
		void FinaliseXLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label, TYPEMARK Edge4Label);
		void FinaliseMLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label, TYPEMARK Edge4Label);
		void FinaliseKLabel (LABELLING *Labelling, VERTEX_INDEX Nvert, TYPEMARK Vexity, TYPEMARK Edge1Label, TYPEMARK Edge2Label, TYPEMARK Edge3Label, TYPEMARK Edge4Label);

#endif // _CUE_LABELS_VARLEY_LABELJUNCTIONS_DEFINED
