//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//EntMdlr.cc

#include "EntMdlr.h"
#include "xc_basic/src/matrices/RangoIndice.h"
#include "xc_utils/src/geom/pos_vec/MatrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/TritrizPos3d.h"
#include "xc_utils/src/geom/pos_vec/RangoTritriz.h"
#include "domain/mesh/node/Node.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/SetFilaI.h"
#include "preprocessor/set_mgmt/SetFilaJ.h"
#include "preprocessor/set_mgmt/SetFilaK.h"



#include "med.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::EntMdlr::EntMdlr(Preprocessor *m,const size_t &i)
  : SetEstruct("",m), idx(i), doGenMesh(true), nodos(), elementos() {}

//! @brief Constructor.
//! @param nombre: Identificador del objeto.
//! @param i: Índice para gráficos.
//! @param m: Pointer to preprocesador.
XC::EntMdlr::EntMdlr(const std::string &nombre,const size_t &i,Preprocessor *m)
  : SetEstruct(nombre,m), idx(i), doGenMesh(true), nodos(), elementos() {}


//! @brief Copy constructor.
XC::EntMdlr::EntMdlr(const EntMdlr &otro)
  : SetEstruct(otro), idx(otro.idx), doGenMesh(true), nodos(otro.nodos), elementos(otro.elementos) {}

//! @brief Assignment operator.
XC::EntMdlr &XC::EntMdlr::operator=(const EntMdlr &otro)
  {
    SetEstruct::operator=(otro);
    idx= otro.idx;
    doGenMesh= otro.doGenMesh;
    nodos= otro.nodos;
    elementos= otro.elementos;
    return *this;
  }

//! @brief Asigna el índice al objeto (ver numera en Set).
void XC::EntMdlr::set_indice(const size_t &i)
  { idx= i; }

//! @brief Interfaz con VTK.
int XC::EntMdlr::getVtkCellType(void) const
  {
    std::cerr << "EntMdlr::getVtkCellType: function getVtkCellType must be overloaded in derived classes." << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Interfaz con el formato MED de Salome.
int XC::EntMdlr::getMEDCellType(void) const
  {
    std::cerr << "EntMdlr::getMEDCellType: function getMEDCellType must be overloaded in derived classes." << std::endl;
    return MED_NONE;
  }

//! @brief Borra todo el contenido del objeto.
void XC::EntMdlr::clearAll(void)
  {
    SetEstruct::clearPyProps();
    BorraPtrNodElem();
  }

//! @brief Borra los pointers to nodes y elementos.
void XC::EntMdlr::BorraPtrNodElem(void)
  {
    nodos.clearAll();
    elementos.clearAll();
  }

//! @brief Returns a pointer to the nodo cuyos índices se pasan como parámetros.
XC::Node *XC::EntMdlr::GetNodo(const size_t &i,const size_t &j,const size_t &k)
  {
    if(!nodos.Null())
      return nodos(i,j,k);
    else
      return nullptr;
  }

//! @brief Returns a pointer to the nodo cuyos índices se pasan como parámetros.
const XC::Node *XC::EntMdlr::GetNodo(const size_t &i,const size_t &j,const size_t &k) const
  {
    if(!nodos.Null())
      return nodos(i,j,k);
    else
      return nullptr;
  }

//! @brief Returns the nodo más próximo al punto being passed as parameter.
XC::Node *XC::EntMdlr::getNearestNode(const Pos3d &p)
  { return nodos.getNearestNode(p); }

//! @brief Returns the nodo más próximo al punto being passed as parameter.
const XC::Node *XC::EntMdlr::getNearestNode(const Pos3d &p) const
  {
    EntMdlr *this_no_const= const_cast<EntMdlr *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Returns the índices del nodo que se paras como parámetro.
XC::ID XC::EntMdlr::getNodeIndices(const Node *n) const
  { return nodos.getNodeIndices(n); }

//! @brief Returns the tags de los nodos.
std::vector<int> XC::EntMdlr::getTagsNodos(void) const
  { return nodos.getTags(); }

//! @brief Returns a pointer to the elemento cuyos índices se pasan como parámetros.
XC::Element *XC::EntMdlr::GetElemento(const size_t &i,const size_t &j,const size_t &k)
  {
    if(!elementos.Null())
      return elementos(i,j,k);
    else
      return nullptr;
  }

//! @brief Returns a pointer to the elemento cuyos índices se pasan como parámetros.
const XC::Element *XC::EntMdlr::GetElemento(const size_t &i,const size_t &j,const size_t &k) const
  {
    if(!elementos.Null())
      return elementos(i,j,k);
    else
      return nullptr;
  }

//! @brief Returns the elemento más próximo al punto being passed as parameter.
XC::Element *XC::EntMdlr::getNearestElement(const Pos3d &p)
  { return elementos.getNearestElement(p); }

//! @brief Returns the elemento más próximo al punto being passed as parameter.
const XC::Element *XC::EntMdlr::getNearestElement(const Pos3d &p) const
  {
    EntMdlr *this_no_const= const_cast<EntMdlr *>(this);
    return this_no_const->getNearestElement(p);
  }

//! @brief Returns a pointer to the nodo cuyo identificador is being passed as parameter.
XC::Node *XC::EntMdlr::buscaNodo(const int &tag)
  { return nodos.buscaNodo(tag); }
//! @brief Returns a pointer to the nodo cuyo identificador is being passed as parameter.
const XC::Node *XC::EntMdlr::buscaNodo(const int &tag) const
  { return nodos.buscaNodo(tag); }
//! @brief Returns a pointer to the elemento cuyo identificador is being passed as parameter.
XC::Element *XC::EntMdlr::buscaElemento(const int &tag)
  { return elementos.buscaElemento(tag); }
//! @brief Returns a pointer to the elemento cuyo identificador is being passed as parameter.
const XC::Element *XC::EntMdlr::buscaElemento(const int &tag) const
  { return elementos.buscaElemento(tag); }

//! @brief Creates a set that corresponds to a row of nodes and elements.
XC::SetEstruct *XC::EntMdlr::crea_set_fila(const RangoTritriz &rango,const std::string &nmb)
  {
    SetEstruct *retval= nullptr;
    if(get_preprocessor())
      {
        MapSet &map_set= get_preprocessor()->get_sets();
        if(rango.EsFilaI())
          {
            retval= map_set.crea_set_estruct(GetVarRefFilaI(rango,nmb));
          }
        else if(rango.EsFilaJ())
          {
            XC::SetFilaJ fj= GetVarRefFilaJ(rango,nmb);
            retval= map_set.crea_set_estruct(fj);
          }
        else if(rango.EsFilaK())
          {
            retval= map_set.crea_set_estruct(GetVarRefFilaK(rango,nmb));
          }
        else
	  std::cerr <<  "EntMdlr::crea_set_fila; no se pudo crear the set fila." << std::endl;
      }
    else
      {
	std::cerr << "EntMdlr::crea_set_fila; falta a pointer to preprocesador." << std::endl;
        return nullptr;
      }
    return retval;
  }

//! @brief Return Simpson's weights.
XC::Vector XC::EntMdlr::getSimpsonWeights(const std::string &ijk,const std::string &strExpr,const size_t &f,const size_t &c, const size_t &n) const
  {
    Vector retval;
    const ExprAlgebra e(strExpr);
    if(ijk=="i")
      retval= nodos.IntegSimpsonFilaI(f,c,e,n);
    else if(ijk=="j")
      retval= nodos.IntegSimpsonFilaJ(f,c,e,n);
    else if(ijk=="k")
      retval= nodos.IntegSimpsonFilaK(f,c,e,n);
    return retval;
  }

//! @brief Creates a nodos en la posición being passed as parameter.
XC::Node *XC::EntMdlr::crea_nodo(const Pos3d &pos,size_t i,size_t j, size_t k)
  {
    Node *retval= get_preprocessor()->getNodeLoader().nuevoNodo(pos);
    nodos(i,j,k)= retval;
    return retval;
  }

//! @brief Crea los nodos en las posiciones que se pasan como parámetro.
void XC::EntMdlr::crea_nodos(const TritrizPos3d &posiciones)
  {
    const size_t capas= posiciones.GetCapas();
    if(capas<1) return;
    if(nodos.Null())
      {
        const size_t filas= posiciones(1).getNumFilas();
        const size_t cols= posiciones(1).getNumCols();
        nodos = TritrizPtrNod(capas,filas,cols);

        if(!get_preprocessor()) return;
        for(register size_t i= 1;i<=capas;i++)
          for(register size_t j= 1;j<=filas;j++)
            for(register size_t k= 1;k<=cols;k++)
              crea_nodo(posiciones(i,j,k),i,j,k);
        if(verborrea>5)
	  std::cerr << "EntMdlr::crea_nodos(); creados " << nodos.NumPtrs() << " nodo(s)." << std::endl;
      }
    else
      if(verborrea>2)
        std::clog << "EntMdlr::crea_nodos; los nodos de la entidad: '" << GetNombre() << "' ya existen." << std::endl;
  }

//! @brief Crea los elementos en los nodos que se crearon en crea_nodos.
bool XC::EntMdlr::crea_elementos(meshing_dir dm)
  {
    bool retval= false;
    if(!nodos.empty())
      {
        if(nodos.HasNull())
          std::cerr << "EntMdlr::crea_elementos; existen pointers to node nulos."
                    << " No se crean elementos." << std::endl;
        else
          if(elementos.Null())
            {
              if(get_preprocessor())
                {
                  if(verborrea>4)
                    std::clog << "Creando elementos entidad: '" << GetNombre() << "'...";   
                  const Element *smll= get_preprocessor()->getElementLoader().get_elemento_semilla();
                  if(smll)
                    {
                      elementos= smll->put_on_mesh(nodos,dm);
                      agrega_elementos(elementos);
                      retval= true;
                    }
                  else if(verborrea>0)
                    std::clog << "EntMdlr::crea_elementos; no se estableció el elemento semilla." << std::endl;
                  if(verborrea>4)
                    std::clog << "creados." << std::endl;
                }
              else
                std::cerr << "EntMdlr::crea_elementos; falta a pointer to preprocesador." << std::endl;
            }
      }
    else
      std::cerr << "EntMdlr::crea_elementos; no hay nodos para los elementos." << std::endl;

    return retval;
  }

//! @brief Returns true if the punto toca a la línea.
void XC::EntMdlr::setGenMesh(bool m)
  { doGenMesh= m; }

//! @brief Returns true if the punto toca a la línea.
const bool &XC::EntMdlr::getGenMesh(void) const
  { return doGenMesh; }

//! @brief Creates a punto en la posición being passed as parameter.
XC::Pnt *XC::EntMdlr::crea_punto(const Pos3d &pos)
  { return get_preprocessor()->getCad().getPuntos().Nuevo(pos); }

//! @brief Crea los puntos en las posiciones que se pasan como parámetro.
void XC::EntMdlr::crea_puntos(const MatrizPos3d &posiciones)
  {
    if(verborrea>4)
      std::clog << "Creando puntos linea: '" << GetNombre() << "'...";   
    const size_t filas= posiciones.getNumFilas();
    const size_t cols= posiciones.getNumCols();

    if(get_preprocessor())
      {
        size_t cont= 0;
        for(size_t i= 1;i<=filas;i++)
          for(size_t j= 1;j<=cols;j++)
            {
              crea_punto(posiciones(i,j));
              cont++;
            }
      }
    else
      std::cerr << "Edge::crea_nodos; falta el preprocesador." << std::endl;
    if(verborrea>4)
      std::clog << "creados." << std::endl;
  }

XC::SetFilaI XC::EntMdlr::GetVarRefFilaI(size_t f,size_t c,const std::string &nmb)
  { return SetFilaI(*this,f,c,nmb,get_preprocessor()); }
XC::SetFilaI XC::EntMdlr::GetVarRefFilaI(const RangoIndice &rango_capas,size_t f,size_t c,const std::string &nmb)
  { return SetFilaI(*this,rango_capas,f,c,nmb,get_preprocessor()); }
XC::SetFilaI XC::EntMdlr::GetVarRefFilaI(const RangoTritriz &rango,const std::string &nmb)
  { return GetVarRefFilaI(rango.GetRangoCapas(),rango.GetRangoFilas().Inf(),rango.GetRangoCols().Inf(),nmb); }

XC::SetFilaJ XC::EntMdlr::GetVarRefFilaJ(size_t capa,size_t c,const std::string &nmb)
  { return SetFilaJ(*this,capa,c,nmb,get_preprocessor()); }
XC::SetFilaJ XC::EntMdlr::GetVarRefFilaJ(size_t capa,const RangoIndice &rango_filas,size_t c,const std::string &nmb)
  { return SetFilaJ(*this,capa,rango_filas,c,nmb,get_preprocessor()); }
XC::SetFilaJ XC::EntMdlr::GetVarRefFilaJ(const RangoTritriz &rango,const std::string &nmb)
  { return GetVarRefFilaJ(rango.GetRangoCapas().Inf(),rango.GetRangoFilas(),rango.GetRangoCols().Inf(),nmb); }

XC::SetFilaK XC::EntMdlr::GetVarRefFilaK(size_t capa,size_t f,const std::string &nmb)
  { return SetFilaK(*this,capa,f,nmb,get_preprocessor()); }
XC::SetFilaK XC::EntMdlr::GetVarRefFilaK(size_t capa,size_t f,const RangoIndice &rango_cols,const std::string &nmb)
  { return SetFilaK(*this,capa,f,rango_cols,nmb,get_preprocessor()); }
XC::SetFilaK XC::EntMdlr::GetVarRefFilaK(const RangoTritriz &rango,const std::string &nmb)
  { return GetVarRefFilaK(rango.GetRangoCapas().Inf(),rango.GetRangoFilas().Inf(),rango.GetRangoCols(),nmb); }

//! @brief Returns the cuadrado de la distancia a la posición being passed as parameter.
double XC::EntMdlr::DistanciaA2(const Pos3d &pt) const
  {
    std::cerr << nombre_clase()
              << " function DistanciaA2 not implemented." << std::endl;
    return 0.0;
  }

//! @brief Return the distancia a la posición being passed as parameter.
double XC::EntMdlr::DistanciaA(const Pos3d &pt) const
  { return sqrt(DistanciaA2(pt)); }

//! @brief Impone desplazamiento nulo en los nodos de this set.
void XC::EntMdlr::fix(const SFreedom_Constraint &spc)
  { nodos.fix(spc); }

//! @brief Destructor.
XC::EntMdlr::~EntMdlr(void)
  { clearAll(); }
