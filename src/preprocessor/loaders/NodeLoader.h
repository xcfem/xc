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
//NodeLoader.h

#ifndef NODELOADER_H
#define NODELOADER_H

#include "Loader.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

namespace XC {

class Node;

//!  \ingroup Lodrs
//! 
//! @brief Node creation manager.
class NodeLoader: public Loader
  {
  private:
    size_t ngdl_def_nodo; //!< Default number of DOFs for new nodes.
    size_t ncoo_def_nodo; //!< Number of coordinates for new nodes (1,2 ó 3).
    Node *nodo_semilla; //!< Seed node for semi-automatic meshing.
    void libera(void);
    Node *new_node(const int &tag,const size_t &dim,const int &ngdl,const double &x,const double &y=0.0,const double &z=0.0);
  public:
    NodeLoader(Preprocessor *);
    virtual ~NodeLoader(void);
    const Node *get_nodo_semilla(void) const
      { return nodo_semilla; }
    Node *nuevoNodo(const double &x,const double &y,const double &z);
    Node *nuevoNodo(const double &x,const double &y);
    Node *nuevoNodo(const double &x);
    Node *nuevoNodo(const Pos3d &p);
    Node *nuevoNodo(const Pos2d &p);
    Node *nuevoNodo(const Vector &);
    Node *newSeedNode(void);
    Node *newNodeIDXYZ(const int &,const double &,const double &,const double &);
    Node *newNodeIDXY(const int &,const double &,const double &);
    Node *newNodeIDV(const int &,const Vector &);
    Node *duplicateNode(const int &);

    size_t getDimEspacio(void) const
      { return ncoo_def_nodo; }
    void setDimEspacio(const size_t &dim)
      { ncoo_def_nodo= dim; }
    void setNumGdls(const size_t &ngdl)
      { ngdl_def_nodo= ngdl; }
    size_t getNumGdls(void) const
      { return ngdl_def_nodo; }
    Node *getNode(const int &tag);
    int getDefaultTag(void) const;
    void setDefaultTag(const int &tag);
    void clearAll(void);

    void calculateNodalReactions(bool inclInertia);

  };

} // end of XC namespace

#endif
