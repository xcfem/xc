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
//FVectorData.h

#ifndef FVectorData_h
#define FVectorData_h

#include "FVector.h"
#include "utility/matrix/Vector.h"
#include "utility/actor/actor/MovableVector.h"

namespace XC {

template <size_t SZ>
//! \ingroup ElemFV
//
//! @brief Clase auxiliar para los esfuerzos en un elemento de tipo barra.
class FVectorData: public FVector
  {
  protected:
    double p[SZ];
  public:
    inline const double &operator[](size_t i) const
      { return p[i]; }
    inline double &operator[](size_t i)
      { return p[i]; }
    inline size_t size(void) const
      { return SZ; }
    inline virtual double *getPtr(void)
      { return p; }
    inline virtual const double *getPtr(void) const
      { return p; }
    const Vector &getVector(void) const;
    void putVector(const Vector &v);
    int sendData(CommParameters &cp,DbTagData &dt,const CommMetaData &);
    int receiveData(const CommParameters &cp,DbTagData &dt,const CommMetaData &);

  };

template <size_t SZ>
const Vector &FVectorData<SZ>::getVector(void) const
  {
    static Vector retval(SZ);
    double *tmp= const_cast<double *>(p);
    retval= Vector(tmp,SZ);
    return retval;
  }

template <size_t SZ>
void FVectorData<SZ>::putVector(const Vector &v)
  {
    const size_t sz= std::min(size_t(v.Size()),SZ);
    for(size_t i=0;i<sz;i++)
      p[i]= v[i];
  }

template <size_t SZ>
int FVectorData<SZ>::sendData(CommParameters &cp,DbTagData &dt,const CommMetaData &md)
  { return cp.sendVector(getVector(),dt,md); }

template <size_t SZ>
int FVectorData<SZ>::receiveData(const CommParameters &cp,DbTagData &dt,const CommMetaData &md)
  {
    Vector tmp= getVector();
    int res= cp.receiveVector(tmp,dt,md);
    putVector(tmp);
    return res;
  }

} // end of XC namespace

#endif


