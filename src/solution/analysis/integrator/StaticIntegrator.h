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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/StaticIntegrator.h,v $
                                                                        
                                                                        
#ifndef StaticIntegrator_h
#define StaticIntegrator_h

// File: ~/analysis/integrator/StaticIntegrator.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for StaticIntegrator.
// StaticIntegrator is an algorithmic class for setting up the finite element
// equations for a static analysis and for Incrementing the nodal displacements
// with the values in the soln vector to the LinearSOE object. 
//
// What: "@(#) StaticIntegrator.h, revA"

#include <solution/analysis/integrator/IncrementalIntegrator.h>

namespace XC {
class LinearSOE;
class AnalysisModel;
class FE_Element;
class Vector;

//! @ingroup AnalisisIntegrator
//
//! @defgroup StaticIntegrator Integración del sistema de ecuaciones estático.
//
//! @ingroup StaticIntegrator
//
//! @brief determine the next step for an analysis
class StaticIntegrator: public IncrementalIntegrator
  {
  protected:
    StaticIntegrator(SoluMethod *,int classTag);
  public:
    inline virtual ~StaticIntegrator(void) {}
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    virtual int formEleTangent(FE_Element *theEle);
    virtual int formEleResidual(FE_Element *theEle);
    virtual int formNodTangent(DOF_Group *theDof);        
    virtual int formNodUnbalance(DOF_Group *theDof);    
    
    virtual int newStep(void) =0;
  };
} // fin namespace XC

#endif

