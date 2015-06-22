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
//EsfBeamColumn3d.cc

#include "EsfBeamColumn3d.h"
#include "utility/actor/actor/MovableVector.h"

//! @brief Constructor por defecto.
XC::EsfBeamColumn3d::EsfBeamColumn3d(void)
  : Vector(6) {}

//! @brief Constructor.
XC::EsfBeamColumn3d::EsfBeamColumn3d(const Vector &v)
  : Vector(6)
  {
    const size_t sz= v.Size();
    if(sz>=6)
      for(int i=0;i<6;i++)
        (*this)[i]= v[i];
    else
      std::cerr << "EsfBeamColumn3d; se esperaba un vector de dimensión 6"
                << " se obtuvo: " << v << std::endl;
  }

//! @brief Constructor de copia.
XC::EsfBeamColumn3d::EsfBeamColumn3d(const XC::EsfBeamColumn3d &otro)
  : Vector(otro) {}

//! @brief Operador asignación.
XC::EsfBeamColumn3d &XC::EsfBeamColumn3d::operator=(const EsfBeamColumn3d &otro)
  {
    Vector::operator=((const Vector &)(otro));
    return *this;
  }

int XC::sendEsfBeamColumn3d(const EsfBeamColumn3d &esf,int posDbTag,DbTagData &dt,CommParameters &cp)
  { return cp.sendVector(esf,dt,CommMetaData(posDbTag)); }

int XC::receiveEsfBeamColumn3d(EsfBeamColumn3d &esf,int posDbTag,DbTagData &dt,const CommParameters &cp)
  {
    Vector tmp= esf;
    int res= cp.receiveVector(tmp,dt,CommMetaData(posDbTag));
    esf= EsfBeamColumn3d(tmp);
    return res;
  }
