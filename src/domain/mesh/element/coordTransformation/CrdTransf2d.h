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

// $Revision: 1.3 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/CrdTransf2d.h,v $


// File: ~/CrdTransf/CrdTransf2d.h
//
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for 
// CrdTransf2d. CrdTransf2d provides the abstraction of spatial 
// coordinate transformation for a 2d frame. 

//
// What: "@(#) CrdTransf2d.h, revA"

#ifndef CrdTransf2d_h
#define CrdTransf2d_h

#include "CrdTransf.h"

class Ref2d2d;

namespace XC {
//! \ingroup ElemCT
//
//! @brief Clase base para las transformaciones de coordenadas en 3d.
class CrdTransf2d: public CrdTransf
  {
  protected:
    mutable double cosTheta, sinTheta; //! direction cosines of undeformed element wrt to global system 
    void set_rigid_joint_offsetI(const Vector &rigJntOffsetI);
    void set_rigid_joint_offsetJ(const Vector &rigJntOffsetJ);
    inline double T02(void) const
      { return -cosTheta*nodeIOffset(1)+sinTheta*nodeIOffset(0); }
    inline double T12(void) const
      { return  sinTheta*nodeIOffset(1)+cosTheta*nodeIOffset(0); }
    inline double T35(void) const
      { return -cosTheta*nodeJOffset(1)+sinTheta*nodeJOffset(0); }
    inline double T45(void) const
      { return  sinTheta*nodeJOffset(1)+cosTheta*nodeJOffset(0); }

    void disp_init_nodos(void);
    int computeElemtLengthAndOrient(void) const;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    CrdTransf2d(int tag, int classTag);

    int initialize(Node *node1Pointer, Node *node2Pointer);    
    virtual CrdTransf2d *getCopy(void) const= 0;
    //! @brief Devuelve la longitud inicial del elemento.
    inline double getInitialLength(void) const
      { return L; }
    //! @brief Devuelve la longitud del elemento deformado.
    inline double getDeformedLength(void) const
      { return L; }
    const Vector &getBasicTrialDisp(void) const;
    const Vector &getBasicIncrDisp(void) const;
    const Vector &getBasicIncrDeltaDisp(void) const;
    const Vector &getBasicTrialVel(void) const;
    const Vector &getBasicTrialAccel(void) const;

    const Vector &getInitialI(void) const;
    const Vector &getI(void) const;
    const Vector &getInitialJ(void) const;
    const Vector &getJ(void) const;
    int getInitialLocalAxes(Vector &xAxis, Vector &yAxis) const; 
    int getLocalAxes(Vector &xAxis, Vector &yAxis) const;
    Pos2d getPosNodeI(void) const;
    Pos2d getPosNodeJ(void) const;
    Ref2d2d getLocalReference(void) const;
    Vector getPointLocalCoordFromGlobal(const Vector &xg) const;
    const Vector &getPointGlobalCoordFromBasic(const double &xi) const;
    const Matrix &getPointsGlobalCoordFromBasic(const Vector &) const;
    const Vector &getVectorGlobalCoordFromLocal(const Vector &localCoords) const;
    const Matrix &getVectorGlobalCoordFromLocal(const Matrix &localCoords) const;
    const Vector &getVectorLocalCoordFromGlobal(const Vector &globalCoords) const;

    const Matrix &getCooNodos(void) const;
    const Matrix &getCooPuntos(const size_t &ndiv) const;
    const Vector &getCooPunto(const double &xrel) const;
  };
} // end of XC namespace

#endif
