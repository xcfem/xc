//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
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
//MEDVertexInfo.h
//Wrapper for the MESHING object of MED (to export files to "salome").

#ifndef MEDVERTEXINFO_H
#define MEDVERTEXINFO_H

#include "MEDBaseInfo.h"
#include "xc_basic/src/med_xc/MEDMEM_Meshing.hxx"

namespace XC {

class Mesh;
//! @ingroup MED
//
//!  @brief Information about mesh vertex.
class MEDVertexInfo: public MEDBaseInfo
  {
    std::vector<double> coordenadas; //!< Coordinates of the vertices.
    size_t spaceDimension; //!< Mesh space dimension.
    std::string tipoCoordenadas; //!< "CARTESIAN", "CYLINDRICAL" or "SPHERICAL"
    std::vector<std::string> nombresCoordenadas; //!< Names of the coordinate components ("X","Y","Z",...).
    std::vector<std::string> nombresUnidades; //!< Names of the units ("cm","cm","cm",...).

  protected:
    void new_vertice(size_t i,const std::vector<double> &coo);

  public:
    MEDVertexInfo(void);
    MEDVertexInfo(const Mesh &);

    //! @brief Returns the number of vertices.
    inline size_t getNumVertices(void) const
      { return coordenadas.size()/spaceDimension; }
    const std::vector<double> &getCoordenadas(void) const;
    inline size_t getSpaceDimension(void) const
      { return spaceDimension; }
    inline void setSpaceDimension(const size_t &sz)
      { spaceDimension= sz; }
    inline const std::string &getTipoCoordenadas(void) const
      { return tipoCoordenadas; }
    inline void setTipoCoordenadas(const std::string &str)
      { tipoCoordenadas= str; }
    inline const std::vector<std::string> &getCoordinateNames(void) const
      { return nombresCoordenadas; }
    void setCoordinateNames(const boost::python::list &);
    inline const std::vector<std::string> &getUnitNames(void) const
      { return nombresUnidades; }
    void setUnitNames(const boost::python::list &);
    void newVertex(const size_t &i,const boost::python::list &);

    void to_med(MEDMEM::MESHING &mesh) const;
    void clear(void);

  };
} // end of XC namespace
#endif
