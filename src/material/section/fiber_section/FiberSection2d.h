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
                                                                        
// $Revision: 1.10 $
// $Date: 2003/03/04 00:48:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/FiberSection2d.h,v $
                                                                        
// Written: fmk
// Created: 04/01
//
// Description: This file contains the class definition for 
// FiberSection2d.h. FiberSection2d provides the abstraction of a 
// 2d beam section discretized by fibers. The section stiffness and
// stress resultants are obtained by summing fiber contributions.

#ifndef FiberSection2d_h
#define FiberSection2d_h

#include <material/section/fiber_section/FiberSectionBase.h>
#include <material/section/repres/section/contenedor_fibras.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

namespace XC {
class FiberSectionRepr;

//! \ingroup MATSCCModeloFibras
//
//! @brief Esta clase modeliza el comportamiento de la sección
//! discretizando la misma en un conjunto de fibras. La rigidez
//! de la sección y las resultantes de esfuerzos se obtienen sumando
//! las contribuciones de cada fibra.
class FiberSection2d: public FiberSectionBase
  {
  private:
// AddingSensitivity:BEGIN //////////////////////////////////////////
    int parameterID;
// AddingSensitivity:END ///////////////////////////////////////////
    friend class DqFibras;
    double get_strain(const double &y) const;
  protected:

  public:
    FiberSection2d(MaterialLoader *mat_ldr= nullptr); 
    FiberSection2d(int tag,MaterialLoader *mat_ldr= nullptr); 
    FiberSection2d(int tag,const contenedor_fibras &fibers,MaterialLoader *mat_ldr= nullptr);

    int setInitialSectionDeformation(const Vector &deforms); 
    int setTrialSectionDeformation(const Vector &deforms); 
    virtual double getStrain(const double &y,const double &z= 0) const;

    virtual void setupFibers(void);

    const Matrix &getInitialTangent(void) const;

    int revertToLastCommit(void);    
    int revertToStart(void);

    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType (void) const;
    int getOrder(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag = 0);
	    
    int getResponse(int responseID, Information &info);

    Fiber *addFiber(Fiber &theFiber);
    Fiber *addFiber(int tag,const MaterialLoader &,const std::string &nmbMat,const double &, const Vector &position);

    // AddingSensitivity:BEGIN //////////////////////////////////////////
    int setParameter(const std::vector<std::string> &argv, Parameter &param);
    int updateParameter(int parameterID, Information &info);
    int activateParameter(int parameterID);
    const Vector & getStressResultantSensitivity(int gradNumber, bool conditional);
    const Vector & getSectionDeformationSensitivity(int gradNumber);
    const Matrix & getSectionTangentSensitivity(int gradNumber);
    int   commitSensitivity(const Vector& sectionDeformationGradient, int gradNumber, int numGrads);
    // AddingSensitivity:END ///////////////////////////////////////////
    inline virtual double getCdgZ(void) const
      { return 0.0; }

  };

FiberSection2d FiberSectionReprToFiberSection2d(const int &tag,const FiberSectionRepr &fiberSectionRepr);
} // end of XC namespace

#endif
