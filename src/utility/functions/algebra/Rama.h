// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//Rama.h

#ifndef RAMA_H
#define RAMA_H

#include "ProtoExpr.h"
#include "Segnal.h"
#include "Operando.h"

class ExprFunctor;
class ConjVariables;

class Rama : public ProtoExpresion
  {
  private:
    static const double &no_numero;

    inline void BorraIzdo(void)
      {
        if(izdo)
          {
            izdo->BorraHijos();
            delete izdo;
            izdo= NULL;
          }
      }
    inline void BorraDcho(void)
      {
        if(dcho)
          {
            dcho->BorraHijos();
            delete dcho;
            dcho= NULL;
          }
      }
    inline void BorraHijos(void)
      {
        BorraIzdo();
        BorraDcho();
      }
    friend class ArbolExpr;
    friend class OpUnario;
    friend class OpBinario;
    Rama(const Operando *s,Rama *i,Rama *j= NULL);

    friend Rama *ContraeIzdo(Rama *);
    friend Rama *ContraeDcho(Rama *);
  protected:

    const Operando *data;

    Rama *izdo;
    Rama *dcho;
    
  public:
    friend class OpProd;
    friend class OpSuma;

    //! @brief Constructor por defecto.
    Rama(void)
      : data(NULL), izdo(NULL), dcho(NULL) {}

    explicit Rama(const double &d);
    Rama(const Rama &);
    Rama &operator=(const Rama &);
    bool operator==(const Rama &r2) const;
    bool operator==(const double &d) const;

    Rama *getCopy(void) const
      {
        if(izdo)
          {
            if(dcho) //Izquierdo y derecho.
              return new Rama(data,izdo->getCopy(),dcho->getCopy());
            else //Sólo izquierdo.
              return new Rama(data,izdo->getCopy(),NULL);
          }
        else
          {
            if(dcho) //Sólo derecho.
              return new Rama(data,NULL,dcho->getCopy());
            else //Ni derecho ni izquierdo.
              return new Rama(data,NULL,NULL);
          }
      }
    virtual ~Rama(void);
    inline void PutIzdo(Rama *i)
      {
        BorraIzdo();
        if(i) izdo= i->getCopy();
      }
    inline void PutDcho(Rama *j)
      {
        BorraDcho();
        if(j) dcho= j->getCopy();
      }
/*     inline void Put(const Operando *s,Rama *i,Rama *j) */
/*       { */
/*         data= s; */
/*         izdo= i; */
/*         PutDcho(j); */
/*       } */
    inline const Operando *GetData(void) const
      { return data; }
    inline Rama *getLeft(void) const
      { return izdo; }
    inline Rama *getRight(void) const
      { return dcho; }
    Clase GetClase(void) const;
    const std::string &StrClase(void) const;
    inline const Operando *DatoIzdo(void)
      { return izdo->data; }
    inline const Operando *DatoDcho(void)
      { return dcho->data; }
    inline int GetPrioridad(void) const;
    bool Evaluable(void) const;
    bool EsToNum(void) const;
    const Operando &Opera(void);
    static void Opera(Rama *raiz);
    inline const double &GetValor(void) const
      { return ((GetClase() == operando) ? data->GetValor() : no_numero); }
    void Asigna(Operando *po,const Rama *p)
    //Sustituye todas las ocurrencias de po por p en el arbol.
      {
        if(dcho)
          if(dcho->data == po)
            {
              BorraDcho();
              if(p) dcho= p->getCopy();
            }
        if(izdo)
          if(izdo->data == po)
            {
              BorraIzdo();
              if(p) izdo= p->getCopy();
            }
      }
    void Borra(void)
      {
        BorraHijos();
        data= NULL;
      }
    void GetVariables(ConjVariables &cv) const;
    const std::string &GetFullString(void) const;
    friend Rama *distrib(Rama *raiz);
    friend std::ostream &operator <<(std::ostream &stream,const Rama &e);
  };

Rama *ContraeIzdo(Rama *raiz);
Rama *ContraeDcho(Rama *raiz);

Rama diferencia(const Rama &raiz,const Variable &v);
Rama *simplifica(Rama *raiz);
Rama *distrib(Rama *raiz);
void Escribe(const Rama *raiz,const int &prior, std::ostream &stream);
void RecorrePreorden(Rama *raiz,const ExprFunctor &f);
void RecorreEnorden(Rama *raiz,const ExprFunctor &f);
void RecorrePostorden(Rama *raiz,const ExprFunctor &f);

inline bool operator==(const double &d,const Rama &r)
  { return (r==d); }

#endif



