# -*- coding: utf-8 -*-
def SQLTcreaTablaComb(nmbDBase,nmbTabla):
  '''Creates a table to show the decomposition of the combinations.'''
  {
    nmbAccs= ""
    \pond_acciones
      {
        \for_each_accion
          {nmbAccs= (nmbAccs + " float," + sqlValue(getName))}
        nmbAccs= "("+copiaDesde(",",nmbAccs)+" float)" # Le quitamos la primera coma.
      }
    SQLTcreaTabla(nmbDBase,nmbTabla,nmbAccs){}
  }

def SQLTinsertCombs(nmbDBase,nmbTabla):
  {
    sqlCmd= ""  
    \for_each
      {
        sqlCmd= "insert into "+nmbTabla+ " ("+getDescomp.sqlNames+")"+" values ("+getDescomp.sqlValues+")"
        \sqlite{\nmbDBase{\execute_sql{sqlCmd}}}
      }
  }

def SQLTtablaCombs(nmbDBase,nmbTabla,combinationsName,offset):
  '''Defines a table of combinations of the type being passed as parameter.'''
  cont= offset
  SQLTcreaTabla(nmbDBase,nmbTablaComb,"(idComb INT, descomp STRING)"){}
    \combinations
      {
        \combinationsName
          {
            sqlQuery= 
            \for_each
              {
                sqlQuery= "insert into " + nmbTabla + " values ("+ sqlValue(int(cont)) + "," + sqlValue(getExpandedName) +")"
                \sqlite{\nmbDBase{\execute_sql{sqlQuery}}}
                cont= cont+1
              }
          }
      }
    return cont
  }


def SQLTtablaCombsULS(nmbDBase,nmbTabla,offset):
  '''Defines a table of combinations of the type being passed as parameter.'''
  {
    
    cont= offset
    SQLTcreaTabla(nmbDBase,nmbTabla,"(idComb INT, descomp STRING)"){}
    \combinations
      {
        \comb_elu_persistentes
          {
            \for_each
              {
                \sqlite{\nmbDBase{\execute_sql{"insert into " + nmbTabla + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(getExpandedName) +")"}}}
                cont= cont+1
              }
          }
        \comb_elu_accidentales
          {
            \for_each
              {
                \sqlite{\nmbDBase{\execute_sql{"insert into " + nmbTabla + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(getExpandedName) +")"}}}
                cont= cont+1
              }
          }
        \comb_elu_sismicas
          {
            \for_each
              {
                \sqlite{\nmbDBase{\execute_sql{"insert into " + nmbTabla + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(getExpandedName) +")"}}}
                cont= cont+1
              }
          }
      }
    return cont
  }


def SQLTtablaCombsSLSPF(nmbDBase,nmbTabla,offset):
  '''Creates a table to show the combinations for serviceability limit state in rare situations.'''
  {
    
    cont= offset
    SQLTcreaTabla(nmbDBase,nmbTabla,"(idComb INT, descomp STRING)"){}
    \combinations
      {
        \comb_els_poco_frecuentes
          {
            \for_each
              {
                \sqlite{\nmbDBase{\execute_sql{"insert into " + nmbTabla + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(getExpandedName) +")"}}}
                cont= cont+1
              }
          }
      }
    return cont
  }

def SQLTtablaCombsSLSF(nmbDBase,nmbTabla,offset):
  '''Creates a table to show the combinations for serviceability limit states in frequent situations.'''
  {
    
    cont= offset
    SQLTcreaTabla(nmbDBase,nmbTabla,"(idComb INT, descomp STRING)"){}
    \combinations
      {
        \comb_els_frecuentes
          {
            \for_each
              {
                \sqlite{\nmbDBase{\execute_sql{"insert into " + nmbTabla + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(getExpandedName) +")"}}}
                cont= cont+1
              }
          }
      }
    return cont
  }

def SQLTtablaCombsSLSCP(nmbDBase,nmbTabla,offset):
  '''Creates a table to show the combinations for serviceability limit states in quasi-permanent situations.'''
  {
    
    cont= offset
    SQLTcreaTabla(nmbDBase,nmbTabla,"(idComb INT, descomp STRING)"){}
    \combinations
      {
        \comb_els_cuasi_permanentes
          {
            \for_each
              {
                \sqlite{\nmbDBase{\execute_sql{"insert into " + nmbTabla + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(getExpandedName) +")"}}}
                cont= cont+1
              }
          }
      }
    return cont
  }
