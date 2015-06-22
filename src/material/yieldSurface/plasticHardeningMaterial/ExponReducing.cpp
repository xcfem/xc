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
// ExponReducing.cpp: implementation of the XC::ExponReducing class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/plasticHardeningMaterial/ExponReducing.h"
#include <cstdlib>
#include <cmath>

#define MAT_TAG_EXPON -1
#define DEBG 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::ExponReducing::ExponReducing(int tag)
  :PlasticHardeningMaterial(tag,MAT_TAG_EXPON), Kp0(0.0), alpha(0.0), resFactor(0.0)
  {}

XC::ExponReducing::ExponReducing(int tag, double kp0, double alfa)
:PlasticHardeningMaterial(tag,MAT_TAG_EXPON),
  Kp0(kp0), alpha(alfa), resFactor(0.0)
{
}

XC::ExponReducing::ExponReducing(int tag, double kp0, double alfa, double min_fact)
:PlasticHardeningMaterial(tag,MAT_TAG_EXPON),
  Kp0(kp0), alpha(alfa), resFactor(min_fact)
{
//	std::cerr << "ResFact = " <<  res_fact << std::endl; std::cerr << "\a";
}


double XC::ExponReducing::getTrialPlasticStiffness()
{
	double K ;//= Kp0*exp(-1*val_trial*alpha);

	// if x0 and Kp0 is a const:
	// K = Kp0(1.0  - exp(-alpha*x0 + alpha*val_trial));	
	// K = Kp0*(1.0 - exp(-alpha + alpha*val_trial));
	
	// for pinching type stuff
	K = residual*Kp0*(1 - exp(-1*alpha*val_trial));

	if(sFactor != 1.0)
		K = Kp0*sFactor;
	
	if(K < (Kp0*resFactor))
		K = Kp0*resFactor;

//	std::cerr << "K = " << K << ", sFactor = " << sFactor << std::endl;
	
	if(K <0.0)
	{
		std::cerr << "Ri = " << val_trial << ", Factor = " << K/Kp0 << ", res_fact = " << resFactor << std::endl;
		std::cerr << "\a";
	}
	
	return K;
}


void XC::ExponReducing::Print(std::ostream &s, int flag)
{
	s << "MultiLinear, Tag = " << getTag() << std::endl;
	s << "Kp0 = " << Kp0 << std::endl;
	s << "Alpha = " <<  alpha << std::endl;
}

 XC::PlasticHardeningMaterial *XC::ExponReducing::getCopy(void)
{
 	PlasticHardeningMaterial *theMat = new XC::ExponReducing(getTag(), Kp0, alpha, resFactor);
    return theMat;
}

