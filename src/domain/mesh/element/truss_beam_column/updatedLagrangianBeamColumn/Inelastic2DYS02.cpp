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
// Inelastic2DYS02.cpp
//////////////////////////////////////////////////////////////////////
 
#include "domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/Inelastic2DYS02.h"
#include "material/yieldSurface/evolution/YS_Evolution.h"
#include <domain/mesh/element/truss_beam_column/updatedLagrangianBeamColumn/cyclic_model/CyclicModel.h>
#include <cmath>
/*#include <domain/mesh/element/updatedLagrangianBeamColumn/QuadraticCyclic.h>
#include <domain/mesh/element/updatedLagrangianBeamColumn/BilinearCyclic.h>
#include <domain/mesh/element/updatedLagrangianBeamColumn/LinearCyclic.h>
 */
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::Inelastic2DYS02::Inelastic2DYS02(int tag, double a, double e, double iz,
                                 int Nd1, int Nd2,
				 YieldSurface_BC *ysEnd1,
				 YieldSurface_BC *ysEnd2,
				 // int cyc_type, double wt,
				 CyclicModel *cycModel,
				 double delpmax,
				 double Alfa, double Beta,
				 int rf_algo, bool islinear, double rho
				 )
  :InelasticYS2DGNL (tag, Nd1, Nd2, ysEnd1, ysEnd2,
		     rf_algo, islinear, rho),
  A(a), E(e), Iz(iz), resFactor(1.0),
  //cycType(cyc_type), wT(wt),
  alfa(Alfa), beta(Beta),
  delPmax(delpmax), delPMaxPos(0.0), delPMaxNeg(0.0)
{
  massDof = A*L*rho;
  massDof = massDof/2;
// cModel = new XC::BilinearCyclic(1, 1.0);
//  cModel = new XC::LinearCyclic(1);
//   cModel = new XC::QuadraticCyclic(1, 1.0);
/*
	if(wT < 0 || wT > 1)
	{
		std::cerr << "wt out of range, set to 0.8\n";
		wT = 0.8;
	}

	if(cyc_type == 2)
		cModel = new XC::QuadraticCyclic(tag, wT);
	else
		cModel = new XC::BilinearCyclic(tag, wT);
*/

	cModel = cycModel->getCopy();
}

//! @brief Constructor virtual.
XC::Element* XC::Inelastic2DYS02::getCopy(void) const
  { return new XC::Inelastic2DYS02(*this); }

XC::Inelastic2DYS02::~Inelastic2DYS02()
  { delete cModel; }


int XC::Inelastic2DYS02::commitState()
  {	
	this->XC::InelasticYS2DGNL::commitState();

double dp = fabs(ys1->hModel->getTrialPlasticStrains(0));
	   dp+= fabs(ys2->hModel->getTrialPlasticStrains(0));

double  x=0;
	this->getTrialNaturalDisp(disp);
double displ = -1*disp(2);
	if(fabs(disp(5)) > fabs(displ))
		displ = -1*disp(5);

	   if(displ < 0)
	   {
		   if(dp > delPMaxNeg)
			   delPMaxNeg = dp;
			x = fabs(delPMaxNeg/delPmax);
	   }
	   else
	   {
		   if(dp > delPMaxPos)
			   delPMaxPos = dp;
			x = fabs(delPMaxPos/delPmax);
       }
// set the current peak plastk defo

	resFactor = 1*exp(-alfa*x) + beta;
	if(resFactor > 1.0)
		resFactor = 1.0;

	cModel->commitState(resFactor);
	ys1->hModel->setResidual(cModel->getFactor());
	ys2->hModel->setResidual(cModel->getFactor());

//	std::cerr << this->ys1->hModel->getTrialPlasticStrains(0) << std::endl;
	// double d = this->ys1->hModel->getTrialPlasticStrains(1);

	return 0;
}

int XC::Inelastic2DYS02::update(void)
{
int res = this->XC::InelasticYS2DGNL::update();

	// get x-axis for the cModel
	// using max natural deformation
	this->getTrialNaturalDisp(disp);
double displ = -1*disp(2);
	if(fabs(disp(5)) > fabs(displ))
		displ = -1*disp(5);
	// displ(2, 5) will have the same sign
	// for double curvature
double fcurr = eleForce(4);
	
bool yield = false;
	if(end1Plastify || end2Plastify)
		yield = true;

	cModel->update(fcurr, displ, yield);

	return res;

}

void XC::Inelastic2DYS02::getLocalStiff(Matrix &K) const
{

	double iz = Iz*cModel->getFactor();

    double	EIbyL = E*iz/L;

    K(0, 1) = K(0, 2) = K(0, 4) = K(0, 5)=0;
    K(1, 0) = K(1, 3) =0;
    K(2, 0) = K(2, 3) =0;
    K(3, 1) = K(3, 2) = K(3, 4) = K(3, 5)=0;
    K(4, 0) = K(4, 3) =0;
    K(5, 0) = K(5, 3) =0;

	K(0,0) = K(3,3) = (A/iz)*(EIbyL);
	K(0,3) = K(3,0) = (-A/iz)*(EIbyL);
	K(1,1) = K(4,4) = (12/(L*L))*(EIbyL);
	K(1,4) = K(4,1) = (-12/(L*L))*(EIbyL);
	K(1,2) = K(2,1) = K(1,5) = K(5,1) = (6/L)*(EIbyL);
	K(2,4) = K(4,2) = K(4,5) = K(5,4) = (-6/L)*(EIbyL);
	K(2,2) = K(5,5) = 4*(EIbyL);
	K(2,5) = K(5,2) = 2*(EIbyL);
}//getLocalStiff

