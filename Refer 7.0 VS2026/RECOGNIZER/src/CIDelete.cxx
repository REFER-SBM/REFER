/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIDelete.cxx
+-----------------------------------------------------------------------------+
| Description: Implements the class CIDelete.
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

#include "CIDelete.h"
CIDelete::CIDelete ()
{
    _features = new CIFeatures (&CIEvaluate::Her_Wer, 0.06, 0.08, 1, 1
                               // ,&CIEvaluate::Ach_Aer, 0.62, 0.65, 1, 1   // Not bold Arrows
                               ,&CIEvaluate::Tl_Pch, 1.5, 1.9, BIG, BIG    // Identifies
                               ,&CIEvaluate::Hollowness, 0, 3, BIG, BIG  // Not Shapes
                               ,&CIEvaluate::Ns, 1, 1, 1, 1
                               );
}
