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
** (C) Copyright 1999, The Regents of the University of California    **
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
                                                                        
// $Revision: 1.3 $
// $Date: 2006/01/10 18:16:20 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/database/NEESData.h,v $
                                                                        
                                                                        
#ifndef NeesData_h
#define NeesData_h

// Written: fmk 
// Created: 03/04
//
// Description: This file contains the class definition for NeesData.
// NeesData is a concretes subclass of FE_Datastore. NEESData datastore
// is used to store data from the recorders in a file format that can
// be stored in the NEES data repository.
//
// What: "@(#) NeesData.h, revA"

#include "FE_Datastore.h"
#include <fstream>

namespace XC {

class NEES_table
  {
  public:
    std::string name;
    std::vector<std::string> columns;
    NEES_table    *next;
    bool      hasOutExtension;
    inline size_t getNumColumns(void) const
      { return columns.size(); }
  };

class FEM_ObjectBroker;

//! @ingroup Database
//
class NEESData: public FE_Datastore
  {
  private:
    std::string dataBase;
    NEES_table    *tables;
    int          numTables;
  public:
  /** @brief
   *
   * @param database name
   * @param domain
   * @param object broker
   *
   * @author fmk
   */
    NEESData(const std::string &dataBase,Preprocessor &preprocessor, FEM_ObjectBroker &theBroker);
    ~NEESData(void);

    // method to get a database tag
    int getDbTag(void) const;

    // methods for sending and receiving the data
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress= NULL);    
    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress= NULL);        

    int sendMatrix(int dbTag, int commitTag,const Matrix &, ChannelAddress *theAddress= NULL);
    int recvMatrix(int dbTag, int commitTag,Matrix &, ChannelAddress *theAddress= NULL);
    
    int sendVector(int dbTag, int commitTag,const Vector &, ChannelAddress *theAddress= NULL);
    int recvVector(int dbTag, int commitTag,Vector &, ChannelAddress *theAddress= NULL);
    
    int sendID(int dbTag, int commitTag,const ID &, ChannelAddress *theAddress= NULL);
    int recvID(int dbTag, int commitTag,ID &, ChannelAddress *theAddress= NULL);

    int createTable(const std::string &tableName,const std::vector<std::string> &columns);
    int insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const Vector &data);
    int getData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, Vector &data);

    // the commitState method
    int commitState(int commitTag);        
  };
} // end of XC namespace



#endif

