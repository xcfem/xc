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
//util_matrix.cc

#include "util_matrix.h"
#include "Vector.h"
#include "Matrix.h"
#include "xc_utils/src/base/any_const_ptr.h"

any_const_ptr XC::vector_to_prop_vector(const XC::Vector *ptrVector)
  {
    assert(ptrVector);
    static m_double tmp;
    tmp= vector_to_m_double(*ptrVector);
    return any_const_ptr(&tmp);
  }

any_const_ptr XC::vector_to_prop_vector(const XC::Vector &refVector)
  { return vector_to_prop_vector(&refVector); }

m_double XC::Vector_to_m_double(const Vector &v)
  {
    const size_t sz= v.Size();
    m_double retval(sz,1);
    for(register size_t i=0;i<sz;i++)
      retval(i+1,1)= v(i);
    return retval;
  }

