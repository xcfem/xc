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
//CommParameters.cc

#include "CommParameters.h"
#include "MovableID.h"
#include "MovableBJTensor.h"
#include "MovableVector.h"
#include "MovableMatrix.h"
#include "MovableMatrices.h"
#include "MovableObject.h"
#include "MovableString.h"
#include "MovableStrings.h"
#include "MovableVectors.h"
#include "CommMetaData.h"
#include "PtrCommMetaData.h"
#include "ArrayCommMetaData.h"
#include "MatrixCommMetaData.h"
#include "utility/actor/channel/Channel.h"
#include "material/section/ResponseId.h"
#include "utility/matrix/nDarray/BJtensor.h"

//! @brief Constructor.
XC::CommParameters::CommParameters(int cTag, Channel &theChannel)
  : commitTag(cTag),canal(&theChannel),broker(nullptr) {}

//! @brief Constructor.
XC::CommParameters::CommParameters(int cTag, Channel &theChannel, FEM_ObjectBroker &theBroker)
  : commitTag(cTag),canal(&theChannel),broker(&theBroker) {}

//! @brief Solicita al canal que devuelva un tag para la base de datos.
int XC::CommParameters::getDbTag(void) const
  {
    assert(canal);
    return canal->getDbTag();
  }

//! @brief Devuelve verdadero si el canal es un data store.
bool XC::CommParameters::isDatastore(void) const
  {
    bool retval= false;
    if(canal)
      retval= canal->isDatastore();
    return retval;
  }

//! @brief Envía el vector.
int XC::CommParameters::sendID(const ID &v,const int &dataTag)
  {
    assert(canal);
    return canal->sendID(dataTag,commitTag,v);
  }

//! @brief Recibe el vector.
int XC::CommParameters::receiveID(ID &v,const int &dataTag) const
  {
    assert(canal);
    return canal->recvID(dataTag,commitTag,v);
  }

//! @brief Sends object ID through the channel being passed as parameter.
int XC::CommParameters::sendID(const ID &v,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID mov(v);
    return sendMovable(mov,dt,meta);
  }

//! @brief Receives object ID through the channel being passed as parameter.
int XC::CommParameters::receiveID(ID &v,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID mv;
    int res= receiveMovable(mv,dt,meta);
    v= mv;
    return res;
  }

//! @brief Envía un puntero a ID through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
int XC::CommParameters::sendIDPtr(ID *ptr,DbTagData &dt, const ArrayCommMetaData &meta)
  {
    int retval= 0;
    if(!ptr)
      dt.setDbTagDataPos(meta.getPosFlag(), 1);
    else
      {
        dt.setDbTagDataPos(meta.getPosFlag(), 0);
        dt.setDbTagDataPos(meta.getPosSize(), ptr->Size());
        MovableID mov(*ptr);
        sendMovable(mov,dt,meta);
      }
    return retval;
  }

//! @brief Recibe un puntero a ID through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
XC::ID *XC::CommParameters::receiveIDPtr(ID* &ptr,DbTagData &dt, const ArrayCommMetaData &meta) const
  {
    if(dt.getDbTagDataPos(meta.getPosFlag()) == 0)
      {
        // make some room and read in the ID
        if(!ptr)
          {
            const int n= dt.getDbTagDataPos(meta.getPosSize());
            ptr= new ID(n);
           if(!ptr)
              std::cerr << "receiveIDPtr -- ran out of memory\n";
          }
        if(ptr)
          {
            MovableID mov(*ptr);
            mov.setDbTag(dt.getDbTagDataPos(meta.getPosDbTag()));
            int res= mov.recvSelf(*this);
            if(res < 0)
              std::cerr << "receiveIDPtr - failed to receive ID data\n";
            (*ptr)= static_cast<ID &>(mov);
          }
      }
    return ptr;
  }

//! @brief Envía un puntero a ID through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
int XC::CommParameters::sendMovableIDPtr(MovableID *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return sendMovablePtr(ptr,dt,meta); }


//! @brief Recibe un puntero a ID through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
XC::MovableID *XC::CommParameters::receiveMovableIDPtr(MovableID* &ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return receiveMovablePtr(ptr,dt,meta); }

