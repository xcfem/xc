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
//
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/PlasticHardening2D.h"
#include <material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h>
#include <math.h>

#define strnDebug 0
#define stifDebug 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::PlasticHardening2D::PlasticHardening2D(int tag, int classTag, double min_iso_factor,
				double iso_ratio, double kin_ratio,
				 PlasticHardeningMaterial &kpx_pos,
				 PlasticHardeningMaterial &kpx_neg,
				 PlasticHardeningMaterial &kpy_pos,
				 PlasticHardeningMaterial &kpy_neg, double dir)
:YS_Evolution2D(tag, classTag, min_iso_factor, iso_ratio, kin_ratio),
 defPosX(true), defPosY(true), direction(dir)
{
	if (dir < -1.0 || dir > 1.0)
	{
		std::cerr << "WARNING: PlasticHardening2D() - Dir should be between -1 and +1\n";
		std::cerr << "Set to 0 \n";
		direction = 0.0;
	}

	kpMatXPos = kpx_pos.getCopy();
	kpMatXNeg = kpx_neg.getCopy();
	
	kpMatYPos = kpy_pos.getCopy();
	kpMatYNeg = kpy_neg.getCopy();
}

XC::PlasticHardening2D::~PlasticHardening2D()
{
  if (kpMatXPos != 0)
    delete kpMatXPos;

  if (kpMatXNeg != 0)
    delete kpMatXNeg;

  if (kpMatYPos != 0)
    delete kpMatYPos;

  if (kpMatYNeg != 0)
    delete kpMatYNeg;
}

int XC::PlasticHardening2D::commitState()
{
	this->XC::YS_Evolution2D::commitState();
	
    int res  = kpMatXPos->commitState();
		res += kpMatXNeg->commitState();
		res += kpMatYPos->commitState();
		res += kpMatYNeg->commitState();

	/*if(getTag() ==1)
	{
	if(stifDebug)
	{
		double v0 = getIsoPlasticStiffness(0);
		double v1 = getIsoPlasticStiffness(1);
		std::cerr << v0 << "\t " << v1 << std::endl;
	}
    }*/
    
	return res;
}

int XC::PlasticHardening2D::revertToLastCommit(void)
{
	this->XC::YS_Evolution2D::revertToLastCommit();
	
	kpMatXPos->revertToLastCommit();
	kpMatXNeg->revertToLastCommit();
	kpMatYPos->revertToLastCommit();
	kpMatYNeg->revertToLastCommit();

	return 0;
}


// In a plastic hardening material, Kp_iso == Kp_kin == Kp_equivalent
void XC::PlasticHardening2D::setTrialPlasticStrains(double lamda, const XC::Vector &f, const XC::Vector &g)
{

//	std::cerr << *tmpYSPtr;
	
	double epx = lamda*g(0);
	double epy = lamda*g(1);
//	double val = f(0);
//	double chk = 0.8;
//	val = translate_hist(0);

	defPosX = true;
	if(epx < 0)
		defPosX = false;

//	kpMatXPos->setTrialIncrValue(epx);
//	kpMatXNeg->setTrialIncrValue(-1*epx);
// no need of if.. else - just for remembering the condition

//	pinchX = false;
	if(defPosX)
	{
//		if(val < -1*chk)
//		{
//			std::cerr << "+Pinch [";
//			std::cerr << tmpYSPtr->ele_Location << "]\n";
//			pinchX = true;
//		}
//		else
			kpMatXPos->setTrialIncrValue(epx);

		kpMatXNeg->setTrialIncrValue(-1*epx);
	}
	else
	{
//		if(val >  chk)
//		{
//			std::cerr << "-Pinch [";
//			std::cerr << tmpYSPtr->ele_Location << "]\n";
//			pinchX = true;
//		}
//		else
			kpMatXNeg->setTrialIncrValue(-1*epx);

		kpMatXPos->setTrialIncrValue(epx);
	}

	defPosY = true;
	if(epy < 0)
		defPosY = false;

    if(defPosY)
    {
	//	 if(translate_hist(1) >= 0)
			kpMatYPos->setTrialIncrValue(epy);

		kpMatYNeg->setTrialIncrValue(-1*epy);
	}
	else
	{
	//	 if(translate_hist(1) <= 0)
			kpMatYNeg->setTrialIncrValue(-1*epy);

		kpMatYPos->setTrialIncrValue(epy);
	}

	if(strnDebug)
	{
		std::cerr << "epx = " << epx << ", epy = " << epy << std::endl;
		std::cerr << "bool defPosX = " << defPosX << ", bool defPosY = " << defPosY << std::endl;
	}

}

