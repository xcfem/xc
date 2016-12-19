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
                                                                        
// File: ~/domain/domain/single/SingleDomMRMFreedom_Iter.h
//
// Written: lcpt 
// Created: 03/2015
// Revision: A
//
// Description: This file contains the class definition for 
// SingleDomMRMFreedom_Iter. SingleDomMRMFreedom_Iter is an iter for returning 
// the multiple retained node constraints  of an object of class SingleDomain. 
// SingleDomMRMFreedom_Iters must be written for each subclass of SingleDomain.
// where the storage of the MRMFreedom_Constraints changes.

#ifndef SingleDomMRMFreedom_Iter_h
#define SingleDomMRMFreedom_Iter_h

#include <domain/constraints/MRMFreedom_ConstraintIter.h>
#include <domain/domain/single/SingleDomTIter.h>

namespace XC {

class SingleDomMRMFreedom_Iter: public SingleDomTIter<MRMFreedom_ConstraintIter>
  {
  public:
    SingleDomMRMFreedom_Iter(TaggedObjectStorage *theStorage);
    virtual MRMFreedom_Constraint *operator()(void);
  };
} // end of XC namespace

#endif

