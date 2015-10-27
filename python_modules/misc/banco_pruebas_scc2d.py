# -*- coding: utf-8 -*-
# Define el modelo para probar una sección de fibras en 2d.
import xc_base
import geom
import xc
from model import predefined_spaces
from model import fix_node_6dof

def modeloSecc2d(preprocessor,nmbS):
  nodos= preprocessor.getNodeLoader

  predefined_spaces.gdls_resist_materiales2D(nodos)
  nodos.defaultTag= 1 #El número del próximo nodo será 1.
  nodos.newNodeXY(1,0)
  nodos.newNodeXY(1,0)

  elementos= preprocessor.getElementLoader
  elementos.dimElem= 1
  elementos.defaultMaterial= nmbS
  elementos.defaultTag= 1 #Tag for the next element.
  zls= elementos.newElement("zero_length_section",xc.ID([1,2]));
  return zls


# Define el modelo para probar una sección de fibras.
def nuevoZeroLengthSecc2d(preprocessor,nmbS, tagNodo, tagElem):
  idNod1= tagNodo
  idNod2= (tagNodo+1)

   # Definimos nodos
  nodos.newNodeIDXYZ(idNod1,tagNodo,0,0)
  nodos.newNodeIDXYZ(idNod2,tagNodo,0,0)

  coacciones= preprocessor.getConstraintLoader
  fix_node_6dof.fixNode6DOF(coacciones,idNod1)
  fix_node_6dof.Nodo6DOFGirosLibres(coacciones,idNod2)

  # Definimos elementos
  elementos= preprocessor.getElementLoader
  elementos.defaultMaterial= nmbS
  elementos.defaultTag= tagElem #Tag for the next element.
  zls= elementos.newElement("zero_length_section",xc.ID([idNod1,idNod2]));


