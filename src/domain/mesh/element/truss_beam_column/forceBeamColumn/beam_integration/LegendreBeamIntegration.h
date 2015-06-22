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
// $Date: 2006/01/17 21:12:56 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/LegendreBeamIntegration.h,v $

#ifndef LegendreBeamIntegration_h
#define LegendreBeamIntegration_h

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h>

namespace XC {
class Matrix;
class ElementalLoad;
class Channel;
class FEM_ObjectBroker;

//! \ingroup BeamInteg
//
//! @brief cuadratura Gauss-Legendre
class LegendreBeamIntegration: public BeamIntegration
  {
  public:
    LegendreBeamIntegration(void);

    void getSectionLocations(int nIP, double L, double *xi) const;
    void getSectionWeights(int nIP, double L, double *wt) const;

    BeamIntegration *getCopy(void) const;

    // These two methods do nothing
    int sendSelf(CommParameters &)
      {return 0;}
    int recvSelf(const CommParameters &)
      {return 0;}
    void Print(std::ostream &s, int flag = 0);  
  };
} // fin namespace XC

#endif
