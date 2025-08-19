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

// $Revision: 1.6 $
// $Date: 2003/10/06 18:37:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/forceBeamColumn/ForceBeamColumn2d.h,v $

#ifndef ForceBeamColumn2d_h
#define ForceBeamColumn2d_h

#include <domain/mesh/element/truss_beam_column/NLForceBeamColumn2dBase.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Response;
class ElementalLoad;
class BeamIntegration;
class PrismaticBarCrossSection;

//! @ingroup BeamColumnElemGrp
//!
//! @defgroup OneDimensionalElemF Force based nonlinear beam-column element.
//
//! @ingroup BeamColumnElemGrpF
//! @brief Force based 2D beam column element with PrismaticBarCrossSection type nonlinear material.
class ForceBeamColumn2d: public NLForceBeamColumn2dBase
  {
  private:
    // internal data
    BeamIntegration *beamIntegr; //!< Integration along the beam length.
    FVectorBeamColumn2d v0; //!< Initial deformations due to element loads

    // following are added for subdivision of displacement increment
    int maxSubdivisions; //!< maximum number of subdivisons of dv for local iterations

    std::vector<Vector> persistentInitialDeformation; //!< Persistent initial strain at element level. Used to store the deformation during the inactive phase of the element (if any).
    void free_mem(void);
    void alloc(const BeamIntegration &);
  
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    ForceBeamColumn2d(int tag= 0);
    ForceBeamColumn2d(const ForceBeamColumn2d &);
    ForceBeamColumn2d(int tag,int numSec,const Material *theSection,const CrdTransf *trf,const BeamIntegration *integ);
    ForceBeamColumn2d(int tag, int nodeI, int nodeJ, 
	              int numSections,const std::vector<PrismaticBarCrossSection *> &,
		      BeamIntegration &beamIntegr,
		      CrdTransf2d &coordTransf, double rho = 0.0, 
		      int maxNumIters = 10, double tolerance = 1.0e-12);
    ForceBeamColumn2d &operator=(const ForceBeamColumn2d &);
    Element *getCopy(void) const;
    virtual ~ForceBeamColumn2d(void);
  
    // Element birth and death stuff.
    const std::vector<Vector> &getPersistentInitialSectionDeformation(void) const;
    void incrementPersistentInitialDeformationWithCurrentDeformation(void);
    
    void setDomain(Domain *theDomain);
    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);
    
    void getCurrentDisplacements(Vector &, Vector &);
    int update(void);    
  
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;    
  
    void alive(void);
    void zeroLoad(void);	
    int addLoad(ElementalLoad *, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
  
    const Vector &getResistingForceIncInertia(void) const;            
  
    const BeamIntegration *getIntegrator(void) const;
    boost::python::list getSectionLocationsPy(void) const;
    boost::python::list getSectionWeightsPy(void) const;
    
    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  
    friend std::ostream &operator<<(std::ostream &s, const ForceBeamColumn2d &E);        
    void Print(std::ostream &s, int flag =0) const;    
  
    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
  
  protected:
    void setSectionPointers(const std::vector<PrismaticBarCrossSection *> &);
    int getInitialFlexibility(Matrix &fe) const;
  
  private:
    void getForceInterpolatMatrix(double xi, Matrix &b, const ID &code);
    void getDistrLoadInterpolatMatrix(double xi, Matrix &bp, const ID &code);
    void compSectionDisplacements(std::vector<Vector> &,std::vector<Vector> &) const;
  };

std::ostream &operator<<(std::ostream &, const ForceBeamColumn2d &);
} // end of XC namespace

#endif
