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
//LinearSOEData.cpp

#include <solution/system_of_eqn/linearSOE/LinearSOEData.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that owns this object.
//! @param classTag: identifier of the class.
//! @param N: size of the system.
XC::LinearSOEData::LinearSOEData(AnalysisAggregation *owr,int classTag,int N)
  :LinearSOE(owr,classTag), size(N) {}


//! @brief Initializes storage.
void XC::LinearSOEData::inic(const size_t &sz)
  {
    if(sz>0)
      {
        B.resize(sz);
        X.resize(sz);
        zero();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; Warning - model has zero DOFs"
                << " add nodes or reduce constraints." << std::endl;
  }

//! @brief Return the current size of the system.
int XC::LinearSOEData::getNumEqn(void) const
  { return size; }

//! @brief Zeros the entries in the 1d array for \f$b\f$.
void XC::LinearSOEData::zeroB(void)
  { B.Zero(); }

//! @brief Zeros the entries in the 1d array for \f$x\f$.
void XC::LinearSOEData::zeroX(void)
  { X.Zero(); }

//! @brief Zeros the entries in the 1d array for \f$x\f$ and \f$b\f$.
void XC::LinearSOEData::zero(void)
  {
    zeroB();
    zeroX();
  }

//! @brief If \p loc is within the range of \f$x\f$, sets \f$x(loc) = value\f$.
void XC::LinearSOEData::setX(int loc, double value)
  {
    if(loc < size && loc >=0)
      X[loc] = value;
  }

//! @brief Assigns \f$x\f$.
void XC::LinearSOEData::setX(const Vector &x)
  { X= x; }

//! @brief Sets B= v*fact
//!
//! First tests that \p V and the size of the system are of compatible
//! sizes; if not a warning message is printed and a \f$-1\f$ is returned. The
//! LinearSOE object then sets the vector \p b to be \p fact times
//! the Vector \p V. If \p fact is equal to \f$0.0\f$, \f$1.0\f$ or \f$-1.0\f$,
//! more efficient steps are performed. Returns \f$0\f$. 
//!
//! @param v: vector to add.
//! @param fact: factor that multiplies v.
int XC::LinearSOEData::setB(const Vector &v,const double &fact)
  {
    // check for a quick return
    if(fact == 0.0)
      return 0; //! ??? LCPT (must not set B to 0?).

    if(v.Size() != size)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - incompatible sizes "
		  << size << " and " << v.Size() << std::endl;
        return -1;
      }

    if(fact == 1.0)
      { // do not need to multiply if fact == 1.0
        for(int i=0; i<size; i++)
          { B(i) = v(i); }
      }
    else if(fact == -1.0)
      {
        for(int i=0; i<size; i++)
          { B(i) = -v(i); }
      }
    else
      {
        for(int i=0; i<size; i++)
          { B(i) = v(i) * fact; }
      }
    return 0;
  }

//! @brief add v multiplied by factor to vector B.
//!
//! @param v: vector to add.
//! @param fact: factor that multiplies v.
int XC::LinearSOEData::addB(const Vector &v, const double &fact)
  {
    // check for a quick return
    if(fact == 0.0)
      return 0;


    if(size != v.Size() )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << " - Vector and B not of equal sizes.\n";
        return -1;
      }

    if(fact == 1.0)
      { // do not need to multiply if fact == 1.0
        for(int i=0; i<size; i++)
          B(i) += v(i);
      }
    else if(fact == -1.0)
      {
        for(int i=0; i<size; i++)
          B(i)-= v(i);
      }
    else
      {
        for(int i=0; i<size; i++)
          B(i)+= v(i) * fact;
      }
    return 0;
  }

//! @brief assemblies v multiplied by factor into vector B.
//!
//! First tests that \p loc and \p V are of compatible sizes; if not
//! a warning message is printed and a \f$-1\f$ is returned. The LinearSOE
//! object then assembles \p fact times the Vector \p V into
//! the vector \f$b\f$. The Vector is assembled into \f$b\f$ at the locations
//! given by the ID object \p loc, i.e. \f$b_{loc(i)} += fact * V(i)\f$. If a
//! location specified is outside the range, e.g. \f$-1\f$, the corresponding
//! entry in \p V is not added to \f$b\f$. If \p fact is equal to \f$0.0\f$,
//! \f$1.0\f$ or \f$-1.0\f$, more efficient steps are performed.
//! Returns \f$0\f$.
//!
//! @param v: vector to add.
//! @param id: row indexes.
//! @param fact: factor that multiplies v.
int XC::LinearSOEData::addB(const Vector &v, const ID &id,const double &fact)
  {
    // check for a quick return
    if(fact == 0.0)  return 0;


    // check that m and id are of similar size
    int idSize = id.Size();
    if(idSize != v.Size() )
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << " - vector and ID not of similar sizes.\n";
        return -1;
      }

    if(fact == 1.0)
      { // do not need to multiply if fact == 1.0
        for(int i=0; i<idSize; i++)
          {
            int pos = id(i);
            if(pos <size && pos >= 0)
              B(pos) += v(i);
          }
      }
    else if(fact == -1.0)
      {
        for(int i=0; i<idSize; i++)
          {
            int pos= id(i);
            if(pos <size && pos >= 0)
	      B(pos)-= v(i);
          }
      }
    else
      {
        for(int i=0; i<idSize; i++)
          {
            int pos = id(i);
            if(pos <size && pos >= 0)
              B(pos)+= v(i) * fact;
          }
      }
    return 0;
  }

//! @brief Returns the Vector object created for \f$x\f$.
const XC::Vector &XC::LinearSOEData::getX(void) const
  { return X; }

//! @brief Returns the Vector object created for \f$x\f$.
XC::Vector &XC::LinearSOEData::getX(void)
  { return X; }

//! @brief Returns the Vector object created for \f$b\f$.
const XC::Vector &XC::LinearSOEData::getB(void) const
  { return B; }

//! @brief Returns the Vector object created for \f$b\f$.
XC::Vector &XC::LinearSOEData::getB(void)
  { return B; }

//! @brief Returns the 2-norm of the vector \f$x\f$.
double XC::LinearSOEData::normRHS(void) const
  {
    double norm= 0.0;
    for(int i=0; i<size; i++)
      {
	const double &Yi= B[i];
	norm+= Yi*Yi;
      }
    return sqrt(norm);
  }    

void XC::LinearSOEData::receiveB(const Communicator &comm)
  {
    comm.receiveVector(B,CommMetaData(0)); //XXX assign position.
  }

void XC::LinearSOEData::receiveX(const Communicator &comm)
  {
    comm.receiveVector(X,CommMetaData(0));//XXX assign position.
  }

void XC::LinearSOEData::receiveBX(const Communicator &comm)
  {
    receiveX(comm);
    receiveB(comm);
  }

void XC::LinearSOEData::sendB(Communicator &comm) const
  {
    comm.sendVector(B,CommMetaData(0));//XXX assign position.
  }

void XC::LinearSOEData::sendBX(Communicator &comm) const
  {
    comm.sendVector(X,CommMetaData(0));//XXX assign position.
    sendB(comm);
  }


