//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//FVectorShell.cc

#include "FVectorShell.h"
#include <utility/matrix/Vector.h>

//! @brief Default constructor.
XC::FVectorShell::FVectorShell(void)
  : FVectorData<24>() { zero(); }

//! @brief Copy constructor.
XC::FVectorShell::FVectorShell(const XC::FVectorShell &other)
  : FVectorData<24>(other)
  {
    p[0]= other.p[0];
    p[1]= other.p[1];
    p[2]= other.p[2];
    p[3]= other.p[3];
    p[4]= other.p[4];
    p[5]= other.p[5];
    p[6] = other.p[6];
    p[7] = other.p[7];
    p[8] = other.p[8];
    p[9] = other.p[9];
    p[10] = other.p[10];
    p[11] = other.p[11];
    p[12] = other.p[12];
    p[13] = other.p[13];
    p[14] = other.p[14];
    p[15] = other.p[15];
    p[16] = other.p[16];
    p[17] = other.p[17];
    p[18] = other.p[18];
    p[19] = other.p[19];
    p[20] = other.p[20];
    p[21] = other.p[21];
    p[22] = other.p[22];
    p[23] = other.p[23];
  }

//! @brief Constructor a partir de un vector.
XC::FVectorShell::FVectorShell(const XC::Vector &v)
  {
    if(v.Size()>=24)
      {
        p[0] = v[0];
        p[1] = v[1];
        p[2] = v[2];
        p[3] = v[3];
        p[4] = v[4];
        p[5] = v[5];
        p[6] = v[6];
        p[7] = v[7];
        p[8] = v[8];
        p[9] = v[9];
        p[10] = v[10];
        p[11] = v[11];
        p[12] = v[12];
        p[13] = v[13];
        p[14] = v[14];
        p[15] = v[15];
        p[16] = v[16];
        p[17] = v[17];
        p[18] = v[18];
        p[19] = v[19];
        p[20] = v[20];
        p[21] = v[21];
        p[22] = v[22];
        p[23] = v[23];
      }
    else
      std::cerr << getClassName() << __FUNCTION__
	        << "; a vector of dimension 6 was expected." << std::endl;
  }

//! @brief Assignment operator.
XC::FVectorShell &XC::FVectorShell::operator=(const XC::FVectorShell &other)
  {
    p[0] = other.p[0];
    p[1] = other.p[1];
    p[2] = other.p[2];
    p[3] = other.p[3];
    p[4] = other.p[4];
    p[5] = other.p[5];
    p[6] = other.p[6];
    p[7] = other.p[7];
    p[8] = other.p[8];
    p[9] = other.p[9];
    p[10] = other.p[10];
    p[11] = other.p[11];
    p[12] = other.p[12];
    p[13] = other.p[13];
    p[14] = other.p[14];
    p[15] = other.p[15];
    p[16] = other.p[16];
    p[17] = other.p[17];
    p[18] = other.p[18];
    p[19] = other.p[19];
    p[20] = other.p[20];
    p[21] = other.p[21];
    p[22] = other.p[22];
    p[23] = other.p[23];
    return *this;
  }

void XC::FVectorShell::zero(void)
  {
    p[0]= 0.0;
    p[1]= 0.0;
    p[2]= 0.0;
    p[3]= 0.0;
    p[4]= 0.0;
    p[5]= 0.0;
    p[6]= 0.0;
    p[7]= 0.0;
    p[8]= 0.0;
    p[9]= 0.0;
    p[10]= 0.0;
    p[11]= 0.0;
    p[12]= 0.0;
    p[13]= 0.0;
    p[14]= 0.0;
    p[15]= 0.0;
    p[16]= 0.0;
    p[17]= 0.0;
    p[18]= 0.0;
    p[19]= 0.0;
    p[20]= 0.0;
    p[21]= 0.0;
    p[22]= 0.0;
    p[23]= 0.0;
  }

