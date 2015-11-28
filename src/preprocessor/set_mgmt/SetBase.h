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
//SetBase.h

#ifndef SETBASE_H
#define SETBASE_H

//!  \ingroup Preprocessor
//! 
//!  \defgroup Set Manejo de conjuntos.

#include "preprocessor/EntMdlrBase.h"
#include "preprocessor/ParamMallado.h"

class Lista;

namespace XC {
class SP_Constraint;
class Face;
class Body;
class UniformGrid;

//!  \ingroup Set
//! 
//!  \brief Base de las clases Set y SetEstruct.
class SetBase: public EntMdlrBase
  {
  protected:
    virtual bool procesa_comando(CmdStatus &status);
  public:
    SetBase(const std::string &nmb="",Preprocessor *preprocessor= NULL);
    inline virtual ~SetBase(void)
      {}
    virtual void Malla(dir_mallado dm);
    virtual void fix(const SP_Constraint &);

    virtual std::set<int> getNodeTags(void) const= 0;
    virtual std::set<int> getElementTags(void) const= 0;
    const ID &getIdNodeTags(void) const;
    const ID &getIdElementTags(void) const;


    virtual bool In(const Node *) const= 0;
    virtual bool In(const Element *) const= 0;
    virtual bool In(const Pnt *) const;
    virtual bool In(const Edge *) const;
    virtual bool In(const Face *) const;
    virtual bool In(const Body *) const;
    virtual bool In(const UniformGrid *) const;
   
    void resetTributarias(void) const;
    void calculaLongsTributarias(bool geomInicial= true) const;
    void calculaAreasTributarias(bool geomInicial= true) const;
    void calculaVolsTributarios(bool geomInicial= true) const;

    virtual size_t NumNodos(void) const= 0;
    virtual size_t NumElementos(void) const= 0;
    virtual any_const_ptr GetProp(const std::string &cod) const;
  };

} //fin namespace XC
#endif
