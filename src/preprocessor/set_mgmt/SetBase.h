// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//SetBase.h

#ifndef SETBASE_H
#define SETBASE_H

//!  @ingroup Preprocessor
//! 
//!  \defgroup Set Set management.

#include "preprocessor/EntMdlrBase.h"
#include "preprocessor/MeshingParams.h"
#include "utility/matrix/Vector.h"

namespace XC {
class SFreedom_Constraint;
class Face;
class Body;
class UniformGrid;

//!  @ingroup Set
//! 
//!  \brief Base de las clases Set y SetEstruct.
class SetBase: public EntMdlrBase
  {
    Vector color;
  public:
    SetBase(const std::string &nmb="",Preprocessor *preprocessor= nullptr);
    inline virtual ~SetBase(void) {}
    virtual bool operator==(const SetBase &) const;
    void setColorComponents(const double &, const double &, const double &);
    void setColor(const Vector &);
    const Vector &getColor(void) const;
    virtual void genMesh(meshing_dir dm);
    virtual void fix(const SFreedom_Constraint &);

    virtual std::set<int> getNodeTags(void) const= 0;
    virtual std::set<int> getElementTags(void) const= 0;
    const ID &getIdNodeTags(void) const;
    const ID &getIdElementTags(void) const;


    virtual bool In(const Node *) const= 0;
    virtual bool In(const Element *) const= 0;
    virtual bool In(const Pnt *) const;
    virtual bool In(const Edge *) const;
    virtual bool In(const Face *) const;
    virtual bool In(const Body *) const;
    virtual bool In(const UniformGrid *) const;
   
    void resetTributaries(void) const;
    void computeTributaryLengths(bool initialGeometry= true) const;
    void computeTributaryAreas(bool initialGeometry= true) const;
    void computeTributaryVolumes(bool initialGeometry= true) const;

    virtual size_t getNumberOfNodes(void) const= 0;
    virtual size_t getNumberOfElements(void) const= 0;    
    virtual void createInertiaLoads(const Vector &);
  };

} //end of XC namespace
#endif
