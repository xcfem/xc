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
//BkStressLimSurface2D
//
//////////////////////////////////////////////////////////////////////

#include "material/yieldSurface/evolution/BkStressLimSurface2D.h"
#include <material/yieldSurface/yieldSurfaceBC/YieldSurface_BC.h>
#include <cmath>
#include "xc_basic/src/util/matem.h"

#define evolDebug 0
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XC::BkStressLimSurface2D::BkStressLimSurface2D(int tag, int classTag, double min_iso_factor,
                                                double iso_ratio, double kin_ratio, YieldSurface_BC &lim_surface,
                                                PlasticHardeningMaterial &kinX,
                                                PlasticHardeningMaterial &kinY,
                                                PlasticHardeningMaterial &isoXPos,
                                                PlasticHardeningMaterial &isoXNeg,
                                                PlasticHardeningMaterial &isoYPos,
                                                PlasticHardeningMaterial &isoYNeg,
                                                int restype, double res_Fact, double app_Fact, double dir
                                                )
:YS_Evolution2D(tag, classTag, min_iso_factor, iso_ratio, kin_ratio),
        defPosX(true), defPosY(true), resAlgo(restype),
        resFactor(res_Fact), appFactor(app_Fact), direction(dir), direction_orig(dir)
  {
    if(dir < -1.0 )
      {
        std::cerr << "WARNING: BkStressLimSurface2D() - Dir should be between -1 and +1\n";
        std::cerr << "Set to variable \n";
        direction_orig = 10;
      }

    if(direction_orig > 1)
      direction = 0.0;
                
    kinMatX = kinX.getCopy();
    kinMatY = kinY.getCopy();
    isoMatXPos = isoXPos.getCopy();
    isoMatXNeg = isoXNeg.getCopy();
    isoMatYPos = isoYPos.getCopy();
    isoMatYNeg = isoYNeg.getCopy();
        
    // don't really need a copy for kinMats /LimSurface because only getDrift() is used
    // even if the user reuses.. a separate copy will be created
    // that said ..
    limSurface = lim_surface.getCopy();
    limSurface->setTransformation(0, 1, 1, 1);
  }

XC::BkStressLimSurface2D::~BkStressLimSurface2D()
  {
    if(kinMatX)
      delete kinMatX;
    if(kinMatY)
      delete kinMatY;
    if(isoMatXPos)
      delete isoMatXPos;
    if(isoMatXNeg)
      delete isoMatXNeg;
    if(isoMatYPos)
      delete isoMatYPos;
    if(isoMatYNeg)
      delete isoMatYNeg;
    if(limSurface)
      delete limSurface;
  }

void XC::BkStressLimSurface2D::setResidual(double res)
  {
    kinMatX->setResidual(res);
    kinMatY->setResidual(res);
  }

                
int XC::BkStressLimSurface2D::commitState()
  {
    this->XC::YS_Evolution2D::commitState();

    int res= kinMatX->commitState();
    res+= kinMatY->commitState();
    res+= isoMatXPos->commitState();
    res+= isoMatXNeg->commitState();
    res+= isoMatYPos->commitState();
    res+= isoMatYNeg->commitState();
    return res;
  }

int XC::BkStressLimSurface2D::revertToLastCommit(void)
  {
        this->XC::YS_Evolution2D::revertToLastCommit();

        kinMatX->revertToLastCommit();
        kinMatY->revertToLastCommit();
        isoMatXPos->revertToLastCommit();
        isoMatXNeg->revertToLastCommit();
        isoMatYPos->revertToLastCommit();
        isoMatYNeg->revertToLastCommit();

        return 0;
  }


