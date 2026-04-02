/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CICopy.cxx
+-----------------------------------------------------------------------------+
| Description: Implements the class CICopy.
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@ist.utl.pt
|
| Date: May 98, May 99
+-----------------------------------------------------------------------------+
| 
| Copyright (C) 1998, 1999, 2000 Manuel João da Fonseca
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or any later version.
| 
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
| 
| You should have received a copy of the GNU General Public License
| along with this program; if not, write to the Free Software
| Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
| 
+----------------------------------------------------------------------------*/

#include "CICopy.h"
CICopy::CICopy ()
{
    _features = new CIFeatures (&CIEvaluate::Pch2_Ach, 13, 14, 18, 23
                               ,&CIEvaluate::Tl_Pch, 0.45, 0.7, 0.85, 0.9
							   ,&CIEvaluate::Alt_Ach, 0.45, 0.48, 0.7, 0.75   // Not Ellipses
							   ,&CIEvaluate::Alt_Alq, 0.62, 0.65, 0.81, 0.83 // Not Rectangles & Diamonds
                               ,&CIEvaluate::Plq_Pch, 0.9, 0.94, 0.98, 0.989 // Not Rectangles & Diamonds
                               ,&CIEvaluate::Ns, 1, 1, 1, 1
                               );
}
