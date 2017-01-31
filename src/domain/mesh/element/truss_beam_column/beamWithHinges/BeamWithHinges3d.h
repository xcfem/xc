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
                                                                        
// $Revision: 1.11 $
// $Date: 2003/02/25 23:32:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/beamWithHinges/BeamWithHinges3d.h,v $

#ifndef BeamWithHinges3d_h
#define BeamWithHinges3d_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf3d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "material/section/repres/CrossSectionProperties3d.h"
#include "domain/mesh/element/fvectors/FVectorBeamColumn3d.h"

namespace XC {
class Node;
class Channel;
class FEM_ObjectBroker;

class SeccionBarraPrismatica;

class Response;
class Renderer;

//! \ingroup OneDimensionalElem
//
//! @brief Barra con rótulas en los extremos para problemas tridimensionales.
class BeamWithHinges3d: public BeamColumnWithSectionFDTrf3d
  {
  private:
    CrossSectionProperties3d ctes_scc; //Mechanical properties of the section E,A,Iy,...
    double beta1, beta2;
    double rho;

    Matrix fs[2];
    Vector sr[2];
    Vector  e[2];
    
    Matrix kb;
    Vector q;
    
    Matrix kbCommit;
    Vector qCommit;
    Vector eCommit[2];  

    int initialFlag;
    
    int maxIter;
    double tolerance;

    Matrix *applied_sf;  // Applied section forces due to element loads, 5 x nSections
    FVectorBeamColumn3d p0; // Reactions in the basic system due to element loads
    FVectorBeamColumn3d v0; // Basic deformations due to element loads on the interior
  
    static Matrix theMatrix;
    static Vector theVector;
    static double workArea[];

    void checkNodePtrs(Domain *theDomain);
    void setHinges(void);
    
    void getForceInterpMatrix(Matrix &b, double x, const ID &c);
    void getDistrLoadInterpMatrix(Matrix &bp, double x, const ID &c);
    

  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);  
 public:
    BeamWithHinges3d(int tag, int nodeI, int nodeJ,
		   double E, double A, double Iz, double Iy, double G, double J,
		   SeccionBarraPrismatica &sectionRefI, double hingeIlen, 
		   SeccionBarraPrismatica &sectionRefJ, double hingeJlen,
		   CrdTransf3d &coordTrans, double massDensPerUnitLength = 0.0,
		   int max = 1, double tol = 1.0e-16);
    BeamWithHinges3d(int tag= 0);
    BeamWithHinges3d(int tag,const Material *theSection,const CrdTransf *coordTransf);
    BeamWithHinges3d(const BeamWithHinges3d &);
    //BeamWithHinges3d &operator=(const BeamWithHinges3d &);
    Element *getCopy(void) const;
    ~BeamWithHinges3d(void);
    
    double getRho(void) const
      { return rho; }
    void setRho(const double &r)
      { rho= r; }

    inline CrossSectionProperties3d getSectionProperties(void) const
      { return ctes_scc; }
    void setSectionProperties(const CrossSectionProperties3d &ctes)
      { ctes_scc= ctes; }

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);
    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    
    int update(void);
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;
    
    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    Response *setResponse(const std::vector<std::string> &argv, Information &);
    int getResponse(int responseID, Information &info);
  
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    
    void Print(std::ostream &s, int flag = 0);
  };
} // end of XC namespace

#endif
