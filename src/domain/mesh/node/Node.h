//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
class SVD3d;

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


class DOF_Group;

//! \ingroup Mesh
//!
//!
//! @defgroup Nod Nodos de la malla de elementos finitos.
//!
//! \ingroup Nod
//
//! @brief Nodos de la malla de elementos finitos.
class Node: public MeshComponent
  {
  private:
    // private data associated with each node object
    int numberDOF; // number of dof at Node
    DOF_Group *theDOF_GroupPtr; //!< pointer to associated DOF_Group
    Vector Crd; //!< original nodal coords
    
    NodeDispVectors disp; //! Vectores desplazamiento (commited,trial,...)
    NodeVelVectors vel; //! Vectores velocidad (commited,trial,...)
    NodeAccelVectors accel; //! Vectores aceleración (commited,trial,...)

    Matrix R; //!< nodal (modal?) participation matrix
    Matrix mass; //!< mass matrix
    mutable Vector unbalLoad; //!< unbalanced load
    Vector unbalLoadWithInertia;       
    mutable Vector reaction;
    double alphaM; //!< rayleigh damping factor
    mutable double tributaria; //!< longitud, area o volumen tributario.
    
    Matrix theEigenvectors; //Matriz de autovectores.

    // AddingSensitivity:BEGIN /////////////////////////////////////////
    Matrix dispSensitivity;
    Matrix velSensitivity;
    Matrix accSensitivity;
    int parameterID;
    // AddingSensitivity:END ///////////////////////////////////////////

    static std::deque<Matrix> theMatrices;

    mutable std::set<ContinuaReprComponent *> conectados; //!< Componentes que hacen referencia a este nodo.

    std::set<int> coacciones_freeze;//!< Tags de las coacciones introducidas por el método freeze():
    const ID &get_id_coacciones(void) const;
    void set_id_coacciones(const ID &);

    static DefaultTag defaultTag; //<! tag para el nuevo nodo.
  protected:

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
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

    void conecta(ContinuaReprComponent *el) const;
    void desconecta(ContinuaReprComponent *el) const;
    std::set<const Element *> getElementosConectados(void) const;
    std::set<Element *> getElementosConectados(void);

    const bool isDead(void) const;
    const bool isAlive(void) const;
    const bool isFrozen(void) const;
    const bool isFree(void) const;
    void kill(void);
    void alive(void);
    void freeze_if_dead(NodeLocker *locker);
    void melt_if_alive(NodeLocker *locker);

    SP_Constraint *fix(const SP_Constraint &);
    void fix(const std::vector<int> &, const std::vector<double> &);
    void fix(const ID &, const Vector &);

    // public methods for obtaining the nodal coordinates
    virtual size_t getDim(void) const;
    virtual const Vector &getCrds(void) const;
    virtual Vector &getCrds(void);
    Vector getCrds3d(void) const;
    Pos2d getPosInicial2d(void) const;
    Pos3d getPosInicial3d(void) const;
    Pos2d getPosFinal2d(void) const;
    Pos3d getPosFinal3d(void) const;
    void setPos(const Pos3d &);
    void Mueve(const Vector3d &desplaz);  
    void Transforma(const TrfGeom &trf);
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
    
    // public methods for adding and obtaining load information
    virtual void zeroUnbalancedLoad(void);
    virtual int addUnbalancedLoad(const Vector &load, double fact = 1.0); 
    virtual int addInertiaLoadToUnbalance(const Vector &accel, double fact = 1.0);    
    virtual const Vector &getUnbalancedLoad(void);     
    virtual const Vector &getUnbalancedLoadIncInertia(void);        

    // public methods dealing with the committed state of the node
    virtual int commitState();
    virtual int revertToLastCommit();    
    virtual int revertToStart();        

    // public methods for dynamic analysis
    virtual const Matrix &getMass(void);
    virtual int setMass(const Matrix &theMass);
    virtual int setNumColR(int numCol);
    virtual int setR(int row, int col, double Value);
    virtual const Vector &getRV(const Vector &V);

    virtual int setRayleighDampingFactor(double alphaM);
    virtual const Matrix &getDamp(void);

    void addTributaria(const double &) const;
    void resetTributaria(void) const;
    const double &getTributaria(void) const;

    // public methods for eigen vector
    virtual int setNumEigenvectors(int numVectorsToStore);
    virtual int setEigenvector(int mode, const Vector &eigenVector);
    inline int getNumModes(void) const
      { return theEigenvectors.noCols(); }
    virtual Vector getEigenvector(int mode) const;
    Vector getNormalizedEigenvector(int mode) const;
    virtual const Matrix &getEigenvectors(void);
    Matrix getNormalizedEigenvectors(void) const;
    
    //Factores de participación modal
    double getPulsacion(int mode) const;
    Vector getPulsaciones(void) const;

    //Factores de participación modal
    virtual double getModalParticipationFactor(int mode) const;
    Vector getModalParticipationFactors(void) const;
    virtual double getModalParticipationFactor(int mode,const std::set<int> &gdls) const;
    Vector getModalParticipationFactors(const std::set<int> &gdls) const;
    Vector getModalParticipationFactorsForGdls(const boost::python::list &) const;
    //Factores de distribución
    Vector getDistributionFactor(int mode) const;
    Vector getDistributionFactor(int mode,const std::set<int> &gdls) const;
    Matrix getDistributionFactors(void) const;

    //Masas modales efectivas.
    double getEffectiveModalMass(int mode) const;
    Vector getEffectiveModalMasses(void) const;

    //Fuerza estática equivalente.
    Vector getEquivalentStaticLoad(int mode,const double &) const;

    //Valores máximos modales
    Vector getMaxModalDisplacement(int mode,const double &) const;
    Vector getMaxModalVelocity(int mode,const double &) const;
    Vector getMaxModalAcceleration(int mode,const double &) const;
    Vector getMaxModalDisplacementForGdls(int mode,const double &,const std::set<int> &gdls) const;
    Vector getMaxModalVelocityForGdls(int mode,const double &,const std::set<int> &gdls) const;
    Vector getMaxModalAccelerationForGdls(int mode,const double &,const std::set<int> &gdls) const;
    Vector getMaxModalDisplacementForGdls(int mode,const double &,const boost::python::list &gdls) const;
    Vector getMaxModalVelocityForGdls(int mode,const double &,const boost::python::list &gdls) const;
    Vector getMaxModalAccelerationForGdls(int mode,const double &,const boost::python::list &gdls) const;

    // public methods for output
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

    virtual void Print(std::ostream &s, int flag = 0);

    virtual const Vector &getReaction(void) const;
    const Vector &getResistingForce(const std::set<const Element *> &,const bool &) const;
    SVD3d getResistingSVD3d(const std::set<const Element *> &,const bool &) const;
    virtual int addReactionForce(const Vector &, double factor);
    virtual int resetReactionForce(bool inclInertia);
    void checkReactionForce(const double &);

    // AddingSensitivity:BEGIN /////////////////////////////////////////
    int addInertiaLoadSensitivityToUnbalance(const Vector &accel, double fact = 1.0, bool tag=false);    
    Matrix getMassSensitivity(void);
    virtual const Matrix &getDampSensitivity(void);
    int getCrdsSensitivity(void);
    int saveSensitivity(Vector *v, Vector *vdot, Vector *vdotdot, int gradNum, int numGrads);
    double getDispSensitivity(int dof, int gradNum);
    double getVelSensitivity(int dof, int gradNum);
    double getAccSensitivity(int dof, int gradNum);
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);

    // AddingSensitivity:END ///////////////////////////////////////////

  };

Pos3d pos_nodo(const Node &nod,bool initialGeometry= true);

} // end of XC namespace

#endif

