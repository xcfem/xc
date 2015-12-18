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
//ElementEdgeMecLoad.h
                                                                        
#ifndef ElementEdgeMecLoad_h
#define ElementEdgeMecLoad_h


#include "domain/load/ElementEdgeLoad.h"

namespace XC {
class Matrix;
class FVector;

//! @ingroup ElemLoads
//
//! @brief Carga de tipo mecánico (fuerzas) sobre bordes (o aristas) de elementos.
class ElementEdgeMecLoad : public ElementEdgeLoad
  {
  protected:
    double Trans; //!< Transverse load.
    double Axial; //!< Axial load.
    bool procesa_comando(CmdStatus &status);
    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
  public:
    ElementEdgeMecLoad(int tg, int cTag,const double &,const double &,const ID &,const ID &);
    ElementEdgeMecLoad(int tg, int cTag);

    virtual void addReactionsInBasicSystem(const ID &,const Vector &,const double &,const double &,FVector &);
    virtual void addFixedEndForcesInBasicSystem(const ID &,const Vector &,const double &,const double &,FVector &);

    virtual size_t getDimVectorFuerza(void) const;
    virtual size_t getDimVectorMomento(void) const;
    virtual const Matrix &getLocalForces(void) const;
    virtual const Matrix &getLocalMoments(void) const;
    virtual const Matrix &getGlobalVectors(const Matrix &) const;
    virtual const Matrix &getGlobalForces(void) const;
    virtual const Matrix &getGlobalMoments(void) const;

    void Print(std::ostream &s, int flag =0) const;
    any_const_ptr GetProp(const std::string &cod) const;
  };
} // end of XC namespace

#endif

