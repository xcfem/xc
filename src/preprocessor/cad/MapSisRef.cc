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
//MapSisRef.cc

#include "MapSisRef.h"
#include "preprocessor/Preprocessor.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"

#include "preprocessor/cad/SisRef.h"
#include "preprocessor/set_mgmt/Set.h"


#include "SisRefCartesianas3d.h"

//! @brief Constructor.
XC::MapSisRef::MapSisRef(Cad *cad)
  : MapCadMember<SisRef>(cad) {}

//! @brief Crea un nuevo sistema de referencia.
XC::SisRef *XC::MapSisRef::Nuevo(const std::string &tipo)
  {
    SisRef *retval= busca(getTag());
    if(!retval) //El sistema de referencia es nuevo.
      {
        if(tipo == "cartesianas")
          {
            Preprocessor *preprocessor= getPreprocessor();
            retval= new SisRefCartesianas3d("r"+boost::lexical_cast<std::string>(getTag()),preprocessor);
            (*this)[getTag()]= retval;
            tag++;
          }
        else
	  std::cerr << "MapSisRef::NuevoSisRef; el tipo de sistema de referencia: '" << tipo
                    << "' es desconocido." << std::endl;
      }
    return retval;
  }
