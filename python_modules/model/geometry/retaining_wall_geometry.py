# -*- coding: utf-8 -*-
'''Geometry of retaining walls.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2017, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import xc_base
import geom
from model import predefined_spaces
from postprocess.reports import common_formats as fmt
import matplotlib.pyplot as plt

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

          :ivar name: (string) Identifier.
          :ivar stemHeight: (float) Height of the stem.
          :ivar stemBottomWidth: (float) Stem width at his contact with the footing.
          :ivar stemTopWidth: (float) Stem width at his top.
          :ivar stemBackSlope: (float) Stem back slope expressed as H/V ratio. 
          :ivar footingThickness: (float) Thickness of the footing.
          :ivar bToe: (float) Toe length.
          :ivar bHeel: (float) Heel length.
    '''

    def __init__(self,name= 'prb',stemBottomWidth=0.25,stemTopWidth=0.25,footingThickness= 0.25, stemBackSlope= 0.0):
        '''Constructor 

        :param name: (string) Identifier.
        :param stemBottomWidth: (float) Stem width at his contact with the footing.
        :param stemTopWidth: (float) Stem width at his top.
        :param footingThickness: (float) Thickness of the footing.
        :param stemBackSlope: (float) Stem back slope expressed as H/V ratio. 
        '''
        self.name= name
        self.stemBottomWidth= stemBottomWidth
        self.stemTopWidth= stemTopWidth
        self.stemBackSlope= stemBackSlope
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

    def getTotalHeight(self):
        '''Return total height of the wall.'''
        return self.stemHeight+self.footingThickness

    def getFootingWidth(self):
        '''Return total width of the footing.'''
        return self.bToe+self.stemBottomWidth+self.bHeel
    
    def getDepth(self,y):
        '''Return stem section depth for height "y").'''
        return (self.stemBottomWidth-self.stemTopWidth)/self.stemHeight*y+self.stemTopWidth
      
    def getWFStemHeigth(self):
        ''' Return the height of the stem in the wireframe model.'''
        return self.stemHeight+self.footingThickness/2.0
     
    def getWFStemTopPosition(self):
        ''' Returns the position of the stem top in the wireframe model.'''
        return self.stemTopPosition-geom.Vector2d(self.stemTopWidth/2.0,0.0)
    
    def getWFStemBottomPosition(self):
        ''' Returns the position of the stem bottom in the wireframe model.'''
        h= self.getWFStemHeigth()
        return geom.Pos2d(self.stemTopPosition.x+self.stemBackSlope*h-self.stemBottomWidth/2.0,self.stemTopPosition.y-h)

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
        points= preprocessor.getMultiBlockTopology.getPoints
        self.wireframeModelPoints= dict()
        pos= self.getWFStemTopPosition()
        self.wireframeModelPoints['stemTop']= points.newPoint(geom.Pos3d(pos.x,pos.y,0.0))
        pos= self.getWFStemBottomPosition()
        self.wireframeModelPoints['stemBottom']= points.newPoint(geom.Pos3d(pos.x,pos.y,0.0))
        pos= self.getWFToeEndPosition()
        self.wireframeModelPoints['toeEnd']= points.newPoint(geom.Pos3d(pos.x,pos.y,0.0))
        pos= self.getWFHeelEndPosition()
        self.wireframeModelPoints['heelEnd']= points.newPoint(geom.Pos3d(pos.x,pos.y,0.0))

        lines= preprocessor.getMultiBlockTopology.getLines
        self.wireframeModelLines= dict()
        self.wireframeModelLines['stem']= lines.newLine(self.wireframeModelPoints['stemBottom'].tag,self.wireframeModelPoints['stemTop'].tag)
        self.wireframeModelLines['toe']= lines.newLine(self.wireframeModelPoints['stemBottom'].tag,self.wireframeModelPoints['toeEnd'].tag)
        self.wireframeModelLines['heel']= lines.newLine(self.wireframeModelPoints['heelEnd'].tag,self.wireframeModelPoints['stemBottom'].tag)

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
        return geom.Line2d(toeEndPos,heelEndPos)

    def getFoundationPlane(self):
        ''' Returns the foundation plane.'''
        v= geom.Vector2d(0.0,-self.footingThickness/2.0)
        toeEndPos= self.getWFToeEndPosition()+v
        heelEndPos= self.getWFHeelEndPosition()+v
        return geom.Line2d(toeEndPos,heelEndPos)

    def getFoundationDepth(self,toeFillDepth):
        '''Return wall foundation depth.

           :param toeFillDepth: (float) depht of the soil filling overt the toe.
        '''
        return toeFillDepth+self.footingThickness

    def getContourPoints(self):
        ''' Return a list with the points that form the wall contour.'''
        retval= list()
        retval.append(self.stemTopPosition)
        stemInsideBottom= self.stemTopPosition-geom.Vector2d(0.0,self.stemHeight)
        retval.append(stemInsideBottom)
        toePosTop= stemInsideBottom+geom.Vector2d(-self.bToe, 0.0) # toe
        retval.append(toePosTop)
        
        toePosBottom= toePosTop-geom.Vector2d(0.0,self.footingThickness)
        retval.append(toePosBottom)
        heelEndPosBottom= toePosBottom+geom.Vector2d(self.getFootingWidth(), 0.0) # heel end
        retval.append(heelEndPosBottom)
        heelEndPosTop= heelEndPosBottom+geom.Vector2d(0.0, self.footingThickness)
        retval.append(heelEndPosTop)        
        stemOutsideTop= self.stemTopPosition+geom.Vector2d(self.stemTopWidth,0.0) # stem top
        stemOutsideBottom= stemOutsideTop+geom.Vector2d(self.stemHeight*self.stemBackSlope,-self.stemHeight) #stem bottom
        retval.append(stemOutsideBottom)
        retval.append(stemOutsideTop) # stem top
        return retval
    
    def getXYVertices(self):
        ''' Return the contour X,Y coordinates in two separate
            lists to be used with pyplot.''' 
        x= list()
        y= list()
        vertices= self.getContourPoints()
        for p in vertices:
            x.append(p.x)
            y.append(p.y)
        x.append(vertices[0].x)
        y.append(vertices[0].y)
        return x,y

    def draw(self, notes= None):
        ''' Draw the wall contour using pyplot.

        :param notes: notes to insert in the plot.
        '''
        fig = plt.figure()
        #plt.axis('equal')
        #plt.grid(axis= 'both')
        ax = fig.add_subplot(111, aspect= 'equal') # subplot axes
        x,y= self.getXYVertices()
        ax.fill(x,y,'tab:gray')
        ax.plot(x,y,'k')
        if(notes):
            # build a rectangle in axes coords
            left, width = .25, .5
            bottom, height = .25, .5
            right = left + width
            top = bottom + height
            for text in notes:
                ax.text(left+width/2, top, text,
                horizontalalignment='left',
                verticalalignment='top',
                transform= ax.transAxes)
                top-= 0.05
        plt.show()

    def writeGeometry(self,outputFile):
        '''Write wall geometry in LaTeX format.'''
        outputFile.write("\\begin{tabular}{l}\n")
        outputFile.write("\\textsc{Wall geometry}\\\\\n")
        outputFile.write("Stem top thickness: \\\\\n")
        outputFile.write("$b_{top}= "+fmt.Lengths.format(self.stemTopWidth)+"\\ m$\\\\\n")
        outputFile.write("Stem height: \\\\\n")
        outputFile.write("$h_{stem}= "+fmt.Lengths.format(self.stemHeight)+"\\ m$\\\\\n")
        outputFile.write("Stem bottom thickness: \\\\\n")
        outputFile.write("$b_{bottom}= "+fmt.Lengths.format(self.stemBottomWidth)+"\\ m$\\\\\n")
        outputFile.write("Footing thickness: \\\\\n")
        outputFile.write("$h_{footing}= "+fmt.Lengths.format(self.footingThickness)+"\\ m$\\\\\n")
        outputFile.write("Footing width: \\\\\n")
        outputFile.write("$b_{toe}= "+fmt.Lengths.format(self.bToe)+"\\ m$\\\\\n")
        outputFile.write("$b_{heel}= "+fmt.Lengths.format(self.bHeel)+"\\ m$\\\\\n")
        outputFile.write("$b_{footing}= "+fmt.Lengths.format(self.getFootingWidth())+"\\ m$\\\\\n")
        outputFile.write("\\end{tabular} \\\\\n")
