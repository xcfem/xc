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
// $Date: 2005/11/23 22:48:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/mesh/node/Node.h,v $
                                                                        
                                                                        
#ifndef Node_h
#define Node_h

// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the class interface for Node.
// A Node provides the abstraction for a node in the FEM.
// Nodes have original position, trial displacement, velocity and 
// acceleration and committed displacement, velocity and acceleration 
// (the last committed() trial quantities).
//
// What: "@(#) Node.h, revA"

#include "domain/mesh/MeshComponent.h"
#include "NodeDispVectors.h"
#include "NodeVelVectors.h"
#include "NodeAccelVectors.h"
#include "utility/matrix/Matrix.h"
#include <boost/python/list.hpp>

class Pos2d;
class Pos3d;
class SlidingVectorsSystem3d;
class GeomObj2d;
class GeomObj3d;

namespace XC {
class ContinuaReprComponent;
class Matrix;
class Channel;
class Renderer;
class TrfGeom;
class NodeLocker;
class DefaultTag;
class ElementEdges;
class SetBase;
class MeshEdge;
class DOF_Group;
class DqPtrsElem;

//! @ingroup Mesh
//!
//!
//! @defgroup Nod Mesh nodes.
//
//! @ingroup Nod
//! @brief Mesh node.
//!
//! Nodes are points in space connected by the elements. Nodes
//! have original position, trial displacement, velocity and acceleration, 
//! and committed displacement, velocity and acceleration (the last
//! committed trial quantities). Nodes also store information about any
//! load acting on the node, nodal mass and the nodal participation
//! matrix. In addition, each Node object keeps track of it's associated
//! DOF_Group object. The Node interface provides methods to set and
//! retrieve these quantities.
class Node: public MeshComponent
  {
  private:
    // private data associated with each node object
    int numberDOF; //!< number of DOFs at Node
    DOF_Group *theDOF_GroupPtr; //!< pointer to associated DOF_Group
    Vector Crd; //!< original nodal coords
    
    NodeDispVectors disp; //! Displacement vectors (committed,trial,...)
    NodeVelVectors vel; //! Velocity vectors (committed,trial,...)
    NodeAccelVectors accel; //! Acceleration vectors (committed,trial,...)

    Matrix R; //!< nodal (modal?) participation matrix
    Matrix mass; //!< mass matrix
    mutable Vector unbalLoad; //!< unbalanced load
    mutable Vector unbalLoadWithInertia; //!< unbalanced load with inertia
    mutable Vector reaction;
    double alphaM; //!< rayleigh damping factor
    mutable double tributary; //!< Tributary length, area or volume.
    
    Matrix theEigenvectors; //Eigenvectors matrix.

    // AddingSensitivity:BEGIN /////////////////////////////////////////
    Matrix dispSensitivity;
    Matrix velSensitivity;
    Matrix accSensitivity;
    int parameterID;
    // AddingSensitivity:END ///////////////////////////////////////////

    static std::deque<Matrix> theMatrices;

    mutable std::set<ContinuaReprComponent *> connected; //!< Components (elements, constraints,...) that are connected with this node.

    std::set<int> freeze_constraints;//!< Tags of the constraints created by freeze() method.
    const ID &get_id_constraints(void) const;
    void set_id_constraints(const ID &);

    static DefaultTag defaultTag; //<! tag for next new node.
  protected:

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    // typedefs.
    typedef std::set<Element *> ElementPtrSet; //!< Container of element pointers.
    typedef std::set<const Element *> ElementConstPtrSet; //!< Container of const element pointers.
    
    // constructors
    Node(int classTag);
    Node(int tag, int classTag);
    Node(int tag, int ndof, double Crd1);
    Node(int tag, int ndof, double Crd1, double Crd2);
    Node(int tag, int ndof, double Crd1, double Crd2, double Crd3);
    Node(int tag, int ndof, const Vector &crds);
    Node(const Node &theCopy, bool copyMass = true);
    Node *getCopy(void) const;
    
    // destructor
    virtual ~Node(void);

    static DefaultTag &getDefaultTag(void);

    // public methods dealing with the DOF at the node
    virtual int getNumberDOF(void) const;    
    virtual void setDOF_GroupPtr(DOF_Group *theDOF_Grp);
    virtual DOF_Group *getDOF_GroupPtr(void);

