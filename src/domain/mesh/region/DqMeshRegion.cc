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
//DqMeshRegion.cc

#include "DqMeshRegion.h"
#include "MeshRegion.h" 

//! @brief Libera todas las posiciones.
void XC::DqMeshRegion::libera(void)
  {
    const size_t sz= size();
    for(size_t i= 0;i<sz;i++)
      {
        if((*this)[i])
          delete (*this)[i];
       (*this)[i]= nullptr;
      }
  }

//! @brief Constructor.
XC::DqMeshRegion::DqMeshRegion(void)
  : dq_Reg() {}

//! @brief Constructor de copia.
XC::DqMeshRegion::DqMeshRegion(const DqMeshRegion  &otro)
  : dq_Reg(otro)
  {
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const MeshRegion *tmp= otro[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
  }

//! @brief Operador asignación.
XC::DqMeshRegion &XC::DqMeshRegion::operator=(const DqMeshRegion &otro)
  {
    resize(otro.size());
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const MeshRegion *tmp= otro[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
    return *this;
  }

//! @brief Destructor.
XC::DqMeshRegion::~DqMeshRegion(void)
  { libera(); }


//! @brief Cambia el tamaño del vector.
void XC::DqMeshRegion::resize(const size_t sz)
  {
    libera();
    dq_Reg::resize(sz);
    for(size_t i= 0;i<sz;i++)
      (*this)[i]= nullptr;
  }

void XC::DqMeshRegion::add(const MeshRegion &c)
  { push_back(c.getCopy()); }


XC::MeshRegion *XC::DqMeshRegion::getRegion(int tag)
  {
    MeshRegion *retval= nullptr;
    for(const_iterator i= begin(); i!=end(); i++)
      if((*i)->getTag() == tag)
        {
          retval= *i;
          break;
        }
    return retval;
  }

void XC::DqMeshRegion::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }
