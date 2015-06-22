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
// @ rkaul@stanford.edu
//   ggd@stanford.edu

#include "YS_Section2D02.h"
#include "material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h"
#include "material/yieldSurface/evolution/YS_Evolution.h"
#include <cmath>
#define SEC_TAG_YS2D02 -1


XC::YS_Section2D02::YS_Section2D02(int tag)
  :YieldSurfaceSection2d(tag, SEC_TAG_YS2D02, 0, true), E(0), A(0), I(0), maxPlstkRot(0.0), peakPlstkRot(0.0), iFactor(1.0)
  {}

XC::YS_Section2D02::YS_Section2D02(void)
:YieldSurfaceSection2d(0, SEC_TAG_YS2D02, 0, true),
 E(0), A(0), I(0), maxPlstkRot(0.0), peakPlstkRot(0.0), iFactor(1.0)
{
}

XC::YS_Section2D02::YS_Section2D02
(int tag, double E_in, double A_in, double I_in, double theta_p_max, YieldSurface_BC *ptrys, bool use_kr)
:YieldSurfaceSection2d(tag, SEC_TAG_YS2D02, ptrys, use_kr),
 E(E_in), A(A_in), I(I_in), maxPlstkRot(theta_p_max),
 peakPlstkRot(0.0), iFactor(1.0)
{
    if (E <= 0.0)
    {
      std::cerr << "XC::YS_Section2D02::YS_Section2D02s -- Input E <= 0.0 ... setting E to 1.0\n";
      E = 1.0;
    }

    if (A <= 0.0)
    {
      std::cerr << "XC::YS_Section2D02::YS_Section2D02s -- Input A <= 0.0 ... setting A to 1.0\n";
      A = 1.0;
    }

    if (I <= 0.0)
    {
      std::cerr << "XC::YS_Section2D02::YS_Section2D02s -- Input I <= 0.0 ... setting I to 1.0\n";
      I = 1.0;
    }

    if(maxPlstkRot <= 0.0)
    {
      std::cerr << "XC::YS_Section2D02::YS_Section2D02s -- Input maxPlstkRot <= 0.0 ... setting to 0.0\n";
      maxPlstkRot = 0.0;
    }

}


int XC::YS_Section2D02::commitState(void)
  {
    double pRot = fabs(ys->hModel->getTrialPlasticStrains(0));
    if(pRot > peakPlstkRot)
		peakPlstkRot = pRot;

	if(fabs(maxPlstkRot) <= 1e-10)
		iFactor = 1.0;
	else
		iFactor =  1 - (peakPlstkRot/maxPlstkRot);

	if(iFactor < 0.02)
		iFactor = 0.02;

	std::cerr << peakPlstkRot << "\t" << iFactor << std::endl;
	return this->XC::YieldSurfaceSection2d::commitState();
  }

void XC::YS_Section2D02::getSectionStiffness(XC::Matrix &Ks)
  {
    Ks(0,0) = E*A; Ks(0,1) = 0.0;
    Ks(1,0) = 0.0; Ks(1,1) = E*I*iFactor;
  }

//! @brief Devuelve la matriz de rigidez tangente.
const XC::Matrix &XC::YS_Section2D02::getInitialTangent(void) const
  {
    ks(0,0) = E*A; ks(0,1) = 0.0;
    ks(1,0) = 0.0; ks(1,1) = E*I;
    return ks;
  }


XC::SectionForceDeformation* XC::YS_Section2D02::getCopy(void) const
  {
    // Make a copy of the hinge
    YS_Section2D02 *theCopy =
    new XC::YS_Section2D02 (this->getTag(), E, A, I, maxPlstkRot, ys, use_Kr_orig);
    theCopy->eCommit = eCommit;
    theCopy->sCommit = sCommit;
    theCopy->peakPlstkRot =  peakPlstkRot;

    return theCopy;
  }

void XC::YS_Section2D02::Print (std::ostream &s, int flag)
{
    s << "YS_Section2D02, tag: " << this->getTag() << std::endl;
	s << "\tE: " << E << std::endl;
	s << "\tA: " << A << std::endl;
	s << "\tI: " << I << std::endl;
	s << "\tDegradation Factor      :" << iFactor << std::endl;
	s << "\tPeak plastic-rotation   :" << peakPlstkRot << std::endl;
	s << "\tMaximum plastic-rotation:" << maxPlstkRot << std::endl;
	this->XC::YieldSurfaceSection2d::Print(s, flag);
}
