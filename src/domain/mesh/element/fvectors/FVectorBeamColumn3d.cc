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
//FVectorBeamColumn3d.cc

#include "FVectorBeamColumn3d.h"
#include <utility/matrix/Vector.h>

//! @brief Constructor por defecto.
XC::FVectorBeamColumn3d::FVectorBeamColumn3d(void)
  { zero(); }

//! @brief Constructor de copia.
XC::FVectorBeamColumn3d::FVectorBeamColumn3d(const XC::FVectorBeamColumn3d &otro)
  {
    p[0] = otro.p[0];
    p[1] = otro.p[1];
    p[2] = otro.p[2];
    p[3] = otro.p[3];
    p[4] = otro.p[4];
  }

//! @brief Constructor a partir de un vector.
XC::FVectorBeamColumn3d::FVectorBeamColumn3d(const XC::Vector &v)
  {
    if(v.Size()>=5)
      {
        p[0] = v[0];
        p[1] = v[1];
        p[2] = v[2];
        p[3] = v[3];
        p[4] = v[4];
      }
    else
      std::cerr << "FVectorBeamColumn2d::FVectorBeamColumn2d; se esperaba un vector de dimensión 5" << std::endl;
  }

//! @brief Operador asignación.
XC::FVectorBeamColumn3d &XC::FVectorBeamColumn3d::operator=(const XC::FVectorBeamColumn3d &otro)
  {
    p[0] = otro.p[0];
    p[1] = otro.p[1];
    p[2] = otro.p[2];
    p[3] = otro.p[3];
    p[4] = otro.p[4];
    return *this;
  }

void XC::FVectorBeamColumn3d::zero(void)
  {
    p[0]= 0.0;
    p[1]= 0.0;
    p[2]= 0.0;
    p[3]= 0.0;
    p[4]= 0.0;
  }

XC::FVectorBeamColumn3d &XC::FVectorBeamColumn3d::operator*=(const double &d)
  {
    p[0]*= d;
    p[1]*= d;
    p[2]*= d;
    p[3]*= d;
    p[4]*= d;
    return *this;    
  }

XC::FVectorBeamColumn3d &XC::FVectorBeamColumn3d::operator+=(const FVectorBeamColumn3d &otro)
  {
    p[0]+= otro.p[0];
    p[1]+= otro.p[1];
    p[2]+= otro.p[2];
    p[3]+= otro.p[3];
    p[4]+= otro.p[4];
    return *this;    
  }

XC::FVectorBeamColumn3d &XC::FVectorBeamColumn3d::operator-=(const FVectorBeamColumn3d &otro)
  {
    p[0]-= otro.p[0];
    p[1]-= otro.p[1];
    p[2]-= otro.p[2];
    p[3]-= otro.p[3];
    p[4]-= otro.p[4];
    return *this;    
  }

void XC::FVectorBeamColumn3d::Print(std::ostream &os) const
  {
    os << '[' << p[0] << ',' << p[1] << ',' << p[2] << ',' << p[3] << ',' << p[4] << ']';
  }

