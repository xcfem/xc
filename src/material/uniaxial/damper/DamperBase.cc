//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------

#include "material/uniaxial/damper/DamperBase.h"
#include "utility/matrix/Vector.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

XC::DamperBase::DamperBase(int tag, int classTag)
 :UniaxialMaterial(tag, classTag),
  K(0.0), C(0.0), LGap(0.0),
  NM(1), RelTol(1e-6), AbsTol(1e-10), MaxHalf(15.0)         
  {
    this->setup();
  }

XC::DamperBase::DamperBase(int tag, int classTag, double k, double c, double lgap, double nm, double reltol, double abstol, double maxhalf)
  : UniaxialMaterial(tag, classTag),
    K(k), C(c), LGap(lgap),
    NM(nm), RelTol(reltol), AbsTol(abstol), MaxHalf(maxhalf)       
  {
    //initialize variables
    this->setup();
  }

void XC::DamperBase::setup()
  {
    //initialize variables
    this->revertToStart();
  }

double XC::DamperBase::getStress(void) const
  { return  Tstress; }

double XC::DamperBase::getTangent(void) const
  { return 0.0; }

double XC::DamperBase::getInitialTangent(void) const
  { return 0.0; }

double XC::DamperBase::getDampTangent(void) const
  { return 0.0; }


double XC::DamperBase::getStrain(void) const
  { return Tstrain; }

double XC::DamperBase::getStrainRate(void) const
  { return TVel; }

int XC::DamperBase::commitState(void)
  {
    //commit trial  variables
    Cstrain = Tstrain;
    Cstress = Tstress;
    Ctangent = Ttangent;
    CVel = TVel;
    Cpugr = Tpugr;
    Cnugr = Tnugr; 
  
    return 0;
  }

int XC::DamperBase::revertToLastCommit(void)
  {
    Tstrain = Cstrain;
    Tstress = Cstress;
    Ttangent = Ctangent;
    TVel = CVel;
    Cpugr = Tpugr;
    Cnugr = Tnugr;
  
    return 0;
  }

int XC::DamperBase::revertToStart(void)
  {
    // Initialize state variables
    Tstrain=0.0;
    Tstress=0.0;
    Ttangent = 0.0;
    TVel = 0.0;
    Tpugr = 0.0;
    Tnugr = 0.0;
  
    Cstrain=0.0;
    Cstress = 0.0;
    Ctangent = 0.0;
    CVel = 0.0;
    Cpugr = 0.0;
    Cnugr = 0.0;
    return 0;
  }

double XC::DamperBase::sgn(double dVariable)
  { 
    if (dVariable<0.0)
      { return -1.0; }
    else
      { return 1.0; }
  }

int XC::DamperBase::DormandPrince(double vel0, double vel1, double y0, double h, double& yt, double& eps, double& error) const
  {  
    const double k1 = f(vel0, y0) * h;
  
    const double k2 = f((vel1 - vel0)*(1./5.) + vel0, y0 + (1./5.)*k1) * h;

    const double k3 = f((vel1 - vel0)*(3./10.) + vel0, y0 + (3./40.)*k1 + (9./40.)*k2) * h;

    const double k4 = f((vel1 - vel0)*(4./5.) + vel0, y0 + (44./45.)*k1 + (-56./15.)*k2 + (32./9.)*k3) * h;

    const double k5 = f((vel1 - vel0)*(8./9.) + vel0, y0 + (19372.0/6561.0)*k1 + (-25360.0/2187.0)*k2 + (64448.0/6561.0)*k3 + (-212.0/729.0)*k4) * h;

    const double k6 = f((vel1 - vel0)*(1.) + vel0, y0 + (9017.0/3168.0)*k1 + (-355.0/33.0)*k2 + (46732.0/5247.0)*k3 + (49.0/176.0)*k4 + (-5103.0/18656.0)*k5) * h;

    yt = y0 + (35./384.)*k1 + (500./1113.)*k3 + (125./192.)*k4 + (-2187./6784.)*k5 + (11./84.)*k6;

    const double k7 = f((vel1 - vel0)*(1.) + vel0, yt) * h;

    error = (71./57600.)*k1 + (-71./16695.)*k3 + (71./1920.)*k4 + (-17253./339200.)*k5 + (22./525.)*k6 + (-1./40.)*k7;

    eps = fabs(error/ yt);
    
    return 0;
  }


int XC::DamperBase::sendData(Communicator &comm)
  {
    int res = UniaxialMaterial::sendData(comm);
    res+= comm.sendDoubles(K, C, LGap, getDbTagData(),CommMetaData(2));
    res+= comm.sendDoubles(NM, RelTol, AbsTol, MaxHalf, getDbTagData(),CommMetaData(3));
    res+= comm.sendDoubles(Cstrain, Cstress, Ctangent, CVel, Cpugr, Cnugr, getDbTagData(), CommMetaData(4));
    return res;
  }

int XC::DamperBase::recvData(const Communicator &comm)
  {
    int res = UniaxialMaterial::recvData(comm);
    res+= comm.receiveDoubles(K, C, LGap, getDbTagData(),CommMetaData(2));
    res+= comm.receiveDoubles(NM, RelTol, AbsTol, MaxHalf, getDbTagData(),CommMetaData(3));
    res+= comm.receiveDoubles(Cstrain, Cstress, Ctangent, CVel, Cpugr, Cnugr, getDbTagData(),CommMetaData(4));
    return res;
  }

void XC::DamperBase::Print(std::ostream &s, int flag) const
  {
    s << "DamperBase tag: " << this->getTag() << std::endl;
    s << "  K: " << K << std::endl; 
    s << "  C: " << C << std::endl;
    s << "  LGap: " << LGap << std::endl; 
    s << "  NM: " << NM << std::endl; 
    s << "  RelTol: " << RelTol << std::endl;
    s << "  AbsTol: " << AbsTol << std::endl;
    s << "  MaxHalf: " << MaxHalf << std::endl;    
  }


int XC::DamperBase::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    if ((argv[0]=="E") || (argv[0]=="K"))
      {
	param.setValue(K);
	retval= param.addObject(1, this);
      }
    else if((argv[0]=="eta") || (argv[0]=="C"))
      {
	param.setValue(C);
	retval= param.addObject(2, this);
      }
    else if((argv[0]=="Lgap") || (argv[0]=="LGap"))
      {
	param.setValue(LGap);
	retval= param.addObject(5, this);
      }
    return retval;
  }


int XC::DamperBase::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case 1:
	K = info.theDouble;
	return 0;
      case 2:
	C = info.theDouble;
	return 0;
      case 5:
	LGap = info.theDouble;
	return 0;
      default:
	return -1;
      }
  }
