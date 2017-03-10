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

#include <material/section/SeccionBarraPrismatica.h>
#include <material/section/AggregatorAdditions.h>

namespace XC {
//! \ingroup MATSCC
//
//! @brief SectionAggregator decorates an MP
//! section (couple bending and axial) with an uncoupled shear
//! relation.
class SectionAggregator : public SeccionBarraPrismatica
  {
  private:
    SeccionBarraPrismatica *theSection;
    AggregatorAdditions theAdditions;
    
    Vector *def; //!< Storage for section deformations
    Vector *defzero; //!< Storage for initial section deformations
    Vector *s; //!< Storage for stress resultants
    Matrix *ks;//!< Storage for section stiffness
    Matrix *fs;//!< Storage for section flexibility
    ResponseId *theCode; //!< identifiers for stiffness contribution.

    static double workArea[];
    //static int codeArea[];

    void copia_seccion(const SectionForceDeformation *);
    void check_ptrs(void) const;
    void alloc_storage_ptrs(void);
    void libera_storage_ptrs(void);
    void libera(void);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    SectionAggregator(MaterialLoader *mat_ldr= nullptr); 
    SectionAggregator(int tag,MaterialLoader *mat_ldr= nullptr);
    SectionAggregator(int tag, SeccionBarraPrismatica &theSection,const AggregatorAdditions &theAdditions,MaterialLoader *mat_ldr= nullptr); 
    SectionAggregator(int tag, const AggregatorAdditions &theAdditions,MaterialLoader *mat_ldr= nullptr); 
    SectionAggregator(int tag, SeccionBarraPrismatica &thesection,UniaxialMaterial &theAddition, int c,MaterialLoader *mat_ldr= nullptr);
    SectionAggregator(const SectionAggregator &otro);
    XC::SectionAggregator &operator=(const SectionAggregator &otro);
    ~SectionAggregator(void);

   inline SeccionBarraPrismatica *getSection(void)
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

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
 
    void Print(std::ostream &s, int flag =0) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &info);
    int getResponse(int responseID, Information &info);

    int setVariable(const std::string &argv);
    int getVariable(int variableID, double &info);

  };
} // end of XC namespace

#endif
