# -*- coding: utf-8 -*-
# Crea un rigid_link entre los nodos que se pasan como parámetro.
def creaRigidRodNodos(preprocessor,tagNodA, tagNodB):
  return preprocessor.getConstraintLoader.newRigidRod(tagNodA,tagNodB)
