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
//Vector de matrices de puntos

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

//! \ingroup CadMR
//! 
//! @brief "Tritriz" of elment pointers.
class TritrizPtrPnt: public TritrizPtrBase<MatrizPtrPnt>
  {
  protected:


    friend class MapEsquemas3d;
    TritrizPtrPnt(const size_t capas= 0);
    TritrizPtrPnt(const size_t ,const size_t ,const size_t );
  public:
    inline virtual ~TritrizPtrPnt(void) {}
    Pnt *buscaPunto(const int &tag);
    const Pnt *buscaPunto(const int &tag) const;
    Pnt *getNearestPnt(const Pos3d &p);
    const Pnt *getNearestPnt(const Pos3d &p) const;

    const Cad *getCad(void) const;
    Cad *getCad(void);

    std::deque<size_t> CopiaPuntos(const RangoTritriz &,const std::vector<size_t> &,const Vector3d &);
    TritrizPtrPnt getRangoPuntos(const RangoTritriz &);
    Pnt *getPunto(const VIndices &i);
    TritrizPtrPnt getPuntos(const TritrizIndices &);
    MatrizPtrPnt getPuntos(const MatrizIndices &);
    TritrizPtrPnt getCeldaPuntos(const size_t &,const size_t &,const size_t &,const TritrizIndices &);
    MatrizPtrPnt getCeldaPuntos(const size_t &,const size_t &,const MatrizIndices &);

    Pos3d getCentroide(void) const;

    void setPnt(const size_t &,const size_t &,const size_t &,const int &);
    Pnt *getPnt(const size_t &,const size_t &,const size_t &);


    void Print(std::ostream &os) const;
  };


std::ostream &operator<<(std::ostream &os,const TritrizPtrPnt &);

std::vector<size_t> getIdPuntosQuad(const TritrizPtrPnt::const_ref_capa_i_cte &,const size_t &j,const size_t &k);

} //end of XC namespace.

#endif
