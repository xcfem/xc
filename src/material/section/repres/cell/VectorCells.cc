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
//VectorCells.cc

#include "VectorCells.h"
#include <material/section/repres/cell/Cell.h> 

//! @brief Liberta todas las pociciones.
void XC::VectorCells::libera(void)
  {
    const size_t sz= size();
    for(size_t i= 0;i<sz;i++)
      libera(i);
  }

//! @brief Libera la posición i.
void XC::VectorCells::libera(const size_t i)
  {
    if((*this)[i])
      delete (*this)[i];
    (*this)[i]= nullptr;
  }

//! @brief Guarda una copia de la celda en la posición i.
void XC::VectorCells::alloc(const size_t i,const Cell &c)
  {
    libera(i);
    (*this)[i]= c.getCopy();
  }

//! @brief Constructor.
XC::VectorCells::VectorCells(const size_t &sz)
  : v_cell(sz,nullptr) {}

//! @brief Constructor de copia.
XC::VectorCells::VectorCells(const VectorCells  &otro)
  : v_cell(otro.size(),nullptr)
  {
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const Cell *tmp= otro[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
  }

//! @brief Operador asignación.
XC::VectorCells &XC::VectorCells::operator=(const VectorCells &otro)
  {
    resize(otro.size());
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const Cell *tmp= otro[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
    return *this;
  }

//! @brief Destructor.
XC::VectorCells::~VectorCells(void)
  { libera(); }


//! @brief Destructor.
void XC::VectorCells::resize(const size_t sz)
  {
    libera();
    v_cell::resize(sz);
    for(size_t i= 0;i<sz;i++)
      (*this)[i]= nullptr;
  }

void XC::VectorCells::put(const size_t i,const Cell &c)
  { alloc(i,c); }

void XC::VectorCells::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }
