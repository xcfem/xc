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
#include "xc_utils/src/base/Lista.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "xc_utils/src/base/utils_any.h"
#include "med.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::EntMdlr::EntMdlr(Preprocessor *m,const size_t &i)
  : SetEstruct("",m), idx(i), genMalla(true), nodos(), elementos() {}

//! @brief Constructor.
//! @param nombre: Identificador del objeto.
//! @param i: Índice para gráficos.
//! @param m: Apuntador al preprocesador.
XC::EntMdlr::EntMdlr(const std::string &nombre,const size_t &i,Preprocessor *m)
  : SetEstruct(nombre,m), idx(i), genMalla(true), nodos(), elementos() {}


//! @brief Constructor de copia.
XC::EntMdlr::EntMdlr(const EntMdlr &otro)
  : SetEstruct(otro), idx(otro.idx), genMalla(true), nodos(otro.nodos), elementos(otro.elementos) {}

//! @brief Operador asignación.
XC::EntMdlr &XC::EntMdlr::operator=(const EntMdlr &otro)
  {
    SetEstruct::operator=(otro);
    idx= otro.idx;
    genMalla= otro.genMalla;
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
    std::cerr << "EntMdlr::getVtkCellType: la función getVtkCellType debe definirse en las clases derivadas." << std::endl;
    return VTK_EMPTY_CELL;
  }

//! @brief Interfaz con el formato MED de Salome.
int XC::EntMdlr::getMEDCellType(void) const
  {
    std::cerr << "EntMdlr::getMEDCellType: la función getMEDCellType debe definirse en las clases derivadas." << std::endl;
    return MED_NONE;
  }

//! @brief Borra todo el contenido del objeto.
void XC::EntMdlr::clearAll(void)
  {
    SetEstruct::clearProps();
    BorraPtrNodElem();
  }

//! @brief Borra los punteros a nodos y elementos.
void XC::EntMdlr::BorraPtrNodElem(void)
  {
    nodos.clearAll();
    elementos.clearAll();
  }

//! @brief Devuelve un puntero al nodo cuyos índices se pasan como parámetros.
XC::Node *XC::EntMdlr::GetNodo(const size_t &i,const size_t &j,const size_t &k)
  {
    if(!nodos.Null())
      return nodos(i,j,k);
    else
      return nullptr;
  }

//! @brief Devuelve un puntero al nodo cuyos índices se pasan como parámetros.
const XC::Node *XC::EntMdlr::GetNodo(const size_t &i,const size_t &j,const size_t &k) const
  {
    if(!nodos.Null())
      return nodos(i,j,k);
    else
      return nullptr;
  }

//! @brief Devuelve el nodo más próximo al punto que se pasa como parámetro.
XC::Node *XC::EntMdlr::getNearestNode(const Pos3d &p)
  { return nodos.getNearestNode(p); }

