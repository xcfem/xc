//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
} // fin namespace XC



#endif

