# -*- coding: utf-8 -*-

import xc

# Definición de nodos habitual en problemas de
# elasticidad y resistencia de materiales.


def gdls_elasticidad2D(nodos):
  nodos.dimEspace= 2 #two coord. for each node (x,y).
  nodos.numGdls= 2 #two DOF for each node (Ux,Uy).

# Definición de nodos habitual en problemas de
# resistencia de materiales en dos dimensiones: 2 coordenadas por
# nodo y 3 grados de libertad por nodo: (ux,uy,theta)
def gdls_resist_materiales2D(nodos):
  nodos.dimEspace= 2 #two coord. for each node (x,y).
  nodos.numGdls= 3 #three DOF for each node (Ux,Uy,theta).

def gdls_elasticidad3D(nodos):
  nodos.dimEspace= 3 #three coord. for each node (x,y,z).
  nodos.numGdls= 3 #three DOF for each node (Ux,Uy,Uz).

# Definición de nodos habitual en problemas de
# resistencia de materiales en tres dimensiones: 3 coordenadas por
# nodo y 6 grados de libertad por nodo: (ux,uy,uz,thetaX,thetaY,thetaZ)
def gdls_resist_materiales3D(nodos):
  nodos.dimEspace= 3 #two coord. for each node (x,y,z).
  nodos.numGdls= 6 #three DOF for each node (ux,uy,uz,thetaX,thetaY,thetaZ)
