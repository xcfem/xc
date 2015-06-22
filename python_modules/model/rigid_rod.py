# -*- coding: utf-8 -*-
# Crea un rigid_link entre los nodos que se pasan como parámetro.
def creaRigidRodNodos(mdlr,tagNodA, tagNodB):
  return mdlr.getConstraintLoader.newRigidRod(tagNodA,tagNodB)
