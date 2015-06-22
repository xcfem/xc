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
                                                                        
// $Revision: 1.9 $
// $Date: 2003/03/04 00:48:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/SectionForceDeformation.h,v $
                                                                        
                                                                        
#ifndef SectionForceDeformation_h
#define SectionForceDeformation_h

// File: ~/material/SectionForceDeformation.h
//
// Written: MHS
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class definition for SectionForceDeformation.
// SectionForceDeformation is an abstract base class and thus no objects of it's type
// can be instantiated. It has pure virtual functions which must be
// implemented in it's derived classes. 
//
// What: "@(#) SectionForceDeformation.h, revA"

#include "material/Material.h"

namespace XC {

class Information;
class Response;
class Vector;
class Matrix;
class ResponseId;
class MaterialLoader;

//! \ingroup Mat
//!
//!
//! \defgroup MATSCC Modelización de una sección transversal.
//!
//! \ingroup MATSCC
//
//! @brief Clase base para las secciones transversales. Representa
//! las ecuaciones consititutivas de la sección.
class SectionForceDeformation: public Material
  {
  protected:
    mutable Matrix *fDefault; //!< Matriz de flexibilidad por defecto.
    MaterialLoader *material_loader; //!< Gestor de materiales (búsqueda,...).

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    bool procesa_comando(CmdStatus &status);
    any_const_ptr GetPropShellResponse(const std::string &cod) const;
  public:
    SectionForceDeformation(int tag,int classTag,MaterialLoader *mat_ldr= NULL);
    SectionForceDeformation(const SectionForceDeformation &otro);
    SectionForceDeformation &operator=(const SectionForceDeformation &otro);
    virtual ~SectionForceDeformation(void);

    inline MaterialLoader *getMaterialLoader(void)
      { return material_loader; }

    virtual int setInitialSectionDeformation(const Vector &)= 0;
    virtual int addInitialSectionDeformation(const Vector &);
    inline void setInitialGeneralizedStrain(const Vector &iS)
      { setInitialSectionDeformation(iS); }
    const Vector &getInitialGeneralizedStrain(void) const
      { return getInitialSectionDeformation(); }


    virtual int setTrialSectionDeformation(const Vector &) = 0;
    virtual const Vector &getInitialSectionDeformation(void) const= 0;
    virtual const Vector &getSectionDeformation(void) const= 0;
    double getSectionDeformation(const int &) const;
    double getSectionDeformationByName(const std::string &) const;
    virtual double getStrain(const double &y,const double &z= 0) const;

    virtual const Vector &getStressResultant(void) const= 0;
    double getStressResultant(const int &) const;
    double getStressResultantByName(const std::string &) const;
    virtual const Matrix &getSectionTangent(void) const= 0;
    virtual const Matrix &getInitialTangent(void) const= 0;
    virtual const Matrix &getSectionFlexibility(void) const;
    virtual const Matrix &getInitialFlexibility(void) const;

    inline const Vector &getGeneralizedStress(void) const
      { return getStressResultant(); }
    virtual const Vector &getGeneralizedStrain(void) const
      { return getSectionDeformation(); }

    virtual double getRho(void) const;
 
    virtual SectionForceDeformation *getCopy(void) const= 0;
    virtual const ResponseId &getType(void) const= 0;
    std::string getTypeString(void) const;
    virtual int getOrder(void) const = 0;

    virtual Response *setResponse(const std::vector<std::string> &argv, Information &info);
    virtual int getResponse(int responseID, Information &info);

// AddingSensitivity:BEGIN //////////////////////////////////////////
    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);
    virtual int activateParameter(int parameterID);
    virtual const Vector &getStressResultantSensitivity(int gradNumber, bool conditional);
    virtual const Vector &getSectionDeformationSensitivity(int gradNumber);
    virtual const Matrix &getSectionTangentSensitivity(int gradNumber);
    virtual double getRhoSensitivity(int gradNumber);
    virtual int commitSensitivity(const Vector& sectionDeformationGradient, int gradNumber, int numGrads);
// AddingSensitivity:END ///////////////////////////////////////////
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC


#endif
