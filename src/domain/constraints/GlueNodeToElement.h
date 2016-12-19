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
                                                                        
#ifndef GlueNodeToElement_h
#define GlueNodeToElement_h

// File: ~/domain/constraints/GlueNodeToElement.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class definition for GlueNodeToElement.
// GlueNodeToElement is a class which stores the information for a multiple
// retained nodes constraint. A multipoint constraint relates certain dofs at 
// a constrained node to be related to certains dofs at multiple retained nodes:

// The GlueNodeToElement class assumes time invariant constraints, i.e. the
// constraint matrix does not change over time. All the methods are declared
// as pure virtual, which will allow subclasses for time varying constraints.
//

#include "MRMFreedom_Constraint.h"


namespace XC {

//! @ingroup CCont
//! @defgroup CContMP Condiciones de contorno multinodales.
//
//! @ingroup CContMP
//
//! @brief Glue a node to an element.
class GlueNodeToElement: public MRMFreedom_Constraint
  {

  public:
    // constructors        
    GlueNodeToElement(int tag); // LCPT
    GlueNodeToElement(int tag, const Node &,const Element &, const ID &);
  };
} // end of XC namespace

#endif

