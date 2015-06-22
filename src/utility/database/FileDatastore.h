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
                                                                        
// $Revision: 1.14 $
// $Date: 2006/01/03 20:49:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/database/FileDatastore.h,v $
                                                                        
                                                                        
#ifndef FileDatastore_h
#define FileDatastore_h

// Written: fmk 
// Created: 10/98
//
// Description: This file contains the class definition for FileDatastore.
// FileDatastore is a concrete subclas of FE_Datastore. A FileDatastore 
// object is used in the program to store/restore the geometry and state 
// information in a modeler at a particular instance in the analysis. The
// information is stored in text files.
//
// What: "@(#) FileDatastore.h, revA"

#include <utility/database/FE_Datastore.h>

#include <fstream>
#include <map>
using std::fstream;
using std::map;


#define STREAM_POSITION_TYPE int

namespace XC {
class FEM_ObjectBroker;

typedef struct fileDatastoreOutputFile
  {
    fstream *theFile;
    STREAM_POSITION_TYPE fileEnd;
    int      maxDbTag;
  } FileDatastoreOutputFile;

typedef map<int, FileDatastoreOutputFile *>      MAP_FILES;
typedef MAP_FILES::value_type                    MAP_FILES_TYPE;
typedef MAP_FILES::iterator                      MAP_FILES_ITERATOR;

typedef struct intData
  {
    int *dbTag;
    int *data;     
  } IntData;


typedef struct doubleData
  {
    int *dbTag;
    double *data;     
  } DoubleData;


class FileDatastore: public FE_Datastore
  {
  private:
    // Private methods
    int resizeInt(int newSize);
    int resizeDouble(int newSize);
    void resetFilePointers(void);
    int openFile(const std::string &fileName, FileDatastoreOutputFile *, int dataSize);

    // private attributes
    std::string dataBase;
    MAP_FILES theIDFiles;
    MAP_FILES theVectFiles;
    MAP_FILES theMatFiles;
    MAP_FILES_ITERATOR theIDFilesIter;
    MAP_FILES_ITERATOR theVectFilesIter;
    MAP_FILES_ITERATOR theMatFilesIter;

    int lastDomainChangeStamp;
    int currentCommitTag;
    char *data;
    int sizeData;
    
    IntData    theIntData;
    DoubleData theDoubleData;

    int currentMaxInt;
    int currentMaxDouble;

    void libera(void);
    void alloc(const size_t &sz);
    std::string getFileName(const std::string &, int idSize,int commitTag) const;
  public:
    FileDatastore(const std::string &dataBase,Modelador &mdlr, FEM_ObjectBroker &theBroker);    
    
    ~FileDatastore(void);

    // methods for sending and receiving the data
    int sendMsg(int dbTag, int commitTag, const Message &, ChannelAddress *theAddress =0);    
    int recvMsg(int dbTag, int commitTag, Message &, ChannelAddress *theAddress =0);        

    int sendMatrix(int dbTag, int commitTag, const Matrix &, ChannelAddress *theAddress= NULL);
    int recvMatrix(int dbTag, int commitTag, Matrix &, ChannelAddress *theAddress= NULL);
    
    int sendVector(int dbTag, int commitTag, const Vector &, ChannelAddress *theAddress= NULL);
    int recvVector(int dbTag, int commitTag, Vector &, ChannelAddress *theAddress= NULL);
    
    int sendID(int dbTag, int commitTag,const ID &, ChannelAddress *theAddress=NULL);
    int recvID(int dbTag, int commitTag,ID &, ChannelAddress *theAddress= NULL);

    int createTable(const std::string &tableName,const std::vector<std::string> &columns);
    int insertData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, const Vector &data);
    int getData(const std::string &tableName,const std::vector<std::string> &columns, int commitTag, Vector &data);

    // the commitState method
    int commitState(int commitTag);        
  };
} // fin namespace XC

#endif

