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
//MapSurfaces.h

#ifndef MAPSurfaces_H
#define MAPSurfaces_H

#include "MapEnt.h"
#include "Face.h"

namespace XC {
  class QuadSurface;

//! @ingroup Cad
//
//! @brief Contenedor de puntos del modelo.
class MapSurfaces: public MapEnt<Face>
  {
  private:
    void UpdateSets(Face *) const;
  protected:


  public:
    MapSurfaces(Cad *cad= NULL);

    bool conciliaNDivs(void);
    bool checkNDivs(void) const;

    template <class F>
    Face *Nueva(void);

    QuadSurface *newQuadSurfacePts(const size_t &, const size_t &,const size_t &,const size_t &);
    QuadSurface *newQuadSurfaceLines(const size_t &, const size_t &,const size_t &,const size_t &);
    QuadSurface *newQuadSurfaceGridPoints(const boost::python::list &);
  };


//! @brief Creates a new surface.
template <class F>
Face *MapSurfaces::Nueva(void)
  {
    Face *retval= busca(getTag());
    if(!retval) //Surface is new.
      {
        Preprocessor *preprocessor= getPreprocessor();
        retval= new F(preprocessor);
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
