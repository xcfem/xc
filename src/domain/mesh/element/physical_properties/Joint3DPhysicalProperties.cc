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
//Joint3DPhysicalProperties.cpp

#include "Joint3DPhysicalProperties.h"
#include <material/uniaxial/UniaxialMaterial.h>
#include "utility/actor/actor/CommMetaData.h"

//! @brief Constructor
XC::Joint3DPhysicalProperties::Joint3DPhysicalProperties(const size_t &nMat,const UniaxialMaterial *ptr_mat)
  : UniaxialMatPhysicalProperties(nMat,ptr_mat)
  { }

//! @brief Constructor
XC::Joint3DPhysicalProperties::Joint3DPhysicalProperties(const UniaxialMaterial &springx, const UniaxialMaterial &springy, const UniaxialMaterial &springz)
  : UniaxialMatPhysicalProperties(3,nullptr)
  {
    // make copy of the uniaxial materials for the element

    if(&springx == nullptr)
      {
        std::cerr << "ERROR XC::Joint3D::Joint3D(): The rotational spring in y'z' plane does not exist ";
        exit(-1);
      }
    else
      { theMaterial[0] = springx.getCopy(); }

    if(&springy == nullptr )
      {
        std::cerr << "ERROR XC::Joint3D::Joint3D(): The rotational spring in x'z' plane does not exist ";
        exit(-1);
      }
    else
      { theMaterial[1] = springy.getCopy(); }

    if( &springz == nullptr )
      {
        std::cerr << "ERROR XC::Joint3D::Joint3D(): The rotational spring in x'y' plane does not exist ";
        exit(-1);
      }
    else
      { theMaterial[2] = springz.getCopy(); }
  }

int XC::Joint3DPhysicalProperties::update(const Vector &dispC,const int &offset)
  {
    int result = 0;

    for(int i=0;i<3;i++)
      {
        if( theMaterial[i] != nullptr )
          result = theMaterial[i]->setTrialStrain(dispC(i+offset));
        if( result != 0 ) break;
      }
    return result;
  }

XC::Vector XC::Joint3DPhysicalProperties::getTangent(void) const
  {
    Vector Ktangent(3);
    for(int i=0;i<3;i++ )
      {
        Ktangent[i]= 0;
        if(theMaterial[i])
          Ktangent[i] = theMaterial[i]->getTangent();
      }
    return Ktangent;
  }

XC::Vector XC::Joint3DPhysicalProperties::getStress(void) const
  {
    Vector Force(3) ;
    for(int i=0 ; i<3 ; i++ )
      {
        Force[i]= 0;
        if( theMaterial[i] != nullptr ) 
          Force[i] = theMaterial[i]->getStress();
      }
    return Force;
  }

XC::Vector XC::Joint3DPhysicalProperties::getStrain(void) const
  {
    Vector strain(3) ;
    for(int i=0 ; i<3 ; i++ )
      {
        strain[i]= 0;
        if( theMaterial[i] != nullptr ) 
          strain[i] = theMaterial[i]->getStrain();
      }
    return strain;
  }

XC::Vector XC::Joint3DPhysicalProperties::getStrainStress(void) const
  {
    Vector retval(6) ;
    for( int i =0 ; i<3 ; i++ )
      {
        retval(i) = 0.0;
        retval(i+3) = 0.0;
        if( theMaterial[i] != nullptr )
          {
            retval(i) = theMaterial[i]->getStrain();
            retval(i+3) = theMaterial[i]->getStress();
          }
      }
    return retval;
  }

//! See Joint3D.cpp
XC::Vector XC::Joint3DPhysicalProperties::getResponse7(void) const
  {
    Vector retval(3) ;
    for( int i =0 ; i<3 ; i++ )
      {
        retval(i) = 0.0;
        retval(i+3) = 0.0;
        if( theMaterial[i] != nullptr && theMaterial[i]->getInitialTangent() != 0.0  )
          retval(i) = theMaterial[i]->getStrain() - theMaterial[i]->getStress()/theMaterial[i]->getInitialTangent();
      }
    return retval;
  }