void XC::BkStressLimSurface2D::setTrialPlasticStrains(double lamda, const XC::Vector &f, const XC::Vector &g)
  {
//        double epx = isotropicRatio*lamda*g(0);
//        double epy = isotropicRatio*lamda*g(1);

    // set wrt absolute for easier calibration
    double epx = lamda*g(0);
    double epy = lamda*g(1);

//        std::cerr << "epx = " << epx << ", epy = " << epy << std::endl;
//        std::cerr << "gx  = " << g(0)  << ", gy  = " << g(1)  << std::endl;
//        std::cerr << "\a";

    if(epx > 0)
            defPosX = true;
    else
            defPosX = false;

    if(epy > 0)
            defPosY = true;
    else
            defPosY = false;
            
    isoMatXPos->setTrialIncrValue(epx);
    isoMatXNeg->setTrialIncrValue(-1*epx);
    isoMatYPos->setTrialIncrValue(epy);
    isoMatYNeg->setTrialIncrValue(-1*epy);
    //!! when should sumIsoEp be reset? - using same as plastic hardening

    double x0 = translate_hist(0);
    double y0 = translate_hist(1);
    //const double fx=f(0);
    //const double fy=f(1);

    limSurface->hModel->toOriginalCoord(x0, y0);
    double drift = limSurface->getDrift(x0, y0);

    if(direction_orig > 1)
            direction = fabs(y0);

    if(fabs(y0) >= 0.80)
            direction = 1.0; // constP
    
int resType = resAlgo;
    
    double dR = fabs(drift); // in-case outside    pinching starts late

    switch (resType)
    {
    case 1:
    {
            if(drift >= 0.0)
              {
		if(sgn(g(0)) != sgn(translate_hist(0)))
                            dR = 1.5 + drift; // approx value = 2   metal case
                     else
                            dR = 0.0;
            }
            else     // no pinching
            {
                    //old limSurface->hModel->toOriginalCoord(fx, fy);
                    //old  dR = limSurface->interpolate(x0, y0, fx, fy);


                    // y0 range -1 to +1
                    if(sgn(g(0)) != sgn(translate_hist(0)))
                    {
                              // dR = 2.0 + drift; // drift < 0
                            dR = fabs(limSurface->getDrift(0.0, y0))*2 - fabs(drift);
//          not required
//                        else
//                                dR = fabs(drift);

//                        std::cerr << "!!drift 0, y0 = " << limSurface->getDrift(0.0, y0)
//                             << ", drift = " << drift << std::endl;
                    }
            }

            break;
    } //case 1 - Metals

    case 2:
    {
            if(drift >= 0.0)
                    dR = 0.0;
            else     // pinching starts early
            {
//                        limSurface->hModel->toOriginalCoord(fx, fy);
//                        dR = limSurface->interpolate(x0, y0, fx, fy);
                    if(sgn(g(0)) != sgn(translate_hist(0)))
                            dR = fabs(limSurface->getDrift(0.0, y0))*2 - fabs(drift);
            }

            break;
    }//case 2 - Pinching,  Kp =  Kp0 -> 0

    case 3:
    {
            if(drift >= 0.0)
                    dR = 0.0;
            break;
    }//case 3 - Pinching, Kp = 0 -> Kp0 -> 0

    case 4:
    {
            if(drift >= 0.0)
            {
                    if(sgn(g(0)) == sgn(translate_hist(0)))
                            dR = 0.0;
            }
/*                else
            {
                    if(sgn(g(0)) != sgn(translate_hist(0)))
                            dR = 0.0;
            }
*/                
            break;
    }
    
    default:
    {
            std::cerr << "WARNING - Unknown residual algo\n";
            std::cerr << *this;
            if(drift >= 0.0)
                    dR = 0.0;
    }
    
    } // switch - algo

double sfactor = 1.0;
    
    resHardening = false;
    resApproach  = false;
    if(drift >= 0.0)
    {
            if(sgn(g(0)) == sgn(translate_hist(0)))
            {
                    resHardening = true;
                    if(resType > 1)
                            sfactor = resFactor; 
            }
            else
            {
                    resApproach  = true;
                    if(resType > 1)
                            sfactor = appFactor;

            }

//                std::cerr << "----- Drift > 0 --- ( " << sfactor << ")\n";
    }
    
    // absolute values - no need to have history 
    kinMatX->setTrialValue(dR, sfactor);
    kinMatY->setTrialValue(dR, sfactor);
  }

