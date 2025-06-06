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
//ProtoBeam2d.h

#ifndef ProtoBeam2d_h
#define ProtoBeam2d_h

#include "domain/mesh/element/truss_beam_column/BeamColumn.h"
#include "domain/mesh/element/utils/physical_properties/ElasticSection2dPhysicalProperties.h"

namespace XC {
class CrdTransf2d;

//! @ingroup OneDimensionalElem
//!
//! @defgroup BeamColumnElemGrp Beam-column elements.
//
//!
//! @brief Base class for 2D beam elements.
//! @ingroup BeamColumnElemGrp
class ProtoBeam2d: public BeamColumn
  {
  public:
    typedef ElasticSection2dPhysicalProperties PhysProp;
  protected:
    static Matrix extrapolation_matrix; //!< Extrapolation matrix.
    
    PhysProp physicalProperties; //!< pointers to the material objects and physical properties.
    Vector persistentInitialDeformation; //!< Persistent initial strain at element level. Used to store the deformation during the inactive phase of the element (if any).
    int sendData(Communicator &);
    int recvData(const Communicator &);
    void set_material(const Material *m);

  public:
    ProtoBeam2d(int tag, int class_tag,const Material *m= nullptr);
    ProtoBeam2d(int tag, int class_tag, double A, double E, double I, int Nd1, int Nd2);
    ProtoBeam2d(int tag, int class_tag, double A, double alpha, double E, double G, double I, int Nd1, int Nd2);
    int getNumDOF(void) const;
    
    // Element birth and death stuff.
    const Vector &getPersistentInitialSectionDeformation(void) const;
    void incrementPersistentInitialDeformationWithCurrentDeformation(void);
    
    const CrossSectionProperties2d &getSectionProperties(void) const;
    CrossSectionProperties2d &getSectionProperties(void);
    void setSectionProperties(const CrossSectionProperties2d &);
    const PrismaticBarCrossSection *getSectionPtr(const size_t &) const;
    void setMaterial(const std::string &);    
    
    double getRho(void) const;
    void setRho(const double &r); 
    double getLinearRho(void) const;

    inline PhysProp &getPhysicalProperties(void)
      { return physicalProperties; }
    inline const PhysProp &getPhysicalProperties(void) const
      { return physicalProperties; }
    void setPhysicalProperties(const PhysProp &);
    inline virtual std::set<std::string> getMaterialNames(void) const
      { return physicalProperties.getMaterialNames(); }
    
    
    int setInitialSectionDeformation(const Vector &);
    inline const Vector &getInitialSectionDeformation(void) const
      { return (*physicalProperties[0]).getInitialSectionDeformation(); }
    virtual const Vector &computeCurrentStrain(void) const;
    const Vector &getSectionDeformation(void) const;
    inline const Vector &getInitialStrain(void) const
      { return (*physicalProperties[0]).getInitialSectionDeformation(); }
    inline void setInitialStrain(const Vector &e)
      { (*physicalProperties[0]).setInitialSectionDeformation(e); }
    
    virtual int update(void);
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    void alive(void);
    void zeroLoad(void);
    virtual void createInertiaLoad(const Vector &);

    Vector getVDirStrongAxisLocalCoord(void) const;
    Vector getVDirWeakAxisLocalCoord(void) const;
    double getStrongAxisAngle(void) const;
    double getWeakAxisAngle(void) const;

    virtual const Matrix &getExtrapolationMatrix(void) const
      { return extrapolation_matrix; }
    
    boost::python::list getValuesAtNodes(const std::string &, bool silent= false) const;
  };
} // end of XC namespace

#endif
