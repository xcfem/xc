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
//UniaxialMatPhysicalProperties.cpp

#include "UniaxialMatPhysicalProperties.h"
#include <material/uniaxial/UniaxialMaterial.h>

//! @brief Constructor
XC::UniaxialMatPhysicalProperties::UniaxialMatPhysicalProperties(const size_t &nMat,const UniaxialMaterial *ptr_mat)
  : PhysicalProperties<UniaxialMaterial>(nMat,ptr_mat)
  { }

//! @brief Constructor
XC::UniaxialMatPhysicalProperties::UniaxialMatPhysicalProperties(const std::vector<UniaxialMaterial *> &ptr_mats)
  : PhysicalProperties<UniaxialMaterial>(ptr_mats.size(),nullptr)
  {
    const size_t sz= ptr_mats.size();
    for(size_t i= 0;i<sz;i++)
      theMaterial.setMaterial(i,ptr_mats[i]->getCopy());
  }

XC::Vector XC::UniaxialMatPhysicalProperties::getStresses(void) const
  {
    const size_t sz= size();
    Vector retval(sz);
    for(size_t i=0;i<sz;i++)
      if(theMaterial[i]!=nullptr)
        retval[i]= theMaterial[i]->getStress();
    return retval;
  }

//! @brief print out element data
void XC::UniaxialMatPhysicalProperties::Print(std::ostream &s)
  {
    std::cerr << "UniaxialMatPhysicalProperties\t" << theMaterial.size() << std::endl;
  }

