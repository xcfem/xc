// -*-c++-*-
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


#ifndef TDConcrete_h
#define TDConcrete_h

#include "material/uniaxial/concrete/TDConcreteBase.h"

namespace XC {

class TDConcrete : public TDConcreteBase
  {
  private:
    // matpar : Concrete FIXED PROPERTIES
    //	double fcT; // Time Dependent Strength
    double tcr; //!< creep model age in days.
    double epsshu; //!< ultimate shrinkage strain εsh,u, as per ACI 209R-92
    double epssha; //!< fitting parameter within the shrinkage time evolution function as per ACI 209R-92.
    double epscru; //!< ultimate creep coefficient φu, as per ACI 209R-92.
    double epscra; //!< concrete creep exponent fitting constant as per ACI 209R-92.
    double epscrd; //!< fitting constant within the creep time evolution function as per ACI 209R-92

    //Added by AMK:
    double eps_cr;
    double eps_sh;
    double epsP_cr;
    double epsP_sh;
    double phi_i;
    
    std::vector<float> PHI_i;

    void Tens_Envlp (double epsc, double &sigc, double &Ect);
    void Compr_Envlp (double epsc, double &sigc, double &Ect);    
  protected:
    size_t resize(void);
    int sendData(Communicator &);
    int recvData(const Communicator &);
    
  public:
    TDConcrete(int tag= 0);
    TDConcrete(int tag, double _fc, double _ft, double _Ec, double _beta, double _age, double _epsshu, double _epssha, double _tcr, double _epscru, double _epscra, double _epscrd, double _tcast);
    virtual ~TDConcrete(void);
    void setup_parameters(void);

    UniaxialMaterial *getCopy(void) const;

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double setCreepStrain(double time, double stress); //Added by AMK
    double setStress(double strain, double &stiff); //Added by AMK
    double getPHI_i(void) const; //Added by AMK      
    double getCreep(void) const; //Added by AMK
    double setPhi(double time, double tp); //Added by AMK
    double setShrink(double time); //Added by AMK
    double getShrink(void) const; //Added by AMK
    
    void setUltimateShrinkage(const double &);
    double getUltimateShrinkage(void) const;
    void setShrinkageParameter(const double &);
    double getShrinkageParameter(void) const;
    void setCreepRelationshipAge(const double &);
    double getCreepRelationshipAge(void) const;
    void setUltimateConcreteCreep(const double &);
    double getUltimateConcreteCreep(void) const;
    void setCreepExponentParameter(const double &);
    double getCreepExponentParameter(void) const;
    void setCreepDParameter(const double &);
    double getCreepDParameter(void) const;

    
    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);    
    
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
    
    void Print(std::ostream &s, int flag =0) const;

    int getVariable(const std::string &variable, Information &) const;
	
	
    //Added by AMK for recording Creep and Mechanical Strain:
    Response *setResponse(const std::vector<std::string> &, Information &);
    int getResponse(int responseID, Information &matInformation);

  };

} // end of XC namespace

#endif

