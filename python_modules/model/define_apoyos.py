# -*- coding: utf-8 -*-

import xc

def defApoyoX(preprocessor,iNod, iElem, matKX):
  '''
  Define un apoyo para el nodo cuyo tag se pasa como parámetro.
  La rigidez  del apoyo viene dada por el material que se pasa como parámetro.
  '''
  # Definimos elementos
  nodos= preprocessor.getNodeLoader
  nod= nodos.duplicateNode(iNod)
  retvalNodTag= nod.tag

  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= matKX
  elems.defaultTag= iElem #Next element number.
  zl= elems.newElement("zero_length",xc.ID([retvalNodTag,iNod]))
  zl.clearMaterials()
  zl.setMaterial(0,matKX)
  # fin de la definición de elementos
  # Boundary conditions
  coacciones= preprocessor.getConstraintLoader
  numGdls= preprocessor.getNodeLoader.numGdls
  for i in range(0,numGdls):
    spc= coacciones.newSPConstraint(retvalNodTag,i,0.0)
  return retvalNodTag

def defApoyoXY(preprocessor,iNod, iElem, matKX, matKY):
  '''
  Define un apoyo para el nodo cuyo tag se pasa como parámetro.
  Las rigideces del apoyo vienen dadas por los materiales que se pasan como parámetro.
  '''
  # Definimos elementos
  nodos= preprocessor.getNodeLoader
  nod= nodos.duplicateNode(iNod)
  retvalNodTag= nod.tag

  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= matKX
  elems.defaultTag= iElem #Next element number.
  zl= elems.newElement("zero_length",xc.ID([retvalNodTag,iNod]))
  zl.clearMaterials()
  zl.setMaterial(0,matKX)
  zl.setMaterial(1,matKY)
  # fin de la definición de elementos
  # Boundary conditions
  coacciones= preprocessor.getConstraintLoader
  numGdls= preprocessor.getNodeLoader.numGdls
  for i in range(0,numGdls):
    spc= coacciones.newSPConstraint(retvalNodTag,i,0.0)
  return retvalNodTag

def defApoyoXYZ(preprocessor,iNod, iElem, matKXName, matKYName, matKZName):
  '''
  Define un apoyo para el nodo cuyo tag se pasa como parámetro.
  Las rigideces del apoyo vienen dadas por los materiales que se pasan como parámetro.
  '''
  # Definimos elementos
  nodos= preprocessor.getNodeLoader
  nod= nodos.duplicateNode(iNod)
  retvalNodTag= nod.tag

  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= matKXName
  elems.defaultTag= iElem #Next element number.
  zl= elems.newElement("zero_length",xc.ID([retvalNodTag,iNod]))
  zl.clearMaterials()
  zl.setMaterial(0,matKXName)
  zl.setMaterial(1,matKYName)
  zl.setMaterial(2,matKZName)
  # fin de la definición de elementos
  # Boundary conditions
  coacciones= preprocessor.getConstraintLoader
  numGdls= preprocessor.getNodeLoader.numGdls
  for i in range(0,numGdls):
    spc= coacciones.newSPConstraint(retvalNodTag,i,0.0)
  return retvalNodTag

def defApoyoUniaxialProb2D(preprocessor, iNod, iElem, nmbMat, dir):
  '''
  Define un apoyo para el nodo cuyo tag se pasa como parámetro.
   Las rigidez del apoyo viene dada por el material que se pasa como parámetro.
   La dirección en la que actúa el apoyo la pasamos mediante un vector
  '''
  # Node duplication
  nodos= preprocessor.getNodeLoader
  nod= nodos.duplicateNode(iNod)
  retvalNodTag= nod.tag

  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= nmbMat
  elems.defaultTag= iElem #Next element number.
  zl= elems.newElement("zero_length",xc.ID([retvalNodTag,iNod]))
  zl.setupVectors(xc.Vector([dir[0],dir[1],0]),xc.Vector([-dir[1],dir[0],0]))
  zl.clearMaterials()
  zl.setMaterial(0,nmbMat)
  # fin de la definición de elementos
  # Boundary conditions
  coacciones= preprocessor.getConstraintLoader
  spc= coacciones.newSPConstraint(retvalNodTag,0,0.0)
  spc= coacciones.newSPConstraint(retvalNodTag,1,0.0)
  spc= coacciones.newSPConstraint(retvalNodTag,2,0.0)
  return retvalNodTag

# Coloca apoyos en todos los nodos de la lista (numera los elementos con el mismo índice de los nodos).
def defApoyoNodosListaXYZ(preprocessor,tagNodos, tagElementos, matKXName, matKYName, matKZName):
  sz= len(tagNodos)
  nodosNuevos= []
  for i in range(0,sz):
    nodosNuevos.append(defApoyoXYZ(preprocessor,tagNodos[i],tagElementos[i],matKXName,matKYName,matKZName))
  return nodosNuevos

