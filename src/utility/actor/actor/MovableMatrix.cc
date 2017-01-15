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
//MovableMatrix.cc

#include "MovableMatrix.h"

#include "utility/actor/channel/Channel.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
XC::MovableMatrix::MovableMatrix(const size_t &nr,const size_t &nc)
  :Matrix(nr,nc),MovableObject(MATRIX_TAG_Matrix) {}

//! @brief Constructor.
XC::MovableMatrix::MovableMatrix(const Matrix &m)
  :Matrix(m),MovableObject(MATRIX_TAG_Matrix) {}

//! @brief Asigna el matrix.
void XC::MovableMatrix::setMatrix(const Matrix &v)
  { Matrix::operator=(v); }

//! @brief Devuelve un vector para almacenar los dbTags
//! de los miembros de la clase.
XC::DbTagData &XC::MovableMatrix::getDbTagData(void) const
  {
    static DbTagData retval(3);
    return retval;
  }

//! @brief Envia la matriz through the channel being passed as parameter.
int XC::MovableMatrix::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dbTag= getDbTag();
    inicComm(3);
    const int dataTag= cp.getDbTag();
    const int nr= noRows();
    const int nc= noCols();
    const int sz= nr*nc;
    setDbTagDataPos(0,nr);
    setDbTagDataPos(1,nc);
    setDbTagDataPos(2,dataTag);

    int res= cp.sendIdData(getDbTagData(),dbTag);
    if(res<0)
      std::cerr << "MovableMatrix::sendSelf() - failed to send ID data\n";
    if(sz>0)
      {
        res+= cp.sendMatrix(*this,dataTag);
        if(res<0)
          std::cerr << "MovableMatrix::sendSelf() - failed to send Disp data\n";
      }
    return res;
  }

int XC::MovableMatrix::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dbTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dbTag);
    if(res < 0)
      {
        std::cerr << "MovableVector::recvSelf() - failed to receive ID data\n";
        return res;
      }
    else
      {
        const int nr= getDbTagDataPos(0);
        const int nc= getDbTagDataPos(1);
        resize(nr,nc);
        if((nr>0)&&(nc>0))
          {
            const int dataTag= getDbTagDataPos(2);
            res= cp.receiveMatrix(*this,dataTag);
          }
      }
    if(res<0)
      std::cerr << "MovableMatrix::recvSelf - failed to receive data\n";
    return res;
  }

