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
//TritrizPtrPnt.h
//Vector of point matrices

#ifndef TRITRIZPTRPNT_H
#define TRITRIZPTRPNT_H

#include "xc_utils/src/nucleo/EntCmd.h"
#include "MatrizPtrPnt.h"
#include "TritrizPtrBase.h"
#include "utility/matrix/Vector.h"

#include "xc_basic/src/matrices/TritrizIndices.h"

class ExprAlgebra;
class Intervalo1D;
class Lista;
class RangoIndice;
class RangoTritriz;

namespace XC{

//! \ingroup MultiBlockTopologyMR
//! 
//! @brief "Tritriz" of elment pointers.
class TritrizPtrPnt: public TritrizPtrBase<MatrizPtrPnt>
  {
  protected:


    friend class Framework3d;
    TritrizPtrPnt(const size_t n_layers= 0);
    TritrizPtrPnt(const size_t ,const size_t ,const size_t );
  public:
    inline virtual ~TritrizPtrPnt(void) {}
    Pnt *findPoint(const int &tag);
    const Pnt *findPoint(const int &tag) const;
    Pnt *getNearestPnt(const Pos3d &p);
    const Pnt *getNearestPnt(const Pos3d &p) const;

    const MultiBlockTopology *getMultiBlockTopology(void) const;
    MultiBlockTopology *getMultiBlockTopology(void);

    std::deque<size_t> copyPoints(const RangoTritriz &,const std::vector<size_t> &,const Vector3d &);
    TritrizPtrPnt getPointsOnRange(const RangoTritriz &);
    Pnt *getPoint(const VIndices &i);
    TritrizPtrPnt getPoints(const TritrizIndices &);
    MatrizPtrPnt getPoints(const MatrizIndices &);
    TritrizPtrPnt getCellPoints(const size_t &,const size_t &,const size_t &,const TritrizIndices &);
    MatrizPtrPnt getCellPoints(const size_t &,const size_t &,const MatrizIndices &);

    Pos3d getCentroide(void) const;

    void setPnt(const size_t &,const size_t &,const size_t &,const int &);
    Pnt *getPnt(const size_t &,const size_t &,const size_t &);


    void Print(std::ostream &os) const;
  };


std::ostream &operator<<(std::ostream &os,const TritrizPtrPnt &);

std::vector<size_t> getIdPointsQuad(const TritrizPtrPnt::constant_i_layer_const_ref &,const size_t &j,const size_t &k);

} //end of XC namespace.

#endif