# Coloca apoyos en todos los nodos de la lista de puntos.
def defApoyoPuntos(lstPuntosApoyo, offset, matKX, matKY, matKZ):
  nodosApoyo= [] 
  elementosApoyo= [] 
  szListaPuntos= lstPuntosApoyo.size
  # Formamos la lista de nodos de arranque.
  for ii in range(0,szListaPuntos+1):
    pnt= preprocessor.getCad.getPnt(lstPuntosApoyo[ii])
    tagNodo= pnt.getTagNodo()
    nodosApoyo.append(tagNodo)
    elementosApoyo.append(tagNodo+offset)
  nodosArranque= defApoyoNodosLista(nodosApoyo,elementosApoyo,matKX,matKY,matKZ)
  return nodosArranque

# Define un apoyo para el nodo cuyo tag se pasa como parámetro.
def defApoyoXYRigZ(preprocessor,iNod, iElem, matKX, matKY):
  # Node duplication
  nodos= preprocessor.getNodeLoader
  nod= nodos.duplicateNode(iNod)
  retvalNodTag= nod.tag

  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= matKX
  elems.defaultTag= iElem #Next element number.
  zl= elems.newElement("zero_length",xc.ID([retvalNodTag,iNod]))
  zl.clearMaterials()
  zl.setMaterial(0,matKX)
  zl.setMaterial(1,matKY)
  # fin de la definición de elementos
  # Boundary conditions
  coacciones= preprocessor.getConstraintLoader
  numGdls= preprocessor.getNodeLoader.numGdls
  for i in range(0,numGdls):
    spc= coacciones.newSPConstraint(retvalNodTag,i,0.0)
  eDofs= coacciones.newEqualDOF(retvalNodTag,iNod,xc.ID([2]))
  #eDofs.dofs= xc.ID([2])
  return retvalNodTag


'''
Define un elemento entre los nodos cuyos tags se pasan como parámetros.
   La rigidez del elemento viene definida por los nombres de materiales que
   se pasan como parámetro.
'''
def colocaApoyoEntreNodos(preprocessor,iNodA, iNodB, iElem, matKX, matKY, matKZ, matKTHX, matKTHY, matKTHZ):
  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= matKX
  elems.defaultTag= iElem #Next element number.
  zl= elems.newElement("zero_length",xc.ID([iNodA,iNodB]))
  zl.clearMaterials()
  zl.setMaterial(0,matKX)
  zl.setMaterial(1,matKY)
  zl.setMaterial(2,matKZ)
  zl.setMaterial(3,matKTHX)
  zl.setMaterial(4,matKTHY)
  zl.setMaterial(5,matKTHZ)

'''
Define un elemento entre los nodos cuyos tags se pasan como parámetros.
   La rigidez del elemento viene definida por los nombres de materiales que
   se pasan como parámetro.
'''
def colocaApoyoXYEntreNodos(preprocessor,iNodA, iNodB, iElem, matKX, matKY):
  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= matKX
  elems.defaultTag= iElem #Next element number.
  zl= elems.newElement("zero_length",xc.ID([iNodA,iNodB]))
  zl.clearMaterials()
  zl.setMaterial(0,matKX)
  zl.setMaterial(1,matKY)

'''
Define un elemento entre los nodos cuyos tags se pasan como parámetros.
   La rigidez del elemento viene definida por el nombre del material que
   se pasa como parámetro.
'''
def colocaApoyoXEntreNodos(preprocessor,iNodA, iNodB, iElem, matKX):
  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= matKX
  elems.defaultTag= iElem #Next element number.
  zl= elems.newElement("zero_length",xc.ID([iNodA,iNodB]))
  zl.clearMaterials()
  zl.setMaterial(0,matKX)

# Define un elemento entre los nodos cuyos tags se pasan como parámetros.
def colocaApoyoYEntreNodos(preprocessor,iNodA, iNodB, iElem, matKY):
  # Definimos elementos
  elems= preprocessor.getElementLoader
  elems.dimElem= 3 # Tridimensional space.
  elems.defaultMaterial= matKY
  elems.defaultTag= iElem #Next element number.
  zl= elems.newElement("zero_length",xc.ID([iNodA,iNodB]))
  zl.clearMaterials()
  zl.setMaterial(1,matKY)

# Define un elemento entre los nodos pertenecientes a los puntos cuyos tags se pasan como parámetros.
def colocaApoyoEntrePuntos(preprocessor,iPtA, iPtB, iElem, matKX, matKY, matKZ, matKTHX, matKTHY, matKTHZ):
  tgNodA= preprocessor.getCad.getPnt(iPtA).getTagNode()
  tgNodB= preprocessor.getCad.getPnt(iPtB).getTagNode()
  colocaApoyoNodos(tgNodA,tgNodB,iElem,matKX,matKY,matKZ,matKTHX,matKTHY,matKTHZ)
