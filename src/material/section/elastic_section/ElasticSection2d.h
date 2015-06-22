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
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/fiber_section/ElasticSection2d.h,v $
                                                                        
                                                                        
///////////////////////////////////////////////////////
// File:  ~/Src/element/hinge/ElasticSection2d.h
//
// Written by Matthew Peavy
//
// Written:  Feb 13, 2000
// Debugged: Feb 14, 2000
// Revised:  May 2000 -- MHS
//
//
// Purpose:  This header file contains the prototype
// for the ElasticSection2d class.

#ifndef ElasticSection2d_h
#define ElasticSection2d_h

#include <material/section/elastic_section/BaseElasticSection2d.h>

namespace XC {
class Channel;
class FEM_ObjectBroker;
class Information;

//! \ingroup MATSCCElastica
//
//! @brief Sección transversal formada
//! por un material elástico lineal en un problema bidimensional
//! (3 esfuerzos por sección).
class ElasticSection2d: public BaseElasticSection2d
  {
  private:
    static Vector s;

  protected:
    bool procesa_comando(CmdStatus &status);

  public:
    ElasticSection2d(int tag, double E, double A, double I);
    ElasticSection2d(int tag, double EA, double EI);
    ElasticSection2d(int tag,MaterialLoader *mat_ldr= NULL);    
    ElasticSection2d(void);    

    const Vector &getStressResultant(void) const;
    const Matrix &getSectionTangent(void) const;
    const Matrix &getInitialTangent(void) const;
    const Matrix &getSectionFlexibility(void) const;
    const Matrix &getInitialFlexibility(void) const;

    SectionForceDeformation *getCopy(void) const;
    const ResponseId &getType(void) const;
    int getOrder(void) const;
    
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
    
    any_const_ptr GetProp(const std::string &cod) const;
    void Print(std::ostream &s, int flag =0) const;
  };
} // fin namespace XC

#endif
