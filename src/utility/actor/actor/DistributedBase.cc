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
//DistributedBase.cpp

#include "DistributedBase.h"
#include "DbTagData.h"
#include <iostream>
#include <typeinfo>

//! @brief Constructor.
XC::DistributedBase::DistributedBase(void)
  {}


//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::DistributedBase::getDbTagData(void) const
  {
    static DbTagData retval(0);
    std::string nmb= typeid(*this).name();
    std::cerr << nmb << "; la función getDbTagData debe redefinirse en la clase."
              << std::endl;
    return retval;
  }

const int &XC::DistributedBase::getDbTagDataPos(const int &i) const
  { return getDbTagData().getDbTagDataPos(i); }

void XC::DistributedBase::setDbTagDataPos(const int &i,const int &v)
  { return getDbTagData().setDbTagDataPos(i,v); }

void XC::DistributedBase::inicComm(const int &dataSize) const
  { return getDbTagData().inicComm(dataSize); }

