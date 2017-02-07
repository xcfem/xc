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
