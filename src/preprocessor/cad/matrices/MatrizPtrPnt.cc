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
//MatrizPtrPnt.cc

#include "MatrizPtrPnt.h"
#include "TritrizPtrPnt.h"
#include "preprocessor/cad/entidades/Pnt.h"
#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"


#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"

#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/cad/Cad.h"
#include "xc_basic/src/matrices/RangoMatriz.h"



void XC::MatrizPtrPnt::setPnt(const size_t &j,const size_t &k,const int &id_punto)
  {
    if(check_range(j,k))
      {
        Cad *c= getCad();
        Pnt *tmp= at(j,k);
        if(tmp!= nullptr)
          std::clog << "Warning!, position: (" 
                    << j << "," << k 
                    << ") is already assigned to point: "
                    << tmp->GetNombre() << std::endl;
        at(j,k)= c->getPuntos().busca(id_punto);
     }
   else
     std::cerr << "(MatrizPtrPnt::setPnt): '"
               << "'; indices: ("
               << j << ',' << k << ") out of range;"
               << " number of rows: " << fls << " number of columns: " << cls
               << std::endl;
  }

XC::Pnt *XC::MatrizPtrPnt::getPnt(const size_t &j,const size_t &k)
  {
    Pnt *retval= nullptr;
    if(check_range(j,k))
      retval= at(j,k);
    return retval;
  }

//! @brief Devuelve, si lo encuentra, un puntero al punto
//! cuyo tag se pasa como parámetro.
XC::Pnt *XC::MatrizPtrPnt::buscaPunto(const size_t &tag)
  {
    Pnt *retval= nullptr;
    Pnt *tmp= nullptr;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->GetTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
const XC::Cad *XC::MatrizPtrPnt::getCad(void) const
  {
    const Cad *retval= nullptr;
    const EntCmd *ptr= Owner();
    assert(ptr);
    const MapEsquemas2d *e2d= dynamic_cast<const MapEsquemas2d *>(ptr);
    if(e2d)
      retval= e2d->getCad();
    else //Tritriz.
      {
        const TritrizPtrPnt *t= dynamic_cast<const TritrizPtrPnt *>(ptr);
        retval= t->getCad();
      }
    assert(retval);
    return retval;
  }

//! @brief Devuelve un apuntador al objeto Cad.
XC::Cad *XC::MatrizPtrPnt::getCad(void)
  {
    Cad *retval= nullptr;
    EntCmd *ptr= Owner();
    assert(ptr);
    MapEsquemas2d *e2d= dynamic_cast<MapEsquemas2d *>(ptr);
    if(e2d)
      retval= e2d->getCad();
    else //Tritriz.
      {
        TritrizPtrPnt *t= dynamic_cast<TritrizPtrPnt *>(ptr);
        retval= t->getCad();
      }
    assert(retval);
    return retval;
  }

//! @brief Devuelve, si lo encuentra, un puntero al punto
//! cuyo tag se pasa como parámetro.
const XC::Pnt *XC::MatrizPtrPnt::buscaPunto(const size_t &tag) const
  {
    const Pnt *retval= nullptr;
    const Pnt *tmp= nullptr;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->GetTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Devuelve el punto más próximo al punto being passed as parameter.
XC::Pnt *XC::MatrizPtrPnt::getNearestPnt(const Pos3d &p)
  {
    Pnt *retval= nullptr, *ptrPnt= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          ptrPnt= operator()(j,k);
          if(ptrPnt)
            {
              tmp= ptrPnt->DistanciaA2(p);
              if(tmp<d)
                {
                  d= tmp;
                  retval= ptrPnt;
                }
            }
        }
    return retval;
  }

//! @brief Devuelve el punto más próximo al punto being passed as parameter.
const XC::Pnt *XC::MatrizPtrPnt::getNearestPnt(const Pos3d &p) const
  {
    MatrizPtrPnt *this_no_const= const_cast<MatrizPtrPnt *>(this);
    return this_no_const->getNearestPnt(p);
  }

//! @brief Devuelve una matriz con los identificadores de los puntos.
m_int XC::MatrizPtrPnt::getTags(void) const
  {
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    m_int retval(nfilas,ncols,-1);
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          const Pnt *ptr= operator()(j,k);
          if(ptr)
            retval(j,k)= ptr->GetTag();
        }
    return retval;
  }

//! @brief Devuelve el centroide del esquema.
Pos3d XC::MatrizPtrPnt::getCentroide(void) const
  {
    Pos3d retval;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    const double sz= nfilas*ncols;
    GEOM_FT x= 0.0, y= 0.0, z= 0.0;
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          const Pnt *ptr= operator()(j,k);
          if(ptr)
            {
              const Pos3d &p= ptr->GetPos();
              x+= p.x();
              y+= p.y();
              z+= p.z();
            }
        }
    x/=sz; y/=sz; z/=sz;
    retval= Pos3d(x,y,z);
    return retval;
  }

//! @brief Copia los puntos del rango being passed as parameter, colocándolos
//! en las posiciones de la matriz que resultan de sumar a los índices (i,j) del
//! punto los valores del vector offsetIndices es decir (i,j)->(i+offsetIndices[0],j+offsetIndices[1])
//! y desplazando su posición geométrica según el vector vectorOffset.
std::deque<size_t> XC::MatrizPtrPnt::CopiaPuntos(const RangoMatriz &rango,const std::vector<size_t> &offsetIndices,const Vector3d &vectorOffset= Vector3d())
  {
    Cad *cad= getCad();
    std::deque<size_t> retval;
    const RangoIndice &rfilas= rango.GetRangoFilas();
    const RangoIndice &rcols= rango.GetRangoCols();
    for(size_t i= rfilas.Inf();i<=rfilas.Sup();i++)
      for(size_t j= rcols.Inf();j<=rcols.Sup();j++)
        {
          const Pnt *p= operator()(i,j);
          if(p)
            {
              Pnt *nuevo= cad->getPuntos().Copia(p,vectorOffset);
              (*this)(i+offsetIndices[0],j+offsetIndices[1])= nuevo;
              retval.push_back(nuevo->GetTag());
            }
        }
    return retval;
  }  
