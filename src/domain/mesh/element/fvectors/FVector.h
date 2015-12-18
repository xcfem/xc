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
//FVector.h

#ifndef FVector_h
#define FVector_h

#include <iostream>
#include <cstddef>

namespace XC {
class Vector;

//! \ingroup Elem
//
//! @defgroup ElemFV Esfuerzos en elementos.
//!
//! \ingroup ElemFV
//
//! @brief Esfuerzos en un elemento.
class FVector
  {
  public:
    virtual const double &operator[](size_t i) const= 0;
    virtual double &operator[](size_t i)= 0;
    virtual void zero(void)= 0;
    virtual size_t size(void) const
      { return 0; }
    virtual double *getPtr(void)
      { return NULL; }
    virtual void Print(std::ostream &os) const= 0;
    virtual ~FVector(void)
      {}

    virtual void addForce(const size_t &inod,const double &,const double &,const double &);
    virtual void addMoment(const size_t &inod,const double &,const double &,const double &);
  };

std::ostream &operator<<(std::ostream &,const FVector &f);

} // end of XC namespace

#endif


