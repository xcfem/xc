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
//RgQuadCell.h

#ifndef RgQuadCell_h 
#define RgQuadCell_h 


#include <iostream>
#include "material/section/repres/geom_section/region/RegionSecc.h" 

class Rejilla2d;

namespace XC {

//! @ingroup MATSCCRegions
//
//! @brief Region of a section that creates quadrilateral cells.
//
//         nDivIJ= 4
// L +---+---+---+---+ K
//   |   |   |   |   |
//   +---+---+---+---+ nDivKL= 2
//   |   |   |   |   |
// I +---+---+---+---+ J
class RgQuadCell: public RegionSecc
  {
    mutable Rejilla2d *rejilla; //!< Grid points.
  protected:
    int nDivIJ, nDivJK; //!< number of divisions.
    void free_mem(void) const;
    const Rejilla2d &alloc(const Rejilla2d &) const;

  public:
    RgQuadCell(Material *);
    RgQuadCell(Material *, int numSubdivIJ, int numSubdivJK);
    RgQuadCell(const RgQuadCell &);
    RgQuadCell &operator=(const RgQuadCell &);
    virtual ~RgQuadCell(void);

    void setDiscretization(int numSubdivIJ, int numSubdivJK);

    inline void setNDivIJ(const int &n)
      { nDivIJ= n; }
    inline int getNDivIJ(void) const
      { return nDivIJ; }
    inline void setNDivJK(const int &n)
      { nDivJK= n; }
    inline int getNDivJK(void) const
      { return nDivJK; }
    //inquiring functions
    inline const int &nDivCirc(void) const
      { return nDivIJ; }
    inline int &nDivCirc(void)
      { return nDivIJ; }
    inline const int &nDivRad(void) const
      { return nDivJK; }
    inline int &nDivRad(void)
      { return nDivJK; }
    int getNumCells(void) const;
    void getDiscretization(int &numSubdivIJ, int &numSubdivJK) const;
    virtual const Rejilla2d &getMesh(void) const=0;
    Vector getCenterOfMassRejilla(void) const;
    Vector getVertCoords(const size_t &,const size_t &) const;
    Matrix getCellVertCoords(const size_t &,const size_t &) const;
    

  };
} // end of XC namespace


#endif

