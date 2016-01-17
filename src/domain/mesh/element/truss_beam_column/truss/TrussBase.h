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
//TrussBase.h
                                                  
#ifndef TrussBase_h
#define TrussBase_h

#include "ProtoTruss.h"

namespace XC {
class Channel;
class UniaxialMaterial;

//! \ingroup ElemBarraArt
//
//! @brief Clase base para la barra articulada.
class TrussBase : public ProtoTruss
  {
  protected:
    double L;	    //!< length of truss based on undeformed configuration.
    double cosX[3]; //!< Cosenos directores.

    int sendData(CommParameters &cp);
    int recvData(const CommParameters &cp);
    void inicializa(void);
    void setup_L_cos_dir(void);
    void set_load(const Vector &);
    void alloc_load(const size_t &);

  public:
    TrussBase(int classTag,int tag,int dimension, int Nd1, int Nd2);
    TrussBase(int classTag,int tag,int dimension);
    TrussBase(int classTag);
    TrussBase(const TrussBase &);
    TrussBase &operator=(const TrussBase &);

    const Vector *getLoad(void) const
      { return &load; }
    Vector *getLoad(void)
      { return &load; }
    const double &getL(void) const;


  };
} // end of XC namespace

#endif




