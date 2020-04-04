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
                                                                        
// $Revision: 1.9 $
// $Date: 2003/03/04 00:48:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/SectionForceDeformation.h,v $
                                                                        
                                                                        
#ifndef SectionForceDeformation_h
#define SectionForceDeformation_h

// File: ~/material/SectionForceDeformation.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class definition for SectionForceDeformation.
// SectionForceDeformation is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes. 
//
// What: "@(#) SectionForceDeformation.h, revA"

#include "material/Material.h"

namespace XC {

class Information;
class Response;
class Vector;
class Matrix;
class ResponseId;
class MaterialHandler;

//! @ingroup Mat
//
//! \defgroup MATSCC Force deformation section model.
//
//! @ingroup MATSCC
//! @brief Base class for force deformation section models. Constitutive
//! equations of the section.
class SectionForceDeformation: public Material
  {
  protected:
    mutable Matrix *fDefault; //!< Default flexibility matrix.
    MaterialHandler *material_handler; //!< Material definition handler (search,...).

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);
  public:
    SectionForceDeformation(int tag,int classTag,MaterialHandler *mat_ldr= nullptr);
    SectionForceDeformation(const SectionForceDeformation &);
    SectionForceDeformation &operator=(const SectionForceDeformation &);
    virtual ~SectionForceDeformation(void);

    inline MaterialHandler *getMaterialHandler(void)
      { return material_handler; }

    virtual void zeroInitialSectionDeformation(void)= 0;
    virtual int setInitialSectionDeformation(const Vector &)= 0;
    virtual int addInitialSectionDeformation(const Vector &);
    inline void setInitialGeneralizedStrain(const Vector &iS)
      { setInitialSectionDeformation(iS); }
    const Vector &getInitialGeneralizedStrain(void) const
      { return getInitialSectionDeformation(); }


    //! @brief Set the value of the trial section deformation vector,
    //! \f$esec\f$ to be {\em def}. To return \f$0\f$ if successful,
    //! a negative number if not.
    virtual int setTrialSectionDeformation(const Vector &) = 0;
    virtual const Vector &getInitialSectionDeformation(void) const= 0;
    //! @brief Return the trial section deformation vector, \f$esec\f$.
    virtual const Vector &getSectionDeformation(void) const= 0;
    double getSectionDeformation(const int &) const;
    double getSectionDeformationByName(const std::string &) const;
    virtual double getStrain(const double &y,const double &z= 0) const= 0;

    //! @brief Return the section resisting forces, \f$ssec\f$,
    //! at the current trial state.
    virtual const Vector &getStressResultant(void) const= 0;
    double getStressResultant(const int &) const;
    double getStressResultantByName(const std::string &) const;
    //! @brief Return the section tangent stiffness matrix, \f$ksec\f$,
    //! at the current trial state.
    virtual const Matrix &getSectionTangent(void) const= 0;
    virtual const Matrix &getInitialTangent(void) const= 0;
    virtual const Matrix &getSectionFlexibility(void) const;
    virtual const Matrix &getInitialFlexibility(void) const;

    inline const Vector &getGeneralizedStress(void) const
      { return getStressResultant(); }
    virtual const Vector &getGeneralizedStrain(void) const
      { return getSectionDeformation(); }

    virtual double getRho(void) const;

    //! @brief Virtual constructor.
    virtual SectionForceDeformation *getCopy(void) const= 0;
    //! @brief Return the ordering and type of response quantities
    //! returned by this object.
    //!
    //! Return the section ResponseId code that indicates
    //! the ordering and type of response quantities returned
    //! by the section. Lets the calling object
    //! (e.g. an Element) know how to interpret the quantites returned by this
    //! object.
    virtual const ResponseId &getType(void) const= 0;
    std::string getTypeString(void) const;
    //! @brief Return the number of response quantities provided by the section.
    virtual int getOrder(void) const = 0;

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &info);
    virtual int getResponse(int responseID, Information &info);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);
    virtual const Vector &getStressResultantSensitivity(int gradNumber, bool conditional);
    virtual const Vector &getSectionDeformationSensitivity(int gradNumber);
    virtual const Matrix &getSectionTangentSensitivity(int gradNumber);
    virtual const Matrix &getSectionFlexibilitySensitivity(int gradIndex);
    virtual const Matrix &getInitialTangentSensitivity(int gradIndex);
    virtual const Matrix &getInitialFlexibilitySensitivity(int gradIndex);
    virtual double getRhoSensitivity(int gradNumber);
    virtual int commitSensitivity(const Vector& sectionDeformationGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////
  };

} // end of XC namespace


#endif
