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
//Face.cc

#include "Face.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/geom/d3/BND3d.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "med.h"
#include "preprocessor/Preprocessor.h"
#include "preprocessor/set_mgmt/Set.h"
#include "vtkCellType.h"

//! @brief Constructor.
XC::Face::Face(void)
  : CmbEdge(nullptr,0), ndivj(0) {}

//! @brief Constructor.
XC::Face::Face(Preprocessor *m,const size_t &ndivI, const size_t &ndivJ)
  : CmbEdge(m,ndivI), ndivj(ndivJ) {}

//! @brief Constructor.
//! @param nombre: Identificador del objeto.
//! @param m: Apuntador al modelador.
//! @param nd: Número de divisiones.
XC::Face::Face(const std::string &nombre,Preprocessor *m,const size_t &ndivI, const size_t &ndivJ)
  : CmbEdge(nombre,m,ndivI), ndivj(ndivJ) {}

//! @brief Lee un objeto Face desde el archivo de entrada.
bool XC::Face::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Face) Procesando comando: " << cmd << std::endl;
    if(cmd == "ndivI")
      {
        SetNDivI(interpretaSize_t(status.GetString()));
        return true;
      }
    else if(cmd == "ndivJ")
      {
        SetNDivJ(interpretaSize_t(status.GetString()));
        return true;
      }
    else if(cmd == "ndiv") //Igual número de divisiones en ambos ejes.
      {
        const size_t tmp= interpretaSize_t(status.GetString());
        SetNDivI(tmp);
        SetNDivJ(tmp);
        return true;
      }
    else
      return CmbEdge::procesa_comando(status);
  }

//! @brief Asigna el número de divisiones en el eje i.
void XC::Face::SetNDivI(const size_t &ndi)
  { CmbEdge::ndiv= ndi; }

//! @brief Asigna el número de divisiones en el eje j.
void XC::Face::SetNDivJ(const size_t &ndj)
  { ndivj= ndj; }

//! @brief Inserta el cuerpo que se pasa como parámetro en la lista de los
//! que tocan a esta superficie.
void XC::Face::inserta_body(Body *b)
  { cuerpos_sup.insert(b); }

//! @brief Actualiza la topología.
void XC::Face::actualiza_topologia(void)
  {
    for(std::deque<Lado>::iterator i=lineas.begin();i!=lineas.end();i++)
      (*i).Borde()->inserta_surf(this);
  }

//! @brief Devuelve el índice en esta superficie del borde común con otra superficie si existe.
size_t XC::Face::BordeComun(const XC::Face &otra) const
  {
    size_t cont= 1;
    if(this == &otra) return cont; //Son la misma todos los bordes son comunes.
    for(std::deque<Lado>::const_iterator i=lineas.begin();i!=lineas.end();i++,cont++)
      {
        if((*i).Borde()->Toca(otra))
          return cont;
      }
    return 0;
  }

//! Devuelve:
//! - 1 si la línea es común a ambas superficies y la orientación es la misma.
//! - -1 si la línea es común a ambas superficies y la orientación es la contraria.
//! - 0 si la línea no es común a ambas superficies.
int XC::Face::SentidoBorde(const XC::Edge *l,const XC::Face &otra) const
  {
    //Buscamos los indices de las lineas en una 
    const size_t ind_l_esta= IndiceEdge(l);
    if(ind_l_esta == 0)
      {
        std::cerr << "La línea :" << l->GetNombre() 
                  << " no es un borde de la superficie: " << GetNombre() << std::endl;
        return 0;
      }
    const size_t ind_l_otra= otra.IndiceEdge(l);
    if(ind_l_otra == 0)
      {
        std::cerr << "La línea :" << l->GetNombre() 
                  << " no es un borde de la superficie: " << otra.GetNombre() << std::endl;
        return 0;
      }
    //Buscamos los Lado en cada superficie;
    const Lado *l_esta= GetLado(ind_l_esta);
    const Lado *l_otra= otra.GetLado(ind_l_otra);
    if(l_esta->P2() == l_otra->P2())
      return 1;
    else
      return -1;
  }

//! @brief Devuelve el vértice cuyo índice se pasa como parámetro.
const XC::Pnt *XC::Face::GetVertice(const size_t &i) const
  { return GetLado(i)->P1(); }

//! @brief Devuelve la lista de superficies que tocan a la línea.
std::set<const XC::Face *> XC::GetSupsTocan(const Edge &p)
  { return p.SupsTocan(); }

//! @brief Devuelve verdadero si la línea toca al cuerpo.
bool XC::Face::Toca(const XC::Body &b) const
  {
    std::set<const Body *>::const_iterator i= cuerpos_sup.find(&b);
    return (i!=cuerpos_sup.end());
  }

