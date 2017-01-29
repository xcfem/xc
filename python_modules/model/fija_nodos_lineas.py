
__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= " ana.Ortega.Ort@gmail.com, l.pereztato@gmail.com"

def ConstraintsForLineInteriorNodes(setLinea, constraints, constraint):
  '''Aplying constraint on the interior nodes of a model line.'''
  tags= setLinea.getNodeLayers.getLayer(0).getTagsInteriorNodes()
  for i in tags:
    constraint(constraints,i)

def ConstraintsForLineExtremeNodes(setLinea, constraints, constraint):
  '''Aplying constraint on the extreme nodes of a model line.'''
  fN= setLinea.firstNode.tag
  lN= setLinea.lastNode.tag
  constraint(constraints,fN)
  constraint(constraints,lN)

