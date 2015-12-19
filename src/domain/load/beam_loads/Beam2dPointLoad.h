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
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/14 23:00:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/Beam2dPointLoad.h,v $
                                                                        
#ifndef Beam2dPointLoad_h
#define Beam2dPointLoad_h

// Written: fmk 

// Purpose: This file contains the class definition for Beam2dPointLoad.

#include "domain/load/beam_loads/BeamPointLoad.h"

namespace XC {

class FVector;

//! @ingroup ElemLoads
//
//! @brief Carga puntual sobre elementos barra de dos dimensiones.
class Beam2dPointLoad: public BeamPointLoad
  {
  private:
    static Vector data;
  protected:
    bool procesa_comando(CmdStatus &status);
  public:
    Beam2dPointLoad(int tag, double Pt, double x,const ID &theElementTags, double Pa = 0.0);
    Beam2dPointLoad(int tag);    
    Beam2dPointLoad(void);    

    inline const double &pTrans(void) const
      { return Trans; }
    inline const double &pAxial(void) const
      { return Axial; }

    size_t getDimVectorFuerza(void) const;
    size_t getDimVectorMomento(void) const;
    const Matrix &getLocalForces(void) const;
    const Matrix &getLocalMoments(void) const;

    const Matrix &getAppliedSectionForces(const double &L,const XC::Matrix &xi,const double &loadFactor);
    void addReactionsInBasicSystem(const double &,const double &,FVector &);
    void addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &);
    void addElasticDeformations(const double &L,const ConstantesSecc2d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);       
    any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif

