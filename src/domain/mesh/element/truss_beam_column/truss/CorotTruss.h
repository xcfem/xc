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
                                                                        
// $Revision: 1.7 $
// $Date: 2003/03/12 19:20:46 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/truss/CorotTruss.h,v $

#ifndef CorotTruss_h
#define CorotTruss_h

// Written: MHS 
// Created: May 2001
//
// Description: This file contains the class definition for CorotTruss,
// a small strain, large displacement corotational space truss element,
// as described by Crisfield in "Nonlinear Finite Element Analysis of
// Solids and Structures", Vol. 1, 1991, J.T. Wiley.

#include "CorotTrussBase.h"
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Channel;
class UniaxialMaterial;

//! \ingroup ElemBarraArt
//
//! @brief Elemento de tipo barra articulada con formulación corrotacional.
class CorotTruss: public CorotTrussBase
  {
  private:
    UniaxialMaterial *theMaterial; //!< pointer to a material

    double A;//!< area of CorotTruss
  protected:
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
  public:
    CorotTruss(int tag, int dim,int Nd1, int Nd2, UniaxialMaterial &theMaterial,double A);
    CorotTruss(int tag,int dimension,const Material *ptr_mat);
    CorotTruss(void);
    CorotTruss(const CorotTruss &);
    CorotTruss &operator=(const CorotTruss &);
    Element *getCopy(void) const;
    ~CorotTruss(void);

    // public methods to obtain information about dof & connectivity    
    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        
    int update(void);

    const Material *getMaterial(void) const;
    Material *getMaterial(void);
    virtual double getRho(void) const;
    inline const double &getArea(void) const
      { return A; }
    inline void setArea(const double &a)
      { A= a; }

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;
    const Matrix &getMass(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    double getAxil(void) const;
    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;            

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);    

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInfo);
    int getResponse(int responseID, Information &eleInformation);
    any_const_ptr GetProp(const std::string &cod) const;
  };
} // fin namespace XC

#endif




