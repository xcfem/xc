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
//UnbalAndTangent.cpp

#include "UnbalAndTangent.h"


bool XC::UnbalAndTangent::libera(void)
  {
    // delete tangent and residual if created specially
    if(nDOF>=unbalAndTangentArray.size())
      {
        if(theTangent) delete theTangent;
        theTangent= nullptr;
        if(theResidual) delete theResidual;
        theResidual= nullptr;
        return true;
      }
    else
      return false;
  }

void XC::UnbalAndTangent::alloc(void)
  {
    // create matrices and vectors for each object instance
    if(libera())
      {
        theResidual=new Vector(nDOF);
        theTangent=new Matrix(nDOF, nDOF);
        if(theResidual == 0 || theResidual->Size() ==0 ||  theTangent ==0 || theTangent->noRows() ==0)
          {       
            std::cerr << "UnbalAndTangent::alloc() ";
            std::cerr << " ran out of memory for vector/Matrix of size :";
            std::cerr << nDOF << std::endl;
            exit(-1);
          }
      }
    else
      {
        theResidual= unbalAndTangentArray.setUnbalance(nDOF);
        theTangent= unbalAndTangentArray.setTangent(nDOF);
      }             
  }

void XC::UnbalAndTangent::copia(const UnbalAndTangent &otro)
  {
    // create matrices and vectors for each object instance
    if(libera())
      {
        if(otro.theResidual) theResidual=new Vector(*otro.theResidual);
        if(otro.theTangent) theTangent=new Matrix(*otro.theTangent);
        if(theResidual == 0 || theResidual->Size() ==0 ||  theTangent ==0 || theTangent->noRows() ==0)
          {       
            std::cerr << "UnbalAndTangent::copia() ";
            std::cerr << " ran out of memory for vector/Matrix of size :";
            std::cerr << nDOF << std::endl;
            exit(-1);
          }
      }
    else
      {
        theResidual= otro.theResidual;
        theTangent= otro.theTangent;
      }             
  }

//! @brief Construictor.
XC::UnbalAndTangent::UnbalAndTangent(const size_t &n,UnbalAndTangentStorage &a)
  :nDOF(n), theResidual(nullptr), theTangent(nullptr), unbalAndTangentArray(a) 
  { alloc(); }

//! @brief Constructor de copia.
XC::UnbalAndTangent::UnbalAndTangent(const UnbalAndTangent &otro)
  :nDOF(0), theResidual(nullptr), theTangent(nullptr), unbalAndTangentArray(otro.unbalAndTangentArray) 
  {
    libera();
    nDOF= otro.nDOF;
    copia(otro);
  }

//! @brief Operador asignación.
XC::UnbalAndTangent &XC::UnbalAndTangent::operator=(const UnbalAndTangent &otro)
  {
    libera();
    unbalAndTangentArray= otro.unbalAndTangentArray;
    nDOF= otro.nDOF;
    copia(otro);
    return *this;
  }

//! @brief destructor.
XC::UnbalAndTangent::~UnbalAndTangent(void)
  { libera(); }

//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix &XC::UnbalAndTangent::getTangent(void) const
  {
    assert(theTangent);
    return *theTangent;
  }

//! @brief Devuelve la matriz de rigidez tangente.
XC::Matrix &XC::UnbalAndTangent::getTangent(void)
  {
    assert(theTangent);
    return *theTangent;
  }

//! @brief Devuelve el vector residuo.
const XC::Vector &XC::UnbalAndTangent::getResidual(void) const
  {
    assert(theResidual);
    return *theResidual;
  }

//! @brief Devuelve el vector residuo.
XC::Vector &XC::UnbalAndTangent::getResidual(void)
  {
    assert(theResidual);
    return *theResidual;
  }
