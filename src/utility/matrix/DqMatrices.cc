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
//DqMatrices.cc

#include "DqMatrices.h"
#include "Matrix.h"

//! @brief Constructor.
XC::DqMatrices::DqMatrices(const size_t &num)
  : dq_ptr_Matrices(num,static_cast<Matrix *>(nullptr)) {}

// //! @brief Constructor de copia.
// XC::DqMatrices::DqMatrices(const DqMatrices &otro)
//   : dq_ptr_Matrices(otro) {}

// //! @brief Operador asignación.
// XC::DqMatrices &XC::DqMatrices::operator=(const DqMatrices &otro)
//   {
//     dq_ptr_Matrices::operator=(otro);
//     return *this;
//   }

void XC::DqMatrices::clear(void)
  {
    iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        delete (*i);
    erase(begin(),end());
  }

//! @brief Destructor:
XC::DqMatrices::~DqMatrices(void)
  { clear(); }

void XC::DqMatrices::Print(std::ostream &s,const int &flag)
  {
    s << "\tNumber of Matrices: " << size() << std::endl;
  }
