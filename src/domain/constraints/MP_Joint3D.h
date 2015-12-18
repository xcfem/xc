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
                                                                        
// $Revision: 1.2 $
// $Date: 2004/09/01 04:01:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/joint/MP_Joint3D.h,v $
                                                                        
#ifndef MP_Joint3D_h
#define MP_Joint3D_h

// Written: Arash Altoontash, Gregory Deierlein
// Created: 04/03
// Revision: Arash

// Purpose: This file contains the class definition for MP_Joint3D.
// It is a sub class for MP_Constraint specialized to be used for simple joint 
// connection element. MP_Joint3D defines a nonlinear, time dependent multi 
// point constraint.
//

#include <domain/constraints/MP_Constraint.h>
#include "utility/matrix/Vector.h"


namespace XC {
class Matrix;
class ID;
class Node;

//! @ingroup CContMP
//
//! @brief ??.
class MP_Joint3D: public MP_Constraint
  {
  private:
    int nodeRotation; //!< tag for the node to define the rotation vector
    // for shear rotation
    int RotDOF; //!< tag for the shear mode that results in rotation
    int nodeDisplacement; //!< tag for the node to define the rotation vector
    // for shear displacement
    int DispDOF; //!< tag for the shear mode that results in displacement
    int LargeDisplacement; //!< flag for large displacements
    // 0 for constant constraint matrix(small deformations)
    // 1 for time varying constraint matrix(large deformations)
    // 2 for large deformations with length correction
    Node *RetainedNode;	//!< to identify the retained node
    Node *ConstrainedNode; //!< to identify  the constrained node
    Node *RotationNode;
    Node *DisplacementNode;
    
    Vector RotNormVect;
    Vector DspNormVect;
    
    int dbTag3;	//!< need a dbTag for the two ID's
    
    double Length0;
  public:
    // constructors        
    MP_Joint3D();

    MP_Joint3D(Domain *theDomain, int tag, int nodeRetain, int nodeConstr,
		int nodeRot, int Rotdof, int nodeDisp, int Dispdof, int LrgDsp = 0 );
    ~MP_Joint3D(void);

    // method to get information about the constraint
    int applyConstraint(double pseudoTime);
    bool isTimeVarying(void) const;
    const Matrix &getConstraint(void);    
    void setDomain(Domain *theDomain);

    // methods for output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);
  };
} // end of XC namespace

#endif