//! @brief Envía un puntero a vector through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
int XC::CommParameters::sendMovableVectorPtr(MovableVector *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return sendMovablePtr(ptr,dt,meta); }


//! @brief Recibe un puntero a vector through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
XC::MovableVector *XC::CommParameters::receiveMovableVectorPtr(MovableVector* &ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return receiveMovablePtr(ptr,dt,meta); }

//! @brief Envía la matriz through the channel being passed as parameter.
int XC::CommParameters::sendMatrix(const Matrix &v,const int &dataTag)
  {
    assert(canal);
    return canal->sendMatrix(dataTag,commitTag,v);
  }

//! @brief Recibe la matriz through the channel being passed as parameter.
int XC::CommParameters::receiveMatrix(Matrix &v,const int &dataTag) const
  {
    assert(canal);
    return canal->recvMatrix(dataTag,commitTag,v);
  }

//! @brief Envía la matriz through the channel being passed as parameter.
int XC::CommParameters::sendMatrix(const Matrix &m,DbTagData &dt, const CommMetaData &meta)
  {
    MovableMatrix mov(m);
    return sendMovable(mov,dt,meta);
  }

//! @brief Recibe la matriz through the channel being passed as parameter.
int XC::CommParameters::receiveMatrix(Matrix &m,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableMatrix mv(m);
    int res= receiveMovable(mv,dt,meta);
    m= static_cast<const Matrix &>(mv);
    return res;
  }

//! @brief Envía un puntero a la matriz through the channel being passed as parameter.
int XC::CommParameters::sendMatrixPtr(Matrix *ptr,DbTagData &dt, const MatrixCommMetaData &meta)
  {
    int retval= 0;
    const int posFlag= meta.getPosFlag();
    if(!ptr)
      dt.setDbTagDataPos(posFlag, 1);
    else
      {
        dt.setDbTagDataPos(posFlag, 0);
        dt.setDbTagDataPos(meta.getPosNumFilas(), ptr->noRows());
        dt.setDbTagDataPos(meta.getPosNumCols(), ptr->noCols());
        MovableMatrix mov(*ptr);
        retval= sendMovable(mov,dt,meta);
      }
    return retval;
  }

//! @brief Recibe un puntero a matriz through the channel being passed as parameter.
XC::Matrix *XC::CommParameters::receiveMatrixPtr(Matrix* &ptr,DbTagData &dt, const MatrixCommMetaData &meta) const
  {
    if(dt.getDbTagDataPos(meta.getPosFlag()) == 0)
      {
        // make some room and read in the ID
        if(!ptr)
          {
            const int nr= dt.getDbTagDataPos(meta.getPosNumFilas());
            const int nc= dt.getDbTagDataPos(meta.getPosNumCols());
            ptr= new Matrix(nr,nc);
            if(!ptr)
              std::cerr << "receiveMatrixPtr -- ran out of memory\n";
          }
        if(ptr)
          {
            MovableMatrix mov(*ptr);
            mov.setDbTag(dt.getDbTagDataPos(meta.getPosDbTag()));
            int res= mov.recvSelf(*this);
            if(res < 0)
              std::cerr << "receiveIDPtr - failed to receive ID data\n";
            (*ptr)= static_cast<Matrix &>(mov);
          }
      }
    return ptr;
  }

//! @brief Envía un puntero al tensor through the channel being passed as parameter.
int XC::CommParameters::sendTensorPtr(BJtensor *ptr,DbTagData &dt, const TensorCommMetaData &)
  {
    int retval= 0;
    std::cerr << "CommParameters::sendTensorPtr no implementada." << std::endl;
    return retval;
  }

//! @brief Recibe un puntero a matriz through the channel being passed as parameter.
XC::BJtensor *XC::CommParameters::receiveTensorPtr(BJtensor* &ptr,DbTagData &dt, const TensorCommMetaData &) const
  {
    std::cerr << "CommParameters::receiveTensorPtr no implementada." << std::endl;
    return ptr;
  }

