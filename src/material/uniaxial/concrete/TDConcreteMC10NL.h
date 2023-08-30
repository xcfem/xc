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
// Nikola D. Tosic (ntosic@imk.grf.bg.ac.rs)
// Department for Materials and Structure, Faculty of Civil Engineering, University of Belgrade, Serbia
// Adam M. Knaack (adam.knaack@schaefer-inc.com) 
// Schaefer-Inc, Cincinnati, Ohio, USA
// Yahya C. Kurama (ykurama@nd.edu)
// Department of Civil and Environmental Engineering and Earth Sciences, College of Engineering, University of Notre Dame, Notre Dame, Indiana, USA
//----------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------
// Created: 2019
// Last updated: 2019
//----------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------
// Description: This file contains the source code of TDConcreteMC10NL. 
// TDConcreteMC10NL is a time-dependent concrete material model that calculates
// creep and shrinkage strains.
/*-------------------------------
! Concrete Compression - Non-linear with linear softening
! Concrete Tension - Tamai, S., Shima, H., Izumo, J., Okamura, H. 1988. Average Stress-Strain Relationship in Post Yield Range of Steel Bar in Concrete, Concrete Library of JSCE, No. 11, 117-129.
! Concrete Creep - Linear superposition of creep coefficient, Model Code 2010 time function
! Concrete Shrinkage - Model Code 2010 time function
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

#ifndef TDConcreteMC10NL_h
#define TDConcreteMC10NL_h 

#include "material/uniaxial/concrete/TDConcreteMC10Base.h"

namespace XC {

class TDConcreteMC10NL : public TDConcreteMC10Base
  {
  private:
    void Tens_Envlp (double epsc, double &sigc, double &Ect);
    void Compr_Envlp (double epsc, double &sigc, double &Ect);
  public:
    TDConcreteMC10NL(int tag= 0);
    TDConcreteMC10NL(int tag, double _fc, double _fcu, double _espcu, double _ft, double _Ec, double _Ecm, double _beta, double _age, double _epsba, double _epsbb, double _epsda, double _epsdb, double _phiba, double _phibb, double _phida, double _phidb, double _tcast, double _cem);

    virtual ~TDConcreteMC10NL(void);

    UniaxialMaterial *getCopy(void) const;

    int setTrialStrain(double strain, double strainRate = 0.0); 
    double setCreepBasicStrain(double time, double stress); //Added by AMK //ntosic: split into basic and drying creep
    double setCreepDryingStrain(double time, double stress); //Added by AMK //ntosic: split into basic and drying creep
    double setStress(double strain, double &stiff); //Added by AMK
    
    int commitState(void);
    
    void Print(std::ostream &, int flag =0);
  };

} // end of XC namespace

#endif

