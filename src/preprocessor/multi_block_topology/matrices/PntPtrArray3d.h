// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//PntPtrArray3d.h
//Vector of point matrices

#ifndef PNTPTRARRAY3d_H
#define PNTPTRARRAY3d_H

#include "utility/kernel/CommandEntity.h"
#include "PntPtrArray.h"
#include "PtrArray3dBase.h"
#include "utility/matrix/Vector.h"
#include "utility/matrices/3d_arrays/Indices3dArray.h"

class ExprAlgebra;
class Intervalo1D;
class RangoIndice;
class Array3dRange;

namespace XC{

//! @ingroup MultiBlockTopologyMR
//! 
//! @brief Three-dimenstional array of point pointers.
class PntPtrArray3d: public PtrArray3dBase<PntPtrArray>
  {
  protected:


    friend class Framework3d;
    PntPtrArray3d(const size_t n_layers= 0);
    PntPtrArray3d(const size_t ,const size_t ,const size_t );
  public:
    inline virtual ~PntPtrArray3d(void) {}
    Pnt *findPoint(const int &tag);
    const Pnt *findPoint(const int &tag) const;
    Pnt *getNearestPnt(const Pos3d &p);
    const Pnt *getNearestPnt(const Pos3d &p) const;

    const MultiBlockTopology *getMultiBlockTopology(void) const;
    MultiBlockTopology *getMultiBlockTopology(void);

    std::deque<size_t> copyPoints(const Array3dRange &,const std::vector<size_t> &,const Vector3d &);
    PntPtrArray3d getPointsOnRange(const Array3dRange &);
    Pnt *getPoint(const VIndices &i);
    PntPtrArray3d getPoints(const Indices3dArray &);
    PntPtrArray getPoints(const IndicesMatrix &);
    PntPtrArray3d getCellPoints(const size_t &,const size_t &,const size_t &,const Indices3dArray &);
    PntPtrArray getCellPoints(const size_t &,const size_t &,const IndicesMatrix &);

    Pos3d getCentroid(void) const;

    void setPnt(const size_t &,const size_t &,const size_t &,const int &);
    Pnt *getPnt(const size_t &,const size_t &,const size_t &);


    void Print(std::ostream &os) const;
  };


std::ostream &operator<<(std::ostream &os,const PntPtrArray3d &);

std::vector<size_t> getIdPointsQuad(const PntPtrArray3d::constant_i_layer_const_ref &,const size_t &j,const size_t &k);

} //end of XC namespace.

#endif