//! @brief Envía un puntero a matriz through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
int XC::CommParameters::sendMovableMatrixPtr(MovableMatrix *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return sendMovablePtr(ptr,dt,meta); }

//! @brief Recibe un puntero a matriz through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
XC::MovableMatrix *XC::CommParameters::receiveMovableMatrixPtr(MovableMatrix* &ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return receiveMovablePtr(ptr,dt,meta); }

//! @brief Envía las matrices.
int XC::CommParameters::sendMatrices(const std::vector<Matrix> &matrices,DbTagData &dt, const CommMetaData &meta)
  {
    const size_t sz= matrices.size();

    dt.setDbTagDataPos(0, sz);

    ID dbTags(sz);

    int res= 0;
    int loc= 0;
    for(std::vector<Matrix>::const_iterator i= matrices.begin();i!=matrices.end();i++)
      res+= sendMatrix(*i,loc++);
    res+= sendID(dbTags,dt,meta);

    return res;
  }

//! @brief Recibe las matrices.
int XC::CommParameters::receiveMatrices(std::vector<Matrix> &matrices,DbTagData &dt, const CommMetaData &meta) const
  {
    const int sz= dt.getDbTagDataPos(0);
    ID dbTags(sz);
    int res= receiveID(dbTags,dt,meta);
    matrices.resize(sz);
    int loc= 0;
    for(std::vector<Matrix>::iterator i= matrices.begin();i!=matrices.end();i++)
      res+= receiveMatrix(*i,loc++);
    return res;
  }

//! @brief Envía la cadena de caracteres through the channel being passed as parameter.
int XC::CommParameters::sendString(const std::string &str,DbTagData &dt, const CommMetaData &meta)
  {
    MovableString mv(str);
    return sendMovable(mv,dt,meta);
  }

//! @brief Recibe la cadena de caracteres through the channel being passed as parameter.
int XC::CommParameters::receiveString(std::string &str,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableString mv;
    int res= receiveMovable(mv,dt,meta);
    str= mv.getString();
    return res;
  }

//! @brief Envía el tensor through the channel being passed as parameter.
int XC::CommParameters::sendTensor(const BJtensor &t,DbTagData &dt, const CommMetaData &meta)
  {
    MovableBJTensor mv(t);
    return sendMovable(mv,dt,meta);
  }

//! @brief Recibe el tensor through the channel being passed as parameter.
int XC::CommParameters::receiveTensor(BJtensor &t,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableBJTensor mv;
    int res= receiveMovable(mv,dt,meta);
    t= mv;
    return res;
  }

int XC::CommParameters::sendStrings(std::deque<std::string> &strings,DbTagData &dt, const CommMetaData &meta)
  {
    MovableStrings mv(strings);
    return sendMovable(mv,dt,meta);
  }

int XC::CommParameters::receiveStrings(std::deque<std::string> &strings,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableStrings mv(strings);
    int res= receiveMovable(mv,dt,meta);
    strings= mv.getStrings();
    return res;
  }

int XC::CommParameters::sendStrings(std::vector<std::string> &strings,DbTagData &dt, const CommMetaData &meta)
  {
    const size_t sz= strings.size();
    std::deque<std::string> tmp(sz);
    for(size_t i=0;i<sz;i++)
      tmp[i]= strings[i];
    return sendStrings(tmp,dt,meta);
  }

int XC::CommParameters::receiveStrings(std::vector<std::string> &strings,DbTagData &dt, const CommMetaData &meta) const
  {
    std::deque<std::string> tmp;
    int res= receiveStrings(tmp,dt,meta);
    const size_t sz= tmp.size();
    for(size_t i=0;i<sz;i++)
      strings[i]= tmp[i];
    return res;
  }

//! @brief Envía el miembro data through the channel being passed as parameter.
int XC::CommParameters::sendIdData(const DbTagData &data,const int &dataTag)
  { return data.sendIdData(*this,dataTag); }

//! @brief Recibe el miembro data through the channel being passed as parameter.
int XC::CommParameters::receiveIdData(DbTagData &data,const int &dataTag) const
  { return data.receiveIdData(*this,dataTag); }

