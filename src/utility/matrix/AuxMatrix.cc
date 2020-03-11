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
//AuxMatrix.cpp

#include "utility/matrix/AuxMatrix.h"

//! @brief Constructor.
XC::AuxMatrix::AuxMatrix(const size_t &szDoubleWork,const size_t &szIntWork)
  :sizeDoubleWork(szDoubleWork), sizeIntWork(szIntWork), matrixWork(szDoubleWork), intWork(szIntWork)
  {}

void XC::AuxMatrix::resize(const size_t &szd,const size_t &szi)
  {
    matrixWork.resize(szd);
    sizeDoubleWork= szd;
    intWork.resize(szi);
    sizeIntWork= szi;
  }
    
const size_t &XC::AuxMatrix::getSizeDoubleWork(void) const
  { return sizeDoubleWork; }

const size_t &XC::AuxMatrix::getIntDoubleWork(void) const
  { return sizeIntWork; }

const double *XC::AuxMatrix::getMatrixWork(void) const
  { return matrixWork.getDataPtr(); }
  
double *XC::AuxMatrix::getMatrixWork(void)
  { return matrixWork.getDataPtr(); }

const int *XC::AuxMatrix::getIntWork(void) const
  { return intWork.getDataPtr(); }
  
int *XC::AuxMatrix::getIntWork(void)
  { return intWork.getDataPtr(); }

