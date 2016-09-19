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
// $Date: 2003/02/25 23:33:34 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/SectionAggregator.h,v $
                                                                        
                                                                        
// File: ~/section/SectionAggregator.h
//
// Written: MHS
// Created: Jun 2000
// Revision: A
//
// Description: This file contains the class definition for 
// SectionAggregator.  SectionAggregator decorates an MP
// section (couple bending and axial) with an uncoupled shear
// relation.
//
// What: "@(#) SectionAggregator.h, revA"

#ifndef SectionAggregator_h
#define SectionAggregator_h

#include <material/section/SeccionBarraPrismatica.h>
#include <material/section/AggregatorAdditions.h>

namespace XC {
//! \ingroup MATSCC
//
//! @brief SectionAggregator decorates an MP
//! section (couple bending and axial) with an uncoupled shear
//! relation.
class SectionAggregator : public SeccionBarraPrismatica
  {
  private:
    SeccionBarraPrismatica *theSection;
    AggregatorAdditions theAdditions;
    
    Vector *def; //!< Storage for section deformations
    Vector *defzero; //!< Storage for initial section deformations
    Vector *s; //!< Storage for stress resultants
    Matrix *ks;//!< Storage for section stiffness
    Matrix *fs;//!< Storage for section flexibility
    ResponseId *theCode; //!< GDLs a los que éste material agrega rigidez.

    static double workArea[];
    //static int codeArea[];

    void copia_seccion(const SectionForceDeformation *);
    void check_ptrs(void) const;
    void alloc_storage_ptrs(void);
    void libera_storage_ptrs(void);
    void libera(void);
  protected:
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public:
    SectionAggregator(MaterialLoader *mat_ldr= NULL); 
    SectionAggregator(int tag,MaterialLoader *mat_ldr= NULL);
    SectionAggregator(int tag, SeccionBarraPrismatica &theSection,const AggregatorAdditions &theAdditions,MaterialLoader *mat_ldr= NULL); 
    SectionAggregator(int tag, const AggregatorAdditions &theAdditions,MaterialLoader *mat_ldr= NULL); 
    SectionAggregator(int tag, SeccionBarraPrismatica &thesection,UniaxialMaterial &theAddition, int c,MaterialLoader *mat_ldr= NULL);
    SectionAggregator(const SectionAggregator &otro);
    XC::SectionAggregator &operator=(const SectionAggregator &otro);
    ~SectionAggregator(void);

   inline SeccionBarraPrismatica *getSection(void)
     { return theSection; }
   void setSection(const std::string &sectionName);
   inline AggregatorAdditions &getAdditions(void)
     { return theAdditions; }
   void setAddtions(const std::vector<std::string> &,const std::vector<std::string> &);
   void setAddtionsPyList(const boost::python::list &,const boost::python::list &);


    int setInitialSectionDeformation(const Vector &deforms); 
    int setTrialSectionDeformation(const Vector &deforms); 
    virtual double getStrain(const double &y,const double &z) const;
    const Vector &getInitialSectionDeformation(void) const;
    const Vector &getSectionDeformation(void) const;
    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
    const Matrix &getInitialFlexibility(void) const;

    int commitState(void);
    int revertToLastCommit(void);    
    int revertToStart(void);
 
    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
 
    void Print(std::ostream &s, int flag =0) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &info);
    int getResponse(int responseID, Information &info);

    int setVariable(const std::string &argv);
    int getVariable(int variableID, double &info);

  };
} // end of XC namespace

#endif
