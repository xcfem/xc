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
                                                                        
 //----------------------------------------------------------------------------------------------------------------------------
 // Developed by:
 // Adam M. Knaack (adam.knaack@schaefer-inc.com) 
 // Schaefer-Inc, Cincinnati, Ohio, USA
 // Nikola D. Tosic (ntosic@imk.grf.bg.ac.rs)
 // Department for Materials and Structure, Faculty of Civil Engineering, University of Belgrade, Serbia
 // Yahya C. Kurama (ykurama@nd.edu)
 // Department of Civil and Environmental Engineering and Earth Sciences, College of Engineering, University of Notre Dame, Notre Dame, Indiana, USA
 //----------------------------------------------------------------------------------------------------------------------------

 //----------------------------------------------------------------------------------------------------------------------------
 // Created: 2012
 // Last updated: 2019
 //----------------------------------------------------------------------------------------------------------------------------

 //----------------------------------------------------------------------------------------------------------------------------
 // Description: This file contains the source code of TDConcrete. 
 // TDConcrete is a time-dependent concrete material model that calculates
 // creep and shrinkage strains.
 /*-------------------------------
 ! Concrete Compression - Linear
 ! Concrete Tension - Tamai, S., Shima, H., Izumo, J., Okamura, H. 1988. Average Stress-Strain Relationship in Post Yield Range of Steel Bar in Concrete, Concrete Library of JSCE, No. 11, 117-129.
 ! Concrete Creep - Linear superposition of creep coefficient, ACI 209 time function
 ! Concrete Shrinkage - ACI 209 time function
 -------------------------------*/
 // Detailed descriptions of the model and its implementation can be found in the following:
 // (1) Knaack, A.M., Kurama, Y.C. 2018. Modeling Time-Dependent Deformations: Application for Reinforced Concrete Beams with 
 //     Recycled Concrete Aggregates. ACI Structural J. 115, 175�190. doi:10.14359/51701153
 // (2) Knaack, A.M., 2013. Sustainable concrete structures using recycled concrete aggregate: short-term and long-term behavior
 //     considering material variability. PhD Dissertation, Civil and Environmental Engineering and Earth Sciences, University of Notre Dame, Notre Dame, Indiana, USA, 680 pp.
 // A manual describing the use of the model and sample files can be found at:
 // <https://data.mendeley.com/datasets/z4gxnhchky/3>
 //----------------------------------------------------------------------------------------------------------------------------

 //----------------------------------------------------------------------------------------------------------------------------
 // Disclaimer: This software is provided �as is�, without any warranties, expressed or implied. In no event shall the developers be liable for any claim, damages, or liability arising from or in connection with this software.
 //----------------------------------------------------------------------------------------------------------------------------


#include "material/uniaxial/concrete/TDConcreteBase.h" //Changed by AMK
#include "domain/mesh/element/utils/Information.h"
#include <float.h>
#include "domain/domain/Domain.h" //Added by AMK to get current Domain time;
#include "utility/recorder/response/MaterialResponse.h"
#include <cmath>
#include <boost/algorithm/string.hpp>
#include "domain/mesh/element/Element.h"

int XC::TDConcreteBase::creepControl= 0;
double XC::TDConcreteBase::creepDt= 0.0;

//! @brief Sets initial values for the concrete parameters.
void XC::TDConcreteBase::setup_parameters(void)
  {
    ecminP= 0.0;
    ecmaxP= 0.0;
    deptP= 0.0;

    sigCr= fabs(sigCr);
    eP= Ec; //Added by AMK
    epsP= 0.0;
    sigP= 0.0;
    eps= 0.0;
    sig= 0.0;
    e= Ec; //Added by AMK
    Et= Ec;
    count= 0; //Added by AMK
    resize();
    epsInit= 0.0; //Added by AMK
    sigInit= 0.0; //Added by AMK
    eps_total= 0.0; //Added by AMK
    epsP_total= 0.0; //Added by AMK

    eps_m= 0.0; //Added by AMK

    t_load= -1.0; //Added by AMK
    crack_flag= 0;
    iter= 0;
        
    //Change inputs into the proper sign convention:
    fpc= -1.0*fabs(fpc); 
  }

