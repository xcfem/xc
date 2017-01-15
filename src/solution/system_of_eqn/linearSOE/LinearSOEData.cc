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
XC::LinearSOEData::LinearSOEData(SoluMethod *owr,int classtag,int N)
  :LinearSOE(owr,classtag), size(N) {}


void XC::LinearSOEData::inic(const size_t &sz)
  {
    if(sz>0)
      {
        B.resize(sz);
        X.resize(sz);
        zero();
      }
    else
      std::cerr << "¡OJO! LinearSOEData::inicBX el modelo no tiene ningún grado de libertad"
                << " agrege algún nodo o cambie el gestor de coacciones." << std::endl;
  }

int XC::LinearSOEData::getNumEqn(void) const
  { return size; }

void XC::LinearSOEData::zeroB(void)
  { B.Zero(); }

void XC::LinearSOEData::zeroX(void)
  { X.Zero(); }

void XC::LinearSOEData::zero(void)
  {
    zeroB();
    zeroX();
  }

void XC::LinearSOEData::setX(int loc, double value)
  {
    if(loc < size && loc >=0)
      X[loc] = value;
  }

void XC::LinearSOEData::setX(const Vector &x)
  { X= x; }

int XC::LinearSOEData::setB(const Vector &v,const double &fact)
  {
    // check for a XC::quick return
    if(fact == 0.0)  return 0;

    if(v.Size() != size)
      {
        std::cerr << "WARNING XC::LinearSOEData::setB() -";
        std::cerr << " incomptable sizes " << size << " and " << v.Size() << std::endl;
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

int XC::LinearSOEData::addB(const Vector &v, const double &fact)
  {
    // check for a XC::quick return
    if(fact == 0.0)  return 0;



    if(size != v.Size() )
      {
        std::cerr << "XC::LineaSOEData::addB() - Vector and B not of equal sizes\n";
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

int XC::LinearSOEData::addB(const XC::Vector &v, const XC::ID &id,const double &fact)
  {
    // check for a XC::quick return
    if(fact == 0.0)  return 0;


    // check that m and id are of similar size
    int idSize = id.Size();
    if(idSize != v.Size() )
      {
        std::cerr << "LinearSOEData::addB()        - Vector and XC::ID not of similar sizes\n";
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

const XC::Vector &XC::LinearSOEData::getX(void) const
  { return X; }

XC::Vector &XC::LinearSOEData::getX(void)
  { return X; }

const XC::Vector &XC::LinearSOEData::getB(void) const
  { return B; }

XC::Vector &XC::LinearSOEData::getB(void)
  { return B; }

double XC::LinearSOEData::normRHS(void) const
  {
    double norm= 0.0;
    for(register int i=0; i<size; i++)
      {
	const double &Yi= B[i];
	norm+= Yi*Yi;
      }
    return sqrt(norm);
  }    

void XC::LinearSOEData::receiveB(const CommParameters &cp)
  {
    cp.receiveVector(B,CommMetaData(0)); //XXX asignar posición.
  }

void XC::LinearSOEData::receiveX(const CommParameters &cp)
  {
    cp.receiveVector(X,CommMetaData(0));//XXX asignar posición.
  }

void XC::LinearSOEData::receiveBX(const CommParameters &cp)
  {
    receiveX(cp);
    receiveB(cp);
  }

void XC::LinearSOEData::sendB(CommParameters &cp) const
  {
    cp.sendVector(B,CommMetaData(0));//XXX asignar posición.
  }

void XC::LinearSOEData::sendBX(CommParameters &cp) const
  {
    cp.sendVector(X,CommMetaData(0));//XXX asignar posición.
    sendB(cp);
  }


