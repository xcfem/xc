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
//ShellUniformLoad.cpp

#include "ShellUniformLoad.h"
#include "domain/mesh/element/utils/fvectors/FVectorShell.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/plane/shell/ShellMITC4Base.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"

XC::ShellUniformLoad::ShellUniformLoad(int tag,const double &wt,const double &wa1,const double &wa2,const ID &theElementTags)
  :ShellMecLoad(tag, LOAD_TAG_ShellUniformLoad,theElementTags),
   Trans(wt), Axial1(wa1), Axial2(wa2) {}
XC::ShellUniformLoad::ShellUniformLoad(int tag, const Vector &Fxyz, const ID &theElementTags)
  :ShellMecLoad(tag, LOAD_TAG_ShellUniformLoad, theElementTags),
   Trans(Fxyz[2]), Axial1(Fxyz[0]), Axial2(Fxyz[1]) {}

XC::ShellUniformLoad::ShellUniformLoad(int tag)
  :ShellMecLoad(tag, LOAD_TAG_ShellUniformLoad),
   Trans(0.0), Axial1(0.0), Axial2(0.0){}

std::string XC::ShellUniformLoad::Category(void) const
  { return "uniform"; }

//! @brief Returns force expressed in local coordinates.
XC::Vector XC::ShellUniformLoad::getLocalForce(void) const
  {
    Vector retval(3);
    retval(0)= Axial1;
    retval(1)= Axial2;
    retval(2)= Trans;
    return retval;
  }

//! @brief Returns moment expressed in local coordinates.
XC::Vector XC::ShellUniformLoad::getLocalMoment(void) const
  {
    Vector retval(3);
    retval(0)= 0.0;
    retval(1)= 0.0;
    retval(2)= 0.0;
    return retval;
  }

//! @brief Returns the components of the force vectors.
const XC::Matrix &XC::ShellUniformLoad::getLocalForces(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= Wx();
        retval(i,1)= Wy();
        retval(i,2)= Wz();
      }
    return retval;
  }

//! @brief Returns the components of the moment vectors.
const XC::Matrix &XC::ShellUniformLoad::getLocalMoments(void) const
  {
    static Matrix retval;
    const size_t sz= numElements();
    retval= Matrix(sz,3);
    for(size_t i=0; i<sz; i++)
      {
        retval(i,0)= 0.0;
        retval(i,1)= 0.0;
        retval(i,2)= 0.0;
      }
    return retval;
  }


const XC::Vector &XC::ShellUniformLoad::getData(int &type, const double &loadFactor) const
  {
    type = getClassTag();
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    static const Vector trash;
    return trash;
  }

//! @brief Adds the load al consistent load vector (see page 108 libro Eugenio Oñate).
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param p0 element load vector.
void XC::ShellUniformLoad::addReactionsInBasicSystem(const std::vector<double> &areas,const double &loadFactor,FVectorShell &p0) const
  {
    //Loads over nodes.
    const double Px= Wx()*loadFactor;
    const double Py= Wy()*loadFactor;
    const double Pz= Wz()*loadFactor;

    // Reactions in basic system
    for(int i= 0;i<4;i++)
      {
        const double a= areas[i];
        p0.addForce(i,Px*a,Py*a,Pz*a); // i-th node.
      }
  }

//! @brief ??
//! @param areas tributary areas for each node.
//! @param loadFactor Load factor.
//! @param q0 
void XC::ShellUniformLoad::addFixedEndForcesInBasicSystem(const std::vector<double> &,const double &,FVectorShell &) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented." << std::endl;
  }


//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ShellUniformLoad::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

//! @brief Send data through the communicator argument.
int XC::ShellUniformLoad::sendData(Communicator &comm)
  {
    int res= BidimLoad::sendData(comm);
    res+= comm.sendDoubles(Trans,Axial1,Axial2,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::ShellUniformLoad::recvData(const Communicator &comm)
  {
    int res= BidimLoad::recvData(comm);
    res+= comm.receiveDoubles(Trans,Axial1,Axial2,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Send the object through the communicator argument.
int XC::ShellUniformLoad::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(6);
    int result= sendData(comm);

    result+= comm.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send extra data.\n";
    return result;
  }

//! @brief Receive the object through the communicator argument.
int XC::ShellUniformLoad::recvSelf(const Communicator &comm)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }

void  XC::ShellUniformLoad::Print(std::ostream &s, int flag) const
  {
    s << "ShellUniformLoad - Reference load" << std::endl;
    s << "  Transverse:  " << Trans << std::endl;
    s << "  Axial1:      " << Axial1 << std::endl;
    s << "  Axial2:      " << Axial2 << std::endl;
    s << "  Elements acted on: " << getElementTags();
  }
