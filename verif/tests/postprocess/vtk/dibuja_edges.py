# -*- coding: utf-8 -*-

import xc_base
import geom
import xc

prueba= xc.ProblemaEF()
mdlr= prueba.getModelador
# Definimos geometria

puntos= mdlr.getCad.getPoints
pt1= puntos.newPntIDPos3d(1, geom.Pos3d(0.0,0.0,5.0) )
pt2= puntos.newPntIDPos3d(2, geom.Pos3d(0.0,0.0,10.0) )
pt3= puntos.newPntIDPos3d(3, geom.Pos3d(5.0,0.0,10.0) )
pt4= puntos.newPntIDPos3d(4, geom.Pos3d(0.0,5.0,10.0) )

lines= mdlr.getCad.getLines
l1= lines.newLine(1,2)
l2= lines.newLine(1,3)
l3= lines.newLine(1,4)
l4= lines.newLine(2,4)
l5= lines.newLine(2,3)
l6= lines.newLine(4,3)

fName= "/tmp/dibuja_edges.tiff"
import os
pth= os.path.dirname(__file__)
if(not pth):
  pth= "."
fNameB= pth+"/dibuja_edges.verif.tiff"

from xcVtk.malla_cad import vtk_grafico_cad
defDisplay= vtk_grafico_cad.RecordDefDisplayCAD()
defDisplay.plotCadModel(mdlr,"total",None,fName)

ratio1= xc_base.compare_images(fName,fNameB)

# os.system("rm -f " + fName)

#print "ratio1= ",(ratio1)

import os
fname= os.path.basename(__file__)
if  (abs(ratio1)<1e-10) :
  print "test ",fname,": ok."
else:
  print "test ",fname,": ERROR."

