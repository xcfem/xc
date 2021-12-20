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
//PyDictDatastore.h

#ifndef PyDictDatastore_h
#define PyDictDatastore_h

#include <utility/database/FE_Datastore.h>

namespace XC {
//! @ingroup Utils
//!
//! @defgroup Database Database utilities.
//
//
//! @brief Store model data in a Python dictionary.
//! @ingroup Database
class PyDictDatastore: public FE_Datastore
  {
  private:
    std::string fileName;
    boost::python::dict pyDict; //!< python dictionary.
    void insertData(const std::string &,const int &,const int &,const double *,const int &);
    void insertData(const std::string &,const int &,const int &,const int *,const int &);
    std::vector<double> retrieveDoubleData(const std::string &,const int &,const int &);
    std::vector<int> retrieveIntData(const std::string &,const int &,const int &);
  public:
    PyDictDatastore(const std::string &,Preprocessor &, FEM_ObjectBroker &,int dbRun = 0);    

    // methods for sending and receiving matrices, vectors and id's
    int sendMsg(int , int , const Message &, ChannelAddress *a= nullptr);    
    int recvMsg(int , int , Message &, ChannelAddress *a= nullptr);        

    int sendMatrix(int , int , const Matrix &,ChannelAddress *a= nullptr);
    int recvMatrix(int , int , Matrix &, ChannelAddress *a= nullptr);

    int sendVector(int , int , const Vector &,ChannelAddress *a= nullptr);
    int recvVector(int , int , Vector &,ChannelAddress *a= nullptr);
    
    int sendID(int , int ,const ID &,ChannelAddress *a= nullptr);
    int recvID(int , int ,ID &,ChannelAddress *a= nullptr);    

    void jsonWrite(std::ostream &) const;
    void jsonRead(std::istream &);
    int save(const int &commitTag);
    int restore(const int &commitTag);
  };
} // end of XC namespace

#endif
