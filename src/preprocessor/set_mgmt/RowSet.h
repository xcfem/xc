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
//RowSet.h

#ifndef ROWSET_H
#define ROWSET_H

#include "SetEstruct.h"
#include "xc_utils/src/matrices/3d_arrays/Array3dRange.h"

namespace XC {

class SFreedom_Constraint;

//!  @ingroup Set
//! 
//!  @brief Set of objects in a row.
//! 
//!  A RowSet object contains 0 or more:
//!  - Nodes.
//!  - Elements.
//!  that correspond to a row_[ijk] of an EntMdlr object.
template <class ROWTTZNOD,class ROWTTZELEM>
class RowSet: public SetEstruct
  {
    ROWTTZNOD node_row; //!< Reference to the nodes of a row.
    ROWTTZELEM element_row; //!< Reference to the elements of a row.
  public:
    typedef typename ROWTTZNOD::reference reference_nod;
    typedef typename ROWTTZNOD::const_reference const_reference_nod; 

    typedef typename ROWTTZELEM::reference reference_elem;
    typedef typename ROWTTZELEM::const_reference const_reference_elem; 
  public:
    RowSet(const ROWTTZNOD &fn,const ROWTTZELEM &fe,const std::string &nmb="",Preprocessor *preprocessor= nullptr);
    RowSet(const RowSet &);
    RowSet &operator=(const RowSet &);
    virtual SetEstruct *getCopy(void) const;
    reference_nod Node(const size_t &i)
      { return node_row(i); }
    const reference_nod Node(const size_t &i) const
      { return node_row(i); }
    reference_elem Element(const size_t &i)
      { return element_row(i); }
    const reference_elem Element(const size_t &i) const
      { return element_row(i); }

    Array3dRange NodeRange(void) const
      { return Array3dRange(node_row); }
    Array3dRange ElementRange(void) const
      { return Array3dRange(element_row); }

    virtual size_t getNumNodeLayers(void) const
      { return node_row.getNumberOfLayers(); }
    virtual size_t getNumNodeRows(void) const
      { return node_row.getNumberOfRows(); }
    virtual size_t getNumNodeColumns(void) const
      { return node_row.getNumberOfColumns(); }
    virtual size_t getNumElementLayers(void) const
      { return element_row.getNumberOfLayers(); }
    virtual size_t getNumElementRows(void) const
      { return element_row.getNumberOfRows(); }
    virtual size_t getNumElementColumns(void) const
      { return element_row.getNumberOfColumns(); }

    virtual XC::Node *getNode(const size_t &i=1,const size_t &j=1,const size_t &k=1)
      { return node_row(i,j,k); }
    virtual const XC::Node *getNode(const size_t &i=1,const size_t &j=1,const size_t &k=1) const
      { return node_row(i,j,k); }
    virtual XC::Element *getElement(const size_t &i=1,const size_t &j=1,const size_t &k=1)
      { return element_row(i,j,k); }
    virtual const XC::Element *getElement(const size_t &i=1,const size_t &j=1,const size_t &k=1) const
      { return element_row(i,j,k); }

    //void fix(int &tag_fix,const SFreedom_Constraint &);

  };

template <class ROWTTZNOD,class ROWTTZELEM>
RowSet<ROWTTZNOD,ROWTTZELEM>::RowSet(const ROWTTZNOD &fn,const ROWTTZELEM &fe,const std::string &nmb,Preprocessor *preprocessor)
  : SetEstruct(nmb,preprocessor), node_row(fn), element_row(fe) {}

template <class ROWTTZNOD,class ROWTTZELEM>
RowSet<ROWTTZNOD,ROWTTZELEM>::RowSet(const RowSet &other)
  : SetEstruct(other), node_row(other.node_row), element_row(other.element_row) {}

  //! @brief Assignment operator.
template <class ROWTTZNOD,class ROWTTZELEM>
RowSet<ROWTTZNOD,ROWTTZELEM> &XC::RowSet<ROWTTZNOD,ROWTTZELEM>::operator=(const RowSet &other)
  {
    SetEstruct::operator=(other);
    node_row= other.node_row;
    element_row= other.element_row;
    return *this;
  }

//! @brief Virtual constructor.
template <class ROWTTZNOD,class ROWTTZELEM>
SetEstruct *XC::RowSet<ROWTTZNOD,ROWTTZELEM>::getCopy(void) const
  { return new RowSet<ROWTTZNOD,ROWTTZELEM>(*this); }


/* //! @brief Impone desplazamiento nulo en the nodes de this set. */
/* template <class ROWTTZNOD,class ROWTTZELEM> */
/* void XC::RowSet<ROWTTZNOD,ROWTTZELEM>::fix(const SFreedom_Constraint &spc) */
/*   { fix(node_row,spc); } */


} //end of XC namespace
#endif
