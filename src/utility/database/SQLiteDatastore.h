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
//SQLiteDatastore.h

#ifndef SQLiteDatastore_h
#define SQLiteDatastore_h

#include "DBDatastore.h"
#include "xc_utils/src/sqlite/SqLiteDatabase.h"

namespace XC {
class SQLiteDatastore: public DBDatastore
  {
  private:
    bool connection;
    SqLiteDatabase db; //!< Base de datos SqLite.
    std::string query;
    bool insertaDatos(const std::string &,const int &,const int &,const void *,const int &,const int &);
    bool actualizaDatos(const std::string &,const int &,const int &,const void *,const int &,const int &);
    const void *recuperaDatos(const std::string &tbName,const int &dbTag,const int &commitTag,const int &sz);
  protected:
    int createOpenSeesDatabase(const std::string &projectName);
    int execute(const std::string &query);
  public:
    SQLiteDatastore(const std::string &,Preprocessor &, FEM_ObjectBroker &,int dbRun = 0);    

    // methods for sending and recieving matrices, vectors and id's
    int sendMsg(int , int , const Message &, ChannelAddress *a= NULL);    
    int recvMsg(int , int , Message &, ChannelAddress *a= NULL);        

    int sendMatrix(int , int , const Matrix &,ChannelAddress *a= NULL);
    int recvMatrix(int , int , Matrix &, ChannelAddress *a= NULL);

    int sendVector(int , int , const Vector &,ChannelAddress *a= NULL);
    int recvVector(int , int , Vector &,ChannelAddress *a= NULL);
    
    int sendID(int , int ,const ID &,ChannelAddress *a= NULL);
    int recvID(int , int ,ID &,ChannelAddress *a= NULL);    

    int createTable(const std::string &,const std::vector<std::string> &);
    int insertData(const std::string &,const std::vector<std::string> &, int , const Vector &);
    int getData(const std::string &,const std::vector<std::string> &, int , Vector &);
  };
} // fin namespace XC

#endif
