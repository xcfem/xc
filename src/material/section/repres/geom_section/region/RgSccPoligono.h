//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//RgSccPoligono.h

#ifndef RgSccPoligono_h 
#define RgSccPoligono_h 

#include <material/section/repres/geom_section/region/RgQuadCell.h>
#include "utility/matrix/Matrix.h"
#include "xc_utils/src/geom/d2/poligonos2d/Poligono2d.h"

class Poligono2d;

namespace XC {

//! @ingroup MATSCCRegiones
//
//! Poligonal patch for cross-section discretization.
class RgSccPoligono: public RgQuadCell
  {
  private:
    Poligono2d plg;//!< Polygon that defines the patch.
  protected:

  public:
    RgSccPoligono(Material *);
    RgSccPoligono(Material *, int numSubdivIJ, int numSubdivJK, const Poligono2d &);
    
    const Rejilla2d &getMesh(void) const;
    const VectorCells &getCells(void) const;
    RegionSecc *getCopy(void) const;

    double getMaxY(void) const;
    double getMaxZ(void) const;
    double getMinY(void) const;
    double getMinZ(void) const;
    Poligono2d getPoligono(void) const;


    void Print(std::ostream &s, int flag =0) const;   
    friend std::ostream &operator<<(std::ostream &s, RgSccPoligono &rg_scc_plg);    
  };
} // end of XC namespace


#endif

 
