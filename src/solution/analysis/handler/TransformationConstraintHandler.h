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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/28 21:35:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/TransformationConstraintHandler.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: May 1998.
// Revision: A
//
// Description: This file contains the class definition for 
// TransformationConstraintHandler. TransformationConstraintHandler is a 
// constraint handler for handling constraints using the Transformation method.
// for each element and degree-of-freedom at a node it constructs regular
// FE_Element and DOF_Groups if there is no SP_Constraint or MP_Constraint
// constraining an elements node or the node; otherwise a TransformationFE
// element and a TransformationDOF_Group are created. 
//
// What: "@(#) TransformationConstraintHandler.h, revA"

#ifndef TransformationConstraintHandler_h
#define TransformationConstraintHandler_h

#include <solution/analysis/handler/ConstraintHandler.h>

namespace XC {
class FE_Element;
class DOF_Group;

//! @ingroup AnalisisCH
//
//! @brief TransformationConstraintHandler is a constraint handler
//! for handling constraints using the Transformation method.
//! For each element and degree-of-freedom at a node it constructs regular
//! FE_Element and DOF_Groups if there is no SP_Constraint or MP_Constraint
//! constraining an elements node or the node; otherwise a TransformationFE
//! element and a TransformationDOF_Group are created.
class TransformationConstraintHandler : public ConstraintHandler
  {
  private:
    std::set<FE_Element *> theFEs;
    std::vector<DOF_Group *> theDOFs;

    int numDOF;
    int numConstrainedNodes;

    int numTransformationFEs;

    friend class ModelWrapper;
    friend class FEM_ObjectBroker;
    TransformationConstraintHandler(ModelWrapper *);
    ConstraintHandler *getCopy(void) const;
  public:

    int handle(const ID *nodesNumberedLast =0);
    int applyLoad();
    void clearAll(void);    
    int enforceSPs(void);    
    int doneNumberingDOF(void);        

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif




