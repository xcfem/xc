// -*-c++-*-
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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2002, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.2 $
// $Date: 2003/10/15 16:49:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/database/BerkeleyDbDatastore.h,v $

#ifndef BerkeleyDbDatastore_h
#define BerkeleyDbDatastore_h

// Written:  fmk
// Created:  02/02
//
// Description: This file contains the class definition for BerkeleyDbDatastore.
// BerkeleyDbDatastore is a concrete subclas of DBDatastore. 
//
// What: "@(#) BerkeleyDbDatastore.h, revA"

#include <utility/database/DBDatastore.h>
#include <db.h>

namespace XC {
//! @ingroup Database
//
//! @brief Store model data in a <a href="https://en.wikipedia.org/wiki/Berkeley_DB">Berkeley DB</a> database.
//
//! A BerkeleyDbDatastore object is used in the program to store/restore
//! the geometry and state information in a model at a particular instance in
//! the analysis. The information is stored using the BerkeleyDB Database.
class BerkeleyDbDatastore: public DBDatastore
  {
  private:
    DB_ENV *dbenv; //!< database environment - directory containing files
    DB *dbMatrix; //!< file holding the data for the Matrices
    DB *dbVector; //!< file holding the data for the Vectors
    DB *dbID; //!< file holding the data for the ID's

    bool connection;

    DBT key, data;  // structures for the key data pair

    std::string project;
    static const size_t sz_dbl= sizeof(double);
    static const size_t sz_int= sizeof(int);

    void setup_key(const int &,const int &,const int &);
    void setup_data(const void *,const int &,const int &);
    void setup_structures(const int &,const int &,const void *,const int &,const int &);
    bool create_aux_entities(void);
    DBTYPE get_type(const std::string &);
    int createDB(void);
    int open(const DBTYPE &type);
    void close(void);
  public:
    BerkeleyDbDatastore(const std::string &projectName, Preprocessor &preprocessor, FEM_ObjectBroker &theBroker,const std::string &type= "");
    ~BerkeleyDbDatastore(void);

    // method to get a database tag
    int getDbTag(void) const;
  
    // methods for sending and receiving matrices, vectors and id's
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress= nullptr);    
    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress= nullptr);        

    int sendMatrix(int dbTag, int commitTag, const Matrix &theMatrix, ChannelAddress *theAddress= nullptr);
    int recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress= nullptr);
  
    int sendVector(int dbTag, int commitTag, const Vector &,ChannelAddress *theAddress= nullptr);
    int recvVector(int dbTag, int commitTag, Vector &,ChannelAddress *theAddress= nullptr);
  
    int sendID(int dbTag, int commitTag, const ID &,ChannelAddress *theAddress= nullptr);
    int recvID(int dbTag, int commitTag, ID &theID, ChannelAddress *theAddress= nullptr);    
  };
} // end of XC namespace

#endif
