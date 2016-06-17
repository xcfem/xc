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
#include "preprocessor/Preprocessor.h"
#include "domain/domain/Domain.h"

#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"


//! @brief Constructor por defecto.
XC::Loader::Loader(Preprocessor *owr)
  : EntCmd(owr), MovableObject(0), preprocessor(owr) 
  {}

//! @brief Devuelve el dominio en el que se cargan los elementos.
XC::Domain *XC::Loader::getDomain(void) const
  {
    if(preprocessor)
      return preprocessor->GetDominio();
    else
      {
	std::cerr << "(Loader) necesito un preprocesador." << std::endl;
        return nullptr;
      }
  }

//! @brief Asigna el preprocesador.
void XC::Loader::set_preprocessor(Preprocessor *p)
  { preprocessor= p; }

XC::Loader::~Loader(void)
  { preprocessor= nullptr; }

//! @brief Sends object through the channel being passed as parameter.
int XC::Loader::sendSelf(CommParameters &cp)
  {
    std::cerr << "Loader::sendSelf() no implementada.\n";    
    return -1;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::Loader::recvSelf(const CommParameters &cp)
  {
    std::cerr << "Loader::recvSelf() no implementada.\n";    
    return -1;
  }

