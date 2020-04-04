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
//Communicator.cc

#include "Communicator.h"
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
XC::Communicator::Communicator(int cTag, Channel &theChannel)
  : commitTag(cTag),canal(&theChannel),broker(nullptr) {}

//! @brief Constructor.
XC::Communicator::Communicator(int cTag, Channel &theChannel, FEM_ObjectBroker &theBroker)
  : commitTag(cTag),canal(&theChannel),broker(&theBroker) {}

//! @brief Ask the channel for a tag for the database.
int XC::Communicator::getDbTag(void) const
  {
    assert(canal);
    return canal->getDbTag();
  }

//! @brief Returns true if it's a data store.
bool XC::Communicator::isDatastore(void) const
  {
    bool retval= false;
    if(canal)
      retval= canal->isDatastore();
    return retval;
  }

//! @brief Sends vector.
int XC::Communicator::sendID(const ID &v,const int &dataTag)
  {
    assert(canal);
    return canal->sendID(dataTag,commitTag,v);
  }

//! @brief Receives el vector.
int XC::Communicator::receiveID(ID &v,const int &dataTag) const
  {
    assert(canal);
    return canal->recvID(dataTag,commitTag,v);
  }

//! @brief Sends an ID object through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendID(const ID &v,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID mov(v);
    return sendMovable(mov,dt,meta);
  }

//! @brief Receives an ID object through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveID(ID &v,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID mv;
    int res= receiveMovable(mv,dt,meta);
    v= mv;
    return res;
  }

//! @brief Sends an ID pointer through the communicator being passed as parameter.
//!
//! @param ptr: ID pointer to transmit.
//! @param meta: index where the object dbTag and size are stored.
int XC::Communicator::sendIDPtr(ID *ptr,DbTagData &dt, const ArrayCommMetaData &meta)
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
//!
//! @param ptr: ID pointer to receive.
//! @param meta: index where the object dbTag and size are stored.
XC::ID *XC::Communicator::receiveIDPtr(ID* &ptr,DbTagData &dt, const ArrayCommMetaData &meta) const
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
              std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive ID data\n";
            (*ptr)= static_cast<ID &>(mov);
          }
      }
    return ptr;
  }

//! @brief Sends a MovableID pointer through the communicator argument.
//! @param ptr: MovableID pointer to send.
//! @param meta: indexes where the data are stored. 
int XC::Communicator::sendMovableIDPtr(MovableID *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return sendMovablePtr(ptr,dt,meta); }


//! @brief Receives a MovableID pointer through the communicator argument.
//! @param ptr: MovableID pointer to receive.
//! @param meta: indexes where the data are stored. 
XC::MovableID *XC::Communicator::receiveMovableIDPtr(MovableID* &ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return receiveMovablePtr(ptr,dt,meta); }

//! @brief Sends a MovableVector pointer through the communicator argument.
//! @param ptr: MovableVector pointer to send.
//! @param meta: indexes where the data are stored. 
int XC::Communicator::sendMovableVectorPtr(MovableVector *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return sendMovablePtr(ptr,dt,meta); }


//! @brief Receives a MovableVector pointer through the communicator argument.
//! @param ptr: MovableVector pointer to receive.
//! @param meta: indexes where the data are stored. 
XC::MovableVector *XC::Communicator::receiveMovableVectorPtr(MovableVector* &ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return receiveMovablePtr(ptr,dt,meta); }

//! @brief Send the matrix through the communicator argument.
int XC::Communicator::sendMatrix(const Matrix &v,const int &dataTag)
  {
    assert(canal);
    return canal->sendMatrix(dataTag,commitTag,v);
  }

//! @brief Receives the matrix through the communicator argument.
int XC::Communicator::receiveMatrix(Matrix &v,const int &dataTag) const
  {
    assert(canal);
    return canal->recvMatrix(dataTag,commitTag,v);
  }

//! @brief Send the matrix through the communicator argument
//!.
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendMatrix(const Matrix &m,DbTagData &dt, const CommMetaData &meta)
  {
    MovableMatrix mov(m);
    return sendMovable(mov,dt,meta);
  }

//! @brief Receives the matrix through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveMatrix(Matrix &m,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableMatrix mv(m);
    int res= receiveMovable(mv,dt,meta);
    m= static_cast<const Matrix &>(mv);
    return res;
  }

