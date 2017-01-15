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
//NodeVectors
                                                                        
                                                                        
#ifndef NodeVectors_h
#define NodeVectors_h

#include "utility/actor/actor/MovableObject.h"
#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {

class Vector;
class Channel;
class FEM_ObjectBroker;

//! \ingroup Nod
//
//! @brief Vectores para almacenar los valores de prueba («trial»)
//! y los valores consumados («commited») del desplazamiento, velocidad, etc.
//! del nodo.
class NodeVectors: public EntCmd, public MovableObject
  {
  protected:
    size_t numVectors; //!< número de vectores.
    Vector *commitData; //!< commited quantities
    Vector *trialData; //!< trial quantities
    
    double *data; //!< double array holding the displ.

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    int createData(const size_t &);
    void libera(void);
    void copia(const NodeVectors &);
  public:
    // constructors
    NodeVectors(const size_t &nv= 2);
    NodeVectors(const NodeVectors &otro);        
    NodeVectors &operator=(const NodeVectors &otro);
    virtual ~NodeVectors(void);

    // public methods dealing with the DOF at the node
    size_t getVectorsSize(void) const;

    // public methods for obtaining committed and trial 
    // response quantities of the node
    virtual const Vector &getData(const size_t &) const;
    virtual const Vector &getTrialData(const size_t &) const;
    const Vector &getTrialData(void) const;
    const Vector &getCommitData(void) const;

    // public methods for updating the trial response quantities
    virtual int setTrialData(const size_t &nDOF,const double &value,const size_t &dof);
    virtual int setTrialData(const size_t &nDOF,const Vector &);    
    virtual int incrTrialData(const size_t &nDOF,const Vector &);    
    
    // public methods dealing with the committed state of the node
    virtual int commitState(const size_t &nDOF);
    virtual int revertToLastCommit(const size_t &nDOF);    
    virtual int revertToStart(const size_t &nDOF);        

    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

} // end of XC namespace

#endif

