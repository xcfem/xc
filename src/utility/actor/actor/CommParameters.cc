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

//! @brief Solicita al canal que devuelva un tag para la database.
int XC::CommParameters::getDbTag(void) const
  {
    assert(canal);
    return canal->getDbTag();
  }

//! @brief Returns true if it's a data store.
bool XC::CommParameters::isDatastore(void) const
  {
    bool retval= false;
    if(canal)
      retval= canal->isDatastore();
    return retval;
  }

//! @brief Sends vector.
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

//! @brief Sends an ID object through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendID(const ID &v,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID mov(v);
    return sendMovable(mov,dt,meta);
  }

//! @brief Receives an ID objet through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveID(ID &v,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID mv;
    int res= receiveMovable(mv,dt,meta);
    v= mv;
    return res;
  }

//! @brief Sends an ID pointer through the communicator being passed as parameter.
//! @param ptr: ID pointer to transmit.
//! @param meta: index where the object dbTag and size are stored.
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

//! @brief Receives an ID pointer through the communicator being passed as parameter.
//! @param ptr: ID pointer to receive.
//! @param meta: index where the object dbTag and size are stored.
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

//! @brief Sends a MovableID pointer through the channel being passed as parameter.
//! @param ptr: MovableID pointer to send.
//! @param meta: indexes where the data are stored. 
int XC::CommParameters::sendMovableIDPtr(MovableID *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return sendMovablePtr(ptr,dt,meta); }


//! @brief Receives a MovableID pointer through the channel being passed as parameter.
//! @param ptr: MovableID pointer to receive.
//! @param meta: indexes where the data are stored. 
XC::MovableID *XC::CommParameters::receiveMovableIDPtr(MovableID* &ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return receiveMovablePtr(ptr,dt,meta); }

//! @brief Sends a MovableVector pointer through the channel being passed as parameter.
//! @param ptr: MovableVector pointer to send.
//! @param meta: indexes where the data are stored. 
int XC::CommParameters::sendMovableVectorPtr(MovableVector *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return sendMovablePtr(ptr,dt,meta); }


//! @brief Receives a MovableVector pointer through the channel being passed as parameter.
//! @param ptr: MovableVector pointer to receive.
//! @param meta: indexes where the data are stored. 
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
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendMatrix(const Matrix &m,DbTagData &dt, const CommMetaData &meta)
  {
    MovableMatrix mov(m);
    return sendMovable(mov,dt,meta);
  }

//! @brief Recibe la matriz through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveMatrix(Matrix &m,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableMatrix mv(m);
    int res= receiveMovable(mv,dt,meta);
    m= static_cast<const Matrix &>(mv);
    return res;
  }

//! @brief Envía a pointer a la matriz through the channel being passed as parameter.
//! @param meta: index where the object dbTag, number of rows and number of columns are stored.
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

//! @brief Recibe a pointer a matriz through the channel being passed as parameter.
//! @param meta: index where the object dbTag, number of rows and number of columns are stored.
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

//! @brief Envía a pointer al tensor through the channel being passed as parameter.
int XC::CommParameters::sendTensorPtr(BJtensor *ptr,DbTagData &dt, const TensorCommMetaData &)
  {
    int retval= 0;
    std::cerr << "CommParameters::sendTensorPtr not implemented." << std::endl;
    return retval;
  }

//! @brief Recibe a pointer a matriz through the channel being passed as parameter.
XC::BJtensor *XC::CommParameters::receiveTensorPtr(BJtensor* &ptr,DbTagData &dt, const TensorCommMetaData &) const
  {
    std::cerr << "CommParameters::receiveTensorPtr not implemented." << std::endl;
    return ptr;
  }

//! @brief Sends a MovableMatrix pointer through the channel being passed as parameter.
//! @param ptr: MovableMatrix pointer to send.
//! @param meta: indexes where the data are stored. 
int XC::CommParameters::sendMovableMatrixPtr(MovableMatrix *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return sendMovablePtr(ptr,dt,meta); }

//! @brief Receives a MovableMatrix pointer through the channel being passed as parameter.
//! @param ptr: MovableMatrix pointer to receive.
//! @param meta: indexes where the data are stored. 
XC::MovableMatrix *XC::CommParameters::receiveMovableMatrixPtr(MovableMatrix* &ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return receiveMovablePtr(ptr,dt,meta); }

//! @brief Sends the matrices.
//! @param meta: index where the object dbTag is stored.
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
//! @param meta: index where the object dbTag is stored.
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
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendString(const std::string &str,DbTagData &dt, const CommMetaData &meta)
  {
    MovableString mv(str);
    return sendMovable(mv,dt,meta);
  }

//! @brief Recibe la cadena de caracteres through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveString(std::string &str,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableString mv;
    int res= receiveMovable(mv,dt,meta);
    str= mv.getString();
    return res;
  }

