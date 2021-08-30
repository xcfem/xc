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

#ifndef ElasticBeam2dBase_h
#define ElasticBeam2dBase_h

#include "domain/mesh/element/truss_beam_column/ProtoBeam2d.h"

namespace XC {


//! @brief Base class for 2D elastic beam elements.
//! @ingroup BeamColumnElemGrp
class ElasticBeam2dBase: public ProtoBeam2d
  {
  protected:
    CrdTransf2d *theCoordTransf; //!< Coordinate transformation.

    void set_transf(const CrdTransf *trf);
    
    int sendData(Communicator &comm);
    int recvData(const Communicator &comm);

  public:
    ElasticBeam2dBase(int tag, int classTag);
    ElasticBeam2dBase(int tag, int classTag, const Material *m,const CrdTransf *trf);
    ElasticBeam2dBase(int tag, int classTag, double A, double E, double I, 
		        int Nd1, int Nd2, CrdTransf2d &theTransf,
		        double rho = 0.0);
    ElasticBeam2dBase(int tag, int classTag, double A, double alpha, double E,
		        double G, double I,
		        int Nd1, int Nd2, CrdTransf2d &theTransf,
		        double rho = 0.0);
    ElasticBeam2dBase(const ElasticBeam2dBase &);
    ElasticBeam2dBase &operator=(const ElasticBeam2dBase &);
    ~ElasticBeam2dBase(void);

    void setDomain(Domain *theDomain);
    
    const Vector &getVDirStrongAxisGlobalCoord(bool initialGeometry) const;
    const Vector &getVDirWeakAxisGlobalCoord(bool initialGeometry) const;
    
    virtual CrdTransf *getCoordTransf(void);
    virtual const CrdTransf *getCoordTransf(void) const;    
  };
} // end of XC namespace

#endif