size_t XC::TDConcreteBase::resize(void)
  {
    size_t newSize= 10;
    if(count<2) // restart.
      {
	E_i.resize(newSize);
	DSIG_i.resize(newSize);
	dsig_i.resize(newSize);
	TIME_i.resize(newSize);
	DTIME_i.resize(newSize);
      }
    else
      { 
	if(static_cast<size_t>(count+1)>=E_i.size())
	  {
            newSize= 2*(count+1);
	    E_i.resize(newSize);
	    DSIG_i.resize(newSize);
	    dsig_i.resize(newSize);
	    TIME_i.resize(newSize);
	    DTIME_i.resize(newSize);
	  }
      }
    return newSize;
  }

//! @brief Constructor.
XC::TDConcreteBase::TDConcreteBase(int tag, int classTag)
  : RawConcrete(tag, classTag) {}

//! @brief Constructor.
XC::TDConcreteBase::TDConcreteBase(int tag, int classTag, double _fpc, double _ft, double _Ec, double _beta, double _age, double _tcast): 
  RawConcrete(tag, classTag, _fpc, 0.0, 0.0),
  ft(_ft), Ec(_Ec), age(_age), beta(_beta), tcast(_tcast)
  {
    // setup_parameters(); Called in the constructors of derived classes.
  }

double XC::TDConcreteBase::getInitialTangent(void) const
  {
    return Ec; //Added by AMK
  }

double XC::TDConcreteBase::getCurrentTime(void) const
  {
    double currentTime= -1.0;

    const Domain *theDomain= this->getDomain();
    if(theDomain)
      { currentTime= theDomain->getCurrentTime(); }
    return currentTime;
  }    

double XC::TDConcreteBase::getStrain(void) const
  {
    return eps_total; //Added by AMK
  //return eps;
  }

double XC::TDConcreteBase::getStress(void) const
  {
    return sig;
  }

double XC::TDConcreteBase::getTangent(void) const
  {
    return e;
  }

double XC::TDConcreteBase::getMech(void) const
  {
        return eps_m;
  }

//! @brief Assigns concrete tensile strength.
void XC::TDConcreteBase::setFt(const double &d)
  {
    ft= d;
    if(ft < 0.0)
      {
        ft= -ft;
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; Warning!, tensile strength must be positive."
		  << std::endl;
      }
  }

//! @brief Returns concrete tensile strength.
double XC::TDConcreteBase::getFt(void) const
  { return ft; }

//! @brief Assigns concrete stiffness.
void XC::TDConcreteBase::setEc(const double &d)
  { Ec= d; }

//! @brief Returns concrete stiffness.
double XC::TDConcreteBase::getEc(void) const
  { return Ec; }

//! @brief Assign current concrete stiffness.
void XC::TDConcreteBase::setEt(const double &d)
  { Et= d; }

//! @brief Returns current concrete stiffness.
double XC::TDConcreteBase::getEt(void) const
  { return Et; }

//! @brief Assigns beta.
void XC::TDConcreteBase::setBeta(const double &d)
  { beta= d; }

//! @brief Returns beta.
double XC::TDConcreteBase::getBeta(void) const
  { return beta; }

//! @brief Assigns age.
void XC::TDConcreteBase::setAge(const double &d)
  { age= d; }

//! @brief Returns age.
double XC::TDConcreteBase::getAge(void) const
  { return age; }

//! @brief Assign tcast.
void XC::TDConcreteBase::setTCast(const double &d)
  { tcast= d; }

//! @brief Return tcast.
double XC::TDConcreteBase::getTCast(void) const
  { return tcast; }


void XC::TDConcreteBase::setCreepOn(void)
  { creepControl= 1; }

void XC::TDConcreteBase::setCreepOff(void)
  { creepControl= 0; }

void XC::TDConcreteBase::setCreepDt(const double &d)
  { creepDt= d; }

double XC::TDConcreteBase::getCreepDt(void)
  { return creepDt; }

