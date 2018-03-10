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
    size_t ndof_def_node; //!< Default number of DOFs for new nodes.
    size_t ncoo_def_node; //!< Number of coordinates for new nodes (1,2 or 3).
    Node *seed_node; //!< Seed node for semi-automatic meshing.
    void free_mem(void);
    Node *new_node(const int &tag,const size_t &dim,const int &ndof,const double &x,const double &y=0.0,const double &z=0.0);
  public:
    NodeLoader(Preprocessor *);
    virtual ~NodeLoader(void);
    const Node *get_seed_node(void) const
      { return seed_node; }
    Node *newNode(const double &x,const double &y,const double &z);
    Node *newNode(const double &x,const double &y);
    Node *newNode(const double &x);
    Node *newNode(const Pos3d &p);
    Node *newNode(const Pos2d &p);
    Node *newNode(const Vector &);
    Node *newSeedNode(void);
    Node *newNodeIDXYZ(const int &,const double &,const double &,const double &);
    Node *newNodeIDXY(const int &,const double &,const double &);
    Node *newNodeIDV(const int &,const Vector &);
    Node *duplicateNode(const int &);

    size_t getDimEspacio(void) const
      { return ncoo_def_node; }
    void setDimEspacio(const size_t &dim)
      { ncoo_def_node= dim; }
    void setNumDOFs(const size_t &ndof)
      { ndof_def_node= ndof; }
    size_t getNumDOFs(void) const
      { return ndof_def_node; }
    Node *getNode(const int &tag);
    int getDefaultTag(void) const;
    void setDefaultTag(const int &tag);
    void clearAll(void);

    void calculateNodalReactions(bool inclInertia);

  };

} // end of XC namespace

#endif
