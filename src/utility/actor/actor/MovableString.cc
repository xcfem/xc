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
//MovableString.cc

#include "MovableString.h"
#include "utility/actor/channel/Channel.h"
#include "utility/actor/objectBroker/FEM_ObjectBroker.h"
#include "utility/matrix/ID.h"

//! @brief Envía la cadena de caracteres a través del canal que se pasa como parámetro.
const XC::ID &stringToId(const std::string &str)
  {
    static XC::ID retval;
    static const int ratio= sizeof(int)/sizeof(char);
    const int *intPtr= reinterpret_cast<const int *>(str.c_str());
    const size_t sz= str.size()/ratio;
    retval= XC::ID(intPtr,sz+1);
    return retval;
  }

//! @brief Recibe la cadena de caracteres a través del canal que se pasa como parámetro.
const std::string &idToString(const XC::ID &id)
  {
    static std::string retval;
    const size_t sz= id.Size();
    if(sz>0)
      {
        const char *charPtr= reinterpret_cast<const char *>(id.getDataPtr());
        retval= std::string(charPtr);
      }
    else
      retval= "";
    return retval;
  }

//! @brief Constructor.
XC::MovableString::MovableString(const std::string &v)
  :MovableID(1) 
  { setString(v); }

//! @brief Asigna la cadena de caracteres.
void XC::MovableString::setString(const std::string &v)
  { setID(stringToId(v)); }

const std::string &XC::MovableString::getString(void)
  { return idToString(*this); }




