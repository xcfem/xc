//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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


