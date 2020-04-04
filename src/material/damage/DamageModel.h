// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
  
                             
// $Revision: 1.1 $
// $Date: 2004/09/01 03:54:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/damage/DamageModel.h,v $
                                                                        
#ifndef DamageModel_h
#define DamageModel_h         
                                                               
// Written: Arash Altoontash, Gregory Deierlein
// Created: 08/02
// Revision: AA
//
// Description: This file contains the class definition for 
// damage model Damage modelis a base class and 
// thus no objects of it's type can be instantiated. It has pure virtual 
// functions which must be implemented in it's derived classes. 

#include <utility/actor/actor/MovableObject.h>
#include <utility/tagged/TaggedObject.h>
#include <utility/matrix/Vector.h>


namespace XC {
class Response;
class DamageResponse;
 class Element;


enum DamageType
  {
    NotSpecified,
    Force,
    Deformation,
    PlasticDefo,
    TotalEnergy,
    PlasticEnergy
  };

//! @ingroup Mat
//!
//! @defgroup DamageModelGrp Damage models.
//
//! @ingroup DamageModelGrp
//
//! @brief Base class for damage models. A damage model is an
//! operator that computes the Damage Index by a applying a
//! specific damage rule.
//!
//! The damage model incorporates existing damage formulations for
//! representing deteriorating parameters in component force-deformation
//! relationships and for component performance  assessment. 
//! The damage for individual elements is calculated based on element data
//! such as element deformations,  forces,  or  dissipated  energy.  These
//! engineering  parameters  must  be  extracted  from  the  solution  and
//! processed  for  calculating  a  Damage  Index.  A  Damage Model is
//! defined as an operator that calculates the Damage Index by applying
//! a specific damage rule  (e.g.  Park-Ang  damage  model).
//! See Arash Altoontash dissertation "Performance Assessment Of Reinforced
//! Concrete Beam-Column Joints". August 2004.
class DamageModel:  public TaggedObject, public MovableObject
  {
  public:
    DamageModel(int tag, int classTag);    

    virtual int setTrial(const Vector &trialVector) = 0;
    virtual double getDamage(void) = 0;
    virtual double getPosDamage(void) = 0;
    virtual double getNegDamage(void) = 0;
    
    virtual int commitState(void) = 0;
    virtual int revertToLastCommit(void) = 0;    
    virtual int revertToStart(void) = 0;        
    
    virtual DamageModel *getCopy(void) const= 0;
    
    virtual int setVariable(const std::string &argv)  { return -1; }
    virtual int getVariable(int variableID, double &info)  { return -1; }
    
    virtual int setParameter(const std::vector<std::string> &argv, Information &theInformation);
    virtual int updateParameter(int responseID, Information &theInformation);	
    virtual Response *setResponse(const std::vector<std::string> &argv, Information &theInformation) = 0;
    virtual int getResponse(int responseID, Information &info) = 0;
    
    
    virtual int sendSelf(Communicator &) = 0;  
    virtual int recvSelf(const Communicator &) = 0;
    virtual void Print(std::ostream &s, int flag =0) const=0;
  };

} // end of XC namespace

#endif
