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

#include "xc_utils/src/nucleo/EntConNmb.h"
#include "xc_basic/src/texto/LabelContainer.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Preprocessor;
class Pnt;
class Edge;
class Face;

//!  \ingroup Preprocessor
//! 
//!  \brief Base de los objetos del preprocessor.
class EntMdlrBase: public EntConNmb, public MovableObject
  {
  private:
    Preprocessor *preprocessor; //!< Preprocessor.
  protected:
    LabelContainer etiquetas;

    const Preprocessor *get_preprocessor(void) const;
    Preprocessor *get_preprocessor(void);
    int sendIdsEtiquetas(const int &,const int &, CommParameters &);
    int recvIdsEtiquetas(const int &,const int &, const CommParameters &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    EntMdlrBase(const std::string &nmb="",Preprocessor *preprocessor= NULL);
    EntMdlrBase(const EntMdlrBase &otro);
    EntMdlrBase &operator=(const EntMdlrBase &otro);
    inline const Preprocessor *GetPreprocessor(void) const
      { return get_preprocessor(); }
    inline Preprocessor *GetPreprocessor(void)
      { return get_preprocessor(); }

    size_t GetTag(void) const;

    Pnt *BuscaPnt(const size_t &);    
    const Pnt *BuscaPnt(const size_t &) const;
    Edge *BuscaEdge(const size_t &);    
    const Edge *BuscaEdge(const size_t &) const;
    Face *BuscaFace(const size_t &);    
    const Face *BuscaFace(const size_t &) const;
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

} //end of XC namespace
#endif
