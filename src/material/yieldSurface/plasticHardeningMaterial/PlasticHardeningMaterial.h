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
#ifndef PlasticHardeningMaterial_h
#define PlasticHardeningMaterial_h

#include "material/Material.h"

namespace XC {
class Information;
class Response;

//! @ingroup Mat
//
//! @brief Material plástico con endurecimiento por deformación.
class PlasticHardeningMaterial : public Material
  {
  protected:
    double val_hist, val_trial;
    double residual, sFactor;
  public:
    PlasticHardeningMaterial (int tag, int classTag);

	// could be delta_plastic, |back-stress|, 
	// distance between force-point on YS and conjugate
	// point on Bounding-Surface
    virtual int setTrialValue(double xVal, double factor);
    virtual int setTrialIncrValue(double dxVal);
    void setResidual(double res=1);

    const Vector &getGeneralizedStress(void) const;
    const Vector &getGeneralizedStrain(void) const;

    virtual void setInitialGeneralizedStrain(const Vector &);
    const Vector &getInitialGeneralizedStrain(void) const;

    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);

    virtual double getTrialPlasticStiffness()=0;
    double getTrialValue(void);
    virtual PlasticHardeningMaterial *getCopy(void)= 0;

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &matInformation);
    virtual int getResponse (int responseID, Information &matInformation);
    virtual void Print(std::ostream &s, int flag =0);

    virtual int sendSelf(CommParameters &)
      {return -1;}
    virtual int recvSelf(const CommParameters &)
      {return -1;}
  };
} // end of XC namespace


#endif

