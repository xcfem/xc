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

#ifndef MEMBRANEPLATEFIBERSECTIONBASE_H
#define MEMBRANEPLATEFIBERSECTIONBASE_H

#include "PlateBase.h"
#include "material/MaterialVector.h"


namespace XC {

class NDMaterial;

//! @brief Base class for fiber models for plate/membrane materials.
//! @ingroup MATPLAC
class MembranePlateFiberSectionBase: public PlateBase
  {
  protected:
    static constexpr int order= 8;
    
    static Vector stressResultant;
    static Matrix tangent;

    
    MaterialVector<NDMaterial> theFibers; //!< pointers to five materials (fibers)
    Vector strainResultant;
    Vector initialStrain;

    void init(const size_t &);
    void alloc(const size_t &, const NDMaterial &);
    void alloc(const std::vector<NDMaterial *> &);
    void copy_fibers(const MembranePlateFiberSectionBase &);
    void free(void);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public: 
    MembranePlateFiberSectionBase(int tag, int classTag, const size_t &numFibers= 0);
    MembranePlateFiberSectionBase(int tag, int classTag, const double &thickness, const size_t &numFibers, const NDMaterial &Afiber);
    MembranePlateFiberSectionBase(int tag, int classTag, const double &thickness, const std::vector<NDMaterial *> &);
    MembranePlateFiberSectionBase(const MembranePlateFiberSectionBase &);
    MembranePlateFiberSectionBase &operator=(const MembranePlateFiberSectionBase &);
    virtual ~MembranePlateFiberSectionBase(void);

    void setMaterial(const NDMaterial &);
    void setMaterial(const std::vector<NDMaterial *> &);
    void setMaterial(const std::vector<std::string> &);
    void setMaterialPy(const boost::python::list &);

    int getOrder(void) const;
    const ResponseId &getResponseType(void) const;
    
    int commitState(void); //swap history variables
    int revertToLastCommit(void); //revert to last saved state
    int revertToStart(void); //revert to start

    int setInitialSectionDeformation(const Vector &strain_from_element);
    void zeroInitialSectionDeformation(void);
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const; //send back the strain
    Vector getVonMisesStressAtFibers(void) const;
    double getMinVonMisesStress(void) const;
    double getMaxVonMisesStress(void) const;
    double getAvgVonMisesStress(void) const;
    
    virtual Matrix getValues(const std::string &, bool silent= false) const;

    //print out data
    void Print(std::ostream &s, int flag ) const;

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
    
    Response *setResponse(const std::vector<std::string> &, Information &);

    // parameters
    int setParameter(const std::vector<std::string> &, Parameter &);
  }; //end of MembranePlateFiberSectionBase declarations

} // end of XC namespace

#endif
