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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite XC::Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              19AUg2003
//# UPDATE HISTORY:    Sept 2003
//#                    May28, 2004
//#
//===============================================================================

#include <material/nD/FiniteDeformation/FiniteDeformationElastic3D.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/nDarray/Tensor.h>
#include <utility/matrix/nDarray/stresst.h>
#include <utility/matrix/nDarray/straint.h>
#include <cmath>
#include <utility/matrix/ID.h>

using namespace XC;

//-----------------------------------------------------------------------------------------------------------------------------------------------
XC::FiniteDeformationElastic3D::FiniteDeformationElastic3D(int tag,
                                                       int classTag,
                                                       double rho_in= 0.0)
:XC::FiniteDeformationMaterial(tag, classTag), rho(rho_in)
  {}

XC::FiniteDeformationElastic3D::FiniteDeformationElastic3D(int tag)
  : FiniteDeformationMaterial(tag,ND_TAG_FiniteDeformationElastic3D), rho(0.0)
  {}

//------------------------------------------------------------------------------------------------------------------------------------------------
XC::FiniteDeformationElastic3D::FiniteDeformationElastic3D( )
: FiniteDeformationMaterial(0, 0), rho(0.0)
{}

//! @brief Return material density.
double XC::FiniteDeformationElastic3D::getRho(void) const
  { return rho; }

//--------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::setTrialF(const XC::straintensor &f)
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::setTrialFIncr(const XC::straintensor &df)
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::setTrialC(const XC::straintensor &c)
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::setTrialCIncr(const XC::straintensor &dc)
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return 0;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
const XC::straintensor &XC::FiniteDeformationElastic3D::getF(void) const
  {
     std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
     exit(-1);
     static straintensor retval;
     return retval;
  }

//-----------------------------------------------------------------------------------------------------------------------------------------------------
const XC::straintensor &XC::FiniteDeformationElastic3D::getC(void) const
  {
     std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
     exit(-1);
     static straintensor retval;
     return retval;
  }

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::Tensor &XC::FiniteDeformationElastic3D::getTangentTensor(void) const
  {
    std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
    exit(-1);
    // Just to make it compile
    XC::Tensor *ret = new Tensor;
    return *ret;
  }

//! @brief Return the material initial stiffness.
const XC::Tensor &XC::FiniteDeformationElastic3D::getInitialTangentTensor(void) const
  {
    std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
    exit(-1);
    // Just to make it compile
    XC::Tensor *ret = new Tensor;
    return *ret;
  }
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::straintensor &XC::FiniteDeformationElastic3D::getStrainTensor(void) const
  {
    std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
    exit(-1);
    // Just to make it compile
    static XC::straintensor ret;
    return ret;
  }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::stresstensor &XC::FiniteDeformationElastic3D::getStressTensor(void) const
  {
    std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
    exit(-1);
    static XC::stresstensor ret; 
    return ret;
  }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::stresstensor XC::FiniteDeformationElastic3D::getPK1StressTensor(void) const
  {
    std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
    exit(-1);
    XC::stresstensor ret; 
    return ret;
  }
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::stresstensor XC::FiniteDeformationElastic3D::getCauchyStressTensor(void) const
  {
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   XC::stresstensor ret; 
   return ret;
  }
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::commitState(void)
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return -1;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::revertToLastCommit(void)
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return -1;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::revertToStart(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility\n";
    exit(-1);
    return -1;
  }

//! @brief Virtual constructor.
XC::NDMaterial * XC::FiniteDeformationElastic3D::getCopy(void) const
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return 0;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
XC::NDMaterial * XC::FiniteDeformationElastic3D::getCopy(const std::string &type) const
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const std::string &XC::FiniteDeformationElastic3D::getType(void) const
  {
     std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
     exit(-1);
     static const std::string retval= "error";
     return retval;
  }
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::getOrder(void) const
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return 0;
}

//! @brief Send object members through the communicator argument.
int XC::FiniteDeformationElastic3D::sendData(Communicator &comm)
  {
    int res= FiniteDeformationMaterial::sendData(comm);
    res+= comm.sendDouble(rho,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::FiniteDeformationElastic3D::recvData(const Communicator &comm)
  {
    int res= FiniteDeformationMaterial::recvData(comm);
    res+= comm.receiveDouble(rho,getDbTagData(),CommMetaData(1));
    return res;
  }


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void XC::FiniteDeformationElastic3D::Print(std::ostream &s, int flag) const
{
   std::cerr << "FiniteDeformationElastic3D-- subclass responsibility\n";
   exit(-1);
   return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::FiniteDeformationElastic3D::updateParameter(int parameterID, Information &info)
  { return -1; }
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