const XC::Vector &XC::BkStressLimSurface2D::getEquiPlasticStiffness()
  {        
    double kp_kin_x =  kinMatX->getTrialPlasticStiffness();
    double kp_kin_y =  kinMatY->getTrialPlasticStiffness();
    double kp_iso_x =  isoMatXPos->getTrialPlasticStiffness();
    double kp_iso_y =  isoMatYPos->getTrialPlasticStiffness();

    if(!defPosX)
            kp_iso_x =  isoMatXNeg->getTrialPlasticStiffness();
    if(!defPosY)
            kp_iso_y =  isoMatYNeg->getTrialPlasticStiffness();

//        std::cerr << *isoMatYPos;
//        std::cerr << *isoMatXPos;
            
    v2(0) =isotropicRatio*kp_iso_x + kinematicRatio*kp_kin_x;
    v2(1) =isotropicRatio*kp_iso_y + kinematicRatio*kp_kin_y;
    
    if(isotropicFactor(0) <=minIsoFactor)  
            v2(0) = 0;

    if(isotropicFactor(1) <=minIsoFactor)
            v2(1) = 0;

      
    return v2;
  }

double XC::BkStressLimSurface2D::getTrialPlasticStrains(int dir)
  {
    if(dir == 0 && defPosX)
            return isoMatXPos->getTrialValue();
    else if(dir == 0 && !defPosX)
            return isoMatXNeg->getTrialValue();
    else if (dir == 1 && defPosY)
            return isoMatYPos->getTrialValue();
    else if (dir == 1 && !defPosY)
            return isoMatYNeg->getTrialValue();
    else
            std::cerr << "XC::BkStressLimSurface2D::getTrialPlasticStrains(double dir) - incorrect dir||condition \n";
    return 0;
  }

double XC::BkStressLimSurface2D::getCommitPlasticStrains(int dir)
  {
    std::cerr << "WARNING: XC::BkStressLimSurface2D::getCommitPlasticStrains(.) "
              << " not yet implemented" << std::endl;
    return this->getTrialPlasticStrains(dir);
  }


double XC::BkStressLimSurface2D::getIsoPlasticStiffness(int dir)
  {
    if(dir == 0 && defPosX)
            return isoMatXPos->getTrialPlasticStiffness();
    else if(dir == 0 && !defPosX)
            return isoMatXNeg->getTrialPlasticStiffness();
    else if (dir == 1 && defPosY)
            return isoMatYPos->getTrialPlasticStiffness();
    else if (dir == 1 && !defPosY)
            return isoMatYNeg->getTrialPlasticStiffness();
    else
            std::cerr << "XC::BkStressLimSurface2D::getIsoPlasticStiffness(double dir) - incorrect dir/condition \n";
    return 0;        
  }

double XC::BkStressLimSurface2D::getKinPlasticStiffness(int dir)
  {
    if(dir == 0)
      return kinMatX->getTrialPlasticStiffness();
    else if (dir == 1)
      return kinMatY->getTrialPlasticStiffness();
    else
      std::cerr << "XC::BkStressLimSurface2D::getKinPlasticStiffness(double dir) - incorrect dir\n";
    return 0;

  }

XC::Vector& XC::BkStressLimSurface2D::getEvolDirection(XC::Vector &f_new)
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

void XC::BkStressLimSurface2D::Print(std::ostream &s, int flag)
  {
    s << "BkStressLimSurface2D \n";
    s << "iso_Ratio = " << isotropicRatio << "\n";
    s << "isotropicFactor_hist = " << isotropicFactor_hist;
    s << "translateX       = " << translate(0) << ",\ttranslateY = " << translate(1) << "\n";
    s << "\n";
  }