XC::FVectorShell &XC::FVectorShell::operator*=(const double &d)
  {
    p[0]*= d;
    p[1]*= d;
    p[2]*= d;
    p[3]*= d;
    p[4]*= d;
    p[5]*= d;
    p[6]*= d;
    p[7]*= d;
    p[8]*= d;
    p[9]*= d;
    p[10]*= d;
    p[11]*= d;
    p[12]*= d;
    p[13]*= d;
    p[14]*= d;
    p[15]*= d;
    p[16]*= d;
    p[17]*= d;
    p[18]*= d;
    p[19]*= d;
    p[20]*= d;
    p[21]*= d;
    p[22]*= d;
    p[23]*= d;
    return *this;    
  }

XC::FVectorShell &XC::FVectorShell::operator+=(const FVectorShell &other)
  {
    p[0]+= other.p[0];
    p[1]+= other.p[1];
    p[2]+= other.p[2];
    p[3]+= other.p[3];
    p[4]+= other.p[4];
    p[5]+= other.p[5];
    p[6]+= other.p[6];
    p[7]+= other.p[7];
    p[8]+= other.p[8];
    p[9]+= other.p[9];
    p[10]+= other.p[10];
    p[11]+= other.p[11];
    p[12]+= other.p[12];
    p[13]+= other.p[13];
    p[14]+= other.p[14];
    p[15]+= other.p[15];
    p[16]+= other.p[16];
    p[17]+= other.p[17];
    p[18]+= other.p[18];
    p[19]+= other.p[19];
    p[20]+= other.p[20];
    p[21]+= other.p[21];
    p[22]+= other.p[22];
    p[23]+= other.p[23];
    return *this;    
  }

XC::FVectorShell &XC::FVectorShell::operator-=(const FVectorShell &other)
  {
    p[0]-= other.p[0];
    p[1]-= other.p[1];
    p[2]-= other.p[2];
    p[3]-= other.p[3];
    p[4]-= other.p[4];
    p[5]-= other.p[5];
    p[6]-= other.p[6];
    p[7]-= other.p[7];
    p[8]-= other.p[8];
    p[9]-= other.p[9];
    p[10]-= other.p[10];
    p[11]-= other.p[11];
    p[12]-= other.p[12];
    p[13]-= other.p[13];
    p[14]-= other.p[14];
    p[15]-= other.p[15];
    p[16]-= other.p[16];
    p[17]-= other.p[17];
    p[18]-= other.p[18];
    p[19]-= other.p[19];
    p[20]-= other.p[20];
    p[21]-= other.p[21];
    p[22]-= other.p[22];
    p[23]-= other.p[23];
    return *this;    
  }

//! @brief Adds the force being passed as parameter.
void XC::FVectorShell::addForce(const size_t &inod,const double &P1,const double &P2,const double &P3)
  {
    if(inod>3)
      std::cerr << "FVectorShell::addForce; node: "
                << inod << "out of range (0..3)." << std::endl;
    const size_t i= inod*6;
    p[i]-= P1;  //Node 1: i=0. Node 2: i=6. Node 3: i= 12. Node 4: i= 18
    p[i+1]-= P2;
    p[i+2]-= P3;
  }

//! @brief Adds the bending moment being passed as parameter.
void XC::FVectorShell::addMoment(const size_t &inod,const double &M1,const double &M2,const double &M3)
  {
    if(inod>3)
      std::cerr << "FVectorShell::addMoment; node: "
                << inod << "out of range (0..3)." << std::endl;
    const size_t i= inod*6+3;
    p[i]-= M1;  //Node 1: i=3. Node 2: i=9. Node 3: i= 15. Node 4: i= 21
    p[i+1]-= M2;
    p[i+2]-= M3;
  }

void XC::FVectorShell::Print(std::ostream &os) const
  {
    os << '[' << p[0] << ',' << p[1] << ',' << p[2] << ',' << p[3] << ',' << p[4] << ','
              << p[5] << ',' << p[6] << ',' << p[7] << ',' << p[8] << ',' << p[9] << ','
              << p[10] << ',' << p[11] << ',' << p[12] << ',' << p[13] << ',' << p[14] << ','
              << p[15] << ',' << p[16] << ',' << p[17] << ',' << p[18] << ',' << p[19] << ','
              << p[20] << ',' << p[21] << ',' << p[22] << ',' << p[23] << ']';
  }

