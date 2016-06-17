//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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

