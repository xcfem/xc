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
// $Source: /usr/local/cvs/OpenSees/SRC/CrdTransf3d.h,v $


// File: ~/CrdTransf/CrdTransf3d.h
//
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for 
// CrdTransf3d. CrdTransf3d provides the abstraction of spatial 
// coordinate transformation for a 3d frame. 

//
// What: "@(#) CrdTransf3d.h, revA"

#ifndef CrdTransf3d_h
#define CrdTransf3d_h

#include "CrdTransf.h"
#include "utility/matrix/Matrix.h"

class Ref3d3d;

namespace XC {
class Vector;

//! \ingroup ElemCT
//
//! @brief Clase base para las transformaciones de coordenadas en 3d.
class CrdTransf3d: public CrdTransf
  {
  protected:
    mutable Matrix R; //!< Transformation matrix

    void set_rigid_joint_offsetI(const Vector &rigJntOffsetI);
    void set_rigid_joint_offsetJ(const Vector &rigJntOffsetJ);
    static void inic_ug(const Vector &d1,const Vector &d2,double *ug);
    void modif_ug_init_disp(double *ug) const;
    void global_to_local(const double *ug,double *ul) const;
    void calc_Wu(const double *ug,double *ul,double *Wu) const;
    const Vector &calc_ub(const double *ul,Vector &) const;

    static Vector vectorI;
    static Vector vectorJ;
    static Vector vectorK;
    static Vector vectorCoo;
    virtual int computeElemtLengthAndOrient(void) const= 0;
    virtual int calculaEjesLocales(void) const= 0;

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    CrdTransf3d(int tag, int classTag);
    CrdTransf3d(int tag, int classTag, const Vector &vecInLocXZPlane);
    
    virtual CrdTransf3d *getCopy(void) const= 0;
    int initialize(Node *node1Pointer, Node *node2Pointer);
    virtual void set_xz_vector(const Vector &vecInLocXZPlane);
    Vector get_xz_vector(void) const;
    const Vector &getI(void) const;
    const Vector &getJ(void) const;
    const Vector &getK(void) const;
    int getLocalAxes(Vector &xAxis, Vector &yAxis, Vector &zAxis) const; 
    Pos3d getPosNodeI(void) const;
    Pos3d getPosNodeJ(void) const;
    Ref3d3d getLocalReference(void) const;
    Vector getPointLocalCoordFromGlobal(const Vector &xg) const;
    const Vector &getPointGlobalCoordFromBasic(const double &xi) const;
    const Matrix &getPointsGlobalCoordFromBasic(const Vector &) const;
    const Vector &getVectorGlobalCoordFromLocal(const Vector &localCoords) const;
    const Matrix &getVectorGlobalCoordFromLocal(const Matrix &localCoords) const;
    const Vector &getVectorLocalCoordFromGlobal(const Vector &globalCoords) const;

    const Matrix &getCooNodos(void) const;
    const Matrix &getCooPuntos(const size_t &ndiv) const;
    const Vector &getCooPunto(const double &xrel) const;

    void gira(const double &);
  };
} // end of XC namespace

#endif
