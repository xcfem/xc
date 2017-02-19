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
//KRSeccion.cc

#include "KRSeccion.h"

double XC::KRSeccion::value= 0.0;
double XC::KRSeccion::vas1= 0.0;
double XC::KRSeccion::vas2= 0.0;
double XC::KRSeccion::vas1as2= 0.0;

void XC::KRSeccion::libera(void)
  {
    if(R)
      {
        delete R;
        R= nullptr;
      }
    if(K)
      {
        delete K;
        K= nullptr;
      }
  }

void XC::KRSeccion::alloc(const size_t &dim)
  {
    libera();
    R= new Vector(rData,dim);
    K= new Matrix(kData,dim,dim);
  }

void XC::KRSeccion::copia(const KRSeccion &otra)
  {
    libera();
    rData[0]= otra.rData[0]; rData[1]= otra.rData[1];
    rData[2]= otra.rData[2]; rData[3]= otra.rData[3];

    kData[0]= otra.kData[0]; kData[1]= otra.kData[1]; kData[2]= otra.kData[2]; kData[3]= otra.kData[3];
    kData[4]= otra.kData[4]; kData[5]= otra.kData[5]; kData[6]= otra.kData[6]; kData[7]= otra.kData[7];
    kData[8]= otra.kData[8]; kData[9]= otra.kData[9]; kData[10]= otra.kData[10]; kData[11]= otra.kData[11];
    kData[12]= otra.kData[12]; kData[13]= otra.kData[13]; kData[14]= otra.kData[14]; kData[15]= otra.kData[15];

    alloc(otra.dim());
  }

void XC::KRSeccion::zero(void)
  {
    rData[0]= 0.0; rData[1]= 0.0;
    rData[2]= 0.0; rData[3]= 0.0;

    kData[0]= 0.0; kData[1]= 0.0; kData[2]= 0.0; kData[3]= 0.0;
    kData[4]= 0.0; kData[5]= 0.0; kData[6]= 0.0; kData[7]= 0.0;
    kData[8]= 0.0; kData[9]= 0.0; kData[10]= 0.0; kData[11]= 0.0;
    kData[12]= 0.0; kData[13]= 0.0; kData[14]= 0.0; kData[15]= 0.0;
  }

//! @brief Constructor.
XC::KRSeccion::KRSeccion(const size_t &dim)
  : R(nullptr), K(nullptr)
  {
    alloc(dim);
    zero();
  }

//! @brief Copy constructor.
XC::KRSeccion::KRSeccion(const KRSeccion &otra)
  : R(nullptr), K(nullptr)
  {
    copia(otra);
  }

//! @brief Assignment operator.
XC::KRSeccion &XC::KRSeccion::operator=(const KRSeccion &otro)
  {
    copia(otro);
    return *this;
  }

//! @brief Destructor.
XC::KRSeccion::~KRSeccion(void)
  {
    libera();
  }
