# -*- coding: utf-8 -*-
# Utilities for line related manipulations.

# Returns line's interior nodes.
def getInteriorNodes(line):
  nNodes= line.getNumNodes
  retval= []
  for i in range(2,nNodes):
    n= line.getNodeI(i)
    retval.append(n)
  return retval

# Returns line's first node.
def getFirstNode(line):
  return line.getNodeI(1)

# Returns line's last node.
def getLastNode(line):
  nNodes= line.getNumNodes
  return line.getNodeI(nNodes-1)

