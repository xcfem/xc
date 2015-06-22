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
// MultiLinearKp.cpp: implementation of the XC::MultiLinearKp class.
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/plasticHardeningMaterial/MultiLinearKp.h"
#include <cstdlib>

#define MAT_TAG_MULTILINEAR -1
#define DEBG 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::MultiLinearKp::MultiLinearKp(int tag)
  :PlasticHardeningMaterial(tag,MAT_TAG_MULTILINEAR)
  {}

XC::MultiLinearKp::MultiLinearKp(int tag, XC::Vector &sum_plas_defo, XC::Vector &kp)
:PlasticHardeningMaterial(tag,MAT_TAG_MULTILINEAR),
sumPlasDefo(sum_plas_defo.Size()+1), Kp(kp.Size()+1)
{
 	if(sumPlasDefo.Size() != Kp.Size())
 	{
 		std::cerr << "ERROR MultiLinear() - incompatible vector sizes\n";
 	}

 	numPoints = sum_plas_defo.Size();

 	for(int i=0; i < numPoints; i++)
 	{
 		sumPlasDefo(i) =  sum_plas_defo(i);
 		if(sumPlasDefo(i) < 0)
 			std::cerr << "ERROR MultiLinear() -  sumPlasDefo < 0\n";

 		Kp(i) = kp(i);
 	}
 	if(sumPlasDefo(0) != 0)
 		std::cerr << "WARNING MultiLinear() -  sumPlasDefo(0) != 0\n";

 	Kp(numPoints)          = Kp(numPoints -1);
 	sumPlasDefo(numPoints) = sumPlasDefo(numPoints -1)*1000;

  }

double XC::MultiLinearKp::getTrialPlasticStiffness()
  {
    double K = 0;
    double sumDisp = val_trial;

	if( sumDisp > sumPlasDefo(numPoints-1))
	{
		K = residual*Kp(numPoints-1);
		
		if(sFactor != 1.0)
			K = Kp(0)*sFactor;
		return K;
	}
		
	for(int i=0; i< numPoints; i++)
	{
		double x1 = sumPlasDefo(i);
		double y1 = Kp(i);
		double x2 = sumPlasDefo(i+1);
		double y2 = Kp(i+1);
		
		if(sumDisp < x2 && sumDisp >= x1)
		{
			if (sumDisp == x1)
				return y1;
				
			if(x2 == x1)
			{
				std::cerr << "WARNING - XC::MultiLinear::getTangent() x2 -x1 = 0 \n";
				return 0;
			}
			
			double m = (y2 - y1)/(x2 -x1);
		    double b = y1 - m*x1;
		    K = m*sumDisp + b;
		 	break;
		}
	}

	if(sFactor != 1.0)
		K = Kp(0)*sFactor;
	else
		K = residual*K;

	// std::cerr << "K = " << K << ", sFactor = " << sFactor << std::endl; // std::cerr << "\a";
	return K;
}


void XC::MultiLinearKp::Print(std::ostream &s, int flag)
{
	this->XC::PlasticHardeningMaterial::Print(s, flag);
	
	s << "+-MultiLinear" << std::endl;
	s << "    Kp = " << this->getTrialPlasticStiffness();
	s << "    SumPlasDefo XC::Vector = " <<  sumPlasDefo;
	s << "    Kp XC::Vector          = " <<  Kp << std::endl;
}

 XC::PlasticHardeningMaterial *XC::MultiLinearKp::getCopy(void)
{
	XC::Vector spd(numPoints);
    XC::Vector kp(numPoints);

    for(int i =0; i < numPoints; i++)
    {
    	spd(i) =  sumPlasDefo(i);
    	kp(i)  =  Kp(i);
    }

    // Don't want to pass the actual vectors or else the size will
    // keep on increasing by 1
 	PlasticHardeningMaterial *theMat = new XC::MultiLinearKp(getTag(), spd, kp);
    return theMat;
}

