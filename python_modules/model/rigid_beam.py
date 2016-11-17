# -*- coding: utf-8 -*-

import xc_base
import xc
from materials import typical_materials
from miscUtils import LogMessages as lmsg

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016,LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

def creaRigidBeamNodos(preprocessor,tagNodA, tagNodB):
  '''Create a rigid_link between the nodes being passed as parameters.

  :param   tagNodA: tag of the master node.
  :param   tagNodB: tag of the slave node.
  '''
  return preprocessor.getConstraintLoader.newRigidBeam(tagNodA,tagNodB)


def creaFulcrumNodos3d(preprocessor, tagNodA, tagNodB):
  '''
   Crea un rigid_link articulado en tabNodoB
   entre los nodos que se pasan como parámetro.
   Le llamamos «fulcrum» porque puede pivotar
   en torno al node designado por tagNodoB

  :param   tagNodA: tag of the master node.
  :param   tagNodB: tag of the slave node.
  '''
  nodos= preprocessor.getNodeLoader
  coordNodoB= nodos.getNode(tagNodB).getCoo
  fulcrumNode= nodos.newNodeFromVector(coordNodoB)
  constraints= preprocessor.getConstraintLoader
  rb= constraints.newRigidBeam(tagNodA,fulcrumNode.tag)
  ed= constraints.newEqualDOF(fulcrumNode.tag,tagNodB,xc.ID([0,1,2]))

def creaRigidBeamPuntos(preprocessor,idPointA, idPointB):
  '''Create a rigid_link between the nodes at points passed as parameters.

  :param   idPointA: id of the point that "contains" the master node.
  :param   idPointB: id of the point that "contains" the slave node.
  '''
  tagNodeA= preprocessor.getPoint(idPointA).getTagNode
  tagNodeB= preprocessor.getPoint(igPointB).getTagNode
  creaRigidBeamNodos(preprocessor,tagNodeA,tagNodeB)

def putHuge3DBarBetweenNodes(preprocessor,tagNodA, tagNodB, nmbTransf):
  '''
  Creates a very stiff bar between the two nodes being passed as parameters.
  (it's a workaround to the problem with the reactions values in nodes when
  using multipoint constraints. This problem will be solved with the
  implementation of MP_ConstraintBase::addResistingForceToNodalReaction).

  :param   tagNodA: tag of bar's from node.
  :param   tagNodB: tag of bar's to node.
  :param   nmbTransf: name of the coordinate transformation to use for the new bar.
  '''
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
  #print "defMat= ", defMat
  elementos.defaultMaterial= matName
  elem= elementos.newElement("elastic_beam_3d",xc.ID([tagNodA,tagNodB]))
  elementos.defaultMaterial= defMat
  scc= elem.sectionProperties
  #print "A= ", elem.sectionProperties.A
  return elem

def creaBarraGordaNodos(preprocessor,tagNodA, tagNodB, nmbTransf):
  '''DEPRECATED; use putHuge3DBarBetweenNodes.'''
  lmsg.warning('creaBarraGordaNodos DEPRECATED; use putHuge3DBarBetweenNodes.')
  return putHuge3DBarBetweenNodes(preprocessor,tagNodA, tagNodB, nmbTransf)

def creaBarraGordaPuntos(idPointA, idPointB, nmbTransf):
  '''
  Creates a very stiff bar between the nodes on the two points being passed
  as parameters. (it's a workaround to the problem with the reactions values
  in nodes when using multipoint constraints. This problem will be solved with the
  implementation of MP_ConstraintBase::addResistingForceToNodalReaction).

  :param   tagNodA: tag of bar's from node.
  :param   tagNodB: tag of bar's to node.
  :param   nmbTransf: name of the coordinate transformation to use for the new bar.
  '''
  tagNodeA= preprocessor.getPoint(idPointA).getTagNode
  tagNodeB= preprocessor.getPoint(igPointB).getTagNode
  return creaBarraGordaNodos(tagNodeA,tagNodeB)
