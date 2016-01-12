# -*- coding: utf-8 -*-

import xc_base
import xc
from materials import typical_materials

# Crea un rigid_link entre los nodos que se pasan como parámetro.
def creaRigidBeamNodos(preprocessor,tagNodA, tagNodB):
  return preprocessor.getConstraintLoader.newRigidBeam(tagNodA,tagNodB)


'''
Crea un rigid_link articulado en tabNodoB
   entre los nodos que se pasan como parámetro.
   Le llamamos «fulcrum» porque puede pivotar
   en torno al nodeo designado por tagNodoB
'''
def creaFulcrumNodos3d(preprocessor, tagNodA, tagNodB):
  nodos= preprocessor.getNodeLoader
  coordNodoB= nodos.getNode(tagNodB).getCoo
  fulcrumNode= nodos.newNodeFromVector(coordNodoB)
  constraints= preprocessor.getConstraintLoader
  rb= constraints.newRigidBeam(tagNodA,fulcrumNode.tag)
  ed= constraints.newEqualDOF(fulcrumNode.tag,tagNodB,xc.ID([0,1,2]))

# Crea un rigid_beam entre los nodos de los puntos que se pasan como parámetro.
def creaRigidBeamPuntos(preprocessor,idPuntoA, idPuntoB):
  tagNodeA= preprocessor.getPoint(idPuntoA).getTagNode
  tagNodeB= preprocessor.getPoint(igPuntoB).getTagNode
  creaRigidBeamNodos(preprocessor,tagNodeA,tagNodeB)

'''
Coloca una barra gorda (muy rígida) entre los nodos que se pasan como parámetro.
   (lo uso porque las condiciones de contorno multipunto hacen cosas raras con los valores de las
    reacciones)
'''
def creaBarraGordaNodos(preprocessor,tagNodA, tagNodB, nmbTransf):
  elementos= preprocessor.getElementLoader
  elementos.defaultTransformation= nmbTransf
  # Material definition
  matName= 'bar' + str(tagNodA) + str(tagNodB) + nmbTransf
  A= 10
  E= 1e14
  G= 1e12
  Iz= 10
  Iy= 10
  J= 10
  scc= typical_materials.defElasticSection3d(preprocessor,matName,A,E,G,Iz,Iy,J)
  defMat= elementos.defaultMaterial
  print "defMat= ", defMat
  elementos.defaultMaterial= matName
  elem= elementos.newElement("elastic_beam_3d",xc.ID([tagNodA,tagNodB]))
  elementos.defaultMaterial= defMat
  scc= elem.sectionProperties
  print "A= ", elem.sectionProperties.A
  return elem

'''
Coloca una barra gorda (muy rígida) entre los nodos de los puntos que se pasan como parámetro.
   (lo uso porque las condiciones de contorno multipunto hacen cosas raras con los valores de las
    reacciones)
'''
def creaBarraGordaPuntos(idPuntoA, idPuntoB, nmbTransf):
  tagNodeA= preprocessor.getPoint(idPuntoA).getTagNode
  tagNodeB= preprocessor.getPoint(igPuntoB).getTagNode
  return creaBarraGordaNodos(tagNodeA,tagNodeB)
