//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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


#include "ThreedimStrainLoad.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/ID.h"


#include "utility/actor/actor/MovableVectors.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/Matrix.h"


XC::ThreedimStrainLoad::ThreedimStrainLoad(int tag, const std::vector<Vector> &t,const ID &theElementTags)
  :ThreedimLoad(tag, LOAD_TAG_ThreedimStrainLoad, theElementTags), strains(t) {}
XC::ThreedimStrainLoad::ThreedimStrainLoad(int tag,const size_t &sz,const Vector &t,const ID &theElementTags)
  :ThreedimLoad(tag, LOAD_TAG_ThreedimStrainLoad, theElementTags), strains(sz,t) {}

XC::ThreedimStrainLoad::ThreedimStrainLoad(int tag,const size_t &sz, const ID &theElementTags)
  :ThreedimLoad(tag, LOAD_TAG_ThreedimStrainLoad, theElementTags), strains(sz) {}

XC::ThreedimStrainLoad::ThreedimStrainLoad(int tag,const size_t &sz, const Vector &t)
  :ThreedimLoad(tag, LOAD_TAG_ThreedimStrainLoad), strains(sz,t) {}

XC::ThreedimStrainLoad::ThreedimStrainLoad(int tag,const size_t &sz)
  :ThreedimLoad(tag, LOAD_TAG_ThreedimStrainLoad), strains(sz) {}

XC::ThreedimStrainLoad::ThreedimStrainLoad(const size_t &sz)
  :ThreedimLoad(0,LOAD_TAG_ThreedimStrainLoad), strains(sz) {}

//! @brief Sets the strains for a Gauss point.
//! @param i: Gauss point index.
//! @param j: Strain component.
//! @param strain: Strain value.
void XC::ThreedimStrainLoad::setStrainComp(const size_t &i,const size_t &j,const double &strain)
  {
    if(i<strains.size())
      {
        Vector &def= strains.at(i);
        if(j<size_t(def.Size()))
          def(j)= strain;
        else
          std::cerr << getClassName() << "::setStrainComp "
                    << " component: " << j
	            << " doesn't exist." << std::endl;
      }
    else
      std::cerr << getClassName() << "::setStrainComp "
                << " gauss point: "  << i
                << " doesn't exist." << std::endl;
  }

//! @brief Asigna las strains.
void XC::ThreedimStrainLoad::setStrains(const Matrix &def)
  {
    const int nRows= def.noRows();
    const int nCols= def.noCols();
    std::vector<Vector> tmp(nRows);
    Vector ri(nCols);
    for(int i= 0;i<nRows;i++)
      {
        for(int j= 0;j<nCols;j++)
          ri[j]= def(i,j);
        tmp[i]= ri;
      }
    strains= tmp;
  }

const XC::Vector &XC::ThreedimStrainLoad::getData(int &type, const double &loadFactor) const
  {
    type = getClassTag();
    std::cerr << getClassName() << "::" << __FUNCTION__
              << " not implemented yet." << std::endl;
    static const Vector trash;
    return trash;
  }


//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::ThreedimStrainLoad::getDbTagData(void) const
  {
    static DbTagData retval(6);
    return retval;
  }

//! @brief Send data through the communicator argument.
int XC::ThreedimStrainLoad::sendData(Communicator &comm)
  {
    int res= ThreedimLoad::sendData(comm);
    res+= comm.sendVectors(strains,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receive data through the communicator argument.
int XC::ThreedimStrainLoad::recvData(const Communicator &comm)
  {
    int res= ThreedimLoad::recvData(comm);
    res+= comm.receiveVectors(strains,getDbTagData(),CommMetaData(5));
    return res;
  }

int XC::ThreedimStrainLoad::sendSelf(Communicator &comm)
  {
    inicComm(6);
    int res= sendData(comm);

    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ThreedimStrainLoad::sendSelf() - failed to send data\n";    
    return res;
  }

int XC::ThreedimStrainLoad::recvSelf(const Communicator &comm)
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

void XC::ThreedimStrainLoad::Print(std::ostream &s, int flag) const
  {
    s << "ThreedimStrainLoad" << std::endl;
    if(!strains.empty())
      {
        std::vector<Vector>::const_iterator i= strains.begin();
        s << *i;
        for(;i!=strains.end();i++)
          s << ", " << *i;
      }
    ThreedimLoad::Print(s,flag);
  }
