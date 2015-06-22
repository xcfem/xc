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
// $Date: 2003/06/10 00:36:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/HingeRadauTwoBeamIntegration3d.h,v $

#ifndef HingeRadauTwoBeamIntegration3d_h
#define HingeRadauTwoBeamIntegration3d_h

#include "HingeBeamIntegration3d.h"

namespace XC {

//! \ingroup BeamInteg
//
//! @brief ??.
class HingeRadauTwoBeamIntegration3d: public HingeBeamIntegration3d
  {
  public:
    HingeRadauTwoBeamIntegration3d(double E, double A, double Iz,double Iy, double G, double J,double lpI, double lpJ);
    HingeRadauTwoBeamIntegration3d(const ConstantesSecc3d &cts= ConstantesSecc3d(),const double &lpi=0.0,const double &lpj=0.0);
  
    void getSectionLocations(int numSections, double L, double *xi) const;
    void getSectionWeights(int numSections, double L, double *wt) const;
  
    int addElasticFlexibility(double L, Matrix &fe);

    double getTangentDriftI(double L, double LI, double q2, double q3, bool yAxis);
    double getTangentDriftJ(double L, double LI, double q2, double q3, bool yAxis);

    BeamIntegration *getCopy(void) const;

    void Print(std::ostream &s, int flag = 0);
  };
} // fin namespace XC

#endif
