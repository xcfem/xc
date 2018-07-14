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
//QuadSurface.h

#ifndef QUADSURFACE_H
#define QUADSURFACE_H

#include "Face.h"
#include "preprocessor/multi_block_topology/matrices/PntPtrArray.h"

namespace XC {

//! @ingroup MultiBlockTopologyEnt
//!
//! @brief Quadrangle defined by its four vertices.
class QuadSurface: public Face
  {
  protected:
    Pos3dArray get_positions(void) const;
    const Edge *get_lado_homologo(const Edge *l) const;
  public:
    QuadSurface(Preprocessor *m,const size_t &ndivI= 4, const size_t &ndivJ= 4);
    virtual SetEstruct *getCopy(void) const;
    //! @brief Returns the number of vertices.
    inline virtual size_t getNumberOfVertices(void) const
      { return 4; }

    void setPoints(const ID &);
    void setPoints(const PntPtrArray &pntPtrs);
    void setPoints(const m_int &);
    void defGridPoints(const boost::python::list &);

    Vector3d getIVector(void) const;
    Vector3d getJVector(void) const;
    Vector3d getKVector(void) const;

    void SetNDiv(const Edge *l,const size_t &nd);
    virtual void ConciliaNDivIJ(void);
    virtual void SetNDivI(const size_t &ndi);
    virtual void SetNDivJ(const size_t &ndi);
    void SetElemSizeI(const double &sz);
    void SetElemSizeJ(const double &sz);
    void SetElemSizeIJ(const double &,const double &);

    bool checkNDivs(const size_t &i,const size_t &j) const;
    bool checkNDivs(void) const;
    void create_nodes(void);
    void genMesh(meshing_dir dm);
  };

} //end of XC namespace
#endif
