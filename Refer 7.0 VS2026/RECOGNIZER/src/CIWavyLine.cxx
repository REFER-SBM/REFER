/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIWavyLine.cxx
+-----------------------------------------------------------------------------+
| Description: Implements the class CIWavyLine.
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

#include "CIWavyLine.h"
CIWavyLine::CIWavyLine ()
{
    _features = new CIFeatures (&CIEvaluate::Her_Wer, 0.06, 0.08, 0.4, 0.45 // separate from lines
                               //,&CIEvaluate::Hm_Wbb, 0.9, 0.98, 1.1, 1.1   // separate from Arrows
                               ,&CIEvaluate::Tl_Pch, 0.5, 0.55, 1.5, 1.9
                               ,&CIEvaluate::Hollowness, 0, 3, BIG, BIG
                               ,&CIEvaluate::Ns, 1, 1, 1, 1
                               );
}