    size_t getNumberOfConnectedConstraints(void) const;

    void connect(ContinuaReprComponent *el) const;
    void disconnect(ContinuaReprComponent *el) const;
    size_t getNumberOfConnectedElements(void) const;
    ElementConstPtrSet getConnectedElements(void) const;
    ElementPtrSet getConnectedElements(void);
    const MeshEdge *next(const std::deque<MeshEdge> &, const std::set<const MeshEdge *> &) const;

    const bool isDead(void) const;
    const bool isAlive(void) const;
    const bool isFrozen(void) const;
    const bool isFree(void) const;
    void kill(void);
    void alive(void);
    void freeze_if_dead(NodeLocker *locker);
    void melt_if_alive(NodeLocker *locker);

    SFreedom_Constraint *fix(const SFreedom_Constraint &);
    void fix(const std::vector<int> &, const std::vector<double> &);
    void fix(const ID &, const Vector &);

    // public methods for obtaining the nodal coordinates
    virtual size_t getDim(void) const;
    virtual const Vector &getCrds(void) const;
    virtual Vector &getCrds(void);
    Vector getCrds3d(void) const;
    Pos2d getPosition2d(const Vector &) const;
    Pos3d getPosition3d(const Vector &) const;
    Pos2d getInitialPosition2d(void) const;
    Pos3d getInitialPosition3d(void) const;
    Pos2d getCurrentPosition2d(const double &factor= 1.0) const;
    Pos3d getCurrentPosition3d(const double &factor= 1.0) const;
    bool In(const GeomObj3d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool In(const GeomObj2d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool Out(const GeomObj3d &,const double &factor= 1.0, const double &tol= 0.0) const;
    bool Out(const GeomObj2d &,const double &factor= 1.0, const double &tol= 0.0) const;
    void setPos(const Pos3d &);
    void Move(const Vector3d &desplaz);  
    void Transform(const TrfGeom &trf);
    double getDist2(const Pos2d &p,bool initialGeometry= true) const;
    double getDist(const Pos2d &p,bool initialGeometry= true) const;
    double getDist2(const Pos3d &p,bool initialGeometry= true) const;
    double getDist(const Pos3d &p,bool initialGeometry= true) const;

    // public methods for obtaining committed and trial 
    // response quantities of the node
    virtual const Vector &getDisp(void) const;
    Vector getDispXYZ(void) const;
    Vector getRotXYZ(void) const;
    virtual const Vector &getVel(void) const;
    Vector getVelXYZ(void) const;
    Vector getOmegaXYZ(void) const;
    virtual const Vector &getAccel(void) const;    
    Vector getAccelXYZ(void) const;
    Vector getAlphaXYZ(void) const;
    virtual const Vector &getIncrDisp(void) const;
    virtual const Vector &getIncrDeltaDisp(void) const;

    virtual const Vector &getTrialDisp(void) const;
    virtual const Vector &getTrialVel(void) const;
    virtual const Vector &getTrialAccel(void) const;    

    // public methods for updating the trial response quantities
    virtual int setTrialDispComponent(double value, int dof);    
    virtual int setTrialDisp(const Vector &);    
    virtual int setTrialVel(const Vector &);    
    virtual int setTrialAccel(const Vector &);        

    virtual int incrTrialDisp(const Vector &);    
    virtual int incrTrialVel(const Vector &);    
    virtual int incrTrialAccel(const Vector &);        

    const NodalLoad *newLoad(const Vector &);
    void createInertiaLoad(const Vector &);
    // public methods for adding and obtaining load information
    virtual void zeroUnbalancedLoad(void);
    virtual int addUnbalancedLoad(const Vector &load, double fact = 1.0); 
    virtual int addInertiaLoadToUnbalance(const Vector &accel, double fact = 1.0);    
    virtual const Vector &getUnbalancedLoad(void) const;     
    virtual const Vector &getUnbalancedLoadIncInertia(void) const;        

    // public methods dealing with the committed state of the node
    virtual int commitState();
    virtual int revertToLastCommit();    
    virtual int revertToStart();        

    // public methods for dynamic analysis
    virtual const Matrix &getMass(void) const;
    virtual int setMass(const Matrix &theMass);
    virtual int setNumColR(int numCol);
    virtual int setR(int row, int col, double Value);
    virtual const Vector &getRV(const Vector &V);

    virtual int setRayleighDampingFactor(double alphaM);
    virtual const Matrix &getDamp(void) const;

    void addTributary(const double &) const;
    void resetTributary(void) const;
    const double &getTributary(void) const;

    // public methods for eigen vector
    virtual int setNumEigenvectors(int numVectorsToStore);
    virtual int setEigenvector(int , const Vector &);
    inline int getNumModes(void) const
      { return theEigenvectors.noCols(); }
    virtual Vector getEigenvector(int ) const;
    Vector getNormalizedEigenvector(int ) const;
    virtual const Matrix &getEigenvectors(void) const;
    Matrix getNormalizedEigenvectors(void) const;
    Pos2d getEigenPosition2d(const double &, int) const;
    Pos3d getEigenPosition3d(const double &, int) const;
    Vector3d getEigenvectorDisp3dComponents(int ) const;
    Vector3d getEigenvectorRot3dComponents(int ) const;
    
    //Angular frequencies.
    double getAngularFrequency(int) const;
    Vector getAngularFrequencies(void) const;

    //Modal participation factors.
    virtual double getModalParticipationFactor(int mode) const;
    Vector getModalParticipationFactors(void) const;
    virtual double getModalParticipationFactor(int mode,const std::set<int> &dofs) const;
    Vector getModalParticipationFactors(const std::set<int> &dofs) const;
    Vector getModalParticipationFactorsForDOFs(const boost::python::list &) const;
    //Distribution factors.
    Vector getDistributionFactor(int mode) const;
    Vector getDistributionFactor(int mode,const std::set<int> &dofs) const;
    Matrix getDistributionFactors(void) const;

    //Effective modal masses
    double getEffectiveModalMass(int mode) const;
    Vector getEffectiveModalMasses(void) const;

    //Equivalent static load.
    Vector getEquivalentStaticLoad(int mode,const double &) const;

    //Maximum modal values
    Vector getMaxModalDisplacement(int mode,const double &) const;
    Vector getMaxModalVelocity(int mode,const double &) const;
    Vector getMaxModalAcceleration(int mode,const double &) const;
    Vector getMaxModalDisplacementForDOFs(int mode,const double &,const std::set<int> &dofs) const;
    Vector getMaxModalVelocityForDOFs(int mode,const double &,const std::set<int> &dofs) const;
    Vector getMaxModalAccelerationForDOFs(int mode,const double &,const std::set<int> &dofs) const;
    Vector getMaxModalDisplacementForDOFs(int mode,const double &,const boost::python::list &dofs) const;
    Vector getMaxModalVelocityForDOFs(int mode,const double &,const boost::python::list &dofs) const;
    Vector getMaxModalAccelerationForDOFs(int mode,const double &,const boost::python::list &dofs) const;

    // public methods for output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    virtual void Print(std::ostream &s, int flag = 0);

    Vector3d get3dForceComponents(const Vector &) const;
    Vector3d get3dMomentComponents(const Vector &) const;
    
    virtual const Vector &getReaction(void) const;
    Vector3d getReactionForce3d(void) const;
    Vector3d getReactionMoment3d(void) const;
    const Vector &getResistingForce(const ElementConstPtrSet &,const bool &) const;
    SlidingVectorsSystem3d getResistingSlidingVectorsSystem3d(const ElementConstPtrSet &,const bool &) const;
    virtual int addReactionForce(const Vector &, double factor);
    virtual int resetReactionForce(bool inclInertia);
    bool checkReactionForce(const double &) const;

    // AddingSensitivity:BEGIN /////////////////////////////////////////
    int addInertiaLoadSensitivityToUnbalance(const Vector &accel, double fact = 1.0, bool tag=false);    
    Matrix getMassSensitivity(void) const;
    virtual const Matrix &getDampSensitivity(void) const;
    int getCrdsSensitivity(void) const;
    int saveSensitivity(Vector *v, Vector *vdot, Vector *vdotdot, int gradNum, int numGrads);
    double getDispSensitivity(int dof, int gradNum) const;
    double getVelSensitivity(int dof, int gradNum) const;
    double getAccSensitivity(int dof, int gradNum) const;
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);

    // AddingSensitivity:END ///////////////////////////////////////////

  };

Pos3d pos_node(const Node &nod,bool initialGeometry= true);

} // end of XC namespace

#endif

