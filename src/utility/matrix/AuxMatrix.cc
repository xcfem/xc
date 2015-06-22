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
//Matrix.cpp

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