//! @brief Devuelve el nodo más próximo al punto que se pasa como parámetro.
const XC::Node *XC::EntMdlr::getNearestNode(const Pos3d &p) const
  {
    EntMdlr *this_no_const= const_cast<EntMdlr *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Devuelve los índices del nodo que se paras como parámetro.
XC::ID XC::EntMdlr::getNodeIndices(const Node *n) const
  { return nodos.getNodeIndices(n); }

//! @brief Devuelve los tags de los nodos.
std::vector<int> XC::EntMdlr::getTagsNodos(void) const
  { return nodos.getTags(); }

//! @brief Devuelve un puntero al elemento cuyos índices se pasan como parámetros.
XC::Element *XC::EntMdlr::GetElemento(const size_t &i,const size_t &j,const size_t &k)
  {
    if(!elementos.Null())
      return elementos(i,j,k);
    else
      return nullptr;
  }

//! @brief Devuelve un puntero al elemento cuyos índices se pasan como parámetros.
const XC::Element *XC::EntMdlr::GetElemento(const size_t &i,const size_t &j,const size_t &k) const
  {
    if(!elementos.Null())
      return elementos(i,j,k);
    else
      return nullptr;
  }

//! @brief Devuelve el elemento más próximo al punto que se pasa como parámetro.
XC::Element *XC::EntMdlr::getNearestElement(const Pos3d &p)
  { return elementos.getNearestElement(p); }

//! @brief Devuelve el elemento más próximo al punto que se pasa como parámetro.
const XC::Element *XC::EntMdlr::getNearestElement(const Pos3d &p) const
  {
    EntMdlr *this_no_const= const_cast<EntMdlr *>(this);
    return this_no_const->getNearestElement(p);
  }

//! @brief Devuelve un puntero al nodo cuyo identificador se pasa como parámetro.
XC::Node *XC::EntMdlr::buscaNodo(const int &tag)
  { return nodos.buscaNodo(tag); }
//! @brief Devuelve un puntero al nodo cuyo identificador se pasa como parámetro.
const XC::Node *XC::EntMdlr::buscaNodo(const int &tag) const
  { return nodos.buscaNodo(tag); }
//! @brief Devuelve un puntero al elemento cuyo identificador se pasa como parámetro.
XC::Element *XC::EntMdlr::buscaElemento(const int &tag)
  { return elementos.buscaElemento(tag); }
//! @brief Devuelve un puntero al elemento cuyo identificador se pasa como parámetro.
const XC::Element *XC::EntMdlr::buscaElemento(const int &tag) const
  { return elementos.buscaElemento(tag); }

//! @brief Crea un conjunto que corresponde a una fila de nodos y elementos.
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
	  std::cerr <<  "EntMdlr::crea_set_fila; no se pudo crear el conjunto fila." << std::endl;
      }
    else
      {
	std::cerr << "EntMdlr::crea_set_fila; falta un puntero al preprocesador." << std::endl;
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

//! @brief Crea un nodos en la posición que se pasa como parámetro.
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
bool XC::EntMdlr::crea_elementos(dir_mallado dm)
  {
    bool retval= false;
    if(!nodos.empty())
      {
        if(nodos.HasNull())
          std::cerr << "EntMdlr::crea_elementos; existen punteros a nodo nulos."
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
                      elementos= smll->coloca_en_malla(nodos,dm);
                      agrega_elementos(elementos);
                      retval= true;
                    }
                  else if(verborrea>0)
                    std::clog << "EntMdlr::crea_elementos; no se estableció el elemento semilla." << std::endl;
                  if(verborrea>4)
                    std::clog << "creados." << std::endl;
                }
              else
                std::cerr << "EntMdlr::crea_elementos; falta un puntero al preprocesador." << std::endl;
            }
      }
    else
      std::cerr << "EntMdlr::crea_elementos; no hay nodos para los elementos." << std::endl;

    return retval;
  }

//! @brief Devuelve verdadero si el punto toca a la línea.
void XC::EntMdlr::setGenMalla(bool m)
  { genMalla= m; }

//! @brief Devuelve verdadero si el punto toca a la línea.
const bool &XC::EntMdlr::getGenMalla(void) const
  { return genMalla; }

//! @brief Crea un punto en la posición que se pasa como parámetro.
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

//! @brief Devuelve el cuadrado de la distancia a la posición que se pasa como parámetro.
double XC::EntMdlr::DistanciaA2(const Pos3d &pt) const
  {
    std::cerr << nombre_clase()
              << " la función DistanciaA2 no está definida." << std::endl;
    return 0.0;
  }

//! @brief Devuelve la distancia a la posición que se pasa como parámetro.
double XC::EntMdlr::DistanciaA(const Pos3d &pt) const
  { return sqrt(DistanciaA2(pt)); }

//! @brief Impone desplazamiento nulo en los nodos de este conjunto.
void XC::EntMdlr::fix(const SP_Constraint &spc)
  { nodos.fix(spc); }

//! @brief Destructor.
XC::EntMdlr::~EntMdlr(void)
  { clearAll(); }
