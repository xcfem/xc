# -*- coding: utf-8 -*-
import sqlite3 as sqlite

# Create a databas with the name being passed as parameter.
def SQLTcreaDBase(nmbDBase):
  conn = sqlite.connect(nmbDBase)

# Open an existing database with the name being passed as parameter.
def SQLTabreDBase(nmbDBase):
  conn = sqlite.connect(nmbDBase)

# Creates a database with the name and the definition being passed as parameter.
def SQLTcreaTabla(nmbDBase,nmbTabla,defTabla):
 conn = sqlite.connect(nmbDBase)
 c= conn.cursor()
 c.execute("create table "+nmbTabla+" "+defTabla)

def SQLTborraTabla(nmbDBase,nmbTabla):
 conn = sqlite.connect(nmbDBase)
 c= conn.cursor()
 c.execute("drop table if exists '"+nmbTabla+"'")

def SQLTexisteTabla(nmbDBase,tablename):
  retval= False
  conn= sqlite.connect(nmbDBase)
  dbcur= conn.cursor()
  query= "SELECT count(*) FROM sqlite_master WHERE type='table' AND name= '"+tablename+ "';"
  dbcur.execute(query)
  if dbcur.fetchone()[0] == 1:
    retval= True
  dbcur.close()
  return retval
