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
                                                                        
// $Revision: 1.8 $                                                              
// $Date: 2006/01/13 01:07:48 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/CorotCrdTransf2d.h,v $ 
                                        
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the class definition for
// CorotCrdTransf2d.h. CorotCrdTransf2d provides the
// abstraction of a corotation transformation for a planar frame element

// What: "@(#) CorotCrdTransf2d.h, revA"

#ifndef CorotCrdTransf2d_h
#define CorotCrdTransf2d_h

#include "CrdTransf2d.h"
#include <utility/matrix/Matrix.h>

namespace XC {
//! \ingroup ElemCT
//
//! @brief Transformación de coordenadas corrotacional en 3d.
class CorotCrdTransf2d: public CrdTransf2d
  {
  private:
    double cosAlpha, sinAlpha; // direction cossines of deformed element wrt to local system
    double Ln;                 // deformed element length
    double Lx, Ly;             // components of the deformed member
    mutable double Lxdot, Lydot; //!< time derivatives of components of the deformed member
    mutable double Lxdotdot, Lydotdot; //!< double time derivatives of components of the deformed member
    
    Vector ub;                 // basic displacements
    Vector ubcommit;           // commited basic displacements
    Vector ubpr;               // previous basic displacements
    
    bool nodeOffsets;

    static Matrix Tlg;         // matrix that transforms from global to local coordinates
    static Matrix Tbl;         // matrix that transforms from local  to basic coordinates
    static Matrix kg;     
    static Vector uxg;     
    static Vector pg;     
    static Vector dub;     
    static Vector Dub;     

    int compElemtLengthAndOrient(void);
    int compElemtLengthAndOrientWRTLocalSystem(const Vector &ul);
    void transfLocalDisplsToBasic(const Vector &ul);
    void getTransfMatrixLocalGlobal(Matrix &Tlg) const;
    void getTransfMatrixBasicLocal(Matrix &Tbl) const;
    const Matrix &getGeomStiffMatrix(const Vector &pb) const;
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    CorotCrdTransf2d(int tag= 0);
    CorotCrdTransf2d(int tag, const Vector &rigJntOffsetI, const Vector &rigJntOffsetJ);
    
    int initialize(Node *nodeIPointer, Node *nodeJPointer);
    int update(void);
    double getInitialLength(void) const;
    double getDeformedLength(void) const;
    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);
    
    const Vector &getBasicTrialDisp(void) const;
    const Vector &getBasicIncrDisp(void) const;
    const Vector &getBasicIncrDeltaDisp(void) const;
    const Vector &getBasicTrialVel(void) const;
    const Vector &getBasicTrialAccel(void) const;
    
    const Vector &getGlobalResistingForce(const Vector &basicForce, const Vector &uniformLoad) const;
    const Matrix &getGlobalStiffMatrix(const Matrix &basicStiff, const Vector &basicForce) const;
    const Matrix &getInitialGlobalStiffMatrix(const Matrix &basicStiff) const;
    
// AddingSensitivity:BEGIN //////////////////////////////////
    const Vector &getBasicDisplSensitivity(int gradNumber);
    const Vector &getGlobalResistingForceShapeSensitivity(const Vector &q,const Vector &p0,int gradNumber);
    const Vector &getBasicTrialDispShapeSensitivity(void);
    bool isShapeSensitivity(void);
    double getdLdh(void);
    double getd1overLdh(void);
// AddingSensitivity:END //////////////////////////////////

    CrdTransf2d *getCopy(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    void Print(std::ostream &s, int flag = 0);
    
    // functions used in post-processing only    
    const Vector &getPointGlobalCoordFromLocal(const Vector &) const;
    const Vector &getPointGlobalDisplFromBasic(double xi, const Vector &basicDisps) const;
    const Vector &getI(void);
    const Vector &getJ(void);
    int getLocalAxes(Vector &xAxis, Vector &yAxis) const; 
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace
#endif
