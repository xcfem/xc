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
//BandArpackppSOE.cpp

#include <solution/system_of_eqn/eigenSOE/BandArpackppSOE.h>
#include <solution/system_of_eqn/eigenSOE/BandArpackppSolver.h>
#include "solution/graph/graph/Graph.h"
#include "utility/matrix/Matrix.h"
#include "utility/matrix/ID.h"
//#include "boost/any.hpp"


//! @brief Constructor.
XC::BandArpackppSOE::BandArpackppSOE(SoluMethod *owr,const double &shift)
  :ArpackSOE(owr,EigenSOE_TAGS_BandArpackppSOE,shift), A(), M() {}

//! @brief Asigna el solver que se empleará en la solución del problema de autovalores.
bool XC::BandArpackppSOE::setSolver(EigenSolver *newSolver)
  {
    bool retval= false;
    BandArpackppSolver *tmp= dynamic_cast<BandArpackppSolver *>(newSolver);
    if(tmp)
      {
        tmp->setEigenSOE(*this);
        retval= ArpackSOE::setSolver(tmp);
      }
    else
      std::cerr << "BandArpackppSOE::setSolver; solver incompatible con sistema de ecuaciones." << std::endl;
    return retval;
  }

//! @brief Asigna el tamaño del problema.
int XC::BandArpackppSOE::setSize(Graph &theGraph)
  {
    int result = 0;
    int newSize = theGraph.getNumVertex();
    if(newSize==0)
      std::cerr << "¡OJO! error en " << nombre_clase() << "::setSize; el modelo no tiene ningún grado de libertad,"
                << " agrege algún nodo o cambie el gestor de coacciones." << std::endl;
    if(size!=newSize)
      {
        size= newSize;
        A= msp_double(size,size);
        M= msp_double(size,size);
        resize_mass_matrix_if_needed(size);      
      }
    return result;
  }

//! @brief Ensambla en A la matriz being passed as parameter multiplicada por el parámetro fact.
int XC::BandArpackppSOE::addToMatrix(msp_double &MT,const Matrix &m, const ID &id,const double &fact)
  {
    // check for a XC::quick return 
    if(fact == 0.0) return 0;

    // check that m and id are of similar size
    const int idSize = id.Size();
    if(idSize != m.noRows() && idSize != m.noCols())
      {
        std::cerr << "BandArpackppSOE::addToMatrix() -- Matrix and XC::ID not of similar sizes,\n";
        return -1;
      }
    if(fact == 1.0)
      { // nos ahorramos la multiplicación.
        for(int i= 0;i<idSize;i++)
          {
            const int row= id(i)+1;
            if(row <= size && row > 0)
              {
                for(int j= 0;j<idSize;j++)
                  {
	            const int col= id(j)+1;
	            if(col <= size && col > 0 && (m(i,j)!= 0.0))
                      MT(row,col)+= m(i,j);
	          }// for j
              }
          }// for i
      }
    else if(fact == -1.0)
      { // nos ahorramos la multiplicación.
        for(int i= 0;i<idSize;i++)
          {
            const int row= id(i)+1;
            if(row <= size && row > 0)
              {
                for(int j= 0;j<idSize;j++)
                  {
	            const int col= id(j)+1;
	            if(col <= size && col > 0 && (m(i,j)!= 0.0))
                      MT(row,col)-= m(i,j);
	          }// for j
              }
          }// for i
      }
    else
      {
        for(int i= 0;i<idSize;i++)
          {
            const int row = id(i)+1;
            if(row <= size && row > 0)
              {
                for(int j= 0;j<idSize;j++)
                  {
                    const int col = id(j)+1;
	            if(col <= size && col > 0 && (m(i,j)!= 0.0))
                      MT(row,col)+= m(i,j)*fact;
	          }  // for j
              }
          }  // for i
      }
    MT.Compacta();
    return 0;
  }

//! @brief Ensambla en A la matriz being passed as parameter multiplicada por el parámetro fact.
int XC::BandArpackppSOE::addA(const Matrix &a, const ID &id, double fact)
  { return addToMatrix(A,a,id,fact); }


//! @brief Anula la matriz A.
void XC::BandArpackppSOE::zeroA(void)
  { A.Anula(); }

//! @brief Ensambla en M la matriz being passed as parameter multiplicada por el parámetro fact.
int XC::BandArpackppSOE::addM(const Matrix &m, const ID &id, double fact)
  { 
    int retval= 0;
    //Añadido LCPT.
    if(fact!=0.0)
      {
        const int idSize = id.Size();
        // check that m and id are of same size
        if(idSize != m.noRows() && idSize != m.noCols())
          {
            std::cerr << "BandArpackppSOE::addM(); Matrix and ID not of similar sizes\n";
            retval= -1;
          }
        else
          {
            resize_mass_matrix_if_needed(size);      
            int col= 0, row= 0;
            if(fact==1.0)
              {
                for(int i= 0;i<idSize;i++)
                  {
                    row= id(i);
                    if(row < size && row >= 0)
                      {
                        for(int j= 0;j<idSize;j++)
                          {
                            col= id(j);
                            if(col < size && col >= 0 && (m(i,j)!= 0.0))
                              massMatrix(row,col)+= m(i,j);
                           }// for j
                      }
                  }// for i
              }
            else
              {
                for(int i= 0;i<idSize;i++)
                  {
                    row= id(i);
                    if(row < size && row >= 0)
                      {
                        for(int j= 0;j<idSize;j++)
                          {
                            col= id(j);
                            if(col < size && col >= 0 && (m(i,j)!= 0.0))
                              massMatrix(row,col)+= m(i,j)*fact;
                           }// for j
                      }
                  }// for i
               }
           }
      }
    //Fin añadido LCPT.
    retval= addToMatrix(M,m,id,fact);
    return retval;
  }

//! @brief Anula la matriz M.
void XC::BandArpackppSOE::zeroM(void)
  { 
    EigenSOE::zeroM();
    M.Anula();
  }

int XC::BandArpackppSOE::sendSelf(CommParameters &cp)
  { return 0; }
    
int XC::BandArpackppSOE::recvSelf(const CommParameters &cp)
  { return 0; }