//! @brief Send a pointer to the matrix through the communicator argument.
//!
//! @param meta: index where the object dbTag, number of rows and number of columns are stored.
int XC::Communicator::sendMatrixPtr(Matrix *ptr,DbTagData &dt, const MatrixCommMetaData &meta)
  {
    int retval= 0;
    const int posFlag= meta.getPosFlag();
    if(!ptr)
      dt.setDbTagDataPos(posFlag, 1);
    else
      {
        dt.setDbTagDataPos(posFlag, 0);
        dt.setDbTagDataPos(meta.getPositionNumberOfRows(), ptr->noRows());
        dt.setDbTagDataPos(meta.getPositionNumberOfColumns(), ptr->noCols());
        MovableMatrix mov(*ptr);
        retval= sendMovable(mov,dt,meta);
      }
    return retval;
  }

//! @brief Receives a pointer to the matrix through the communicator argument.
//!
//! @param meta: index where the object dbTag, number of rows and number of columns are stored.
XC::Matrix *XC::Communicator::receiveMatrixPtr(Matrix* &ptr,DbTagData &dt, const MatrixCommMetaData &meta) const
  {
    if(dt.getDbTagDataPos(meta.getPosFlag()) == 0)
      {
        // make some room and read in the ID
        if(!ptr)
          {
            const int nr= dt.getDbTagDataPos(meta.getPositionNumberOfRows());
            const int nc= dt.getDbTagDataPos(meta.getPositionNumberOfColumns());
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
              std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive ID data\n";
            (*ptr)= static_cast<Matrix &>(mov);
          }
      }
    return ptr;
  }

//! @brief Send a pointer to the tensor through the communicator argument.
int XC::Communicator::sendTensorPtr(BJtensor *ptr,DbTagData &dt, const TensorCommMetaData &)
  {
    int retval= 0;
    std::cerr << "Communicator::sendTensorPtr not implemented." << std::endl;
    return retval;
  }

//! @brief Receives a pointer to tensor through the communicator argument.
XC::BJtensor *XC::Communicator::receiveTensorPtr(BJtensor* &ptr,DbTagData &dt, const TensorCommMetaData &) const
  {
    std::cerr << "Communicator::receiveTensorPtr not implemented." << std::endl;
    return ptr;
  }

//! @brief Sends a MovableMatrix pointer through the communicator argument.
//! @param ptr: MovableMatrix pointer to send.
//! @param meta: indexes where the data are stored. 
int XC::Communicator::sendMovableMatrixPtr(MovableMatrix *ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return sendMovablePtr(ptr,dt,meta); }

//! @brief Receives a MovableMatrix pointer through the communicator argument.
//! @param ptr: MovableMatrix pointer to receive.
//! @param meta: indexes where the data are stored. 
XC::MovableMatrix *XC::Communicator::receiveMovableMatrixPtr(MovableMatrix* &ptr,DbTagData &dt, const PtrCommMetaData &meta)
  { return receiveMovablePtr(ptr,dt,meta); }

//! @brief Sends the matrices.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendMatrices(const std::vector<Matrix> &matrices,DbTagData &dt, const CommMetaData &meta)
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

//! @brief Receives las matrices.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveMatrices(std::vector<Matrix> &matrices,DbTagData &dt, const CommMetaData &meta) const
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

//! @brief Send the text string through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendString(const std::string &str,DbTagData &dt, const CommMetaData &meta)
  {
    MovableString mv(str);
    return sendMovable(mv,dt,meta);
  }

//! @brief Receives la text string through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveString(std::string &str,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableString mv;
    int res= receiveMovable(mv,dt,meta);
    str= mv.getString();
    return res;
  }

//! @brief Sends tensor through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendTensor(const BJtensor &t,DbTagData &dt, const CommMetaData &meta)
  {
    MovableBJTensor mv(t);
    return sendMovable(mv,dt,meta);
  }

//! @brief Receives el tensor through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveTensor(BJtensor &t,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableBJTensor mv;
    int res= receiveMovable(mv,dt,meta);
    t= mv;
    return res;
  }

//! @brief Sends the string container through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendStrings(std::deque<std::string> &strings,DbTagData &dt, const CommMetaData &meta)
  {
    MovableStrings mv(strings);
    return sendMovable(mv,dt,meta);
  }

//! @brief Receives the string container through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveStrings(std::deque<std::string> &strings,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableStrings mv(strings);
    int res= receiveMovable(mv,dt,meta);
    strings= mv.getStrings();
    return res;
  }

