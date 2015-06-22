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
//DqVectors.cc

#include "DqVectors.h"
#include "Vector.h"

//! @brief Constructor.
XC::DqVectors::DqVectors(const size_t &num)
  : dq_ptr_Vectors(num,static_cast<Vector *>(nullptr)) {}

// //! @brief Constructor de copia.
// XC::DqVectors::DqVectors(const DqVectors &otro)
//   : dq_ptr_Vectors(otro) {}

// //! @brief Operador asignación.
// XC::DqVectors &XC::DqVectors::operator=(const DqVectors &otro)
//   {
//     dq_ptr_Vectors::operator=(otro);
//     return *this;
//   }

void XC::DqVectors::clear(void)
  {
    iterator i= begin();
    for(;i!= end();i++)
      if(*i)
        delete (*i);
    erase(begin(),end());
  }

//! @brief Destructor:
XC::DqVectors::~DqVectors(void)
  { clear(); }

void XC::DqVectors::Print(std::ostream &s,const int &flag)
  {
    s << "\tNumber of Vectors: " << size() << std::endl;
  }
