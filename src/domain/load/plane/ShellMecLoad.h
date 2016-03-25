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
//ShellMecLoad.h
                                                                        
#ifndef ShellMecLoad_h
#define ShellMecLoad_h


#include "BidimMecLoad.h"

namespace XC {
class FVectorShell;

//! @ingroup ElemLoads
//
//! @brief Carga de tipo mecánico (fuerzas) sobre elementos de tipo viga.
class ShellMecLoad: public BidimMecLoad
  {
  protected:

  public:
    ShellMecLoad(int tag, int classTag,const double &Trans,const double &,const double &,const ID &theElementTags);
    ShellMecLoad(int tag, int classTag);

    virtual void addReactionsInBasicSystem(const double &,const double &,FVectorShell &);
    virtual void addFixedEndForcesInBasicSystem(const double &,const double &,FVectorShell &);

    virtual size_t getDimVectorFuerza(void) const;
    virtual size_t getDimVectorMomento(void) const;
    virtual Vector getLocalForce(void) const;
    virtual Vector getLocalMoment(void) const;
    virtual const Matrix &getLocalForces(void) const;
    virtual const Matrix &getLocalMoments(void) const;
    virtual const Matrix &getGlobalVectors(const Matrix &) const;
    virtual const Matrix &getGlobalForces(void) const;
    virtual const Matrix &getGlobalMoments(void) const;
  };
} // end of XC namespace

#endif

