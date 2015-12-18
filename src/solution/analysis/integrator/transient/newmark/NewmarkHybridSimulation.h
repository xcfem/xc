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

// $Revision: 1.1 $
// $Date: 2005/12/19 22:39:21 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/NewmarkHybridSimulation.h,v $

#ifndef NewmarkHybridSimulation_h
#define NewmarkHybridSimulation_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 09/05
// Revision: A
//
// Description: This file contains the class definition for NewmarkHybridSimulation.
// NewmarkHybridSimulation is an algorithmic class for performing a transient analysis
// using the Newmark integration scheme.
//
// What: "@(#) NewmarkHybridSimulation.h, revA"

#include <solution/analysis/integrator/transient/newmark/NewmarkBase2.h>

namespace XC {
class ConvergenceTest;

//! @ingroup NewmarkIntegrator
//
//! @brief NewmarkHybridSimulation is an algorithmic
//! class for performing a transient analysis
//! using the Newmark integration scheme.
class NewmarkHybridSimulation : public NewmarkBase2
  {
  private:
    ConvergenceTest *theTest;       // convergence test
    double rFact;                   // displacement increment reduction factor
    ResponseQuantities Ut;  // response quantities at time t
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class SoluMethod;
    NewmarkHybridSimulation(SoluMethod *);
    NewmarkHybridSimulation(SoluMethod *,double gamma, double beta, ConvergenceTest &theTest);
    NewmarkHybridSimulation(SoluMethod *,double gamma, double beta, ConvergenceTest &theTest,const RayleighDampingFactors &rF); 
    Integrator *getCopy(void) const;
  public:
    
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);        
    
    int domainChanged(void);
    int newStep(double deltaT);
    int revertToLastStep(void);
    int update(const Vector &deltaU);
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);
  };
inline Integrator *NewmarkHybridSimulation::getCopy(void) const
  { return new NewmarkHybridSimulation(*this); }
} // end of XC namespace

#endif
