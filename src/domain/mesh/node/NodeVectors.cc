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
//NodeVectors.cpp

#include <domain/mesh/node/NodeVectors.h>
#include <utility/tagged/TaggedObject.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>

#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

void XC::NodeVectors::libera(void)
  {
    // delete anything that we created with new
    if(commitData) delete commitData;
    commitData= nullptr;
    if(trialData) delete trialData;
    trialData= nullptr;
    if(data) delete [] data;
    data= nullptr;
  }

void XC::NodeVectors::copia(const NodeVectors &otro)
  {
    libera();
    numVectors= otro.numVectors;
    if(otro.commitData)
      {
        const size_t nDOF= otro.getVectorsSize();
        const size_t sz= numVectors*nDOF;
        if(this->createData(nDOF) < 0)
          {
            std::cerr << " FATAL NodeVectors::Node(node *) - ran out of memory for data\n";
            exit(-1);
          }
        for(register size_t i=0;i<sz;i++)
          data[i] = otro.data[i];
      }
  }

//! @brief Constructor.
XC::NodeVectors::NodeVectors(const size_t &nv)
  :EntCmd(),MovableObject(NOD_TAG_NodeVectors), numVectors(nv), commitData(nullptr),trialData(nullptr),data(nullptr) {}


//! @brief Constructor de copia.
XC::NodeVectors::NodeVectors(const NodeVectors &otro)
  : EntCmd(otro),MovableObject(NOD_TAG_NodeVectors), numVectors(otro.numVectors), commitData(nullptr), trialData(nullptr), data(nullptr)
  { copia(otro); }

XC::NodeVectors &XC::NodeVectors::operator=(const NodeVectors &otro)
  {
    EntCmd::operator=(otro);
    MovableObject::operator=(otro);
    copia(otro);
    return *this;
  }

//! @brief destructor
XC::NodeVectors::~NodeVectors(void)
  { libera(); }

//! @brief Return the number of node DOFs.
size_t XC::NodeVectors::getVectorsSize(void) const
  {
    if(commitData)
      return commitData->Size();
    else
      return 0;
  }

//! @brief Returns the vector de datos.
const XC::Vector &XC::NodeVectors::getData(const size_t &nDOF) const
  {
    if(!commitData)
      {
        NodeVectors *this_no_const= const_cast<NodeVectors *>(this);
        if(this_no_const->createData(nDOF) < 0)
          {
            std::cerr << "FATAL XC::NodeVectors::getData() -- ran out of memory\n";
            exit(-1);
          }
      }
    // return the committed data
    return *commitData;
  }
  
//! @brief Returns the valores "consumados"
const XC::Vector &XC::NodeVectors::getCommitData(void) const
  {
    assert(commitData);
    return *commitData;
  }

//! @brief Returns the vector de datos de prueba.
const XC::Vector &XC::NodeVectors::getTrialData(const size_t &nDOF) const
  {
    if(!trialData)
      {
        NodeVectors *this_no_const= const_cast<NodeVectors *>(this);
        if(this_no_const->createData(nDOF) < 0)
          {
            std::cerr << "FATAL XC::NodeVectors::getTrialData() -- ran out of memory\n";
            exit(-1);
          }
      }
    return *trialData;
  }

//! @brief Returns the valores de prueba
const XC::Vector &XC::NodeVectors::getTrialData(void) const
  {
    assert(trialData);
    return *trialData;
  }

int XC::NodeVectors::setTrialData(const size_t &nDOF,const double &value,const size_t &dof)
  {
    // check vector arg is of correct size
    if(dof < 0 || dof >= nDOF)
      {
        std::cerr << "WARNING NodeVectors::setTrialData() - incompatible sizes\n";
        return -2;
      }

    // perform the assignment .. we dont't go through Vector interface
    // as we are sure of size and this way is quicker
    if(data)
      data[dof]= value;
    return 0;
  }

//! @brief Asigna el valor being passed as parameter
int XC::NodeVectors::setTrialData(const size_t &nDOF,const Vector &newTrialData)
  {
    // check vector arg is of correct size
    if(size_t(newTrialData.Size()) != nDOF)
      {
        std::cerr << "WARNING NodeVectors::setTrialData() - incompatible sizes\n";
        return -2;
      }

    // construct memory and Vectors for trial and committed
    // accel on first call to this method, getTrialData(),
    // getData(), or incrTrialData()
    if(!data)
      {
        if(this->createData(nDOF) < 0)
          {
            std::cerr << "FATAL NodeVectors::setTrialData() - ran out of memory\n";
            exit(-1);
          }
      }

    // perform the assignment .. we dont't go through XC::Vector interface
    // as we are sure of size and this way is quicker
    for(size_t i=0;i<nDOF;i++)
      data[i]= newTrialData(i);
    return 0;
  }


