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
//MapCuerpos.h

#ifndef MapCuerpos_H
#define MapCuerpos_H

#include "MapEnt.h"
#include "Body.h"

namespace XC {

//! @ingroup Cad
//
//! @brief Body container. 
class MapCuerpos: public MapEnt<Body>
  {
  private:
    void UpdateSets(Body *) const;
  protected:


  public:
    MapCuerpos(Cad *cad= nullptr);

    bool conciliaNDivs(void);
    bool checkNDivs(void) const;

    template <class B>
    Body *New(void);

  };


//! @brief Creates a new cuerpo.
template <class B>
Body *MapCuerpos::New(void)
  {
    Body *retval= busca(getTag());
    if(!retval) //It doesn't already exist.
      {
        Preprocessor *preprocessor= getPreprocessor();
        retval= new B(preprocessor);
        if(retval)
          {
            retval->Nombre()= "f"+boost::lexical_cast<std::string>(getTag());
            (*this)[getTag()]= retval;
            UpdateSets(retval);
            tag++;
	  }
      }
    return retval;
  }

} //end of XC namespace
#endif