//! @brief Sends tensor through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendTensor(const BJtensor &t,DbTagData &dt, const CommMetaData &meta)
  {
    MovableBJTensor mv(t);
    return sendMovable(mv,dt,meta);
  }

//! @brief Recibe el tensor through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveTensor(BJtensor &t,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableBJTensor mv;
    int res= receiveMovable(mv,dt,meta);
    t= mv;
    return res;
  }

//! @brief Sends the string container through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendStrings(std::deque<std::string> &strings,DbTagData &dt, const CommMetaData &meta)
  {
    MovableStrings mv(strings);
    return sendMovable(mv,dt,meta);
  }

//! @brief Receives the string container through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveStrings(std::deque<std::string> &strings,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableStrings mv(strings);
    int res= receiveMovable(mv,dt,meta);
    strings= mv.getStrings();
    return res;
  }

//! @brief Sends the string container through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendStrings(std::vector<std::string> &strings,DbTagData &dt, const CommMetaData &meta)
  {
    const size_t sz= strings.size();
    std::deque<std::string> tmp(sz);
    for(size_t i=0;i<sz;i++)
      tmp[i]= strings[i];
    return sendStrings(tmp,dt,meta);
  }

//! @brief Receives the string container through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveStrings(std::vector<std::string> &strings,DbTagData &dt, const CommMetaData &meta) const
  {
    std::deque<std::string> tmp;
    int res= receiveStrings(tmp,dt,meta);
    const size_t sz= tmp.size();
    for(size_t i=0;i<sz;i++)
      strings[i]= tmp[i];
    return res;
  }

//! @brief Sends miembro data through the channel being passed as parameter.
int XC::CommParameters::sendIdData(const DbTagData &data,const int &dataTag)
  { return data.sendIdData(*this,dataTag); }

//! @brief Recibe el miembro data through the channel being passed as parameter.
int XC::CommParameters::receiveIdData(DbTagData &data,const int &dataTag) const
  { return data.receiveIdData(*this,dataTag); }

//! @brief Sends vector.
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

//! @brief Sends vector.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendVector(const Vector &v,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector mov(v);
    return sendMovable(mov,dt,meta);
  }

//! @brief Recibe el vector.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveVector(Vector &v,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector mv(v);
    int res= receiveMovable(mv,dt,meta);
    v= static_cast<const Vector &>(mv);
    return res;
  }

//! @brief Sends vector.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendVector(const std::vector<double> &v,DbTagData &dt, const CommMetaData &meta)
  { return sendVector(Vector(v),dt, meta); }

//! @brief Recibe el vector.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveVector(std::vector<double> &v,DbTagData &dt, const CommMetaData &meta) const
  {
    static Vector tmp;
    int res= receiveVector(tmp,dt,meta);
    v= vector_to_std_vector(tmp);
    return res;
  }

//! @brief Sends the vector container through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendVectors(std::vector<Vector> &vectores,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVectors mv(vectores);
    return sendMovable(mv,dt,meta);
  }

//! @brief Receives the vector container through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveVectors(std::vector<Vector> &vectores,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVectors mv(vectores);
    int res= receiveMovable(mv,dt,meta);
    vectores= mv.getVectores();
    return res;
  }

//! @brief Sends the Vector pointed by ptr through the channel being passed as parameter.
//! @param ptr: pointer to the vector to send.
//! @param meta: index where the object dbTag and size are stored.
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

//! @brief Receives a Vector pointed by ptr through the channel being passed as parameter.
//! @param ptr: pointer to the vector to receive.
//! @param meta: index where the object dbTag and size are stored.
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
//! @param meta: index where the object dbTag and size are stored.
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
//! @param meta: index where the object dbTag and size are stored.
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


//! @brief Sends a double through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendDouble(const double &db1,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(1);
    v(0)= db1;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(2);
    v(0)= db1; v(1)= db2;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,const double &db3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(3);
    v(0)= db1; v(1)= db2; v(2)= db3;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,const double &db3,const double &db4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(4);
    v(0)= db1; v(1)= db2; v(2)= db3; v(3)= db4;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,const double &db3,const double &db4,const double &db5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(5);
    v(0)= db1; v(1)= db2; v(2)= db3; v(3)= db4; v(4)= db5;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendDoubles(const double &db1,const double &db2,const double &db3,const double &db4,const double &db5,const double &db6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(6);
    v(0)= db1; v(1)= db2; v(2)= db3; v(3)= db4; v(4)= db5; v(5)= db6;
    return sendMovable(v,dt,meta);
  }

//! @brief Recibe el double through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveDouble(double &db1,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(1);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDouble - failed to receive vector data\n";
    db1= v[0];
    return res;
  }

//! @brief Recibe the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1];
    return res;
  }

//! @brief Recibe the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,double &db3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2];
    return res;
  }


//! @brief Recibe the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,double &db3,double &db4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2]; db4= v[3];
    return res;
  }