//! @brief Envía el vector.
int XC::CommParameters::sendVector(const Vector &v,const int &dataTag)
  {
    assert(canal);
    return canal->sendVector(dataTag,commitTag,v);
  }

//! @brief Recibe el vector.
int XC::CommParameters::receiveVector(Vector &v,const int &dataTag) const
  {
    assert(canal);
    return canal->recvVector(dataTag,commitTag,v);
  }

//! @brief Envía el vector.
int XC::CommParameters::sendVector(const Vector &v,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector mov(v);
    return sendMovable(mov,dt,meta);
  }

//! @brief Recibe el vector.
int XC::CommParameters::receiveVector(Vector &v,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector mv(v);
    int res= receiveMovable(mv,dt,meta);
    v= static_cast<const Vector &>(mv);
    return res;
  }

//! @brief Envía el vector.
int XC::CommParameters::sendVector(const std::vector<double> &v,DbTagData &dt, const CommMetaData &meta)
  { return sendVector(Vector(v),dt, meta); }

//! @brief Recibe el vector.
int XC::CommParameters::receiveVector(std::vector<double> &v,DbTagData &dt, const CommMetaData &meta) const
  {
    static Vector tmp;
    int res= receiveVector(tmp,dt,meta);
    v= vector_to_std_vector(tmp);
    return res;
  }

int XC::CommParameters::sendVectors(std::vector<Vector> &vectores,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVectors mv(vectores);
    return sendMovable(mv,dt,meta);
  }

int XC::CommParameters::receiveVectors(std::vector<Vector> &vectores,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVectors mv(vectores);
    int res= receiveMovable(mv,dt,meta);
    vectores= mv.getVectores();
    return res;
  }

//! @brief Envía un puntero al vector through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
int XC::CommParameters::sendVectorPtr(Vector *ptr,DbTagData &dt, const ArrayCommMetaData &meta)
  {
    int retval= 0;
    if(!ptr)
      dt.setDbTagDataPos(meta.getPosFlag(), 1);
    else
      {
        dt.setDbTagDataPos(meta.getPosFlag(), 0);
        dt.setDbTagDataPos(meta.getPosSize(), ptr->Size());
        MovableVector mov(*ptr);
        retval= sendMovable(mov,dt,meta);
      }
    return retval;
  }

//! @brief Recibe un puntero a matriz through the channel being passed as parameter.
//! @param posFlag: Posición de ID en la que se guarda el flag.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
XC::Vector *XC::CommParameters::receiveVectorPtr(Vector* &ptr,DbTagData &dt, const ArrayCommMetaData &meta) const
  {
    if(dt.getDbTagDataPos(meta.getPosFlag()) == 0)
      {
        // make some room and read in the ID
        if(!ptr)
          {
            const int n= dt.getDbTagDataPos(meta.getPosSize());
            ptr= new Vector(n);
            if(!ptr)
              std::cerr << "receiveVectorPtr -- ran out of memory\n";
          }
        if(ptr)
          {
            MovableVector mov(*ptr);
            int res= receiveMovable(mov,dt,meta);
            if(res < 0)
              std::cerr << "receiveIDPtr - failed to receive ID data\n";
            (*ptr)= static_cast<Vector &>(mov);
          }
      }
    return ptr;
  }

//! @brief Envía los valores que contiene el array through the channel being passed as parameter.
int XC::CommParameters::sendDoublePtr(double *ptr,DbTagData &dt, const ArrayCommMetaData &meta)
  {
    int retval= 0;
    if(!ptr)
      dt.setDbTagDataPos(meta.getPosFlag(), 1);
    else
      {
        dt.setDbTagDataPos(meta.getPosFlag(), 0);
        Vector v(ptr,meta.getSize());
        retval= sendVector(v,dt,meta);
      }
    return retval;
  }

