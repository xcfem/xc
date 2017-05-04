# -*- coding: utf-8 -*-
# Define el modelo para probar una sección de fibras en 2d.
import xc_base
import geom
import xc
from model import predefined_spaces
from model import fix_node_6dof

def modeloSecc2d(preprocessor,nmbS):
  nodes= preprocessor.getNodeLoader

  modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
  nodes.defaultTag= 1 #El número del próximo nodo será 1.
  nodes.newNodeXY(1,0)
  nodes.newNodeXY(1,0)

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

   # Define nodes
  nodes.newNodeIDXYZ(idNod1,tagNodo,0,0)
  nodes.newNodeIDXYZ(idNod2,tagNodo,0,0)

  coacciones= preprocessor.getConstraintLoader
  fix_node_6dof.fixNode6DOF(coacciones,idNod1)
  fix_node_6dof.Nodo6DOFGirosLibres(coacciones,idNod2)

  # Definimos elementos
  elementos= preprocessor.getElementLoader
  elementos.defaultMaterial= nmbS
  elementos.defaultTag= tagElem #Tag for the next element.
  zls= elementos.newElement("zero_length_section",xc.ID([idNod1,idNod2]));


