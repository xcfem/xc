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
//SurfaceMap.h

#ifndef MAPSurfaces_H
#define MAPSurfaces_H

#include "EntityMap.h"
#include "preprocessor/multi_block_topology/entities/2d/Face.h"

namespace XC {
  class QuadSurface;

//! @ingroup MultiBlockTopology
//
//! @brief Model points container.
class SurfaceMap: public EntityMap<Face>
  {
  private:
    void UpdateSets(Face *) const;
  public:
    SurfaceMap(MultiBlockTopology *mbt= nullptr);

    bool conciliaNDivs(void);
    bool checkNDivs(void) const;

    template <class F>
    Face *New(void);
    Face *findOrCreateFace(Pnt *,Pnt *,Pnt *,Pnt *);

    QuadSurface *newQuadSurfacePts(const size_t &, const size_t &,const size_t &,const size_t &);
    QuadSurface *newQuadSurfaceLines(const size_t &, const size_t &,const size_t &,const size_t &);
    QuadSurface *newQuadSurfaceGridPoints(const boost::python::list &);
  };


//! @brief Creates a new surface.
template <class F>
Face *SurfaceMap::New(void)
  {
    Face *retval= busca(getTag());
    if(!retval) //Surface is new.
      {
        Preprocessor *preprocessor= getPreprocessor();
        retval= new F(preprocessor);
        if(retval)
          {
            retval->Name()= "f"+boost::lexical_cast<std::string>(getTag());
            (*this)[getTag()]= retval;
            UpdateSets(retval);
            tag++;
	  }
      }
    return retval;
  }
} //end of XC namespace
#endif