const XC::Vector &XC::PlasticHardening2D::getEquiPlasticStiffness()
{
	if(freezeEvolution)
	{
		v2(0) = 0.0;
		v2(1) = 0.0;
		return v2;
	}

	if(defPosX == true)
	  v2(0) =  kpMatXPos->getTrialPlasticStiffness();
	 else
	  v2(0) =  kpMatXNeg->getTrialPlasticStiffness();

//	if(pinchX)
//		v2(0) = 0.123*v2(0);

	if(defPosY == true)
	  v2(1) =  kpMatYPos->getTrialPlasticStiffness();
	else
	  v2(1) =  kpMatYNeg->getTrialPlasticStiffness();


	if(strnDebug)
		std::cerr << "Kp " << v2;

	return v2;
}

double XC::PlasticHardening2D::getTrialPlasticStrains(int dir)
{
	if(dir == 0 && defPosX)
		return kpMatXPos->getTrialValue();
	else if(dir == 0 && !defPosX)
		return kpMatXNeg->getTrialValue();
	else if (dir == 1 && defPosY)
		return kpMatYPos->getTrialValue();
	else if (dir == 1 && !defPosY)
		return kpMatYNeg->getTrialValue();
	else
		std::cerr << "XC::PlasticHardening2D::getTrialPlasticStrains(double dir) - incorrect dir||condition \n";
	return 0;
}

double XC::PlasticHardening2D::getCommitPlasticStrains(int dir)
{
	std::cerr << "XC::PlasticHardening2D::getCommitPlasticStrains(double dir) - not yet implemented \n";
	this->getTrialPlasticStrains(dir);
	return 0;
}

double XC::PlasticHardening2D::getIsoPlasticStiffness(int dir)
{
double kp =0;

	if(dir == 0)
	{
		if(defPosX)
		  kp = kpMatXPos->getTrialPlasticStiffness();
		else
		  kp = kpMatXNeg->getTrialPlasticStiffness();

//		if(pinchX)
//			kp = 0.123*kp;
	}
	else if (dir == 1)
	{
		if(defPosY)
		  kp =  kpMatYPos->getTrialPlasticStiffness();
		 else
		  kp = kpMatYNeg->getTrialPlasticStiffness();
	}
	else
		std::cerr << "WARNING: XC::PlasticHardening2D::getPlasticStiffness(int dir) - incorrect dir\n";
	return kp;
}

double XC::PlasticHardening2D::getKinPlasticStiffness(int dir)
{
	return this->getIsoPlasticStiffness(dir);
}

XC::Vector& XC::PlasticHardening2D::getEvolDirection(XC::Vector &f_new)
{
	// -1 => Radial Evolution
	//  0 => From geometric center (~ normal)
	//  1 => Constant-P

	v2(0) = 0.0;
	if(direction >= 0)                 
		v2(1) = direction*f_new(1);
	else
		v2(1) = direction*translate_init(1);
	
	return v2;
}

void XC::PlasticHardening2D::Print(std::ostream &s, int flag)
{
	s << "PlasticHardening2D \n";
	s << "iso_Ratio = " << isotropicRatio << "\n";
	s << "isotropicFactor_hist = " << isotropicFactor_hist;
	s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
	s << "\n";
}

