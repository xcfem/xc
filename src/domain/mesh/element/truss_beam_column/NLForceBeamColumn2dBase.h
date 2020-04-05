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
//NLForceBeamColumn2dBase.h

#ifndef NLForceBeamColumn2dBase_h
#define NLForceBeamColumn2dBase_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf2d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include "domain/mesh/element/utils/fvectors/FVectorBeamColumn2d.h"
#include "domain/mesh/element/utils/coordTransformation/CrdTransf2d.h"

namespace XC {

//! @ingroup BeamColumnElemGrp Nonlinear beam-column bidimensional elements.
//
//! @brief Nonlinear beam-column bidimensional elements.
class NLForceBeamColumn2dBase: public BeamColumnWithSectionFDTrf2d
  {
    NLForceBeamColumn2dBase &operator=(const NLForceBeamColumn2dBase &);
  protected:
    static const size_t NDM= 2; //!< dimension of the problem (2d)
    static const int NND= 3; //!< number of nodal dof's
    static const size_t NEGD= 6; //!< number of element global dof's
    static const size_t NEBD= 3; //!< number of element dof's in the basic system

    
    // internal data
    int maxIters; //!<maximum number of local iterations
    double tol;	 //!<tolerance for relative energy norm for local iterations

    int initialFlag; //!<indicates if the element has been initialized
	
    Matrix kv; //!<stiffness matrix in the basic system 
    Vector Se; //!<element resisting forces in the basic system

    Matrix kvcommit; //!<committed stiffness matrix in the basic system
    Vector Secommit; //!<committed element end forces in the basic system

    std::vector<Matrix> fs; //!<array of section flexibility matrices
    std::vector<Vector> vs; //!<array of section deformation vectors
    std::vector<Vector> Ssr; //!<array of section resisting force vectors
    std::vector<Vector> vscommit; //!<array of committed section deformation vectors

    Matrix sp; //!<Applied section forces due to element loads, 5 x nSections
    FVectorBeamColumn2d p0; // Reactions in the basic system due to element loads

    mutable Matrix Ki;

    static Matrix theMatrix;
    static Vector theVector;
    static double workArea[];

    void resizeMatrices(const size_t &nSections);
    void initializeSectionHistoryVariables(void);

    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

  public:
    NLForceBeamColumn2dBase(int tag,int classTag,int numSec= 0);
    NLForceBeamColumn2dBase(int tag,int classTag,int numSec,const Material *theSection,const CrdTransf *coordTransf);
    NLForceBeamColumn2dBase(const NLForceBeamColumn2dBase &);
    ~NLForceBeamColumn2dBase(void);

    int getNumDOF(void) const;

    const Matrix &getTangentStiff(void) const;

    const Vector &getResistingForce(void) const;

    inline double getV(void) //Shear force in the middle.
      { return (Secommit(1)+Secommit(2))/theCoordTransf->getInitialLength(); }
    inline double getV1(void) //Shear force in the back end.
      { return  (Secommit(1)+Secommit(2))/theCoordTransf->getInitialLength()+p0[1]; }
    inline double getV2(void) //Shear force in the front end.
      { return -(Secommit(1)+Secommit(2))/theCoordTransf->getInitialLength()+p0[2]; }
    inline double getN1(void) //Axial force.
      { return -Secommit(0)+p0[0]; }
    inline double getN2(void)
      { return Secommit(0); }
    inline double getM1(void)
      { return Secommit(1); }
    inline double getM2(void)
      { return Secommit(2); }
  };
} // end of XC namespace

#endif

