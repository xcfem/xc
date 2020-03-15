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
//NodeDispVectors.cpp

#include <domain/mesh/node/NodeDispVectors.h>
#include <utility/tagged/TaggedObject.h>
#include <utility/matrix/Vector.h>



void XC::NodeDispVectors::free_mem(void)
  {
    NodeVectors::free_mem();
    // delete anything that we created with new
    if(incrDisp) delete incrDisp;
    incrDisp= nullptr;
    if(incrDeltaDisp) delete incrDeltaDisp;
    incrDeltaDisp= nullptr;
  }

//! @brief Constructor.
XC::NodeDispVectors::NodeDispVectors(void)
  :NodeVectors(4),incrDisp(nullptr),incrDeltaDisp(nullptr) {}


//! @brief Copy constructor.
XC::NodeDispVectors::NodeDispVectors(const NodeDispVectors &other)
  : NodeVectors(other), incrDisp(nullptr), incrDeltaDisp(nullptr)
  { copy(other); }

XC::NodeDispVectors &XC::NodeDispVectors::operator=(const NodeDispVectors &other)
  {
    NodeVectors::operator=(other);
    return *this;
  }

//! @brief destructor
XC::NodeDispVectors::~NodeDispVectors(void)
  { free_mem(); }

//! @brief Returns displacement increment.
//! @param nDOF: number of degrees of freedom
const XC::Vector &XC::NodeDispVectors::getIncrDisp(const size_t &nDOF) const
  {
    if(!incrDisp)
      {
        NodeDispVectors *this_no_const= const_cast<NodeDispVectors *>(this);
        if(this_no_const->createDisp(nDOF) < 0)
          {
            std::cerr << "FATAL NodeDispVectors::getTrialDisp() -- ran out of memory\n";
            exit(-1);
          }
      }
    return *incrDisp;
  }

//! @brief Returns delta displacement increment.
//! @param nDOF: number of degrees of freedom
const XC::Vector &XC::NodeDispVectors::getIncrDeltaDisp(const size_t &nDOF) const
  {
    if(!incrDeltaDisp)
      {
        NodeDispVectors *this_no_const= const_cast<NodeDispVectors *>(this);
        if(this_no_const->createDisp(nDOF) < 0)
          {
            std::cerr << "FATAL NodeDispVectors::getTrialDisp() -- ran out of memory\n";
            exit(-1);
          }
      }
    return *incrDeltaDisp;
  }

//! @brief Sets trial values for the displacement components.
//! @param nDOF: number of degrees of freedom.
//! @param dof: component of the displacement to set.
int XC::NodeDispVectors::setTrialDispComponent(const size_t &nDOF,const double &value,const size_t &dof)
  {
    // check vector arg is of correct size
    if(dof < 0 || dof >= nDOF)
      {
        std::cerr << "WARNING NodeDispVectors::setTrialDispComponent() - incompatible sizes\n";
        return -2;
      }

    // construct memory and Vectors for trial and committed
    // accel on first call to this method, getTrialDisp(),
    // getDisp(), or incrTrialDisp()
    if(!trialData)
      {
        if(this->createDisp(nDOF) < 0)
          {
            std::cerr << "FATAL NodeDispVectors::setTrialDispComponent() - ran out of memory\n";
            exit(-1);
          }
      }

    // perform the assignment .. we don't go through Vector interface
    // as we are sure of size and this way is quicker
    const double tDisp = value;
    values[dof+2*nDOF]= tDisp - values[dof+nDOF];
    values[dof+3*nDOF]= tDisp - values[dof];
    values[dof]= tDisp;

    return 0;
  }

