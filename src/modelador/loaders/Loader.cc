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
//Loader.cc

#include "Loader.h"
#include "modelador/Modelador.h"
#include "domain/domain/Domain.h"
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"


//! @brief Constructor por defecto.
XC::Loader::Loader(Modelador *owr)
  : EntCmd(owr), MovableObject(0), modelador(owr) 
  {}

//! @brief Devuelve el dominio en el que se cargan los elementos.
XC::Domain *XC::Loader::getDomain(void) const
  {
    if(modelador)
      return modelador->GetDominio();
    else
      {
	std::cerr << "(Loader) necesito un modelador." << std::endl;
        return nullptr;
      }
  }

//! @brief Asigna el modelador.
void XC::Loader::set_modelador(Modelador *mdlr)
  { modelador= mdlr; }

//! @brief Procesa los comandos que se emplean para definir
//! el modelo de elementos finitos. Interpreta
//! los siguientes comandos:
//!
//! - dom: Lanza el intérprete de comandos del dominio.
bool XC::Loader::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(Loader) Procesando comando: " << cmd << std::endl;
    if(cmd=="dom")
      {
        Domain *tmp= getDomain();
        if(tmp)
          tmp->LeeCmd(status);
        else
	  std::cerr << "(Loader) Procesando comando: " << cmd
                    << "; el apuntador al dominio es nulo." << std::endl;
        return true;
      }
    else    
      return EntCmd::procesa_comando(status);
  }

XC::Loader::~Loader(void)
  { modelador= nullptr; }

//! @brief Envía el objeto por el canal que se pasa como parámetro.
int XC::Loader::sendSelf(CommParameters &cp)
  {
    std::cerr << "Loader::sendSelf() no implementada.\n";    
    return -1;
  }


//! @brief Recibe el objeto por el canal que se pasa como parámetro.
int XC::Loader::recvSelf(const CommParameters &cp)
  {
    std::cerr << "Loader::recvSelf() no implementada.\n";    
    return -1;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//!
//! - mdlr: Devuelve una referencia al modelador al que pertenece este objeto.
//! - dom: Devuelve el una referencia al dominio del problema.
any_const_ptr XC::Loader::GetProp(const std::string &cod) const
  {
    if(cod=="mdlr")
      return any_const_ptr(modelador);
    else if(cod=="dom")
      {
        static Domain *dom_ptr= nullptr;
        dom_ptr= getDomain();
        return any_const_ptr(dom_ptr);
      }
    else
      return EntCmd::GetProp(cod);
  }
