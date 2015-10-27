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

// $Revision: 1.12 $
// $Date: 2005/02/17 22:29:54 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/Element.h,v $


// Written: fmk
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for Element.
// Element is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes.
//
// What: "@(#) Element.h, revA"

#ifndef Element_h
#define Element_h

#include "domain/mesh/MeshComponent.h"
#include "preprocessor/ParamMallado.h"
#include "RayleighDampingFactors.h"
#include "utility/matrix/Matrix.h"
#include "domain/mesh/node/NodeTopology.h"

class TritrizPos3d;
class Pos2d;
class Pos3d;

namespace XC {
class Vector;
class Renderer;
class Info;
class Information;
class Parameter;
class Response;
class ElementalLoad;
class Node;
class TritrizPtrNod;
class TritrizPtrElem;
class SetBase;
class SetEstruct;
class NodePtrsWithIDs;
class Material;
class DqVectors;
class DqMatrices;
class DefaultTag;
class GaussModel;
class MEDGaussModel;
class ParticlePos3d;

//! \ingroup Mesh
//
//! @defgroup Elem Elementos de la malla de elementos finitos.
//!
//! \ingroup Elem
//
//! @brief Elementos de la malla de elementos finitos.
class Element: public MeshComponent
  {
  public:
    static double dead_srf;//!< Factor de reducción de la rigidez para elementos desactivados.
    typedef std::vector<const Node *> NodesEdge; //!< Nodos de un borde del elemento.
    //! @brief Assigns Stress Reduction Factor for element deactivation.
    inline static void setDeadSRF(const double &d)
      { dead_srf= d; }
  private:
    int nodeIndex;

    static std::deque<Matrix> theMatrices;
    static std::deque<Vector> theVectors1;
    static std::deque<Vector> theVectors2;

    void compute_damping_matrix(Matrix &) const;
    static DefaultTag defaultTag; //<! tag para un nuevo elemento.
  protected:
    friend class EntMdlr;
    friend class Preprocessor;
    virtual TritrizPtrElem coloca_en_malla(const TritrizPtrNod &,dir_mallado) const;
    virtual TritrizPtrElem cose(const SetEstruct &f1,const SetEstruct &f2) const;

    const Vector &getRayleighDampingForces(void) const;

    Vector load;//!< vector for applying loads
    mutable RayleighDampingFactors rayFactors; //!< Rayleigh damping factors
                                             //(mutable para que getDamp pueda ser const).
    mutable Matrix Kc; //!< pointer to hold last committed matrix if needed for rayleigh damping
                        //(mutable para que getDamp pueda ser const).
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    Element(int tag, int classTag);
    virtual Element *getCopy(void) const= 0;

    static DefaultTag &getDefaultTag(void);

    // methods dealing with nodes and number of external dof
    virtual int getNumExternalNodes(void) const =0;
    virtual NodePtrsWithIDs &getNodePtrs(void)= 0;	
    virtual const NodePtrsWithIDs &getNodePtrs(void) const= 0;	
    std::vector<int> getIdxNodes(void) const;
    virtual int getNumDOF(void) const= 0;
    virtual size_t getDimension(void) const;
    virtual void setIdNodos(const std::vector<int> &inodos);
    virtual void setIdNodos(const ID &inodos);
    void setDomain(Domain *theDomain);

    // methods dealing with committed state and update
    virtual int commitState(void);
    virtual int revertToLastCommit(void) = 0;
    virtual int revertToStart(void);
    virtual int update(void);
    virtual bool isSubdomain(void);

    // methods to return the current linearized stiffness,
    // damping and mass matrices
    virtual const Matrix &getTangentStiff(void) const= 0;
    virtual const Matrix &getInitialStiff(void) const= 0;
    virtual const Matrix &getDamp(void) const;
    virtual const Matrix &getMass(void) const;

    //Métodos para definir cargas.
    virtual void defEdgeLoadGlobal(const int &iEdge,const Vector &);

    // methods for applying loads
    virtual void zeroLoad(void);	
    virtual int addLoad(ElementalLoad *theLoad, double loadFactor)=0;
    virtual int addInertiaLoadToUnbalance(const Vector &accel)=0;