//! @brief Recib los valores que contiene el array through the channel being passed as parameter.
double *XC::CommParameters::receiveDoublePtr(double* &ptr,DbTagData &dt, const ArrayCommMetaData &meta) const
  {
    if(dt.getDbTagDataPos(meta.getPosFlag()) == 0)
      {
        // make some room and read in the vector
        if(!ptr)
          {
            ptr= new double[meta.getSize()];
            if(!ptr)
              std::cerr << "receiveDoublePtr -- ran out of memory\n";
          }
        if(ptr)
          {
            MovableVector v(meta.getSize());
            int res= receiveMovable(v,dt,meta);
            if(res < 0)
              std::cerr << "receiveDoublePtr - failed to receive vector data\n";
            for(size_t i=0;i<meta.getSize();i++)
              ptr[i]= v[i];
          }
      }
    return ptr;
  }


//! @brief Envía el double through the channel being passed as parameter.
int XC::CommParameters::sendDouble(const double &db1,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(1);
    v(0)= db1;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los doubles through the channel being passed as parameter.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(2);
    v(0)= db1; v(1)= db2;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los doubles through the channel being passed as parameter.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,const double &db3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(3);
    v(0)= db1; v(1)= db2; v(2)= db3;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los doubles through the channel being passed as parameter.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,const double &db3,const double &db4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(4);
    v(0)= db1; v(1)= db2; v(2)= db3; v(3)= db4;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los doubles through the channel being passed as parameter.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,const double &db3,const double &db4,const double &db5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(5);
    v(0)= db1; v(1)= db2; v(2)= db3; v(3)= db4; v(4)= db5;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los doubles through the channel being passed as parameter.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,const double &db3,const double &db4,const double &db5,const double &db6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(6);
    v(0)= db1; v(1)= db2; v(2)= db3; v(3)= db4; v(4)= db5; v(5)= db6;
    return sendMovable(v,dt,meta);
  }

//! @brief Recibe el double through the channel being passed as parameter.
int XC::CommParameters::receiveDouble(double &db1,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(1);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDouble - failed to receive vector data\n";
    db1= v[0];
    return res;
  }

//! @brief Recibe los doubles through the channel being passed as parameter.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1];
    return res;
  }

//! @brief Recibe los doubles through the channel being passed as parameter.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,double &db3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2];
    return res;
  }


//! @brief Recibe los doubles through the channel being passed as parameter.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,double &db3,double &db4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2]; db4= v[3];
    return res;
  }

//! @brief Recibe los doubles through the channel being passed as parameter.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,double &db3,double &db4,double &db5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2]; db4= v[3]; db5= v[4];
    return res;
  }

//! @brief Recibe los doubles through the channel being passed as parameter.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,double &db3,double &db4,double &db5,double &db6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2]; db4= v[3]; db5= v[4]; db6= v[5];
    return res;
  }


//! @brief Envía el bool through the channel being passed as parameter.
int XC::CommParameters::sendBool(const bool &b,DbTagData &dt, const CommMetaData &meta)
  { return sendInt(b,dt,meta); }

int XC::CommParameters::receiveBool(bool &b,DbTagData &dt, const CommMetaData &meta) const
  {
    int tmp= b;
    int res= receiveInt(tmp,dt,meta);
    b= tmp;
    return res;
  }

