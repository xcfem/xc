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
//SetBase.h

#ifndef SETBASE_H
#define SETBASE_H

//!  \ingroup Preprocessor
//! 
//!  \defgroup Set Manejo de conjuntos.

#include "preprocessor/EntMdlrBase.h"
#include "preprocessor/ParamMallado.h"

class Lista;

namespace XC {
class SFreedom_Constraint;
class Face;
class Body;
class UniformGrid;

//!  \ingroup Set
//! 
//!  \brief Base de las clases Set y SetEstruct.
class SetBase: public EntMdlrBase
  {
  public:
    SetBase(const std::string &nmb="",Preprocessor *preprocessor= NULL);
    inline virtual ~SetBase(void)
      {}
    virtual void Malla(dir_mallado dm);
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
   
    void resetTributarias(void) const;
    void calculaLongsTributarias(bool initialGeometry= true) const;
    void calculaAreasTributarias(bool initialGeometry= true) const;
    void calculaVolsTributarios(bool initialGeometry= true) const;

    virtual size_t NumNodos(void) const= 0;
    virtual size_t NumElementos(void) const= 0;
  };

} //end of XC namespace
#endif
