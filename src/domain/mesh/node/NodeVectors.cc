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

void XC::NodeVectors::free_mem(void)
  {
    // delete anything that we created with new
    if(commitData) delete commitData;
    commitData= nullptr;
    if(trialData) delete trialData;
    trialData= nullptr;
  }

void XC::NodeVectors::copy(const NodeVectors &other)
  {
    free_mem();
    numVectors= other.numVectors;
    if(other.commitData)
      {
        const size_t nDOF= other.getVectorsSize();
        const size_t sz= numVectors*nDOF;
        if(this->createData(nDOF) < 0)
          {
            std::cerr << " FATAL NodeVectors::Node(node *) - ran out of memory for data\n";
            exit(-1);
          }
        for(register size_t i=0;i<sz;i++)
          values[i] = other.values[i];
      }
  }

//! @brief Constructor.
XC::NodeVectors::NodeVectors(const size_t &nv)
  :CommandEntity(),MovableObject(NOD_TAG_NodeVectors), numVectors(nv), commitData(nullptr),trialData(nullptr), values() {}


//! @brief Copy constructor.
XC::NodeVectors::NodeVectors(const NodeVectors &other)
  : CommandEntity(other),MovableObject(NOD_TAG_NodeVectors), numVectors(other.numVectors), commitData(nullptr), trialData(nullptr), values()
  { copy(other); }

XC::NodeVectors &XC::NodeVectors::operator=(const NodeVectors &other)
  {
    CommandEntity::operator=(other);
    MovableObject::operator=(other);
    copy(other);
    return *this;
  }

//! @brief destructor
XC::NodeVectors::~NodeVectors(void)
  { free_mem(); }

//! @brief Return the number of node DOFs.
size_t XC::NodeVectors::getVectorsSize(void) const
  {
    if(commitData)
      return commitData->Size();
    else
      return 0;
  }

//! @brief Returns the data vector.
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
  
//! @brief Returns committed values.
const XC::Vector &XC::NodeVectors::getCommitData(void) const
  {
    assert(commitData);
    return *commitData;
  }

//! @brief Returns the data vector de prueba.
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

//! @brief Returns trial values.
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

    // perform the assignment .. we don't go through Vector interface
    // as we are sure of size and this way is quicker
    if(!values.isEmpty())
      values[dof]= value;
    return 0;
  }

//! @brief Set trial values.
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
    if(values.isEmpty())
      {
        if(this->createData(nDOF) < 0)
          {
            std::cerr << "FATAL NodeVectors::setTrialData() - ran out of memory\n";
            exit(-1);
          }
      }

    // perform the assignment .. we don't go through XC::Vector interface
    // as we are sure of size and this way is quicker
    for(size_t i=0;i<nDOF;i++)
      values[i]= newTrialData(i);
    return 0;
  }


int XC::NodeVectors::incrTrialData(const size_t &nDOF,const Vector &incrData)
  {
    // check vector arg is of correct size
    if(size_t(incrData.Size()) != nDOF)
      {
        std::cerr << "WARNING XC::NodeVectors::incrTrialData() - incompatible sizes\n";
        return -2;
      }

    // create a copy if no trial exists and add committed
    if(values.isEmpty())
      {
        if(this->createData(nDOF) < 0)
          {
            std::cerr << "FATAL XC::NodeVectors::incrTrialData() - ran out of memory\n";
            exit(-1);
          }
      }
    // set trial = incr + trial
    for(size_t i= 0;i<nDOF;i++)
      values[i]+= incrData(i);
    return 0;
  }

//! @brief Commit state.
int XC::NodeVectors::commitState(const size_t &nDOF)
  {
    // check data exists, if does set commit = trial, incr = 0.0
    if(trialData)
      {
        for(register size_t i=0; i<nDOF; i++)
          values[i+nDOF] = values[i];
      }
    return 0;
  }


//! @brief Returns the vectors its last committed value.
int XC::NodeVectors::revertToLastCommit(const size_t &nDOF)
  {
    // check data exists, if does set trial = last commit, incr = 0
    if(!values.isEmpty())
      {
        for(size_t i=0;i<nDOF;i++)
          values[i] = values[nDOF+i];
      }
    return 0;
  }


//! @brief Returns the node a su estado inicial.
int XC::NodeVectors::revertToStart(const size_t &nDOF)
  {
    // check data exists, if does set all to zero
    const size_t sz= numVectors*nDOF;
    if(!values.isEmpty())
      {
        for(size_t i=0;i<sz;i++)
          values[i]= 0.0;
      }
    return 0;
  }


//! @brief private method to create the arrays to hold the data
//! values and the Vector objects for the committed and trial quantities.
int XC::NodeVectors::createData(const size_t &nDOF)
  {
    free_mem();
    // trial , committed, incr = (committed-trial)
    const size_t sz= numVectors*nDOF;
    values= Vector(sz);

    if(!values.isEmpty())
      {
        for(size_t i=0;i<sz;i++)
          values[i]= 0.0;

        trialData= new Vector(&values[0], nDOF);
        commitData= new Vector(&values[nDOF], nDOF);

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

//! @brief Returns a vector to store the dbTags
//! de los miembros of the clase.
XC::DbTagData &XC::NodeVectors::getDbTagData(void) const
  {
    static DbTagData retval(2);
    return retval;
  }
//! @brief Sends object members through the communicator argument.
int XC::NodeVectors::sendData(Communicator &comm)
  {
    int res= 0;
    ID idData(3);
    if(!commitData)
      idData(0) = 1;
    else
      {
        idData(0)= 0;
        idData(1)= comm.getDbTag();
        idData(2)= commitData->Size();
        res+= comm.sendVector(*commitData,idData(1));
        if(res < 0)
          {
            std::cerr << "NodeVectors::sendSelf() - failed to send Disp data\n";
            return res;
          }
      }
    res+= comm.sendID(idData,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::NodeVectors::recvData(const Communicator &comm)
  {
    ID idData(3);
    int res= comm.receiveID(idData,getDbTagData(),CommMetaData(1));


    const int dbTag1= idData(1);
    const int nDOF= idData(2);

    if(idData(0) == 0)
      {
        // create the disp vectors if node is a total blank
        createData(nDOF);
        // recv the data
        if(comm.receiveVector(*commitData,dbTag1) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
                      << "; - failed to receive data\n";
            return res;
          }

        // set the trial quantities equal to committed
        for(int i=0; i<nDOF; i++)
          values[i]= values[i+nDOF]; // set trial equal committed
      }
    else if(commitData)
      {
        // if going back to initial we will just zero the vectors
        commitData->Zero();
        trialData->Zero();
      }
    return res;
  }

//! @brief Sends the vector through the communicator.
int XC::NodeVectors::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(2);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::NodeVectors::recvSelf(const Communicator &comm)
  {
    inicComm(2);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

