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
//SectionMatrices.cc

#include "SectionMatrices.h"
#include "utility/actor/actor/MovableVectors.h"
#include "utility/actor/actor/MovableMatrices.h"
#include "utility/matrix/ID.h"

const size_t XC::SectionMatrices::maxNumSections;


//! @brief Allocate section flexibility matrices and section deformation vectors
void XC::SectionMatrices::alloc(const size_t &sz)
  {
    vsSubdivide.resize(sz);
    fsSubdivide.resize(sz);
    SsrSubdivide.resize(sz);
  }

//! @brief Free memory.
void XC::SectionMatrices::free(void)
  {
    vsSubdivide.clear();
    fsSubdivide.clear();
    SsrSubdivide.clear();
  }

XC::SectionMatrices::SectionMatrices(const size_t &sz)
  : MovableObject(0)
  { alloc(sz); }

//! @brief Resize objects.
void XC::SectionMatrices::resize(const size_t &sz)
  { alloc(sz); }

//! @brief Initializes the i-th elements.
void XC::SectionMatrices::initialize(const size_t &i,const int &order)
  {
    fsSubdivide[i]= Matrix(order,order);
    vsSubdivide[i]= Vector(order);
    SsrSubdivide[i]= Vector(order);
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::SectionMatrices::sendSelf(Communicator &comm)
  {
    inicComm(3);

    int res= comm.sendVectors(vsSubdivide,getDbTagData(),CommMetaData(0));
    res+= comm.sendMatrices(fsSubdivide,getDbTagData(),CommMetaData(1));
    res+= comm.sendVectors(SsrSubdivide,getDbTagData(),CommMetaData(2));

    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "SectionMatrices::sendSelf -- failed to send ID data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::SectionMatrices::recvSelf(const Communicator &comm)
  {
    inicComm(3);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "SectionMatrices::recvSelf -- failed to receive ID data\n";
    else
      {
        res+= comm.receiveVectors(vsSubdivide,getDbTagData(),CommMetaData(0));
        res+= comm.receiveMatrices(fsSubdivide,getDbTagData(),CommMetaData(1));
        res+= comm.receiveVectors(SsrSubdivide,getDbTagData(),CommMetaData(2));
      }
    return res;
  }