//! @brief Sends the string container through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendStrings(std::vector<std::string> &strings,DbTagData &dt, const CommMetaData &meta)
  {
    const size_t sz= strings.size();
    std::deque<std::string> tmp(sz);
    for(size_t i=0;i<sz;i++)
      tmp[i]= strings[i];
    return sendStrings(tmp,dt,meta);
  }

//! @brief Receives the string container through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveStrings(std::vector<std::string> &strings,DbTagData &dt, const CommMetaData &meta) const
  {
    std::deque<std::string> tmp;
    int res= receiveStrings(tmp,dt,meta);
    const size_t sz= tmp.size();
    for(size_t i=0;i<sz;i++)
      strings[i]= tmp[i];
    return res;
  }

//! @brief Sends miembro data through the communicator argument.
int XC::Communicator::sendIdData(const DbTagData &data,const int &dataTag)
  { return data.sendIdData(*this,dataTag); }

//! @brief Receives el miembro data through the communicator argument.
int XC::Communicator::receiveIdData(DbTagData &data,const int &dataTag) const
  { return data.receiveIdData(*this,dataTag); }

//! @brief Sends vector.
int XC::Communicator::sendVector(const Vector &v,const int &dataTag)
  {
    assert(canal);
    return canal->sendVector(dataTag,commitTag,v);
  }

//! @brief Receives el vector.
int XC::Communicator::receiveVector(Vector &v,const int &dataTag) const
  {
    assert(canal);
    return canal->recvVector(dataTag,commitTag,v);
  }

//! @brief Sends vector.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendVector(const Vector &v,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector mov(v);
    return sendMovable(mov,dt,meta);
  }

//! @brief Receives el vector.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveVector(Vector &v,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector mv(v);
    int res= receiveMovable(mv,dt,meta);
    v= static_cast<const Vector &>(mv);
    return res;
  }

//! @brief Sends vector.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendVector(const std::vector<double> &v,DbTagData &dt, const CommMetaData &meta)
  { return sendVector(Vector(v),dt, meta); }

//! @brief Receives el vector.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveVector(std::vector<double> &v,DbTagData &dt, const CommMetaData &meta) const
  {
    static Vector tmp;
    int res= receiveVector(tmp,dt,meta);
    v= vector_to_std_vector(tmp);
    return res;
  }

//! @brief Sends the vector container through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendVectors(std::vector<Vector> &vectors,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVectors mv(vectors);
    return sendMovable(mv,dt,meta);
  }

//! @brief Receives the vector container through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveVectors(std::vector<Vector> &vectors,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVectors mv(vectors);
    int res= receiveMovable(mv,dt,meta);
    vectors= mv.getVectores();
    return res;
  }

//! @brief Sends the Vector pointed by ptr through the communicator argument.
//! @param ptr: pointer to the vector to send.
//! @param meta: index where the object dbTag and size are stored.
int XC::Communicator::sendVectorPtr(Vector *ptr,DbTagData &dt, const ArrayCommMetaData &meta)
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

//! @brief Receives a Vector pointed by ptr through the communicator argument.
//! @param ptr: pointer to the vector to receive.
//! @param meta: index where the object dbTag and size are stored.
XC::Vector *XC::Communicator::receiveVectorPtr(Vector* &ptr,DbTagData &dt, const ArrayCommMetaData &meta) const
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
              std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive ID data\n";
            (*ptr)= static_cast<Vector &>(mov);
          }
      }
    return ptr;
  }

//! @brief Send the array data through the communicator argument.
//!
//! @param meta: index where the object dbTag and size are stored.
int XC::Communicator::sendDoublePtr(double *ptr,DbTagData &dt, const ArrayCommMetaData &meta)
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

//! @brief Receive the array data through the communicator argument.
//!
//! @param meta: index where the object dbTag and size are stored.
double *XC::Communicator::receiveDoublePtr(double* &ptr,DbTagData &dt, const ArrayCommMetaData &meta) const
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
              std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
            for(size_t i=0;i<meta.getSize();i++)
              ptr[i]= v[i];
          }
      }
    return ptr;
  }


//! @brief Sends a double through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendDouble(const double &db1,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(1);
    v(0)= db1;
    return sendMovable(v,dt,meta);
  }

//! @brief Send the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendDoubles(const double &db1,const double &db2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(2);
    v(0)= db1; v(1)= db2;
    return sendMovable(v,dt,meta);
  }

//! @brief Send the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendDoubles(const double &db1,const double &db2,const double &db3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(3);
    v(0)= db1; v(1)= db2; v(2)= db3;
    return sendMovable(v,dt,meta);
  }

