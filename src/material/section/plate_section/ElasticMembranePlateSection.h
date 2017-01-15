//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
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
                                                                        
// $Revision: 1.8 $
// $Date: 2003/02/14 23:01:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/section/ElasticMembranePlateSection.h,v $

// Ed "C++" Love
//
//  Elastic Plate Section with membrane
//



#ifndef ElasticMembranePlateSection_h
#define ElasticMembranePlateSection_h

#include <material/section/plate_section/ElasticPlateProto.h>


namespace XC {
//! \ingroup MATPLAC
//
//! @brief Sección elástica para materiales capaces de
//! trabajar como placa y como membrana.
class ElasticMembranePlateSection : public ElasticPlateProto<8>
  {
  private:
    double rhoH ; //!< mass per unit 2D area
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

  public : 
    ElasticMembranePlateSection(void);
    ElasticMembranePlateSection(int tag);
    ElasticMembranePlateSection(int tag, double E, double nu,double h = 1.0, double rho = 0.0 );

    SectionForceDeformation *getCopy(void) const;


    const ResponseId &getType(void) const;

    const Vector &getStressResultant(void) const;
    const Matrix& getSectionTangent(void) const;
    const Matrix& getInitialTangent(void) const;

    void Print(std::ostream &s,int flag);

    double getRho(void) const;
    void setRho(const double &);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);


  };

} // end of XC namespace

#endif
