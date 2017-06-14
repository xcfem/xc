# -*- coding: utf-8 -*-
from __future__ import division
'''Geometry of retaining walls.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

from postprocess.reports import common_formats as fmt

#     Stem |-------- Earth fill
#          |
#          |    
#          |
#          |
#     Toe  |    Heel
#       -------------- Footing
#
#
class CantileverRetainingWallGeometry(object):
  '''Geometry of a cantilever retaining wall.'''

  def __init__(self,name= 'prb',stemBottomWidth=0.25,stemTopWidth=0.25,footingThickness= 0.25):
    '''Constructor '''
    self.name= name
    self.stemBottomWidth= stemBottomWidth
    self.stemTopWidth= stemTopWidth
    self.footingThickness= footingThickness
  def defaultDimensions(self,totalHeight):
    '''Computes default dimension from the total height.'''
    self.stemTopWidth= max(totalHeight/24.0,0.15)
    self.footingThickness= totalHeight/12.0
    self.stemHeight= totalHeight-self.footingThickness
    self.stemBottomWidth= max(self.footingThickness,self.stemTopWidth+0.02*self.stemHeight)
    bFooting= 1.15*(0.2+0.45*totalHeight)
    self.bToe= totalHeight/8.0
    self.bHeel= bFooting-self.bToe-self.stemBottomWidth

  def getFootingWidth(self):
    '''Return total width of the footing.'''
    return self.bToe+self.stemBottomWidth+self.bHeel
  def getDepth(self,y):
    '''Return stem section depth for height "y").'''
    return (self.stemBottomWidth-self.stemTopWidth)/self.stemHeight*y+self.stemTopWidth
  def writeGeometry(self,outputFile):
    '''Write wall geometry in LaTeX format.'''
    outputFile.write("\\begin{tabular}{l}\n")
    outputFile.write("\\textsc{Géométrie mur}\\\\\n")
    outputFile.write("Épaisseur couronnement: \\\\\n")
    outputFile.write("$b_{couronn}= "+fmt.Longs.format(self.stemTopWidth)+"\\ m$\\\\\n")
    outputFile.write("Hauteur voile: \\\\\n")
    outputFile.write("$h_{voile}= "+fmt.Longs.format(self.stemHeight)+"\\ m$\\\\\n")
    outputFile.write("Épaisseur encastrement: \\\\\n")
    outputFile.write("$b_{encast.}= "+fmt.Longs.format(self.stemBottomWidth)+"\\ m$\\\\\n")
    outputFile.write("Épaisseur semelle: \\\\\n")
    outputFile.write("$b_{semelle.}= "+fmt.Longs.format(self.footingThickness)+"\\ m$\\\\\n")
    outputFile.write("\\end{tabular} \\\\\n")
