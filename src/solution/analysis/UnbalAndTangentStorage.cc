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
//UnbalAndTangentStorage.cpp


#include "UnbalAndTangentStorage.h"

XC::UnbalAndTangentStorage::UnbalAndTangentStorage(const size_t &n)
  : theMatrices(n), theVectors(n) {}

const XC::Matrix &XC::UnbalAndTangentStorage::getTangent(const size_t &i) const
  { return theMatrices[i]; }

XC::Matrix &XC::UnbalAndTangentStorage::getTangent(const size_t &i)
  { return theMatrices[i]; }

const XC::Vector &XC::UnbalAndTangentStorage::getUnbalance(const size_t &i) const
  { return theVectors[i]; }

XC::Vector &XC::UnbalAndTangentStorage::getUnbalance(const size_t &i)
  { return theVectors[i]; }

XC::Vector *XC::UnbalAndTangentStorage::setUnbalance(const size_t &i)
  {
    if(theVectors[i].Nulo())
      { theVectors[i]= Vector(i); }
    return &theVectors[i];
  }

XC::Matrix *XC::UnbalAndTangentStorage::setTangent(const size_t &i)
  {
    if(theMatrices[i].Nula())
      { theMatrices[i]= Matrix(i,i); }
    return &theMatrices[i];
  }




