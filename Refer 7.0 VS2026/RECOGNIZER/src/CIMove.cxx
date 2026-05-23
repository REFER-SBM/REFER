/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIMove.cxx
+-----------------------------------------------------------------------------+
| Description: Implements the class CIMove.
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

#include "CIMove.h"
CIMove::CIMove ()
{
    _features = new CIFeatures (&CIEvaluate::Alt_Ach, 0.75, 0.85, 1, 1      // These 2 identify
                               ,&CIEvaluate::Plt_Pch, 0.95, 0.98, 1, 1
                               ,&CIEvaluate::Tl_Pch, 0.2, 0.3, 0.83, 0.93 // and these 2 check if is open
                               ,&CIEvaluate::Pch_Ns_Tl, 1.2, 1.3, 2, 2
                               ,&CIEvaluate::Pch2_Ach, 18, 19, 25, 35     // Not Lines
                               ,&CIEvaluate::Ns, 1, 1, 1, 1
                                );
}