//! @brief Send object members through the communicator argument.
int XC::TDConcreteBase::sendData(Communicator &comm)
  {
    int res= RawConcrete::sendData(comm);
    res+= comm.sendDoubles(ft, Ec, beta, age, getDbTagData(), CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::TDConcreteBase::recvData(const Communicator &comm)
  {
    int res= RawConcrete::recvData(comm);
    res+= comm.receiveDoubles(ft, Ec, beta, age, getDbTagData(),CommMetaData(2));
    return res;
  }
//! @brief Sends object through the communicator argument.
int XC::TDConcreteBase::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::TDConcreteBase::recvSelf(const Communicator &comm)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::TDConcreteBase::Print(std::ostream &s, int flag) const
  {
  s << "TDConcreteBase:(strain, stress, tangent) " << eps << " " << sig << " " << e << std::endl;
  }





  
void XC::TDConcreteBase::Compr_Envlp(double epsc, double &sigc, double &Ect) 
{
//Linear
Ect = Ec;
sigc = Ect*epsc;
  return;
}

int XC::TDConcreteBase::getVariable(const std::string &varName, Information &theInfo) const
  {
    if(varName=="ec")
      {
        theInfo.theDouble = epsc0;
        return 0;
      }
    else
      return -1;
  }

/* Methods added by AMK: */

XC::Response *XC::TDConcreteBase::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {        
    Response *theResponse = nullptr;

    //theOutput.tag("UniaxialMaterialOutput");
    //theOutput.attr("matType", this->getClassType());
    //theOutput.attr("matTag", this->getTag());

    // stress
    if(argv[0]=="stress")
      {
            //theOutput.tag("ResponseType", "sigma11");
            theResponse =  new MaterialResponse(this, 1, this->getStress());
      }  
    // tangent
    else if(argv[0]=="tangent")
      {
            //theOutput.tag("ResponseType", "C11");
            theResponse =  new MaterialResponse(this, 2, this->getTangent());
      }

    // strain
    else if(argv[0]=="strain") {
            //theOutput.tag("ResponseType", "eps11");
            theResponse =  new MaterialResponse(this, 3, this->getStrain());
    }

    // strain
    else if((argv[0]=="stressStrain") || 
                     (argv[0]=="stressANDstrain") ||
                     (argv[0]=="stressAndStrain")) {
            //theOutput.tag("ResponseType", "sig11");
            //theOutput.tag("ResponseType", "eps11");
            theResponse =  new MaterialResponse(this, 4, Vector(2));
    }

    else if(argv[0]=="CreepStressStrainTangent")
      {
            //theOutput.tag("ResponseType", "sig11");
            //theOutput.tag("ResponseType", "eps11");
            //theOutput.tag("ResponseType", "C11");
            //theOutput.tag("ResponseType", "CreepStrain");
            //theOutput.tag("ResponseType", "MechStrain");
            //theOutput.tag("ResponseType", "ShrinkStrain");
            //theOutput.tag("ResponseType", "t_load");
            theResponse = new MaterialResponse(this, 6, Vector(6));
      }
    else if((argv[0]=="stressStrainTangent") || 
                     (argv[0]=="stressANDstrainANDtangent"))
      {
            //theOutput.tag("ResponseType", "sig11");
            //theOutput.tag("ResponseType", "eps11");
            //theOutput.tag("ResponseType", "C11");
            theResponse =  new MaterialResponse(this, 5, Vector(3));
      }

    // stress sensitivity for local sensitivity recorder purpose.  Quan 2009
    // limit:  no more than 10000 random variables/sensitivity parameters
    else if(argv[0].find("stressSensitivity") != std::string::npos)
      {
	std::vector<std::string> tokens;
	boost::split(tokens, argv[0], boost::is_any_of(" "));
        const int gradient= atoi(tokens[1]);
        //theOutput.tag("ResponseType", "sigsens11");
        theResponse =  new MaterialResponse(this, gradient+10000, this->getStress());
      }
    // strain sensivitiy
    else if(argv[0].find("strainSensitivity") != std::string::npos)
      {
	std::vector<std::string> tokens;
	boost::split(tokens, argv[0], boost::is_any_of(" "));
        const int gradient= atoi(tokens[1]);
        //theOutput.tag("ResponseType", "epssens11");
        theResponse =  new MaterialResponse(this, gradient+20000, this->getStrain());
      }
    //theOutput.endTag();
    return theResponse;
  }

