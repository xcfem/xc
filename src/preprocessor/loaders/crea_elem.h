//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
    ELEM *retval= NULL;
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
