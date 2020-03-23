// -*-c++-*-
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
//EntMdlrBase.h

#ifndef ENTMDLRBASE_H
#define ENTMDLRBASE_H

#include "xc_utils/src/kernel/NamedEntity.h"
#include "xc_utils/src/utils/text/LabelContainer.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Preprocessor;
class Pnt;
class Edge;
class Face;

//!  @ingroup Preprocessor
//! 
//!  \brief Base class of the preprocessor objects.
class EntMdlrBase: public NamedEntity, public MovableObject
  {
  protected:
    LabelContainer labels; //!< Label container.

    bool check_preprocessor(void) const;
    int sendIdsLabels(const int &,const int &, CommParameters &);
    int recvIdsLabels(const int &,const int &, const CommParameters &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    EntMdlrBase(const std::string &nmb="",Preprocessor *preprocessor= nullptr);
    EntMdlrBase &operator+=(const EntMdlrBase &);
    EntMdlrBase &operator-=(const EntMdlrBase &);
    EntMdlrBase &operator*=(const EntMdlrBase &);
    virtual bool operator==(const EntMdlrBase &) const;
    const Preprocessor *getPreprocessor(void) const;
    Preprocessor *getPreprocessor(void);
    inline const LabelContainer &getLabels(void) const
      { return labels; }

    size_t getTag(void) const;

    Pnt *BuscaPnt(const size_t &);    
    const Pnt *BuscaPnt(const size_t &) const;
    Edge *BuscaEdge(const size_t &);    
    const Edge *BuscaEdge(const size_t &) const;
    Face *BuscaFace(const size_t &);    
    const Face *BuscaFace(const size_t &) const;
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

EntMdlrBase operator+(const EntMdlrBase &,const EntMdlrBase &);
EntMdlrBase operator-(const EntMdlrBase &,const EntMdlrBase &);
EntMdlrBase operator*(const EntMdlrBase &,const EntMdlrBase &);
std::ostream &operator<<(std::ostream &os, const EntMdlrBase &e);

} //end of XC namespace
#endif
