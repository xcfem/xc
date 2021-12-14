// -*-c++-*-
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
                                                                        
                                                                        
#ifndef DbTagData_h
#define DbTagData_h

#include "utility/matrix/ID.h"
#include <set>

namespace XC {

class Communicator;
class CommMetaData;

//! @ingroup IPComm
//
//! @brief Vector that stores the dbTags of the class members.
class DbTagData
  {
  private:
    ID data;
    mutable std::set<size_t> visitados;

    friend class Communicator;
    int sendIdData(Communicator &,const int &) const;
    int receiveIdData(const Communicator &,const int &);
  public:
    DbTagData(const int &dataSize= 0);
    explicit DbTagData(const ID &);
    void inicComm(const int &dataSize);

    inline int Size(void) const
      { return data.Size(); }
    inline int resize(const int &newSize)
      { return data.resize(newSize); }
    const int &getDbTagDataPos(const size_t &i) const;
    void setDbTagDataPos(const size_t &i,const int &v);
    int send(DbTagData &,Communicator &, const CommMetaData &) const;
    int receive(DbTagData &,const Communicator &,const CommMetaData &);
    void Print(std::ostream &os) const;
  };

std::ostream &operator<<(std::ostream &,const DbTagData &);

} // end of XC namespace

#endif