//! @brief Send the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendDoubles(const double &db1,const double &db2,const double &db3,const double &db4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(4);
    v(0)= db1; v(1)= db2; v(2)= db3; v(3)= db4;
    return sendMovable(v,dt,meta);
  }

//! @brief Send the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendDoubles(const double &db1,const double &db2,const double &db3,const double &db4,const double &db5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(5);
    v(0)= db1; v(1)= db2; v(2)= db3; v(3)= db4; v(4)= db5;
    return sendMovable(v,dt,meta);
  }

//! @brief Send the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendDoubles(const double &db1,const double &db2,const double &db3,const double &db4,const double &db5,const double &db6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableVector v(6);
    v(0)= db1; v(1)= db2; v(2)= db3; v(3)= db4; v(4)= db5; v(5)= db6;
    return sendMovable(v,dt,meta);
  }

//! @brief Receives el double through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveDouble(double &db1,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(1);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    db1= v[0];
    return res;
  }

//! @brief Receives the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveDoubles(double &db1,double &db2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    db1= v[0]; db2= v[1];
    return res;
  }

//! @brief Receives the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveDoubles(double &db1,double &db2,double &db3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2];
    return res;
  }


//! @brief Receives the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveDoubles(double &db1,double &db2,double &db3,double &db4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2]; db4= v[3];
    return res;
  }

//! @brief Receives the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveDoubles(double &db1,double &db2,double &db3,double &db4,double &db5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2]; db4= v[3]; db5= v[4];
    return res;
  }

//! @brief Receives the doubles through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveDoubles(double &db1,double &db2,double &db3,double &db4,double &db5,double &db6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableVector v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    db1= v[0]; db2= v[1]; db3= v[2]; db4= v[3]; db5= v[4]; db6= v[5];
    return res;
  }


//! @brief Sends bool through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendBool(const bool &b,DbTagData &dt, const CommMetaData &meta)
  { return sendInt(b,dt,meta); }

//! @brief Sends the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendBools(const bool &b1,const bool &b2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(2);
    v(0)= b1; v(1)= b2;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendBools(const bool &b1,const bool &b2,const bool &b3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(3);
    v(0)= b1; v(1)= b2; v(2)= b3;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendBools(const bool &b1,const bool &b2,const bool &b3,const bool &b4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(4);
    v(0)= b1; v(1)= b2; v(2)= b3; v(3)= b4;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendBools(const bool &b1,const bool &b2,const bool &b3,const bool &b4,const bool &b5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(5);
    v(0)= b1; v(1)= b2; v(2)= b3; v(3)= b4; v(4)= b5;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendBools(const bool &b1,const bool &b2,const bool &b3,const bool &b4,const bool &b5,const bool &b6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(6);
    v(0)= b1; v(1)= b2; v(2)= b3; v(3)= b4; v(4)= b5; v(5)= b6;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the bool through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveBool(bool &b,DbTagData &dt, const CommMetaData &meta) const
  {
    int tmp= b;
    int res= receiveInt(tmp,dt,meta);
    b= tmp;
    return res;
  }

//! @brief Receives the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveBools(bool &b1,bool &b2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    b1= v[0]; b2= v[1];
    return res;
  }

//! @brief Receives the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveBools(bool &b1,bool &b2,bool &b3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    b1= v[0]; b2= v[1]; b3= v[2];
    return res;
  }


//! @brief Receives the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveBools(bool &b1,bool &b2,bool &b3,bool &b4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    b1= v[0]; b2= v[1]; b3= v[2]; b4= v[3];
    return res;
  }

//! @brief Receives the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveBools(bool &b1,bool &b2,bool &b3,bool &b4,bool &b5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    b1= v[0]; b2= v[1]; b3= v[2]; b4= v[3]; b5= v[4];
    return res;
  }

//! @brief Receives the bools through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveBools(bool &b1,bool &b2,bool &b3,bool &b4,bool &b5,bool &b6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    b1= v[0]; b2= v[1]; b3= v[2]; b4= v[3]; b5= v[4]; b6= v[5];
    return res;
  }

