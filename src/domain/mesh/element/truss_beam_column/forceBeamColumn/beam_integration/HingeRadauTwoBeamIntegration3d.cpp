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

// $Revision: 1.8 $
// $Date: 2004/06/07 23:21:19 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/HingeRadauTwoBeamIntegration3d.cpp,v $

#include <domain/mesh/element/truss_beam_column/forceBeamColumn/beam_integration/HingeRadauTwoBeamIntegration3d.h>

#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <domain/mesh/element/Information.h>

//! @brief Constructor.
XC::HingeRadauTwoBeamIntegration3d::HingeRadauTwoBeamIntegration3d(double e,
							 double a,
							 double iz,
							 double iy,
							 double g,
							 double j,
							 double lpi,
							 double lpj)
  : HingeBeamIntegration3d(BEAM_INTEGRATION_TAG_HingeRadauTwo3d,e,a,iz,iy,g,j,lpi,lpj) {}

//! @brief Constructor.
XC::HingeRadauTwoBeamIntegration3d::HingeRadauTwoBeamIntegration3d(const CrossSectionProperties3d &cts,const double &lpi,const double &lpj)
  : HingeBeamIntegration3d(BEAM_INTEGRATION_TAG_HingeRadauTwo3d,cts,lpi,lpj) {}

//! @brief Devuelve la posición de las secciones en el vector xi.
void XC::HingeRadauTwoBeamIntegration3d::getSectionLocations(int numSections, double L, double *xi) const
  {
    const double two3oneOverL = (2.0/3.0)/L;

    xi[0] = 0.0;
    xi[1] = lpI*two3oneOverL;
    xi[2] = 1.0-lpJ*two3oneOverL;
    xi[3] = 1.0;
    for(int i= 4; i<numSections;i++)
      xi[i] = 0.0;
  }

//! @brief Devuelve los pesos correspondientes a cada sección.
void XC::HingeRadauTwoBeamIntegration3d::getSectionWeights(int numSections, double L, double *wt) const
  {
    const double oneOverL = 1.0/L;

    wt[0] = 0.25*lpI*oneOverL;
    wt[1] = 3.0*wt[0];
    wt[3] = 0.25*lpJ*oneOverL;
    wt[2] = 3.0*wt[3];
    for(int i = 4; i < numSections; i++)
      wt[i] = 1.0;
  }


int XC::HingeRadauTwoBeamIntegration3d::addElasticFlexibility(double L, Matrix &fElastic)
  {
    const double oneOverL = 1.0/L;

    // Length of elastic interior
    const double Le = L-lpI-lpJ;
    double Lover3EI = Le/(3*ctes_scc.EIz());
    double Lover6EI = 0.5*Lover3EI;
  
    // Elastic flexibility of element interior
    static XC::Matrix fe(2,2);
    fe(0,0) = fe(1,1) =  Lover3EI;
    fe(0,1) = fe(1,0) = -Lover6EI;
  
    // Equilibrium transformation matrix
    static XC::Matrix B(2,2);
    const double betaI = lpI*oneOverL;
    const double betaJ = lpJ*oneOverL;
    B(0,0) = 1.0 - betaI;
    B(1,1) = 1.0 - betaJ;
    B(0,1) = -betaI;
    B(1,0) = -betaJ;
  
    // Transform the elastic flexibility of the element
    // interior to the basic system
    static XC::Matrix ftmp(2,2);
    ftmp.addMatrixTripleProduct(0.0, B, fe, 1.0);

    fElastic(1,1) += ftmp(0,0);
    fElastic(1,2) += ftmp(0,1);
    fElastic(2,1) += ftmp(1,0);
    fElastic(2,2) += ftmp(1,1);

    Lover3EI = Le/(3*ctes_scc.EIy());
    Lover6EI = 0.5*Lover3EI;
    fe(0,0) = fe(1,1) =  Lover3EI;
    fe(0,1) = fe(1,0) = -Lover6EI;
    ftmp.addMatrixTripleProduct(0.0, B, fe, 1.0);
    fElastic(3,3) += ftmp(0,0);
    fElastic(3,4) += ftmp(0,1);
    fElastic(4,3) += ftmp(1,0);
    fElastic(4,4) += ftmp(1,1);

    fElastic(0,0) += Le/(ctes_scc.EA());
    fElastic(5,5) += Le/(ctes_scc.GJ());

    return -1;
  }

double XC::HingeRadauTwoBeamIntegration3d::getTangentDriftI(double L, double LI,double q2, double q3, bool yAxis)
  {
    if (LI < lpI)
      return 0.0;
    else
      {
        const double oneOverL = 1.0/L;
        const double betaI = lpI*oneOverL;
        const double qq2 = (1-betaI)*q2 - betaI*q3;
        const double EI = yAxis ? ctes_scc.EIy() : ctes_scc.EIz();
        return (LI-lpI)/3*(LI-lpI)*qq2/(EI);
      }
  }

double XC::HingeRadauTwoBeamIntegration3d::getTangentDriftJ(double L, double LI, double q2, double q3, bool yAxis)
  {
    if(LI > L-lpJ)
      return 0.0;
    else
      {
        const double oneOverL = 1.0/L;
        const double betaJ = lpJ*oneOverL;
        const double qq3 = (1-betaJ)*q3 - betaJ*q2;
        const double EI = yAxis ? ctes_scc.EIy() : ctes_scc.EIz();
        return (L-LI-lpJ)/3*(L-LI-lpJ)*qq3/EI;
      }
  }

//! @brief Constructor virtual.
XC::BeamIntegration *XC::HingeRadauTwoBeamIntegration3d::getCopy(void) const
  { return new HingeRadauTwoBeamIntegration3d(*this); }


void XC::HingeRadauTwoBeamIntegration3d::Print(std::ostream &s, int flag)
  {
    s << "HingeRadauTwo3d" << std::endl;
    ctes_scc.Print(s);

    return;
  }
