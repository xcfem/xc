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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:48 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/EigenIntegrator.h,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/eigenIntegrator/EigenIntegrator.h
//
// Written: Jun Peng
// Created: Wed Jan 27, 1999
// Revision: A
//
// Description: This file contains the class definition of EigenIntegrator.
// EigenIntegrator is an algorithmic class for setting up the finite element 
// equations for a eigen problem analysis. 
//
// This class is inheritanted from the base class of Integrator which was
// created by fmk (Frank).


#ifndef EigenIntegrator_h
#define EigenIntegrator_h

#include <solution/analysis/integrator/Integrator.h>

namespace XC {
class EigenSOE;
class AnalysisModel;
class FE_Element;
class DOF_Group;
class Vector;

//! @ingroup AnalisisIntegrator
//
//! @defgroup EigenIntegrator Integración de problemas de autovalores.
//
//! @ingroup EigenIntegrator
//
//! @brief Integrador para problemas de autovalores.
class EigenIntegrator : public Integrator
  {
  protected:
    int flagK;

    EigenSOE *getEigenSOEPtr(void);
    const EigenSOE *getEigenSOEPtr(void) const;

    friend class SoluMethod;
    EigenIntegrator(SoluMethod *);
    Integrator *getCopy(void) const;
  public:
     
    // methods to form the M and K matrices.
    virtual int formK(void);
    virtual int formM(void);
     
    // methods to instruct the FE_Element and DOF_Group objects
    // how to determing their contribution to M and K
    virtual int formEleTangK(FE_Element *theEle);
    virtual int formEleTangM(FE_Element *theEle);
    virtual int formNodTangM(DOF_Group *theDof);
    virtual int update(const Vector &deltaU);

    virtual int formEleTangent(FE_Element *theEle);
    virtual int formNodTangent(DOF_Group *theDof);
    virtual int formEleResidual(FE_Element *theEle);
    virtual int formNodUnbalance(DOF_Group *theDof);

    virtual int newStep(void);
    
    virtual int getLastResponse(Vector &result, const ID &id);

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);     
  };
inline Integrator *EigenIntegrator::getCopy(void) const
  { return new EigenIntegrator(*this); }
} // end of XC namespace

#endif




