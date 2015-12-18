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
// BerkeleyDbDatastore is a concrete subclas of DBDatastore. A BerkeleyDbDatastore 
// object is used in the program to store/restore the geometry and state 
// information in a model at a particular instance in the analysis. The
// information is stored using the BerkeleyDB Database.
//
// What: "@(#) BerkeleyDbDatastore.h, revA"

#include <utility/database/DBDatastore.h>
#include <db.h>

namespace XC {
class BerkeleyDbDatastore: public DBDatastore
  {
  private:
    DB_ENV *dbenv; //!< database enviroment - directory containing files
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
  
    // methods for sending and recieving matrices, vectors and id's
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress= NULL);    
    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress= NULL);        

    int sendMatrix(int dbTag, int commitTag, const Matrix &theMatrix, ChannelAddress *theAddress= NULL);
    int recvMatrix(int dbTag, int commitTag, Matrix &theMatrix, ChannelAddress *theAddress= NULL);
  
    int sendVector(int dbTag, int commitTag, const Vector &,ChannelAddress *theAddress= NULL);
    int recvVector(int dbTag, int commitTag, Vector &,ChannelAddress *theAddress= NULL);
  
    int sendID(int dbTag, int commitTag, const ID &,ChannelAddress *theAddress= NULL);
    int recvID(int dbTag, int commitTag, ID &theID, ChannelAddress *theAddress= NULL);    
  };
} // end of XC namespace

#endif
