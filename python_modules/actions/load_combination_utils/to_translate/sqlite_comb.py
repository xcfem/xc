# -*- coding: utf-8 -*-
import sqlite3

def SQLTcreaTablaComb(dBaseName,tableName):
    '''Creates a table to show the decomposition of the combinations.

    :param dBaseName: name of the database.
    :param tableName: name of the table.
    '''
    for acc in pond_acciones:
        nmbAccs= (nmbAccs + " float," + sqlValue(acc.name))
    nmbAccs= "("+nmbAccs.partition(',')[2]+" float)" # remove until first comma.
    SQLTcreaTabla(dBaseName,tableName,nmbAccs)

def SQLTinsertCombs(dBaseName,tableName):
    ''' Insert combinations in the table argument.

    :param dBaseName: name of the database.
    :param tableName: name of the table.
    '''
    conn = sqlite3.connect(dBaseName)    
    for comb in loadCombinations:
        sqlCmd= "insert into "+tableName+ " ("+comb.getDescomp.sqlNames+")"+" values ("+comb.getDescomp.sqlValues+")"
        conn.execute(sqlCmd)

def SQLTtablaCombs(dBaseName,tableName,combinationsName,offset):
    '''Defines a table of combinations of the type being passed as parameter.

      :param dBaseName: name of the database.
      :param tableName: name of the table.
    '''
    cont= offset
    SQLTcreaTabla(dBaseName,tableName,"(idComb INT, descomp STRING)")
    combs= combinations[combinationsName]
    conn = sqlite3.connect(dBaseName)    
    for comb in combs:
        sqlQuery= "insert into " + tableName + " values ("+ sqlValue(int(cont)) + "," + sqlValue(comb.getExpandedName) +")"
        conn.execute(sqlQuery)
        cont+= 1
    return cont


def SQLTtablaCombsULS(dBaseName,tableName,offset):
    '''Defines a table of combinations of the type being passed as parameter.

    :param dBaseName: name of the database.
    :param tableName: name of the table.
    '''
    cont= offset
    SQLTcreaTabla(dBaseName,tableName,"(idComb INT, descomp STRING)")
    conn = sqlite3.connect(dBaseName)
    for comb in combinations.comb_elu_persistentes:
        conn.execute("insert into " + tableName + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(comb.getExpandedName) +")")
        cont= cont+1
    for comb in combinations.comb_elu_accidentales:
        conn.execute("insert into " + tableName + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(comb.getExpandedName) +")")
        cont= cont+1
    for comb in combinations.comb_elu_sismicas:
        conn.execute("insert into " + tableName + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(comb.getExpandedName) +")")
        cont= cont+1
    return cont


def SQLTtablaCombsSLSPF(dBaseName,tableName,offset):
    '''Creates a table to show the combinations for serviceability limit state in rare situations.

    :param dBaseName: name of the database.
    :param tableName: name of the table.
    '''    
    cont= offset
    SQLTcreaTabla(dBaseName,tableName,"(idComb INT, descomp STRING)")
    conn = sqlite3.connect(dBaseName)
    for comb in combinations.comb_els_poco_frecuentes:
        conn.execute("insert into " + tableName + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(comb.getExpandedName) +")")
        cont= cont+1
    return cont

def SQLTtablaCombsSLSF(dBaseName,tableName,offset):
  '''Creates a table to show the combinations for serviceability limit states in frequent situations.

    :param dBaseName: name of the database.
    :param tableName: name of the table.
    '''
    cont= offset
    SQLTcreaTabla(dBaseName,tableName,"(idComb INT, descomp STRING)")
    conn = sqlite3.connect(dBaseName)
    for comb in combinations.comb_els_frecuentes:
        conn.execute("insert into " + tableName + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(comb.getExpandedName) +")")
        cont= cont+1
    return cont

def SQLTtablaCombsSLSCP(dBaseName,tableName,offset):
    '''Creates a table to show the combinations for serviceability limit states in quasi-permanent situations.

    :param dBaseName: name of the database.
    :param tableName: name of the table.
    '''
    cont= offset
    SQLTcreaTabla(dBaseName,tableName,"(idComb INT, descomp STRING)")
    conn = sqlite3.connect(dBaseName)
    for comb in combinations.comb_els_cuasi_permanentes:
        conn.execute("insert into " + tableName + " values ("+ sqlValue(int(cont)) + ", " + sqlValue(comb.getExpandedName) +")")
        cont= cont+1
    return cont