//! @brief Devuelve los conjuntos a los que pertenece esta superficie.
std::set<XC::SetBase *> XC::Face::get_sets(void) const
  {
    std::set<SetBase *> retval;
    const Preprocessor *preprocessor= GetPreprocessor();
    if(preprocessor)
      {
        MapSet &sets= const_cast<MapSet &>(preprocessor->get_sets());
        retval= sets.get_sets(this);
      }
    else
      std::cerr << "Face::get_sets; no se ha definido el modelador." << std::endl;
    return retval;
  }

//! @brief Agrega la superficie a los conjuntos que se pasan como parámetro.
void XC::Face::add_to_sets(std::set<SetBase *> &sets)
  {
    for(std::set<SetBase *>::iterator i= sets.begin();i!= sets.end();i++)
      {
        Set *s= dynamic_cast<Set *>(*i);
        if(s) s->GetSuperficies().push_back(this);
      }
  }

//! @brief Devuelve un apuntador al nodo cuyos índices se pasan como parámetro.
XC::Node *XC::Face::GetNodo(const size_t &i,const size_t &j,const size_t &k)
  { return CmbEdge::GetNodo(i,j,k); }

//! @brief Devuelve un apuntador al nodo cuyos índices se pasan como parámetro.
const XC::Node *XC::Face::GetNodo(const size_t &i,const size_t &j,const size_t &k) const
  { return CmbEdge::GetNodo(i,j,k); }

//! @brief Devuelve un apuntador al nodo cuyos índices se pasa como parámetro.
XC::Node *XC::Face::GetNodo(const size_t &i,const size_t &j)
  { return const_cast<Node *>(static_cast<const Face &>(*this).GetNodo(i,j)); }

//! @brief Devuelve un apuntador al nodo cuyos índices se pasa como parámetro.
const XC::Node *XC::Face::GetNodo(const size_t &i,const size_t &j) const
  {
    const Node *retval= nullptr;
    if(nodos.EsCapaICte())
      retval= CmbEdge::GetNodo(1,i,j);
    else if(nodos.EsCapaJCte())
      retval= CmbEdge::GetNodo(i,1,j);
    else if(nodos.EsCapaKCte())
      retval= CmbEdge::GetNodo(i,j,1);
    else
      std::cerr << "Face::GetNodo; el conjunto de nodos no es unidimensional." << std::endl;
    return retval;
  }

//! @brief Devuelve un apuntador al nodo cuyo índice se pasa como parámetro.
XC::Node *XC::Face::GetNodo(const size_t &i)
  {
    std::cerr << "No debe llamarse a Face::GetNodo con un sólo índice." << std::endl; 
    return nullptr;
  }

//! @brief Devuelve un apuntador al nodo cuyo índice se pasa como parámetro.
const XC::Node *XC::Face::GetNodo(const size_t &i) const
  { return const_cast<Node *>(static_cast<const Face &>(*this).GetNodo(i)); }

//! @brief Interfaz con VTK.
int XC::Face::getVtkCellType(void) const
  {
    int retval= VTK_EMPTY_CELL;
    const size_t nl= NumEdges();
    switch(nl)
      {
      case 1:
        retval= VTK_LINE;
      case 2:
        retval= VTK_EMPTY_CELL;
      case 3:
        retval= VTK_TRIANGLE;
      case 4:
        retval= VTK_QUAD;
      default:
        retval= VTK_POLYGON;
      }
    return retval;
  }

//! @brief Interfaz con el formato MED de Salome.
int XC::Face::getMEDCellType(void) const
  {
    const size_t nl= NumEdges();
    int retval= -1;
    switch(nl)
      {
      case 1:
        retval= MED_SEG2;
      case 2:
        retval= MED_NONE;
      case 3:
        retval= MED_TRIA3;
      case 4:
        retval= MED_QUAD4;
      default:
        retval= MED_NONE;
      }
    return retval;
  }

//! Devuelve la propiedad del objeto cuyo código se pasa
//! como parámetro.
//!
//! Soporta las propiedades:
//! -nsup: Devuelve el número de superficies que tienen por borde a esta línea.
any_const_ptr XC::Face::GetProp(const std::string &cod) const
  {
    if(cod== "num_lineas")
      {
        tmp_gp_szt= NumEdges();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="ndivI")
      {
        tmp_gp_szt= NDivI();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="ndivJ")
      {
        tmp_gp_szt= NDivJ();
        return any_const_ptr(tmp_gp_szt);
      }
    else if(cod=="getMEDCellType")
      {
        tmp_gp_int= getMEDCellType();
         return any_const_ptr(tmp_gp_int);
      }
    else if(cod=="nvol")
      {
        tmp_gp_szt= CuerposTocan().size();
        return any_const_ptr(tmp_gp_szt);
      }
    else
      return CmbEdge::GetProp(cod);
  }

