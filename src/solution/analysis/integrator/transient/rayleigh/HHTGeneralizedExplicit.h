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
// $Date: 2005/12/21 00:32:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/HHTGeneralizedExplicit.h,v $


#ifndef HHTGeneralizedExplicit_h
#define HHTGeneralizedExplicit_h

// File: ~/analysis/integrator/HHTGeneralizedExplicit.h
// 
// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 10/05
// Revision: A
//
// Description: This file contains the class definition for HHTGeneralizedExplicit.
// HHTGeneralizedExplicit is an algorithmic class for performing a transient analysis
// using the HHTGeneralizedExplicit integration scheme.
//
// What: "@(#) HHTGeneralizedExplicit.h, revA"

#include "solution/analysis/integrator/transient/rayleigh/HHTBase.h"

namespace XC {

//! @ingroup RayleighIntegrator
//
//! @brief HHTGeneralizedExplicit is an algorithmic class
//! for performing a transient analysis
//! using the HHTGeneralizedExplicit integration scheme.
class HHTGeneralizedExplicit : public HHTBase
  {
  private:
    double alphaF;
    int updateCount; //!< method should only have one update per step
    inline const double &alphaI(void) const
      { return alpha; }
    inline double &alphaI(void)
      { return alpha; }
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class SoluMethod;
    HHTGeneralizedExplicit(SoluMethod *);
    HHTGeneralizedExplicit(SoluMethod *,double rhoB, double alphaF);
    HHTGeneralizedExplicit(SoluMethod *,double rhoB, double alphaF,const RayleighDampingFactors &rF);
    HHTGeneralizedExplicit(SoluMethod *,double alphaI, double alphaF, double beta, double gamma);
    HHTGeneralizedExplicit(SoluMethod *,double alphaI, double alphaF, double beta, double gamma,const RayleighDampingFactors &rF);
    Integrator *getCopy(void) const;
  public:
    
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);        
    
    int domainChanged(void);    
    int newStep(double deltaT);    
    int revertToLastStep(void);        
    int update(const Vector &aiPlusOne);
    int commit(void);
    
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);        
  };
inline Integrator *HHTGeneralizedExplicit::getCopy(void) const
  { return new HHTGeneralizedExplicit(*this); }
} // end of XC namespace

#endif
