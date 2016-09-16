# -*- coding: utf-8 -*-

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (A_OO) "
__copyright__= "Copyright 2016, LCPT, A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com, ana.Ortega.Ort@gmail.com "

import xc



def gdls_elasticidad2D(nodos):
  '''Defines the dimension of the space: nodes by two coordinates (x,y) and two DOF for each node (Ux,Uy)

  :param nodos: nodes loader
  '''
  nodos.dimEspace= 2 #two coord. for each node (x,y).
  nodos.numGdls= 2 #two DOF for each node (Ux,Uy).

def gdls_resist_materiales2D(nodos):
  '''Defines the dimension of the space: nodes by two coordinates (x,y) and three DOF for each node (Ux,Uy,theta)

  :param nodos: nodes loader
  '''
  nodos.dimEspace= 2 #two coord. for each node (x,y).
  nodos.numGdls= 3 #three DOF for each node (Ux,Uy,theta).

def gdls_elasticidad3D(nodos):
  '''Defines the dimension of the space: nodes by three coordinates (x,y,z) and three DOF for each node (Ux,Uy,Uz)

  :param nodos: nodes loader
  '''
  nodos.dimEspace= 3 #three coord. for each node (x,y,z).
  nodos.numGdls= 3 #three DOF for each node (Ux,Uy,Uz).

def gdls_resist_materiales3D(nodos):
  '''Defines the dimension of nodes  three coordinates (x,y,z) and six DOF for each node (Ux,Uy,Uz,thetaX,thetaY,thetaZ)

  :param nodos: nodes loader
  '''
  nodos.dimEspace= 3 #two coord. for each node (x,y,z).
  nodos.numGdls= 6 #three DOF for each node (ux,uy,uz,thetaX,thetaY,thetaZ)
