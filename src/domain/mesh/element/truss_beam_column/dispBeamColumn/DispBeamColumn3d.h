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
                                                                        
// $Revision: 1.11 $
// $Date: 2003/02/25 23:32:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/dispBeamColumn/DispBeamColumn3d.h,v $

// Written: MHS
// Created: Feb 2001
//
// Description: This file contains the class definition for DispBeamColumn3d.
// The element displacement field gives rise to constant axial strain,
// linear curvature, and constant twist angle.

#ifndef DispBeamColumn3d_h
#define DispBeamColumn3d_h

#include <domain/mesh/element/truss_beam_column/BeamColumnWithSectionFDTrf3d.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/truss_beam_column/nonlinearBeamColumn/quadrule/GaussQuadRule1d01.h>
#include "domain/mesh/element/fvectors/FVectorBeamColumn3d.h"

namespace XC {
class Node;
class SeccionBarraPrismatica;
class CrdTransf3d;
class Response;

//! \ingroup ElemBarra
//
//! @brief Elemento barra con material de tipo SeccionBarraPrismatica  para problemas tridimensionales.
class DispBeamColumn3d : public BeamColumnWithSectionFDTrf3d
  {
  private:
    mutable Vector q; //!< Basic force
    FVectorBeamColumn3d q0;  // Fixed end forces in basic system (no torsion)
    FVectorBeamColumn3d p0;  // Reactions in basic system (no torsion)

    double rho;			// Mass density per unit length

    const Matrix &getInitialBasicStiff(void) const;

    static Matrix K;		// Element stiffness, damping, and mass Matrix
    static Vector P;		// Element resisting force vector


    static double workArea[];

    static GaussQuadRule1d01 quadRule;
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    DispBeamColumn3d(int tag= 0);
    DispBeamColumn3d(int tag,int numSec,const Material *theSection,const CrdTransf *trf);
    DispBeamColumn3d(int tag, int nd1, int nd2,
		     int numSections,const std::vector <SeccionBarraPrismatica *> &s, CrdTransf3d &coordTransf, double rho = 0.0);
    Element *getCopy(void) const;

    int getNumDOF(void) const;
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    // public methods to obtain stiffness, mass, damping and residual information    
    int update(void);
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    void zeroLoad(void);
    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInfo);
    int getResponse(int responseID, Information &eleInfo);
  };
} // end of XC namespace

#endif

