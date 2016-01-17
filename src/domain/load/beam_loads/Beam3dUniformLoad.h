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
// $Date: 2003/02/14 23:00:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/beam_loads/Beam3dUniformLoad.h,v $
                                                                        
#ifndef Beam3dUniformLoad_h
#define Beam3dUniformLoad_h

// Written: fmk 

// Purpose: This file contains the class definition for Beam3dUniformLoad.

#include "domain/load/beam_loads/BeamUniformLoad.h"

namespace XC {
class FVector;

//! @ingroup ElemLoads
//
//! @brief Carga uniforme sobre elementos barra de tres dimensiones.
class Beam3dUniformLoad: public BeamUniformLoad
  {
  private:
    double wz;  // Transverse
    double tx;  // Torsor.
    static Vector data;
  protected:
    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);

  public:
    Beam3dUniformLoad(int tag, double wy, double wz, double wx, double tx,const ID &theElementTags);
    Beam3dUniformLoad(int tag, const Vector &Fxyz, double tx,const ID &theElementTags);
    Beam3dUniformLoad(int tag);    
    Beam3dUniformLoad(void);

    int getType(void);
    inline const double &Wx(void) const
      { return Axial; }
    inline const double &Wy(void) const
      { return Trans; }
    inline const double &Wz(void) const
      { return wz; }
    inline const double &Tx(void) const
      { return tx; }
    inline double getTransZComponent(void)
      { return wz; }
    inline void setTransZComponent(const double &d)
      { wz= d; }
    inline void setTorsor(const double &d)
      { tx= d; }
    inline double getTorsor(void)
      { return tx; }

    size_t getDimVectorFuerza(void) const;
    size_t getDimVectorMomento(void) const;
    const Matrix &getLocalForces(void) const;
    const Matrix &getLocalMoments(void) const;

    const Matrix &getAppliedSectionForces(const double &L,const Matrix &xi,const double &loadFactor);
    void addReactionsInBasicSystem(const double &,const double &,FVector &);
    void addFixedEndForcesInBasicSystem(const double &L,const double &loadFactor,FVector &);
    void addElasticDeformations(const double &L,const ConstantesSecc3d &ctes_scc,const double &lpI,const double &lpJ,const double &loadFactor,FVector &v0);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
    void Print(std::ostream &s, int flag =0);

  };
} // end of XC namespace

#endif

