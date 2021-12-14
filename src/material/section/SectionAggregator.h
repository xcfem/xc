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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/02/25 23:33:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/SectionAggregator.h,v $
                                                                        
                                                                        
// File: ~/section/SectionAggregator.h
//
// Written: MHS
// Created: Jun 2000
// Revision: A
//
// Description: This file contains the class definition for 
// SectionAggregator.  SectionAggregator decorates an MP
// section (couple bending and axial) with an uncoupled shear
// relation.
//
// What: "@(#) SectionAggregator.h, revA"

#ifndef SectionAggregator_h
#define SectionAggregator_h

#include <material/section/PrismaticBarCrossSection.h>
#include <material/section/AggregatorAdditions.h>

namespace XC {
//! @ingroup MATSCC
//
//! @brief SectionAggregator decorates an MP
//! section (couple bending and axial) with an uncoupled shear
//! relation.
class SectionAggregator: public PrismaticBarCrossSection
  {
  private:
    PrismaticBarCrossSection *theSection;
    AggregatorAdditions theAdditions;
    
    mutable Vector def; //!< Storage for section deformations
    mutable Vector defzero; //!< Storage for initial section deformations
    mutable Vector s; //!< Storage for stress resultants
    mutable Matrix ks;//!< Storage for section stiffness
    mutable Matrix fs;//!< Storage for section flexibility
    mutable ResponseId theCode; //!< identifiers for stiffness contribution.

    static double workArea[];
    //static int codeArea[];

    void copy_section(const SectionForceDeformation *);
    void resize(void);
    void free_mem(void);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    SectionAggregator(MaterialHandler *mat_ldr= nullptr); 
    SectionAggregator(int tag,MaterialHandler *mat_ldr= nullptr);
    SectionAggregator(int tag, PrismaticBarCrossSection &theSection,const AggregatorAdditions &theAdditions,MaterialHandler *mat_ldr= nullptr); 
    SectionAggregator(int tag, const AggregatorAdditions &theAdditions,MaterialHandler *mat_ldr= nullptr); 
    SectionAggregator(int tag, PrismaticBarCrossSection &thesection,UniaxialMaterial &theAddition, int c,MaterialHandler *mat_ldr= nullptr);
    SectionAggregator(const SectionAggregator &);
    XC::SectionAggregator &operator=(const SectionAggregator &);
    ~SectionAggregator(void);

   inline PrismaticBarCrossSection *getSection(void)
     { return theSection; }
   void setSection(const std::string &sectionName);
   inline AggregatorAdditions &getAdditions(void)
     { return theAdditions; }
   void setAddtions(const std::vector<std::string> &,const std::vector<std::string> &);
   void setAddtionsPyList(const boost::python::list &,const boost::python::list &);


    int setInitialSectionDeformation(const Vector &deforms); 
    int setTrialSectionDeformation(const Vector &deforms); 
    virtual double getStrain(const double &y,const double &z) const;
    void zeroInitialSectionDeformation(void);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;
    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
    const Matrix &getInitialFlexibility(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);
 
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
 
    void Print(std::ostream &s, int flag =0) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &info);
    int getResponse(int responseID, Information &info);

    int setVariable(const std::string &argv);
    int getVariable(int variableID, double &info);

  };
} // end of XC namespace

#endif
