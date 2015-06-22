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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/14 23:00:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Newmark1.h,v $
                                                                        
                                                                        
#ifndef Newmark1_h
#define Newmark1_h

// File: ~/analysis/integrator/Newmark1.h
// 
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for Newmark1.
// Newmark1 is an algorithmic class for performing a transient analysis
// using the Newmark1 integration scheme.
//
// What: "@(#) Newmark1.h, revA"

#include <solution/analysis/integrator/transient/newmark/NewmarkBase2.h>

namespace XC {

//! @ingroup NewmarkIntegrator
//
//! @brief Newmark1 is an algorithmic class
//! for performing a transient analysis
//! using the Newmark1 integration scheme.
class Newmark1 : public NewmarkBase2
  {
  private:
    double c4;  //!< some constants we need to keep
    ResponseQuantities Up;  //!< predicted response quantities at time t+deltat for 1st iter 
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class SoluMethod;
    Newmark1(SoluMethod *);
    Newmark1(SoluMethod *,double gamma, double beta, bool disp = true);
    Newmark1(SoluMethod *,double gamma, double beta,const RayleighDampingFactors &rF); 
    Integrator *getCopy(void) const;
  public:
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);        

    int domainChanged(void);    
    int initialize(void);    
    int newStep(double deltaT);    
    int revertToLastStep(void);            
    int update(const Vector &deltaU);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);  
  };
inline Integrator *Newmark1::getCopy(void) const
  { return new Newmark1(*this); }
} // fin namespace XC

#endif

