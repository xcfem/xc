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
//NodeLoader.h

#ifndef NODELOADER_H
#define NODELOADER_H

#include "Loader.h"

namespace XC {

class Node;

//!  \ingroup Lodrs
//! 
//! @brief Objeto encargado de agregar al dominio los nodos
//! leídos desde archivo.
class NodeLoader: public Loader
  {
  private:
    size_t ngdl_def_nodo; //!< Número de grados del libertad de un nodo por defecto.
    size_t ncoo_def_nodo; //!< Número de coordenadas de los nodos (1,2 ó 3).
    Node *nodo_semilla; //!< Nodo patrón para el mallado semi-automático.
    void libera(void);
    Node *new_node(const int &tag,const size_t &dim,const int &ngdl,const double &x,const double &y=0.0,const double &z=0.0);
  protected:
    Node *lee_nodo(CmdStatus &);
    bool procesa_comando(CmdStatus &status);
  public:
    NodeLoader(Preprocessor *);
    virtual ~NodeLoader(void);
    const Node *get_nodo_semilla(void) const
      { return nodo_semilla; }
    Node *nuevoNodo(const double &x,const double &y,const double &z);
    Node *nuevoNodo(const double &x,const double &y);
    Node *nuevoNodo(const double &x);
    Node *nuevoNodo(const Pos3d &p);
    Node *nuevoNodo(const Pos2d &p);
    Node *nuevoNodo(const Vector &);
    Node *newSeedNode(void);
    Node *newNodeIDXYZ(const int &,const double &,const double &,const double &);
    Node *newNodeIDXY(const int &,const double &,const double &);
    Node *newNodeIDV(const int &,const Vector &);
    Node *duplicateNode(const int &);

    size_t getDimEspacio(void) const
      { return ncoo_def_nodo; }
    void setDimEspacio(const size_t &dim)
      { ncoo_def_nodo= dim; }
    void setNumGdls(const size_t &ngdl)
      { ngdl_def_nodo= ngdl; }
    size_t getNumGdls(void) const
      { return ngdl_def_nodo; }
    Node *getNode(const int &tag);
    int getDefaultTag(void) const;
    void setDefaultTag(const int &tag);
    void clearAll(void);

    void calculateNodalReactions(bool inclInertia);

    any_const_ptr GetProp(const std::string &cod) const;
  };

} // fin namespace XC

#endif