//! @brief Sends the integer through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendInt(const int &i,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(1);
    v(0)= i;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendInts(const int &i1,const int &i2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(2);
    v(0)= i1; v(1)= i2;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendInts(const int &i1,const int &i2,const int &i3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(3);
    v(0)= i1; v(1)= i2; v(2)= i3;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendInts(const int &i1,const int &i2,const int &i3,const int &i4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(4);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendInts(const int &i1,const int &i2,const int &i3,const int &i4,const int &i5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(5);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendInts(const int &i1,const int &i2,const int &i3,const int &i4,const int &i5,const int &i6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(6);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5; v(5)= i6;
    return sendMovable(v,dt,meta);
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveInt(int &i,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(1);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i= v[0];
    return res;
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveInts(int &i1,int &i2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i1= v[0]; i2= v[1];
    return res;
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveInts(int &i1,int &i2,int &i3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2];
    return res;
  }


//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveInts(int &i1,int &i2,int &i3,int &i4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3];
    return res;
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveInts(int &i1,int &i2,int &i3,int &i4,int &i5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4];
    return res;
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveInts(int &i1,int &i2,int &i3,int &i4,int &i5,int &i6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4]; i6= v[5];
    return res;
  }

//! @brief Sends entero through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::sendSzt(const size_t &i,DbTagData &dt, const CommMetaData &meta)
  { return sendDouble(i,dt,meta); }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::sendSzts(const size_t &i1,const size_t &i2,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(2);
    v(0)= i1; v(1)= i2;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(3);
    v(0)= i1; v(1)= i2; v(2)= i3;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,const size_t &i4,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(4);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,const size_t &i4,const size_t &i5,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(5);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5;
    return sendMovable(v,dt,meta);
  }

//! @brief Sends the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::sendSzts(const size_t &i1,const size_t &i2,const size_t &i3,const size_t &i4,const size_t &i5,const size_t &i6,DbTagData &dt, const CommMetaData &meta)
  {
    MovableID v(6);
    v(0)= i1; v(1)= i2; v(2)= i3; v(3)= i4; v(4)= i5; v(5)= i6;
    return sendMovable(v,dt,meta);
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::receiveSzt(size_t &i,DbTagData &dt, const CommMetaData &meta) const
  {
    double tmp= i;
    int res= receiveDouble(tmp,dt,meta);
    i= tmp;
    return res;
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::receiveSzts(size_t &i1,size_t &i2,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(2);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i1= v[0]; i2= v[1];
    return res;
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::receiveSzts(size_t &i1,size_t &i2,size_t &i3,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(3);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2];
    return res;
  }


//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::receiveSzts(size_t &i1,size_t &i2,size_t &i3,size_t &i4,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(4);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3];
    return res;
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::receiveSzts(size_t &i1,size_t &i2,size_t &i3,size_t &i4,size_t &i5,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(5);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__ << "; failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4];
    return res;
  }

//! @brief Receives the integers through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
size_t XC::Communicator::receiveSzts(size_t &i1,size_t &i2,size_t &i3,size_t &i4,size_t &i5,size_t &i6,DbTagData &dt, const CommMetaData &meta) const
  {
    MovableID v(6);
    int res= receiveMovable(v,dt,meta);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__
		<< "; failed to receive vector data\n";
    i1= v[0]; i2= v[1]; i3= v[2]; i4= v[3]; i5= v[4]; i6= v[5];
    return res;
  }



//! @brief Receives a ResponseId object
//!
//! @param meta: index where the object dbTag and size are stored.
XC::ResponseId *XC::Communicator::receiveResponseIdPtr(ResponseId* &ri,DbTagData &dt, const ArrayCommMetaData &m) const
  {
    ID *tmp=nullptr;
    tmp= receiveIDPtr(tmp,dt,m);
    if(tmp)
      ri= new ResponseId(*tmp);
    delete tmp;
    return ri;
  }

//! @brief Sends a movable object through the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::sendMovable(MovableObject &mv,DbTagData &dt, const CommMetaData &meta)
  {
    mv.setDbTag(*this); //Verify that a tag is assigned.
    int retval= mv.sendSelf(*this);
    dt.setDbTagDataPos(meta.getPosDbTag(), mv.getDbTag());
    return retval;
  }

//! @brief Receives a movable object trhrough the communicator argument.
//!
//! @param meta: index where the object dbTag is stored.
int XC::Communicator::receiveMovable(MovableObject &mv,DbTagData &dt, const CommMetaData &meta) const
  {
    mv.setDbTag(dt.getDbTagDataPos(meta.getPosDbTag()));
    int res= mv.recvSelf(*this);
    if(res < 0)
      std::cerr << "Communicator::" << __FUNCTION__
		<< "; failed to receive movable data\n";
    return res;
  }

//! @brief Sends a pointer to movable object through the communicator argument.
//!
//! @param meta: indexes where the flag and the dbTag are stored.
int XC::Communicator::sendMovablePtr(MovableObject *ptr,DbTagData &dt, const PtrCommMetaData &meta)
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


//! @brief Sends a pointer to movable object through the communicator argument.
//!
//! @param meta: indexes where the flag and the dbTag are stored.
int XC::Communicator::sendBrokedPtr(MovableObject *ptr,DbTagData &dt, const BrokedPtrCommMetaData &meta)
  {
    int retval= 0;
    if(ptr)
      {
        dt.setDbTagDataPos(meta.getPosClassTag(), ptr->getClassTag()); 
        retval= sendMovable(*ptr,dt,meta);
      }
    return retval;
  }

XC::ConstraintHandler *XC::Communicator::brokeConstraintHandler(const int &classTag) const
  { return broker->getNewConstraintHandler(classTag); }

XC::DOF_Numberer *XC::Communicator::brokeNumberer(const int &classTag) const
  { return broker->getNewNumberer(classTag); }

XC::AnalysisModel *XC::Communicator::brokeAnalysisModel(const int &classTag) const
  { return broker->getNewAnalysisModel(classTag); }

XC::LinearSOE *XC::Communicator::brokeDDLinearSOE(const int &classTagSOE,const int &classTagSolver) const
  { return broker->getPtrNewDDLinearSOE(classTagSOE,classTagSolver); }

XC::LinearSOE *XC::Communicator::brokeLinearSOE(const int &classTagSOE,const int &classTagSolver) const
  { return broker->getNewLinearSOE(classTagSOE,classTagSolver); }

XC::IncrementalIntegrator *XC::Communicator::brokeIncrementalIntegrator(const int &classTag) const
  { return broker->getNewIncrementalIntegrator(classTag); }

XC::StaticIntegrator *XC::Communicator::brokeStaticIntegrator(const int &classTag) const
  { return broker->getNewStaticIntegrator(classTag); }

XC::TransientIntegrator *XC::Communicator::brokeTransientIntegrator(const int &classTag) const
  { return broker->getNewTransientIntegrator(classTag); }

XC::DomainDecompAlgo *XC::Communicator::brokeDomainDecompAlgo(const int &classTag) const
  { return broker->getNewDomainDecompAlgo(classTag); }

XC::EquiSolnAlgo *XC::Communicator::brokeEquiSolnAlgo(const int &classTag) const
  { return broker->getNewEquiSolnAlgo(classTag); }

XC::GroundMotion *XC::Communicator::brokeGroundMotion(const int &classTag) const
  { return broker->getNewGroundMotion(classTag); }

XC::DomainSolver *XC::Communicator::getNewDomainSolver(void) const
  {
    assert(broker);
    return broker->getNewDomainSolver();
  }

XC::DomainDecompositionAnalysis *XC::Communicator::getNewDomainDecompAnalysis(int classTag, Subdomain &theDomain) const
  {
    assert(broker);
    return broker->getNewDomainDecompAnalysis(classTag,theDomain);
  }

XC::ConvergenceTest *XC::Communicator::getNewConvergenceTest(CommandEntity *owr,int classTag) const
  {
    assert(broker);
    return broker->getNewConvergenceTest(owr,classTag);
  }

XC::TimeSeries *XC::Communicator::getNewTimeSeries(int classTag) const
  {
    assert(broker);
    return broker->getNewTimeSeries(classTag);
  }

XC::CrdTransf2d *XC::Communicator::getNewCrdTransf2d(int classTag) const
  {
    assert(broker);
    return broker->getNewCrdTransf2d(classTag);
  }

XC::CrdTransf3d *XC::Communicator::getNewCrdTransf3d(int classTag) const
  {
    assert(broker);
    return broker->getNewCrdTransf3d(classTag);
  }

XC::CrdTransf *XC::Communicator::getNewCrdTransf(int classTag) const
  {
    assert(broker);
    return broker->getNewCrdTransf(classTag);
  }

XC::ShellCrdTransf3dBase *XC::Communicator::getNewShellCrdTransf3d(int classTag) const
  {
    assert(broker);
    return broker->getNewShellCrdTransf3d(classTag);
  }

XC::BeamIntegration *XC::Communicator::getNewBeamIntegration(int classTag) const
  {
    assert(broker);
    return broker->getNewBeamIntegration(classTag);
  }
