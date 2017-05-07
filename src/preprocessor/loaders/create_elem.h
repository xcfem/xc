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
//create_elem.h

#ifndef CREATE_ELEM_H
#define CREATE_ELEM_H

namespace XC {
class Element;

template <typename ELEM>
ELEM *new_element(int tag_elem)
  {
    ELEM *retval= new ELEM(tag_elem); //We create it.
    return retval;
  }

//! @brief Creates a new element and assigns it a material.
template <typename ELEM>
ELEM *new_element_gen_mat(int tag_elem, const Material *ptrMat)
  {
    ELEM *retval= new ELEM(tag_elem,ptrMat);
    return retval;
  }

//! @brief Creates a new element and assigns it a material.
template <typename ELEM>
ELEM *new_element_dim_gen_mat(int tag_elem,int dime, const Material *ptrMat)
  {
    ELEM *retval= new ELEM(tag_elem,dime,ptrMat);
    return retval;
  }

template <typename ELEM>
ELEM *new_element_dim_gen_mat_dir(int tag_elem,int dime, const Material *ptrMat,int d)
  {
    ELEM *retval= new ELEM(tag_elem, dime, ptrMat,d); //We create it.
    return retval;
  }

template <typename ELEM>
ELEM *new_element_gen_mat_crd(int tag_elem, const Material *ptrMat, const CrdTransf *ptrTrf)
  {
    ELEM *retval= new ELEM(tag_elem,ptrMat,ptrTrf); //We create it.
    return retval;
  }

template <typename ELEM>
ELEM *new_element_dim_gen_mat_crd(int tag_elem,int dime, const Material *ptrMat, const CrdTransf *ptrTrf)
  {
    ELEM *retval= new ELEM(tag_elem,dime,ptrMat,ptrTrf); //We create it.
    return retval;
  }

template <typename ELEM>
ELEM *new_element_ns_gen_mat_crd(int tag_elem, const Material *ptrMat, int ns, const CrdTransf *ptrTrf)
  {
    ELEM *retval= new ELEM(tag_elem,ns,ptrMat,ptrTrf); //We create it.
    return retval;
  }

template <typename ELEM>
ELEM *new_element_ns_gen_mat_crd_integ(int tag_elem, const Material *ptrMat, int ns, const CrdTransf *ptrTrf,const BeamIntegration *ptrInteg)
  {
    ELEM *retval= new ELEM(tag_elem,ns,ptrMat,ptrTrf,ptrInteg); //We create it.
    return retval;
  }

template <typename ELEM, typename MAT>
ELEM *new_element_mat(int tag_elem, const Material *ptrMat)
  {
    ELEM *retval= nullptr;
    const MAT *ptr_mat= dynamic_cast<const MAT *>(ptrMat);
    if(ptr_mat)
      retval= new ELEM(tag_elem,ptr_mat); //We create it.
    return retval;
  }

template <typename ELEM>
ELEM *new_element_mat_crd(int tag_elem, const Material *ptrMat, const CrdTransf *ptrTrf)
  {
    ELEM *retval= new ELEM(tag_elem,ptrMat,ptrTrf); //We create it.
    return retval;
  }
} // end of XC namespace

#endif
