# -*- coding: utf-8 -*-
# Utilities for set management.

def appendPoints(setTo,points):
  #Append points to set.
  pts= setTo.getPoints
  for p in points:
    pts.append(p)

def appendSets(setTo,sets):
  #Append sets to set.
  for s in sets:
    setTo.append(s)

def appendNodesFromLine(setTo,line):
  nn= line.getNumNodes
  nodes= setTo.getNodes
  for i in range(1,nn):
    n= line.getNode(1,1,i)
    nodes.append(n)
  
  


