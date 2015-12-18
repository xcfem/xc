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
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ElasticPlateSection.h,v $

// Ed "C++" Love
//
//  Elastic Plate Section
//

#ifndef ElasticPlateSection_h
#define ElasticPlateSection_h

#include <material/section/plate_section/ElasticPlateProto.h>


namespace XC {
//! \ingroup MATPLAC
//
//! @brief Sección elástica para materiales bidimensionales
//! (como los de placas).
class ElasticPlateSection: public ElasticPlateProto<5>
  {
  public: 
    ElasticPlateSection(int tag) ;
    ElasticPlateSection(void) ;
    ElasticPlateSection(int tag, double E, double nu,double h = 1.0 ) ;
    SectionForceDeformation *getCopy(void) const;

    const ResponseId &getType(void) const;

    const Vector &getStressResultant(void) const;
    const Matrix& getSectionTangent(void) const;
    const Matrix& getInitialTangent(void) const;

    void Print(std::ostream &s,int flag);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };

} // end of XC namespace
#endif