//! @brief Sets trial values for the displacement vector.
//! @param nDOF: number of degrees of freedom.
//! @param newTrialDisp: new trial displacement vector.
int XC::NodeDispVectors::setTrialDisp(const size_t &nDOF,const Vector &newTrialDisp)
  {
    // check vector arg is of correct size
    if(size_t(newTrialDisp.Size()) != nDOF)
      {
        std::cerr << "WARNING NodeDispVectors::setTrialDisp() - incompatible sizes\n";
        return -2;
      }

    // construct memory and Vectors for trial and committed
    // accel on first call to this method, getTrialDisp(),
    // getDisp(), or incrTrialDisp()
    if(!trialData)
      {
        if(this->createDisp(nDOF) < 0)
          {
            std::cerr << "FATAL NodeDispVectors::setTrialDisp() - ran out of memory\n";
            exit(-1);
          }
      }

    // perform the assignment .. we don't go through Vector interface
    // as we are sure of size and this way is quicker
    for(size_t i=0;i<nDOF;i++)
      {
        const double tDisp = newTrialDisp(i);
        values[i+2*nDOF]= tDisp - values[i+nDOF];
        values[i+3*nDOF]= tDisp - values[i];
        values[i] = tDisp;
      }
    return 0;
  }


//! @brief Increments trial displacements.
//! @param nDOF: number of degrees of freedom.
//! @param incrTrialDisp: increment for trial displacement vector.
int XC::NodeDispVectors::incrTrialDisp(const size_t &nDOF,const Vector &incrDispl)
  {
    // check vector arg is of correct size
    if(size_t(incrDispl.Size()) != nDOF)
      {
        std::cerr << "WARNING NodeDispVectors::incrTrialDisp() - incompatible sizes\n";
        return -2;
      }

    // create a copy if no trial exists and add committed
    if(!trialData)
      {
        if(this->createDisp(nDOF) < 0)
          {
            std::cerr << "FATAL NodeDispVectors::incrTrialDisp() - ran out of memory\n";
            exit(-1);
          }
        for(size_t i=0;i<nDOF;i++)
          {
            const double incrDispI = incrDispl(i);
            values[i]= incrDispI;
            values[i+2*nDOF]= incrDispI;
            values[i+3*nDOF]= incrDispI;
          }
        return 0;
      }

    // otherwise set trial = incr + trial
    for(size_t i= 0;i<nDOF;i++)
      {
        double incrDispI = incrDispl(i);
        values[i]+= incrDispI;
        values[i+2*nDOF]+= incrDispI;
        values[i+3*nDOF]= incrDispI;
      }
    return 0;
  }

//! @brief Commits state of displacement vectors.
//! @param nDOF: number of degrees of freedom.
int XC::NodeDispVectors::commitState(const size_t &nDOF)
  {
    // check disp exists, if does set commit = trial, incr = 0.0
    if(trialData)
      {
        for(size_t i=0; i<nDOF; i++)
          {
            values[i+nDOF]= values[i];
            values[i+2*nDOF]= 0.0;
            values[i+3*nDOF]= 0.0;
          }
      }
    return 0;
  }

//! @brief Returns the vectors to its last committed value.
//!
//! @param nDOF: number of degrees of freedom.
int XC::NodeDispVectors::revertToLastCommit(const size_t &nDOF)
  {
    // check disp exists, if does set trial = last commit, incr = 0
    if(!values.isEmpty())
      {
        for(size_t i=0;i<nDOF;i++)
          {
            values[i] = values[i+nDOF];
            values[i+2*nDOF]= 0.0;
            values[i+3*nDOF]= 0.0;
          }
      }
    return 0;
  }

//! @brief Printing.
void XC::NodeDispVectors::Print(std::ostream &s,int flag) const
  {
    if(commitData)
      s << "\tcommitDisps: " << *trialData;
    s << "\n";
  }

//! @brief private method to create the arrays to hold the disp
//! values and the Vector objects for the committed and trial quantaties.
//! @param nDOF: number of degrees of freedom.
int XC::NodeDispVectors::createDisp(const size_t &nDOF)
  {
    // trial , committed, incr = (committed-trial)
    NodeVectors::createData(nDOF);

    incrDisp = new Vector(&values[2*nDOF], nDOF);
    incrDeltaDisp = new Vector(&values[3*nDOF], nDOF);

    if(incrDisp == nullptr || incrDeltaDisp == nullptr)
      {
        std::cerr << "WARNING - NodeDispVectors::createDisp() "
                  << "ran out of memory creating Vectors(double *,int)";
        return -2;
      }
    return 0;
  }
