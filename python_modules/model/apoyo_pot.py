# -*- coding: utf-8 -*-

import xc_base
import xc
from model import define_apoyos

def colocaApoyoFicticioPotDeslizanteNodos(preprocessor,iNodA, iNodB, iElem, nmbMatTeflonKX, nmbMatTeflonKY):
  '''
  Define un elemento elástico que modeliza aproximadamente
     un apoyo pot deslizante entre los nodos cuyos tags se pasan como parámetros.
     d: Diámetro del apoyo.
  '''
  define_apoyos.colocaApoyoXYEntreNodos(preprocessor,iNodA,iNodB,iElem,nmbMatTeflonKX,nmbMatTeflonKY)
  eDofs= preprocessor.getConstraintLoader.newEqualDOF(iNodA,iNodB,xc.ID([2]))


def getReacApoyoFicticioPotDeslizanteNodos(preprocessor,iElem):
  '''Devuelve las reacciones en un elemento que representa a un apoyo pot deslizante '''
  nodos= preprocessor.getNodeLoader
  nodos.calculateNodalReactions(True)
  
  elem= preprocessor.getElementLoader.getElement(iElem)
  reac0= elem.getNodes[0].getReaction
  reac1= elem.getNodes[1].getReaction
  RX= reac0[0]
  RY= reac0[1]
  RZ= reac1[2]
  vReac= xc.Vector([RX,RY,RZ])
  return vReac


'''
Define un elemento elástico que modeliza aproximadamente
   un apoyo pot deslizante según el eje X entre los nodos cuyos tags se pasan como parámetros.
   d: Diámetro del apoyo.
'''
def colocaApoyoFicticioPotDeslizanteXNodos(preprocessor, iNodA, iNodB, iElem, nmbMatTeflonKX):
  define_apoyos.colocaApoyoXEntreNodos(preprocessor,iNodA,iNodB,iElem,nmbMatTeflonKX)
  eDofs= preprocessor.getConstraintLoader.newEqualDOF(iNodA,iNodB,xc.ID([1,2]))


def getReacApoyoFicticioPotDeslizanteXNodos(preprocessor,iElem):
  #Devuelve las reacciones en un elemento que representa a un apoyo pot deslizante 
  nodos= preprocessor.getNodeLoader
  nodos.calculateNodalReactions(True)
  
  elem= preprocessor.getElementLoader.getElement(iElem)
  reac0= elem.getNodes[0].getReaction
  reac1= elem.getNodes[1].getReaction
  RX= reac0[0]
  RY= reac1[1]
  RZ= reac1[2]
  vReac= xc.Vector([RX,RY,RZ])
  return vReac

'''
Define un elemento elástico que modeliza aproximadamente
   un apoyo pot deslizante según el eje Y entre los nodos cuyos tags se pasan como parámetros.
   d: Diámetro del apoyo.
'''
def colocaApoyoFicticioPotDeslizanteYNodos(preprocessor, iNodA, iNodB, iElem, nmbMatTeflonKY):
  define_apoyos.colocaApoyoYEntreNodos(preprocessor, iNodA,iNodB,iElem,nmbMatTeflonKY)
  eDofs= preprocessor.getConstraintLoader.newEqualDOF(iNodA,iNodB,xc.ID([0,2]))

def getReacApoyoFicticioPotDeslizanteYNodos(preprocessor,iElem):
  # Devuelve las reacciones en un elemento que representa a un apoyo pot deslizante
  nodos= preprocessor.getNodeLoader
  nodos.calculateNodalReactions(True)
  
  elem= preprocessor.getElementLoader.getElement(iElem)
  reac0= elem.getNodes[0].getReaction
  reac1= elem.getNodes[1].getReaction
  RX= reac1[0]
  RY= reac0[1]
  RZ= reac1[2]
  vReac= xc.Vector([RX,RY,RZ])
  return vReac