//! @brief Recibe the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,double &db3,double &db4,double &db5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2]; db4= v[3]; db5= v[4];
    return res;
  }

//! @brief Recibe the doubles through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveDoubles(double &db1,double &db2,double &db3,double &db4,double &db5,double &db6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDoubles - failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2]; db4= v[3]; db5= v[4]; db6= v[5];
    return res;
  }


//! @brief Sends bool through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendBool(const bool &b,DbTagData &dt, const CommMetaData &meta)
  { return sendInt(b,dt,meta); }

//! @brief Sends the bool through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveBool(bool &b,DbTagData &dt, const CommMetaData &meta) const
  {
    int tmp= b;
    int res= receiveInt(tmp,dt,meta);
    b= tmp;
    return res;
  }

//! @brief Sends the integer through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendInt(const int &i,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(1);
    v(0)= i;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendInts(const int &i1,const int &i2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(2);
    v(0)= i1; v(1)= i2;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendInts(const int &i1,const int &i2,const int &i3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(3);
    v(0)= i1; v(1)= i2; v(2)= i3;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendInts(const int &i1,const int &i2,const int &i3,const int &i4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(4);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendInts(const int &i1,const int &i2,const int &i3,const int &i4,const int &i5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(5);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendInts(const int &i1,const int &i2,const int &i3,const int &i4,const int &i5,const int &i6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(6);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5; v(5)= i6;
    return sendMovable(v,dt,meta);
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveInt(int &i,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(1);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveDouble - failed to receive vector data\n";
    i= v[0];
    return res;
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveInts(int &i1,int &i2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1];
    return res;
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveInts(int &i1,int &i2,int &i3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2];
    return res;
  }


//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveInts(int &i1,int &i2,int &i3,int &i4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3];
    return res;
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveInts(int &i1,int &i2,int &i3,int &i4,int &i5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4];
    return res;
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveInts(int &i1,int &i2,int &i3,int &i4,int &i5,int &i6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveInts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4]; i6= v[5];
    return res;
  }

//! @brief Sends entero through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::sendSzt(const size_t &i,DbTagData &dt, const CommMetaData &meta)
  { return sendDouble(i,dt,meta); }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(2);
    v(0)= i1; v(1)= i2;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(3);
    v(0)= i1; v(1)= i2; v(2)= i3;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,const size_t &i4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(4);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,const size_t &i4,const size_t &i5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(5);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5;
    return sendMovable(v,dt,meta);
  }

//! @brief Envía the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,const size_t &i4,const size_t &i5,const size_t &i6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(6);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5; v(5)= i6;
    return sendMovable(v,dt,meta);
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::receiveSzt(size_t &i,DbTagData &dt, const CommMetaData &meta) const
  {
    double tmp= i;
    int res= receiveDouble(tmp,dt,meta);
    i= tmp;
    return res;
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1];
    return res;
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,size_t &i3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2];
    return res;
  }


//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,size_t &i3,size_t &i4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3];
    return res;
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,size_t &i3,size_t &i4,size_t &i5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4];
    return res;
  }

//! @brief Recibe the integers through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
size_t XC::CommParameters::receiveSzts(size_t &i1,size_t &i2,size_t &i3,size_t &i4,size_t &i5,size_t &i6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "receiveSzts - failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4]; i6= v[5];
    return res;
  }



//! @brief Receives a ResponseId object
//! @param meta: index where the object dbTag and size are stored.
XC::ResponseId *XC::CommParameters::receiveResponseIdPtr(ResponseId* &ri,DbTagData &dt, const ArrayCommMetaData &m) const
  {
    ID *tmp=nullptr;
    tmp= receiveIDPtr(tmp,dt,m);
    if(tmp)
      ri= new ResponseId(*tmp);
    delete tmp;
    return ri;
  }

//! @brief Sends a movable object through the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::sendMovable(MovableObject &mv,DbTagData &dt, const CommMetaData &meta)
  {
    mv.setDbTag(*this); //Nos aseguramos de que se le asigna un dbTag.
    int retval= mv.sendSelf(*this);
    dt.setDbTagDataPos(meta.getPosDbTag(), mv.getDbTag());
    return retval;
  }

//! @brief Receives a movable object trhrough the channel being passed as parameter.
//! @param meta: index where the object dbTag is stored.
int XC::CommParameters::receiveMovable(MovableObject &mv,DbTagData &dt, const CommMetaData &meta) const
  {
    mv.setDbTag(dt.getDbTagDataPos(meta.getPosDbTag()));
    int res= mv.recvSelf(*this);
    if(res < 0)
      std::cerr << "receiveMovable - failed to receive movable data\n";
    return res;
  }

//! @brief Sends a pointer to movable object through the channel being passed as parameter.
//! @param meta: indexes where the flag and the dbTag are stored.
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


//! @brief Envía a pointer to movable object through the channel being passed as parameter.
//! @param meta: indexes where the flag and the dbTag are stored.
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
