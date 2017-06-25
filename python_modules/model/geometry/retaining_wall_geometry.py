# -*- coding: utf-8 -*-
from __future__ import division
'''Geometry of retaining walls.'''

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
from model import predefined_spaces
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
  '''Geometry of a cantilever retaining wall.

    Attributes:
        :ivar name: (string) Identifier.
        :ivar stemHeight: (float) Height of the stem.
        :ivar stemBottomWidth: (float) Stem width at his contact with the footing.
        :ivar stemTopWidth: (float) Stem width at his top.
        :ivar footingThickness: (float) Thickness of the footing.
        :ivar bToe: (float) Toe length.
        :ivar bHeel: (float) Heel length.
  '''

  def __init__(self,name= 'prb',stemBottomWidth=0.25,stemTopWidth=0.25,footingThickness= 0.25):
    '''Constructor 
    '''
    self.name= name
    self.stemBottomWidth= stemBottomWidth
    self.stemTopWidth= stemTopWidth
    self.footingThickness= footingThickness
    self.stemTopPosition= geom.Pos2d(0.0,0.0) #Default position of stem top.
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
  def getWFStemTopPosition(self):
    ''' Returns the position of the stem top in the wireframe model.'''
    return self.stemTopPosition
  def getWFStemBottomPosition(self):
    ''' Returns the position of the stem bottom in the wireframe model.'''
    return geom.Pos2d(self.stemTopPosition.x,self.stemTopPosition.y-self.stemHeight-self.footingThickness/2.0)
  def getWFToeEndPosition(self):
    ''' Returns the position of the toe end in the wireframe model.'''
    stemBottomPos= self.getWFStemBottomPosition()
    return geom.Pos2d(stemBottomPos.x-self.bToe-self.stemBottomWidth+self.stemTopWidth/2.0,stemBottomPos.y)
  def getWFHeelEndPosition(self):
    ''' Returns the position of the heel end in the wireframe model.'''
    stemBottomPos= self.getWFStemBottomPosition()
    return geom.Pos2d(stemBottomPos.x+self.stemTopWidth/2.0+self.bHeel,stemBottomPos.y)

  def defineWireframeModel(self,nodes):
    self.modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
    preprocessor= self.modelSpace.preprocessor
    points= preprocessor.getCad.getPoints
    self.wireframeModelPoints= dict()
    pos= self.getWFStemTopPosition()
    self.wireframeModelPoints['stemTop']= points.newPntFromPos3d(geom.Pos3d(pos.x,pos.y,0.0))
    pos= self.getWFStemBottomPosition()
    self.wireframeModelPoints['stemBottom']= points.newPntFromPos3d(geom.Pos3d(pos.x,pos.y,0.0))
    pos= self.getWFToeEndPosition()
    self.wireframeModelPoints['toeEnd']= points.newPntFromPos3d(geom.Pos3d(pos.x,pos.y,0.0))
    pos= self.getWFHeelEndPosition()
    self.wireframeModelPoints['heelEnd']= points.newPntFromPos3d(geom.Pos3d(pos.x,pos.y,0.0))
    
    lines= preprocessor.getCad.getLines
    self.wireframeModelLines= dict()
    self.wireframeModelLines['stem']= lines.newLine(self.wireframeModelPoints['stemBottom'].tag,self.wireframeModelPoints['stemTop'].tag)
    self.wireframeModelLines['toe']= lines.newLine(self.wireframeModelPoints['stemBottom'].tag,self.wireframeModelPoints['toeEnd'].tag)
    self.wireframeModelLines['heel']= lines.newLine(self.wireframeModelPoints['stemBottom'].tag,self.wireframeModelPoints['heelEnd'].tag)

  def getToePosition(self):
    ''' Returns the position of the toe (for overturning moment computation).'''
    return self.getWFToeEndPosition()+geom.Vector2d(0.0,-self.footingThickness/2.0)

  def getFoundationCenterPosition(self):
    ''' Returns the position of the foundation center (for excentricity
        computation).'''
    toeEndPos= self.getWFToeEndPosition()
    heelEndPos= self.getWFHeelEndPosition()
    v= (heelEndPos-toeEndPos)*0.5+geom.Vector2d(0.0,-self.footingThickness/2.0)
    return toeEndPos+v

  def getFootingMidPlane(self):
    ''' Returns the midplane of the footing.'''
    toeEndPos= self.getWFToeEndPosition()
    heelEndPos= self.getWFHeelEndPosition()
    return geom.Recta2d(toeEndPos,heelEndPos)

  def getFoundationPlane(self):
    ''' Returns the foundation plane.'''
    v= geom.Vector2d(0.0,-self.footingThickness/2.0)
    toeEndPos= self.getWFToeEndPosition()+v
    heelEndPos= self.getWFHeelEndPosition()+v
    return geom.Recta2d(toeEndPos,heelEndPos)
  
  def getFoundationDepth(self,toeFillDepth):
    '''Return wall foundation depth.
        Args:
            :toeFillDepth: (float) depht of the soil filling overt the toe.
    '''
    return toeFillDepth+self.footingThickness

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
