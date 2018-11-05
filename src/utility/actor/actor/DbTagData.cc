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
//DbTagData.cc

#include "DbTagData.h"
#include "CommParameters.h"
#include "xc_utils/src/utils/misc_utils/print_trace.h"

//! @brief Constructor.
XC::DbTagData::DbTagData(const int &sz)
  : data(sz) {}

//! @brief Constructor.
XC::DbTagData::DbTagData(const ID &id)
  : data(id) {}

//! @brief Parameter initialization to communicate sz objects.
void XC::DbTagData::inicComm(const int &sz)
  {
    const int szOld= data.Size();
    if(sz!=szOld)
      {
        data.resize(sz);
	std::cerr << __FUNCTION__
	          << "; vector size was changed: ("
                  << szOld << "->" << sz << ")" << std::endl;
        print_trace();
      }
    data.Zero();
    visitados.clear();
  }

//! @brief Returns the integer in the position being passed as parameter.
const int &XC::DbTagData::getDbTagDataPos(const size_t &i) const
  {
    static int retval= 0;
    if(i<size_t(data.Size()))
      {
        if(visitados.find(i)!=visitados.end())
	  std::cerr << "DbTagData::getDbTagDataPos: index "
                << i << " already used." << std::endl;
        retval= data(i);
        visitados.insert(i);
      }
    else
      std::cerr << "DbTagData::getDbTagDataPos: index "
                << i << " out of range; [0," << data.Size()-1
                << "]." << std::endl;
    return retval;
  }

//! @brief Sets the integer in the  position being passed as parameter.
void XC::DbTagData::setDbTagDataPos(const size_t &i,const int &v)
  {
    if(i<size_t(data.Size()))
      {
        if(visitados.find(i)!=visitados.end())
	  std::cerr << "DbTagData::setDbTagDataPos: index "
                << i << " already used." << std::endl;
        data[i]= v;
        visitados.insert(i);
      }
    else
      std::cerr << "DbTagData::setDbTagDataPos: index "
                << i << " out of range; [0," << data.Size()-1
                << "]." << std::endl;
  }

//! @brief Convenience function.
int XC::DbTagData::sendIdData(CommParameters &cp,const int &dataTag) const
  { return cp.sendID(data,dataTag); }

//! @brief Convenience function.
int XC::DbTagData::receiveIdData(const CommParameters &cp,const int &dataTag)
  { return cp.receiveID(data,dataTag); }

//! @brief Sends the object.
int XC::DbTagData::send(DbTagData &dt,CommParameters &cp, const CommMetaData &meta) const
  { return cp.sendID(data,dt,meta); }

//! @brief Receive the object.
int XC::DbTagData::receive(DbTagData &dt,const CommParameters &cp, const CommMetaData &meta)
  { return cp.receiveID(data,dt,meta); }

//! @brief Print stuff.
void XC::DbTagData::Print(std::ostream &os) const
  {
    os << "dbTags: [" << data << "] visitados: [";
    if(!visitados.empty())
      {
        std::set<size_t>::const_iterator i= visitados.begin();
        os << *i;i++;
        for(;i!=visitados.end();i++)
          os << " " << *i;
      }
    os << "]";
  }

std::ostream &XC::operator<<(std::ostream &os,const XC::DbTagData &dt)
  {
    dt.Print(os);
    return os;
  }
