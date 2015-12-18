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
// $Date: 2003/02/14 23:02:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/utility/tagged/TaggedObject.h,v $
                                                                        
                                                                        
#ifndef TaggedObject_h
#define TaggedObject_h

// File: ~/utility/tagged/TaggedObject.h
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for TaggedObject.
// A TaggedObject is an object with an integer identifier. It is used as
// a base class by DomainComponent, Graph and other classes in the framework.
//
// What: "@(#) TaggedObject.h, revA"

#include "xc_utils/src/nucleo/EntCmd.h"

namespace XC {
class Domain;
class Vector;
class Matrix;
class FEM_ObjectBroker;

//! @brief Objeto con identificador de tipo entero.
class TaggedObject: public EntCmd
  {
  private:    
    int theTag;
  protected:
    friend class ElementLoader;
    friend class CommParameters;
    template <class T> friend T *getBrokedTagged(const int &,const int &,const int &,FEM_ObjectBroker &,T *(FEM_ObjectBroker::*ptrFunc)(int));
    void setTag(int newTag);  // CAUTION: this is a dangerous method to call
    bool procesa_comando(CmdStatus &status);
  public:
    TaggedObject(int tag,EntCmd *owr= NULL);
    virtual TaggedObject *getCopy(void) const;
    virtual ~TaggedObject(void) {}

    static any_const_ptr get_prop_vector(const Vector *ptrVector);
    static any_const_ptr get_prop_vector(const Vector &ptrVector);
    static any_const_ptr get_row(const Matrix *,int);
    static any_const_ptr get_row(const Matrix &,int);
    static any_const_ptr get_col(const Matrix *,int);
    static any_const_ptr get_col(const Matrix &,int);

    void assignTag(int newTag);
    inline int getTag(void) const;

    virtual any_const_ptr GetProp(const std::string &cod) const;
    virtual void Print(std::ostream &s, int flag =0);       
    friend std::ostream &operator<<(std::ostream &s, TaggedObject &m);        
  };

inline int TaggedObject::getTag(void) const
  { return theTag; }


} // end of XC namespace

#endif


