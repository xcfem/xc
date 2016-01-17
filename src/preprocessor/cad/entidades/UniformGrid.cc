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
//UniformGrid.cc

#include "UniformGrid.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"

//! @brief Constructor.
XC::UniformGrid::UniformGrid(Preprocessor *m,const size_t &i)
  : EntMdlr(m,i),Lx(0.0),Ly(0.0),Lz(0.0),ndiv_x(0),ndiv_y(0),ndiv_z(0)
  {}

//! @brief Constructor.
//! @param nombre: Identificador del objeto.
//! @param m: Apuntador al preprocesador.
//! @param i: Índice para gráficos.
XC::UniformGrid::UniformGrid(const std::string &nombre,Preprocessor *m,const size_t &i)
  : EntMdlr(nombre,i,m),Lx(0.0),Ly(0.0),Lz(0.0),ndiv_x(0),ndiv_y(0),ndiv_z(0)
  {}

//! @brief Constructor virtual.
XC::SetEstruct *XC::UniformGrid::getCopy(void) const
  { return new UniformGrid(*this); }

void XC::UniformGrid::actualiza_topologia(void)
  {
  }

//! @brief Devuelve la dimensión (1,2 ó 3) de la malla.
unsigned short int XC::UniformGrid::GetDimension(void) const
  {
    unsigned short int retval= 0;
    if(ndiv_x > 0) retval++;
    if(ndiv_y > 0) retval++;
    if(ndiv_z > 0) retval++;
    return retval;
  }

//! @brief Devuelve el BND de la malla.
BND3d XC::UniformGrid::Bnd(void) const
  {
    Vector3d v(Lx,Ly,Lz);
    return BND3d(org,org+v);
  }

//! @brief Devuelve el centro de la malla.
Pos3d XC::UniformGrid::GetCentro(void) const
  { return Pos3d(org.x()+Lx/2,org.y()+Ly/2,org.z()+Lz/2); }

//! @brief Devuelve los conjuntos a los que pertenece este objeto.
std::set<XC::SetBase *> XC::UniformGrid::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= GetPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << "UniformGrid::get_sets; no se ha definido el preprocesador." << std::endl;
    return retval;
  }

//! @brief Agrega el «uniform grid» a los conjuntos que se pasan como parámetro.
void XC::UniformGrid::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->GetUniformGrids().push_back(this);
      }
  }

//! @brief Crea los nodos de la malla.
void XC::UniformGrid::crea_nodos(void)
  {
    TritrizPos3d ptos= crea_uniform_grid(Bnd(),ndiv_x,ndiv_y,ndiv_z);
    EntMdlr::crea_nodos(ptos);
  }

//! @brief Crea la malla.
void XC::UniformGrid::Malla(dir_mallado dm)
  {
    crea_nodos();
    crea_elementos(dm);
  }