int XC::NodeVectors::incrTrialData(const size_t &nDOF,const Vector &incrData)
  {
    // check vector arg is of correct size
    if(size_t(incrData.Size()) != nDOF)
      {
        std::cerr << "WARNING XC::NodeVectors::incrTrialData() - incompatable sizes\n";
        return -2;
      }

    // create a copy if no trial exists andd add committed
    if(!data)
      {
        if(this->createData(nDOF) < 0)
          {
            std::cerr << "FATAL XC::NodeVectors::incrTrialData() - ran out of memory\n";
            exit(-1);
          }
      }
    // set trial = incr + trial
    for(size_t i= 0;i<nDOF;i++)
      data[i]+= incrData(i);
    return 0;
  }

int XC::NodeVectors::commitState(const size_t &nDOF)
  {
    // check data exists, if does set commit = trial, incr = 0.0
    if(trialData)
      {
        for(register size_t i=0; i<nDOF; i++)
          data[i+nDOF] = data[i];
      }
    return 0;
  }


//! @brief Returns the vectores al estado correspondiente al último commit.
int XC::NodeVectors::revertToLastCommit(const size_t &nDOF)
  {
    // check data exists, if does set trial = last commit, incr = 0
    if(data)
      {
        for(size_t i=0;i<nDOF;i++)
          data[i] = data[nDOF+i];
      }
    return 0;
  }


//! @brief Returns the nodo a su estado inicial.
int XC::NodeVectors::revertToStart(const size_t &nDOF)
  {
    // check data exists, if does set all to zero
    const size_t sz= numVectors*nDOF;
    if(data)
      {
        for(size_t i=0;i<sz;i++)
          data[i]= 0.0;
      }
    return 0;
  }


//! @brief private method to create the arrays to hold the data
//! values and the Vector objects for the committed and trial quantities.
int XC::NodeVectors::createData(const size_t &nDOF)
  {
    libera();
    // trial , committed, incr = (committed-trial)
    const size_t sz= numVectors*nDOF;
    data= new double[sz];

    if(data)
      {
        for(size_t i=0;i<sz;i++)
          data[i]= 0.0;

        trialData= new Vector(&data[0], nDOF);
        commitData= new Vector(&data[nDOF], nDOF);

        if(!commitData || !trialData)
          {
            std::cerr << "WARNING - XC::NodeVectors::createData() "
                  << "ran out of memory creating Vectors(double *,int)";
            return -2;
          }
        return 0;
      }
    else
      {
        std::cerr << "WARNING - XC::NodeVectors::createData() ran out of memory for array of size " << sz << std::endl;
        return -1;
      }
  }

//! @brief Returns a vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::NodeVectors::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }
//! @brief Envia los miembros del objeto through the channel being passed as parameter.
int XC::NodeVectors::sendData(CommParameters &cp)
  {
    int res= 0;
    ID datos(3);
    if(!commitData)
      datos(0) = 1;
    else
      {
        datos(0)= 0;
        datos(1)= cp.getDbTag();
        datos(2)= commitData->Size();
        res+= cp.sendVector(*commitData,datos(1));
        if(res < 0)
          {
            std::cerr << "NodeVectors::sendSelf() - failed to send Disp data\n";
            return res;
          }
      }
    res+= cp.sendID(datos,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::NodeVectors::recvData(const CommParameters &cp)
  {
    ID datos(3);
    int res= cp.receiveID(datos,getDbTagData(),CommMetaData(1));


    const int dbTag1= datos(1);
    const int nDOF= datos(2);

    if(datos(0) == 0)
      {
        // create the disp vectors if node is a total blank
        createData(nDOF);
        // recv the data
        if(cp.receiveVector(*commitData,dbTag1) < 0)
          {
            std::cerr << "NodeVectors::recvSelf - failed to receive data\n";
            return res;
          }

        // set the trial quantities equal to committed
        for(int i=0; i<nDOF; i++)
          data[i] = data[i+nDOF];  // set trial equal commited
      }
    else if(commitData)
      {
        // if going back to initial we will just zero the vectors
        commitData->Zero();
        trialData->Zero();
      }
    return res;
  }

//! @brief Envia los vectores through the channel being passed as parameter.E
int XC::NodeVectors::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::NodeVectors::recvSelf(const CommParameters &cp)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