//! @brief Envía el entero through the channel being passed as parameter.
int XC::CommParameters::sendInt(const int &i,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(1);
    v(0)= i;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los enteros through the channel being passed as parameter.
int XC::CommParameters::sendInts(const int &i1,const int &i2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(2);
    v(0)= i1; v(1)= i2;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los enteros through the channel being passed as parameter.
int XC::CommParameters::sendInts(const int &i1,const int &i2,const int &i3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(3);
    v(0)= i1; v(1)= i2; v(2)= i3;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los enteros through the channel being passed as parameter.
int XC::CommParameters::sendInts(const int &i1,const int &i2,const int &i3,const int &i4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(4);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los enteros through the channel being passed as parameter.
int XC::CommParameters::sendInts(const int &i1,const int &i2,const int &i3,const int &i4,const int &i5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(5);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los enteros through the channel being passed as parameter.
int XC::CommParameters::sendInts(const int &i1,const int &i2,const int &i3,const int &i4,const int &i5,const int &i6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(6);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5; v(5)= i6;
    return sendMovable(v,dt,meta);
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
int XC::CommParameters::receiveInt(int &i,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(1);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDouble - failed to receive vector data\n";
    i= v[0];
    return res;
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
int XC::CommParameters::receiveInts(int &i1,int &i2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1];
    return res;
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
int XC::CommParameters::receiveInts(int &i1,int &i2,int &i3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2];
    return res;
  }


//! @brief Recibe los enteros through the channel being passed as parameter.
int XC::CommParameters::receiveInts(int &i1,int &i2,int &i3,int &i4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3];
    return res;
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
int XC::CommParameters::receiveInts(int &i1,int &i2,int &i3,int &i4,int &i5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4];
    return res;
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
int XC::CommParameters::receiveInts(int &i1,int &i2,int &i3,int &i4,int &i5,int &i6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4]; i6= v[5];
    return res;
  }

//! @brief Envía el entero through the channel being passed as parameter.
size_t XC::CommParameters::sendSzt(const size_t &i,DbTagData &dt, const CommMetaData &meta)
  { return sendDouble(i,dt,meta); }

//! @brief Envía los enteros through the channel being passed as parameter.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(2);
    v(0)= i1; v(1)= i2;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los enteros through the channel being passed as parameter.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(3);
    v(0)= i1; v(1)= i2; v(2)= i3;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los enteros through the channel being passed as parameter.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,const size_t &i4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(4);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los enteros through the channel being passed as parameter.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,const size_t &i4,const size_t &i5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(5);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía los enteros through the channel being passed as parameter.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,const size_t &i4,const size_t &i5,const size_t &i6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(6);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5; v(5)= i6;
    return sendMovable(v,dt,meta);
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
size_t XC::CommParameters::receiveSzt(size_t &i,DbTagData &dt, const CommMetaData &meta) const
  {
    double tmp= i;
    int res= receiveDouble(tmp,dt,meta);
    i= tmp;
    return res;
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1];
    return res;
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,size_t &i3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2];
    return res;
  }


//! @brief Recibe los enteros through the channel being passed as parameter.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,size_t &i3,size_t &i4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3];
    return res;
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,size_t &i3,size_t &i4,size_t &i5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4];
    return res;
  }

//! @brief Recibe los enteros through the channel being passed as parameter.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,size_t &i3,size_t &i4,size_t &i5,size_t &i6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4]; i6= v[5];
    return res;
  }



XC::ResponseId *XC::CommParameters::receiveResponseIdPtr(ResponseId* &ri,DbTagData &dt, const ArrayCommMetaData &m) const
  {
    ID *tmp=nullptr;
    tmp= receiveIDPtr(tmp,dt,m);
    if(tmp)
      ri= new ResponseId(*tmp);
    delete tmp;
    return ri;
  }

//! @brief Sends object de tipo móvil through the channel being passed as parameter.
int XC::CommParameters::sendMovable(MovableObject &mv,DbTagData &dt, const CommMetaData &meta)
  {
    mv.setDbTag(*this); //Nos aseguramos de que se le asigna un dbTag.
    int retval= mv.sendSelf(*this);
    dt.setDbTagDataPos(meta.getPosDbTag(), mv.getDbTag());
    return retval;
  }

//! @brief Receives object de tipo móvil through the channel being passed as parameter.
int XC::CommParameters::receiveMovable(MovableObject &mv,DbTagData &dt, const CommMetaData &meta) const
  {
    mv.setDbTag(dt.getDbTagDataPos(meta.getPosDbTag()));
    int res= mv.recvSelf(*this);
    if(res < 0)
      std::cerr << "receiveMovable - failed to receive movable data\n";
    return res;
  }

