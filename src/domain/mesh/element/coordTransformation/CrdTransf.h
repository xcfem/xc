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

// $Revision: 1.4 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/coordTransformation/CrdTransf.h,v $


// File: ~/crdTransf/CrdTransf.h
//
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for 
//
// What: "@(#) CrdTransf.h, revA"

#ifndef CrdTransf_h
#define CrdTransf_h

#include "utility/actor/actor/MovableObject.h"
#include <utility/tagged/TaggedObject.h>
#include <utility/matrix/Vector.h>

class TritrizPos3d;

namespace XC {
class Matrix;
class Node;


//! \ingroup Elem
//!
//!
//! @defgroup ElemCT Transformaciones de coordenadas de los elementos.
//!
//! \ingroup ElemCT
//
//! @brief CrdTransf provides the abstraction of a frame 
//! coordinate transformation. It is an abstract base class and 
//! thus no objects of  it's type can be instatiated. It has pure 
//! virtual functions which  must be implemented in it's derived classes.
class CrdTransf: public TaggedObject, public MovableObject
  {
  protected:
    Node *nodeIPtr, *nodeJPtr; //!< punteros a los nodos extremos del elemento.
    mutable double L; //!< element length
    Vector nodeIOffset,nodeJOffset; //!< rigid joint offsets
    std::vector<double> nodeIInitialDisp; //!< Desplazamientos inicial nodo I.
    std::vector<double> nodeJInitialDisp; //!< Desplazamientos inicial nodo J.
    bool initialDispChecked;

    int set_ptr_nodos(Node *nodeIPointer, Node *nodeJPointer);

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    virtual void set_rigid_joint_offsetI(const Vector &rigJntOffsetI)= 0;
    virtual void set_rigid_joint_offsetJ(const Vector &rigJntOffsetJ)= 0;
  public:
    CrdTransf(int tag, int classTag, int dim_joint_offset);
    CrdTransf(void);
    virtual ~CrdTransf(void);
    
    virtual int initialize(Node *node1Pointer, Node *node2Pointer) = 0;
    virtual int update(void) = 0;
    virtual double getInitialLength(void) const= 0;
    virtual double getDeformedLength(void) const= 0;
    
    virtual int commitState(void) = 0;
    virtual int revertToLastCommit(void) = 0;        
    virtual int revertToStart(void) = 0;
    
    virtual const Vector &getBasicTrialDisp(void) const= 0;
    virtual const Vector &getBasicIncrDisp(void) const= 0;
    virtual const Vector &getBasicIncrDeltaDisp(void) const= 0;
    virtual const Vector &getBasicTrialVel(void) const= 0;
    virtual const Vector &getBasicTrialAccel(void) const= 0;
    
    // AddingSensitivity:BEGIN //////////////////////////////////
    virtual const Vector &getBasicDisplSensitivity(int gradNumber);
    virtual const Vector &getGlobalResistingForceShapeSensitivity(const Vector &basicForce, const Vector &uniformLoad);
    virtual const Vector &getBasicTrialDispShapeSensitivity(void);
    // AddingSensitivity:END //////////////////////////////////
    
    virtual const Vector &getGlobalResistingForce(const Vector &basicForce, const Vector &uniformLoad) const= 0;
    virtual const Matrix &getGlobalStiffMatrix(const Matrix &basicStiff, const Vector &basicForce) const= 0;
    virtual const Matrix &getInitialGlobalStiffMatrix(const Matrix &basicStiff) const= 0;
    
    virtual const Vector &getI(void) const= 0;
    virtual const Vector &getJ(void) const= 0;
    virtual Matrix getLocalAxes(bool) const= 0;

    virtual const Vector &getPointGlobalCoordFromLocal(const Vector &localCoords) const= 0;
    virtual const Vector &getPointGlobalCoordFromBasic(const double &xi) const= 0;
    virtual Vector getPointLocalCoordFromGlobal(const Vector &globalCoords) const= 0;
    double getPointBasicCoordFromGlobal(const Vector &globalCoords) const;
    const Matrix &getPointsGlobalCoordFromLocal(const Matrix &localCoords) const;
    virtual const Matrix &getPointsGlobalCoordFromBasic(const Vector &) const= 0;
    virtual const Vector &getPointGlobalDisplFromBasic(double xi, const Vector &basicDisps) const= 0;

    virtual const Vector &getVectorGlobalCoordFromLocal(const Vector &localCoords) const= 0;
    virtual const Matrix &getVectorGlobalCoordFromLocal(const Matrix &localCoords) const= 0;
    virtual const Vector &getVectorLocalCoordFromGlobal(const Vector &globalCoords) const= 0;

    virtual const Matrix &getCooNodos(void) const= 0;
    virtual const Matrix &getCooPuntos(const size_t &ndiv) const= 0;
    virtual const Vector &getCooPunto(const double &xrel) const= 0;

  };
} // end of XC namespace

#endif
