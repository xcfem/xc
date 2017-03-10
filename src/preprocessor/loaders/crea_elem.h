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
//crea_elem.h

#ifndef CREA_ELEM_H
#define CREA_ELEM_H

namespace XC {
class Element;

template <typename ELEM>
ELEM *nuevo_elemento(int tag_elem)
  {
    ELEM *retval= new ELEM(tag_elem); //Lo creamos.
    return retval;
  }

//! @brief Crea un nuevo elemento y le asigna un material.
template <typename ELEM>
ELEM *nuevo_elemento_gen_mat(int tag_elem, Material *ptrMat)
  {
    ELEM *retval= new ELEM(tag_elem,ptrMat);
    return retval;
  }

//! @brief Crea un nuevo elemento y le asigna un material.
template <typename ELEM>
ELEM *nuevo_elemento_dim_gen_mat(int tag_elem,int dime, Material *ptrMat)
  {
    ELEM *retval= new ELEM(tag_elem,dime,ptrMat);
    return retval;
  }

template <typename ELEM>
ELEM *nuevo_elemento_dim_gen_mat_dir(int tag_elem,int dime, Material *ptrMat,int d)
  {
    ELEM *retval= new ELEM(tag_elem, dime, ptrMat,d); //Lo creamos.
    return retval;
  }

template <typename ELEM>
ELEM *nuevo_elemento_gen_mat_crd(int tag_elem, Material *ptrMat, CrdTransf *ptrTrf)
  {
    ELEM *retval= new ELEM(tag_elem,ptrMat,ptrTrf); //Lo creamos.
    return retval;
  }

template <typename ELEM>
ELEM *nuevo_elemento_dim_gen_mat_crd(int tag_elem,int dime, Material *ptrMat, CrdTransf *ptrTrf)
  {
    ELEM *retval= new ELEM(tag_elem,dime,ptrMat,ptrTrf); //Lo creamos.
    return retval;
  }

template <typename ELEM>
ELEM *nuevo_elemento_ns_gen_mat_crd(int tag_elem, Material *ptrMat, int ns, CrdTransf *ptrTrf)
  {
    ELEM *retval= new ELEM(tag_elem,ns,ptrMat,ptrTrf); //Lo creamos.
    return retval;
  }

template <typename ELEM>
ELEM *nuevo_elemento_ns_gen_mat_crd_integ(int tag_elem, Material *ptrMat, int ns, CrdTransf *ptrTrf,BeamIntegration *ptrInteg)
  {
    ELEM *retval= new ELEM(tag_elem,ns,ptrMat,ptrTrf,ptrInteg); //Lo creamos.
    return retval;
  }

template <typename ELEM, typename MAT>
ELEM *nuevo_elemento_mat(int tag_elem, Material *ptrMat)
  {
    ELEM *retval= nullptr;
    const MAT *ptr_mat= dynamic_cast<const MAT *>(ptrMat);
    if(ptr_mat)
      retval= new ELEM(tag_elem,ptr_mat); //Lo creamos.
    return retval;
  }

template <typename ELEM>
ELEM *nuevo_elemento_mat_crd(int tag_elem, Material *ptrMat, CrdTransf *ptrTrf)
  {
    ELEM *retval= new ELEM(tag_elem,ptrMat,ptrTrf); //Lo creamos.
    return retval;
  }
} // end of XC namespace

#endif
