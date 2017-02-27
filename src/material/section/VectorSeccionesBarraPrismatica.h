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
//VectorSeccionesBarraPrismatica.h

#ifndef VectorSeccionesBarraPrismatica_h
#define VectorSeccionesBarraPrismatica_h

#include "xc_utils/src/nucleo/EntCmd.h"
#include "utility/actor/actor/MovableObject.h"
#include <vector>

namespace XC {

class SeccionBarraPrismatica;
class Vector;
class Matrix;
class Material;
class BeamStrainLoad;

//! \ingroup MATSCC
//
//! @brief Vector of pointers to SeccionBarraPrismaticaes.
//! used to store the sections for each integration point.
class VectorSeccionesBarraPrismatica: public EntCmd, public std::vector<SeccionBarraPrismatica *>, public MovableObject
  {
  protected:
    void borra_secciones(void);
    void clearAll(void);
    VectorSeccionesBarraPrismatica &operator=(const VectorSeccionesBarraPrismatica &);
    int sendData(CommParameters &);  
    int recvData(const CommParameters &);

  public:
    typedef std::vector<SeccionBarraPrismatica *> mat_vector;
    typedef mat_vector::iterator iterator;
    typedef mat_vector::reference reference;
    typedef mat_vector::const_reference const_reference;

    VectorSeccionesBarraPrismatica(const size_t &sz);
    VectorSeccionesBarraPrismatica(const size_t &sz,const Material *m);
    VectorSeccionesBarraPrismatica(const size_t &sz,const SeccionBarraPrismatica *matModel);
    VectorSeccionesBarraPrismatica(const VectorSeccionesBarraPrismatica &);
    ~VectorSeccionesBarraPrismatica(void);

    bool isTorsion(void) const;

    void setSection(const SeccionBarraPrismatica *nueva_secc);
    void setSectionCopy(size_t i,SeccionBarraPrismatica *nueva_secc);
    void setupSection(const Material *sec);
    bool setSections(const std::vector<SeccionBarraPrismatica *> &sectionPtrs);
    void zeroInitialSectionDeformations(void);
    void setInitialSectionDeformations(const std::vector<Vector> &vs);
    void addInitialSectionDeformations(const BeamStrainLoad &,const double &,const Matrix &, const double &L);
    void setTrialSectionDeformations(const std::vector<Vector> &vs);

    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

  };

} // end of XC namespace


#endif
