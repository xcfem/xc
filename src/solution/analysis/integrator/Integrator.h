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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/Integrator.h,v $
                                                                        
                                                                        
#ifndef Integrator_h
#define Integrator_h

// File: ~/analysis/integrator/Integrator.h
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class interface for Integrator.
// Integrator is an abstract base class, i.e. no objects of it's
// type can be created. 
//
// What: "@(#) Integrator.h, revA"

#include <utility/actor/actor/MovableObject.h>
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
class FE_Element;
class DOF_Group;
class SystemOfEqn;
class AnalysisModel;
class Vector;
class ID;
class FEM_ObjectBroker;
class RayleighDampingFactors;
class SoluMethod;

//! @ingroup Analisis
//
//! @defgroup AnalisisIntegrator Integración de las ecuaciones del sistema.
//
//! @ingroup AnalisisIntegrator
//
//! @brief IncrementalIntegrator is an algorithmic class for setting up the finite 
//! element equations in an analysis.
class Integrator: public MovableObject, public EntCmd
  {
  protected:
    SoluMethod *getSoluMethod(void);
    const SoluMethod *getSoluMethod(void) const;
    void applyLoadModel(double newTime);
    int updateModel(void);
    int updateModel(double newTime, double dT);
    int commitModel(void);
    double getCurrentModelTime(void);
    void setCurrentModelTime(const double &t);
    void setRayleighDampingFactors(const RayleighDampingFactors &rF);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    bool procesa_comando(CmdStatus &status);
    virtual AnalysisModel *getAnalysisModelPtr(void);
    virtual const AnalysisModel *getAnalysisModelPtr(void) const;

    Integrator(SoluMethod *,int classTag);
    friend class SoluMethod;
    virtual Integrator *getCopy(void) const= 0;
  public:
    inline virtual ~Integrator(void) {}
    virtual int domainChanged(void);

    virtual int formEleTangent(FE_Element *theEle) =0;
    virtual int formNodTangent(DOF_Group *theDof) =0;    
    virtual int formEleResidual(FE_Element *theEle) =0;
    virtual int formNodUnbalance(DOF_Group *theDof) =0;    

    // Methods provided for Domain Decomposition
    virtual int getLastResponse(Vector &result, const ID &id) =0;

    // Method provided for Output
    virtual void Print(std::ostream &s, int flag =0);
  };
} // fin namespace XC

#endif






