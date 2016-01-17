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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/25 23:33:13 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/zeroLength/ZeroLengthSection.h,v $
                                                                        
// Written: MHS
// Created: Sept 2000
//
// Description: This file contains the class definition for ZeroLengthSection.
// A ZeroLengthSection element is defined by two nodes with the same coordinate.
// A SectionForceDeformation object is associated with the nodes to
// provide the basic force-deformation relationship for the element.

#ifndef ZeroLengthSection_h
#define ZeroLengthSection_h

#include "domain/mesh/element/Element0D.h"

namespace XC {
class Matrix;
class Channel;
class SectionForceDeformation;
class Response;

//! \ingroup Elem
//
//! @defgroup ElemZL Elementos de longitud 0.
//!
//! \ingroup ElemZL
//
//! @brief Elemento de longitud cero con material
//! de tipo SectionForceDeformation.
class ZeroLengthSection: public Element0D
  {
  private:
    Matrix A; //!< Transformation matrix ... e = A*(u2-u1)
    mutable Vector v; //!< Section deformation vector, the element basic deformations
    
    Matrix *K; //!< Pointer to element stiffness matrix
    Vector *P; //!< Pointer to element force vector
    
    SectionForceDeformation *theSection;  //!< Pointer to section object
    int order;	//!< Order of the section model
    
    // Class wide matrices for return
    static Matrix K6;
    static Matrix K12;
    
    // Class wide vectors for return
    static Vector P6;
    static Vector P12;

    // private methods
    void setTransformation(void);
    void computeSectionDefs(void) const;

    void setup_section(const Material *sec);
    void inicAv(void);
    void libera(void);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    ZeroLengthSection(int tag, int dimension, int Nd1, int Nd2, const Vector& x, const Vector& yprime, SectionForceDeformation& theSection);
    ZeroLengthSection(int tag, int dimension,const Material *theSection);
    ZeroLengthSection(void);
    ZeroLengthSection(const ZeroLengthSection &otro);
    ZeroLengthSection &operator=(const ZeroLengthSection &otro);
    Element *getCopy(void) const;
    ~ZeroLengthSection(void);

    void setDomain(Domain *theDomain);

    // public methods to set the state of the element    
    int commitState(void);
    int revertToLastCommit(void);        
    int revertToStart(void);        

    // public methods to obtain stiffness, mass, damping and residual information    
    const Matrix &getTangentStiff(void) const;
    const Matrix &getInitialStiff(void) const;

    int addLoad(ElementalLoad *theLoad, double loadFactor);
    int addInertiaLoadToUnbalance(const Vector &accel);

    const Vector &getResistingForce(void) const;
    const Vector &getResistingForceIncInertia(void) const;

    inline SectionForceDeformation *getSection(void)
      { return theSection; }            

    // public methods for element output
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);    

    Response *setResponse(const std::vector<std::string> &argv, Information &eleInformation);
    int getResponse(int responseID, Information &eleInformation);
  };
} // end of XC namespace

#endif




