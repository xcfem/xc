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
//EntMdlrBase.h

#ifndef ENTMDLRBASE_H
#define ENTMDLRBASE_H

#include "xc_utils/src/base/EntConNmb.h"
#include "xc_basic/src/texto/LabelContainer.h"
#include "utility/actor/actor/MovableObject.h"

namespace XC {
class Modelador;
class Pnt;
class Edge;
class Face;

//!  \ingroup Mdlr
//! 
//!  \brief Base de los objetos del modelador.
class EntMdlrBase: public EntConNmb, public MovableObject
  {
  private:
    Modelador *mdlr; //!< Modelador.
  protected:
    LabelContainer etiquetas;

    const Modelador *get_modelador(void) const;
    Modelador *get_modelador(void);
    int sendIdsEtiquetas(const int &,const int &, CommParameters &);
    int recvIdsEtiquetas(const int &,const int &, const CommParameters &);
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    virtual bool procesa_comando(CmdStatus &status);
  public:
    EntMdlrBase(const std::string &nmb="",Modelador *mdlr= NULL);
    EntMdlrBase(const EntMdlrBase &otro);
    EntMdlrBase &operator=(const EntMdlrBase &otro);
    inline const Modelador *GetModelador(void) const
      { return get_modelador(); }
    inline Modelador *GetModelador(void)
      { return get_modelador(); }

    size_t GetTag(void) const;

    virtual any_const_ptr GetProp(const std::string &cod) const;

    Pnt *BuscaPnt(const size_t &);    
    const Pnt *BuscaPnt(const size_t &) const;
    Edge *BuscaEdge(const size_t &);    
    const Edge *BuscaEdge(const size_t &) const;
    Face *BuscaFace(const size_t &);    
    const Face *BuscaFace(const size_t &) const;
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };

} //fin namespace XC
#endif
