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
//ExprFunctor.h

#ifndef EXPRFUNCTOR_H
#define EXPRFUNCTOR_H

class Rama;

class ExprFunctor
  {
  public:
    virtual void operator()(Rama *r) const= 0;
    inline virtual ~ExprFunctor(void) {}
  };

class FctorBorra : public ExprFunctor
  {
  public:
    virtual void operator()(Rama *r) const;
  };

class FctorOpera : public ExprFunctor
  {
  public:
    virtual void operator()(Rama *r) const;
  };

class FctorOperaToNum : public ExprFunctor
  {
  public:
    virtual void operator()(Rama *r) const;
  };

class Operando;
class FctorAsigna : public ExprFunctor
  {
    Operando *po;
    const Rama *p;
  public:
    FctorAsigna(Operando *pop,const Rama *pr): po(pop),p(pr) {}
    virtual void operator()(Rama *r) const;
  };
class ConjVariables;
class FctorGetVariables : public ExprFunctor
  {
    ConjVariables &cvar;
  public:
    FctorGetVariables(ConjVariables &cv): cvar(cv) {}
    virtual void operator()(Rama *r) const;
  };


#endif




