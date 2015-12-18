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
//HHTBase.h,v $


#ifndef HHTBase_h
#define HHTBase_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the class definition for HHTBase.
// HHTBase is an algorithmic class for performing a transient analysis
// using the HHTBase integration scheme.
//
// What: "@(#) HHTBase.h, revA"

#include "solution/analysis/integrator/transient/rayleigh/HHTRayleighBase.h"

namespace XC {

//! @ingroup RayleighIntegrator
//
//! @brief HHTBase is an algorithmic class
//! for performing a transient analysis
//! using the HHTBase integration scheme.
class HHTBase: public HHTRayleighBase
  {
  protected:
    double beta;
    double c1;              // some constants we need to keep

    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    HHTBase(SoluMethod *,int classTag);
    HHTBase(SoluMethod *,int classTag,double alpha);
    HHTBase(SoluMethod *,int classTag,double alpha,const RayleighDampingFactors &rF);
    HHTBase(SoluMethod *,int classTag,double alpha, double beta, double gamma);
    HHTBase(SoluMethod *,int classTag,double alpha, double beta, double gamma,const RayleighDampingFactors &rF);    
  };
} // end of XC namespace

#endif
