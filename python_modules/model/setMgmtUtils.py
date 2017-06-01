# -*- coding: utf-8 -*-
# Utilities for set management.

def appendPoints(setTo,points):
  '''Appends points to set of entities

  :param setTo: set of entities
  :param points: list of points [pt1,pt2, ...]
  '''
  pts= setTo.getPoints
  for p in points:
    pts.append(p)

def appendSets(setTo,sets):
  '''Appends other sets to set of entities

  :param setTo: base set of entities
  :param points: list of sets to be appended [set1,set2, ...]
  '''
  for s in sets:
    setTo.extend(s)

def appendNodesFromLine(setTo,line):
  '''Appends the nodes in the specified line to a set of entities

  :param setTo: set of entities
  :param line: line that contains the nodes 
  '''
  nn= line.getNumNodes
  nodes= setTo.getNodes
  for i in range(1,nn):
    n= line.getNode(1,1,i)
    nodes.append(n)
  
  