//! @brief Envía un puntero a un objeto móvil through the channel being passed as parameter.
//! @param meta: Datos posiciones en las que se guardan el flag y el dbTag
int XC::CommParameters::sendMovablePtr(MovableObject *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  {
    int retval= 0;
    if(!ptr)
      dt.setDbTagDataPos(meta.getPosFlag(), 1);
    else
      {
        dt.setDbTagDataPos(meta.getPosFlag(), 0);
        retval= sendMovable(*ptr,dt,meta);
      }
    return retval;
  }


//! @brief Envía un puntero a un objeto móvil through the channel being passed as parameter.
//! @param meta: Datos posiciones en las que se guardan el flag, el dbTag y el classTag
int XC::CommParameters::sendBrokedPtr(MovableObject *ptr,DbTagData &dt, const BrokedPtrCommMetaData &meta)
  {
    int retval= 0;
    if(ptr)
      {
        dt.setDbTagDataPos(meta.getPosClassTag(), ptr->getClassTag()); 
        retval= sendMovable(*ptr,dt,meta);
      }
    return retval;
  }

XC::ConstraintHandler *XC::CommParameters::brokeConstraintHandler(const int &classTag) const
  { return broker->getNewConstraintHandler(classTag); }

XC::DOF_Numberer *XC::CommParameters::brokeNumberer(const int &classTag) const
  { return broker->getNewNumberer(classTag); }

XC::AnalysisModel *XC::CommParameters::brokeAnalysisModel(const int &classTag) const
  { return broker->getNewAnalysisModel(classTag); }

XC::LinearSOE *XC::CommParameters::brokeDDLinearSOE(const int &classTagSOE,const int &classTagSolver) const
  { return broker->getPtrNewDDLinearSOE(classTagSOE,classTagSolver); }

XC::LinearSOE *XC::CommParameters::brokeLinearSOE(const int &classTagSOE,const int &classTagSolver) const
  { return broker->getNewLinearSOE(classTagSOE,classTagSolver); }

XC::IncrementalIntegrator *XC::CommParameters::brokeIncrementalIntegrator(const int &classTag) const
  { return broker->getNewIncrementalIntegrator(classTag); }

XC::StaticIntegrator *XC::CommParameters::brokeStaticIntegrator(const int &classTag) const
  { return broker->getNewStaticIntegrator(classTag); }

XC::TransientIntegrator *XC::CommParameters::brokeTransientIntegrator(const int &classTag) const
  { return broker->getNewTransientIntegrator(classTag); }

XC::DomainDecompAlgo *XC::CommParameters::brokeDomainDecompAlgo(const int &classTag) const
  { return broker->getNewDomainDecompAlgo(classTag); }

XC::EquiSolnAlgo *XC::CommParameters::brokeEquiSolnAlgo(const int &classTag) const
  { return broker->getNewEquiSolnAlgo(classTag); }

XC::GroundMotion *XC::CommParameters::brokeGroundMotion(const int &classTag) const
  { return broker->getNewGroundMotion(classTag); }

XC::DomainSolver *XC::CommParameters::getNewDomainSolver(void) const
  {
    assert(broker);
    return broker->getNewDomainSolver();
  }

XC::DomainDecompositionAnalysis *XC::CommParameters::getNewDomainDecompAnalysis(int classTag, Subdomain &theDomain) const
  {
    assert(broker);
    return broker->getNewDomainDecompAnalysis(classTag,theDomain);
  }

XC::ConvergenceTest *XC::CommParameters::getNewConvergenceTest(EntCmd *owr,int classTag) const
  {
    assert(broker);
    return broker->getNewConvergenceTest(owr,classTag);
  }

XC::TimeSeries *XC::CommParameters::getNewTimeSeries(int classTag) const
  {
    assert(broker);
    return broker->getNewTimeSeries(classTag);
  }

XC::CrdTransf2d *XC::CommParameters::getNewCrdTransf2d(int classTag) const
  {
    assert(broker);
    return broker->getNewCrdTransf2d(classTag);
  }

XC::CrdTransf3d *XC::CommParameters::getNewCrdTransf3d(int classTag) const
  {
    assert(broker);
    return broker->getNewCrdTransf3d(classTag);
  }

XC::CrdTransf *XC::CommParameters::getNewCrdTransf(int classTag) const
  {
    assert(broker);
    return broker->getNewCrdTransf(classTag);
  }

XC::ShellCrdTransf3dBase *XC::CommParameters::getNewShellCrdTransf3d(int classTag) const
  {
    assert(broker);
    return broker->getNewShellCrdTransf3d(classTag);
  }

XC::BeamIntegration *XC::CommParameters::getNewBeamIntegration(int classTag) const
  {
    assert(broker);
    return broker->getNewBeamIntegration(classTag);
  }