    virtual int setRayleighDampingFactors(const RayleighDampingFactors &rF) const;

    // methods for obtaining resisting force (force includes elemental loads)
    virtual const Vector &getResistingForce(void) const= 0;
    virtual const Vector &getResistingForceIncInertia(void) const;
    const Vector &getNodeResistingComponents(const size_t &,const Vector &) const;
    const Vector &getNodeResistingForce(const size_t &iNod) const;
    const Vector &getNodeResistingForceIncInertia(const size_t &iNod) const;
    const Vector &getNodeResistingForce(const Node *) const;
    const Vector &getNodeResistingForceIncInertia(const Node *) const;
    Vector getEquivalentStaticLoad(int mode,const double &) const;
    Matrix getEquivalentStaticNodalLoads(int mode,const double &) const;

    // method for obtaining information specific to an element
    virtual Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    virtual int getResponse(int responseID, Information &eleInformation);
    Response *setMaterialResponse(Material *,const std::vector<std::string> &,const size_t &,Information &);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual int addInertiaLoadSensitivityToUnbalance(const Vector &accel, bool tag);
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int setMaterialParameter(Material *,const std::vector<std::string> &,const size_t &, Parameter &);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);
    virtual const Vector &getResistingForceSensitivity(int gradNumber);
    virtual const Matrix &getInitialStiffSensitivity(int gradNumber);
    virtual const Matrix &getDampSensitivity(int gradNumber);
    virtual const Matrix &getMassSensitivity(int gradNumber);
    virtual int   commitSensitivity(int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////

    virtual int addResistingForceToNodalReaction(bool inclInertia);

    double MaxCooNod(int i) const;
    double MinCooNod(int i) const;
    const Matrix &getCooNodos(void) const;
    Pos3d getPosNodo(const size_t &i,bool geomInicial= true) const;
    std::list<Pos3d> getPosNodos(bool geomInicial= true) const;
    virtual const Pos3d &getPosCdg(bool geomInicial= true) const;
    const Vector &getCooCdg(bool geomInicial= true) const;
    TritrizPos3d getPuntos(const size_t &ni,const size_t &nj,const size_t &nk,bool geomInicial= true);
    virtual double getDist2(const Pos2d &p,bool geomInicial= true) const;
    virtual double getDist(const Pos2d &p,bool geomInicial= true) const;
    virtual double getDist2(const Pos3d &p,bool geomInicial= true) const;
    virtual double getDist(const Pos3d &p,bool geomInicial= true) const;

    void resetTributarias(void) const;
    void vuelcaTributarias(const std::vector<double> &) const;
    virtual void calculaLongsTributarias(bool geomInicial= true) const;
    virtual double getLongTributaria(const Node *) const;
    virtual double getLongTributariaByTag(const int &) const;
    virtual void calculaAreasTributarias(bool geomInicial= true) const;
    virtual double getAreaTributaria(const Node *) const;
    virtual double getAreaTributariaByTag(const int &) const;
    virtual void calculaVolsTributarios(bool geomInicial= true) const;
    virtual double getVolTributario(const Node *) const;
    virtual double getVolTributarioByTag(const int &) const;

    virtual Vector getInterpolationFactors(const ParticlePos3d &) const;
    virtual Vector getInterpolationFactors(const Pos3d &) const;

    virtual int getVtkCellType(void) const;
    virtual int getMEDCellType(void) const;
    virtual const GaussModel &getGaussModel(void) const;
    MEDGaussModel getMEDGaussModel(void) const;
    virtual NodesEdge getNodesEdge(const size_t &) const;
    virtual int getEdgeNodes(const Node *,const Node *) const;
    int getEdgeNodes(const int &,const int &) const;
    virtual ID getEdgesNode(const Node *) const;
    std::set<int> getEdgesNodes(const NodePtrSet &) const;
    ID getEdgesNodeByTag(const int &) const;
    virtual ID getLocalIndexNodesEdge(const size_t &i) const;

    std::set<SetBase *> get_sets(void) const;
    void add_to_sets(std::set<SetBase *> &);

  };
} // fin namespace XC

#endif

