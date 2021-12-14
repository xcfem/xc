// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//GTSBBoxTree.h
//C++ wrapper para la clase GtsBBoxClass de la biblioteca GTS.

#ifndef GTS_BBOXTREE_H
#define GTS_BBOXTREE_H

#include "gts.h"

class GTSBBoxTree
  {
    GtsBBox *bbt;
  protected:
    GtsBBox const *get_const_ptr(void) const
      { return bbt; }
    GtsBBox *get_ptr(void)
      { return bbt; }

  public:

    GTSBBoxTree(GtsBBox *b= nullptr);
    GTSBBoxTree(const GTSBBoxTree &);
  };


#endif
