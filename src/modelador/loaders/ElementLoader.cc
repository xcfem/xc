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
//ElementLoader.cc

#include "ElementLoader.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/Element.h"
#include "modelador/Modelador.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/utils_any.h"
#include "boost/any.hpp"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/node/Node.h"
#include "utility/tagged/DefaultTag.h"

void XC::ElementLoader::SeedElemLoader::libera(void)
  {
    if(semilla) delete semilla;
    semilla= nullptr;
  }

//! @brief Constructor de copia.
XC::ElementLoader::SeedElemLoader::SeedElemLoader(const SeedElemLoader &otro)
  : ProtoElementLoader(otro), semilla(otro.semilla->getCopy()) {}

//! @brief Operador asignación.
XC::ElementLoader::SeedElemLoader &XC::ElementLoader::SeedElemLoader::operator=(const SeedElemLoader &otro)
  {
    ProtoElementLoader::operator=(otro);
    semilla= otro.semilla->getCopy();
    return *this;
  }

//! @brief Método agrega para la clase encargada de leer el elemento semilla.
void XC::ElementLoader::SeedElemLoader::agrega(XC::Element *e)
  {
    libera();
    assert(e);
    semilla= e;
  }

//! @brief Lee un objeto SeedElemLoader desde archivo
bool XC::ElementLoader::SeedElemLoader::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(SeedElemLoader) Procesando comando: " << cmd << std::endl;
    if(!modelador)
      {
        std::cerr << "(SeedElemLoader) necesito un modelador." << std::endl;
        return false;
      }
    else
      return ProtoElementLoader::procesa_comando(status);
  }

//! @brief Destructor de la clase encargada de leer el elemento semilla.
XC::ElementLoader::SeedElemLoader::~SeedElemLoader(void)
  { libera(); }

void XC::ElementLoader::SeedElemLoader::clearAll(void)
  { libera(); }

int XC::ElementLoader::SeedElemLoader::getDefaultTag(void) const
  {
    int retval= -1;
    const ProtoElementLoader *ldr= dynamic_cast<const ProtoElementLoader *>(Owner());
    if(ldr)
      retval= ldr->getDefaultTag();
    else
      std::cerr << "Error en SeedElemLoader::getTagElem." << std::endl;
    return retval;
  }

XC::ElementLoader::ElementLoader(Modelador *mdlr)
  : ProtoElementLoader(mdlr), seed_elem_loader(mdlr) 
  { seed_elem_loader.set_owner(this); }



//! @brief Lee un objeto ElementLoader desde archivo
bool XC::ElementLoader::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ElementLoader) Procesando comando: " << cmd << std::endl;
    if(!modelador)
      {
        std::cerr << "(ElementLoader) necesito un modelador." << std::endl;
        return false;
      }
    //Si el comando tiene tag lo leemos
    std::deque<boost::any> fnc_indices= status.Parser().SeparaIndices(this);
    static int tag_elem= 0;
    if(fnc_indices.size()>0)
      {
        tag_elem= convert_to_int(fnc_indices[0]); //Tag del elemento.
        Element::getDefaultTag().setTag(tag_elem);
      }

    if(cmd=="element")
      {
        Element *tmp= getElement(tag_elem);
        tmp->LeeCmd(status);
        return true;
      }
    else if(cmd == "seed_elem_loader")
      {
        seed_elem_loader.LeeCmd(status);
        return true;
      }
    else if(cmd == "tag_elemento")
      {
	Element::getDefaultTag().setTag((interpretaInt(status.GetString())));
        return true;
      }
    else
      return ProtoElementLoader::procesa_comando(status);
  }

//! @brief Devuelve el tag por defecto para el siguiente elemento.
int XC::ElementLoader::getDefaultTag(void) const
  { return Element::getDefaultTag().getTag(); }

//! @brief Asigna el tag por defecto para el siguiente elemento.
void XC::ElementLoader::setDefaultTag(const int &tag)
  { Element::getDefaultTag().setTag(tag); }

//! @brief Devuelve un puntero al elemento cuyo tag se pasa como parametro.
XC::Element *XC::ElementLoader::getElement(int tag)
  {
    Element *retval= nullptr;
    Domain *tmp= getDomain();
    if(tmp)
      retval= tmp->getElement(tag);
    else
      std::cerr << "XC::ElementLoader::getElement; el puntero al dominio es nulo." << std::endl;
    return retval;
  }

//! @brief Agrega el elemento al modelo.
void XC::ElementLoader::agrega(XC::Element *e)
  {
    if(e)
      {
        nuevo_elemento(e);
      }
  }

void XC::ElementLoader::clearAll(void)
  {
    seed_elem_loader.clearAll();
    Element::getDefaultTag().setTag(0);
  }

//! @brief Agrega el elemento asignándole primero el tag que corresponde,
//! se emplea en la clase EntMdlr.
void XC::ElementLoader::Agrega(XC::Element *e)
  {
    if(e)
      {
        e->setTag(Element::getDefaultTag().getTag());
        agrega(e);
	Element::getDefaultTag()++;
      }
  }


//! @brief Añade un nuevo elemento al modelo.
void XC::ElementLoader::nuevo_elemento(Element *e)
  {
    getDomain()->addElement(e);
    modelador->UpdateSets(e);
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
any_const_ptr XC::ElementLoader::GetProp(const std::string &cod) const
  {
    return ProtoElementLoader::GetProp(cod);
  }
