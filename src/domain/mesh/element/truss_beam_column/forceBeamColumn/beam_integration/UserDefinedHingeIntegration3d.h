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

// $Revision: 1.3 $
// $Date: 2003/06/10 00:36:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/UserDefinedHingeIntegration3d.h,v $

#ifndef UserDefinedHingeIntegration3d_h
#define UserDefinedHingeIntegration3d_h

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/BeamIntegration.h>

#include <utility/matrix/Vector.h>
#include "material/section/repres/ConstantesSecc3d.h"

namespace XC {

//! \ingroup BeamInteg
//
//! @brief Integración en la barra definida por el usuario.
class UserDefinedHingeIntegration3d : public BeamIntegration
  {
  private:
    Vector ptsL;
    Vector wtsL;
    Vector ptsR;
    Vector wtsR;

    ConstantesSecc3d ctes_scc; //Características de la seccion E,A,Iy,...
  public:
    UserDefinedHingeIntegration3d(int npL, const Vector &ptL, const Vector &wtL,
				int npR, const Vector &ptR, const Vector &wtR,
				double E, double A, double Iz,
				double Iy, double G, double J);
    UserDefinedHingeIntegration3d(int npL, const Vector &ptL, const Vector &wtL,
				  int npR, const Vector &ptR, const Vector &wtR,
				  const ConstantesSecc3d &cts);
    UserDefinedHingeIntegration3d(const ConstantesSecc3d &cts= ConstantesSecc3d());
  
    void getSectionLocations(int numSections, double L, double *xi) const;
    void getSectionWeights(int numSections, double L, double *wt) const;

    void addElasticDeformations(ElementalLoad *theLoad, double loadFactor,double L, double *v0);
    int addElasticFlexibility(double L, Matrix &fe);

    BeamIntegration *getCopy(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);

    void Print(std::ostream &s, int flag = 0);  
  };
} // fin namespace XC

#endif
