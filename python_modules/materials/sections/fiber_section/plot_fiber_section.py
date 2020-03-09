# -*- coding: utf-8 -*-

import math
import cairo
import xc_base
import geom
from geom_utils import aux_cairo_plot

import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
from miscUtils import log_messages as lmsg

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

def plotReinforcement(reinforcement, ctx):
    '''draw section rebars in a postcript file.'''
    for reinfLayer in reinforcement:
        barras= reinfLayer.getReinfBars
        for b in barras:
            ptPlot= b.getPos2d
            rPlot= b.diameter/2.0
            labelPlot= str(int(round(b.diameter*1e3)))
            ctx.move_to(ptPlot.x+rPlot,ptPlot.y)
            ctx.arc(ptPlot.x,ptPlot.y,rPlot,0.0,2*math.pi)
            ctx.stroke_preserve()
            ctx.fill()
            ctx.move_to(ptPlot.x+1.3*rPlot,ptPlot.y)
            ctx.text_path(labelPlot)
            ctx.stroke()

def plotSectionGeometry(geomSection, path):
    ''' draws section geometry in a postscript file'''
    WIDTH, HEIGHT = 256, 256
    surface = cairo.PSSurface(path, WIDTH, HEIGHT)
    ctx = cairo.Context(surface)
    regions= geomSection.getRegions
    bnd= regions.getBnd
    trf= aux_cairo_plot.TransformParams(WIDTH, HEIGHT,aux_cairo_plot.Boundary(bnd.getXMin,bnd.getYMin,bnd.getXMax,bnd.getYMax))
    trf.applyTransform(ctx)# Normalizing the canvas
    ctx.set_line_width(trf.scale/100000)
    ctx.set_source_rgb(0.0, 0.0, 0.0) # Black solid color
    for r in regions:
        trf.plotPolygon(r.getPolygon(),ctx)
    ctx.stroke()
    ctx.set_line_width(trf.scale/300000)
    reinforcement= geomSection.getReinfLayers
    plotReinforcement(reinforcement,ctx)
    trf.plotYZAxis(ctx)
    surface.set_eps(True)
    ctx.show_page()
    surface.finish()

class fibSectFeaturesToplot(object):
    '''Class to generate python plots of the selected features associated with 
    a fiber section.

    :ivar fiberSection: fiber-section to plot
    :ivar colorNeutralAxis: color to display the Neutral axis (defaults to None)
    :ivar colorBendingPlane: color to display the beding plane (defaults to None)
    :ivar colorCompressionPlane: color to display the compression plane (defaults to None)
    :ivar colorTensionPlane: color to display the tension plane (defaults to None)
    :ivar colorIntForcAxis: color to display the internal forces axis (defaults to None)
    :ivar colorLeverArm: color to display the lever arm  (defaults to None)
    :ivar colorEffDepth: color to display the effective depth (defaults to None)
    :ivar colorEffConcrArea: color to display the limit of the effective concrete area (defaults to None)
    :ivar MaxEffHeight: maximum effective height to calculate effective concrete area (defaults to None)
    :ivar colorGrossEffConcrAreaContours: color to display the contours of the gross effective concrete area  (defaults to None)
    '''
    def __init__(self,fiberSection,colorNeutralAxis=None,colorBendingPlane=None,colorCompressionPlane=None,colorTensionPlane=None,colorIntForcAxis=None,colorLeverArm=None,colorEffDepth=None,colorEffConcrArea=None,MaxEffHeight=None,colorGrossEffConcrAreaContours=None):
        self.fiberSection=fiberSection
        self.colorNeutralAxis=colorNeutralAxis
        self.colorBendingPlane=colorBendingPlane
        self.colorCompressionPlane=colorCompressionPlane
        self.colorTensionPlane=colorTensionPlane
        self.colorIntForcAxis=colorIntForcAxis
        self.colorLeverArm=colorLeverArm
        self.colorEffDepth=colorEffDepth
        self.colorEffConcrArea=colorEffConcrArea
        self.MaxEffHeight=MaxEffHeight
        self.colorGrossEffConcrAreaContours=colorGrossEffConcrAreaContours
  
    def generatePlot(self):
        self.contour=self.fiberSection.getRegionsContour()
        fig = plt.figure()
        ax2d=plt.subplot(111)
        ax2d.axis('equal')
        #Fiber section contour
        (y,z)=data_xcpolygon_to_pyplot(self.contour)
        ax2d.plot(y,z,'k',label='Cross-section')
        for reinfLayer in self.fiberSection.getReinfLayers:
            bars=reinfLayer.getReinfBars
            for b in bars:
                cent= b.getPos2d
                R= b.diameter/2.0
                ax2d.add_artist(plt.Circle((cent[0],cent[1]),R,color='black'))
        #Neutral axis 
        if self.colorNeutralAxis != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getNeutralAxis(),self.contour)
            ax2d.plot(y,z,self.colorNeutralAxis,label='Neutral axis')
        #Bending plane
        if self.colorBendingPlane != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getBendingPlaneTrace(),self.contour)
            ax2d.plot(y,z,self.colorBendingPlane,label='Bending plane')
        #Compression plane
        if self.colorCompressionPlane != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getCompressedPlaneTrace(),self.contour)
            ax2d.plot(y,z,self.colorCompressionPlane,label='Compression plane')
        #Tension plane
        if self.colorTensionPlane != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getTensionedPlaneTrace(),self.contour)
            ax2d.plot(y,z,self.colorTensionPlane,label='Tension plane')
        #Internal forces axis
        if self.colorIntForcAxis != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getInternalForcesAxes(),self.contour)
            ax2d.plot(y,z,self.colorIntForcAxis ,label='Internal forces axis')
        #Lever arm
        if self.colorLeverArm != None:
            (y,z)= data_xcsegment_to_pyplot(self.fiberSection.getLeverArmSegment())
            ax2d.plot(y,z,self.colorLeverArm,label='Lever arm')
        #Effective depth
        if self.colorEffDepth != None:
            (y,z)= data_xcsegment_to_pyplot(self.fiberSection.getEffectiveDepthSegment())
            ax2d.plot(y,z,self.colorEffDepth,label='Effective depth')
        #Limit of concrete effective area
        if self.colorEffConcrArea != None:
            if self.MaxEffHeight==None:
                lmsg.error("I can't plot the limit of the effective concrete area, a value to MaxEffHeight must be provided \n")
            else:
                (y,z)=data_axis_to_pyplot(self.fiberSection.getEffectiveConcreteAreaLimitLine(self.MaxEffHeight),self.contour)
                ax2d.plot(y,z,self.colorEffConcrArea,label='Limit of effective concrete area')
        if self.colorGrossEffConcrAreaContours != None:
            if self.MaxEffHeight==None:
                lmsg.error("I can't plot the contours of the gross effective concrete area, a value to MaxEffHeight must be provided \n")
            else:
                countLst=self.fiberSection.getGrossEffectiveConcreteAreaContour(self.MaxEffHeight)
                label='Contours of gross effective concrete area'
                for pol in countLst:
                    (y,z)=data_xcpolygon_to_pyplot(pol)
                    ax2d.plot(y,z,self.colorGrossEffConcrAreaContours,label=label)
                    label=None
        ax2d.legend()
        return fig,ax2d
       

def data_xcpolygon_to_pyplot(XCpolygon):
    '''Return the lists [x1,x2,x3,...] and [y1,y2,y3,...] with the coordinates 
    of the vertexes of the XCpolygon passed as parameter.
    '''
    nv=XCpolygon.getNumVertices()
    x=[XCpolygon.getVertice(i).x for i in range(nv)]
    x.append(x[0])
    y=[XCpolygon.getVertice(i).y for i in range(nv)]
    y.append(y[0])
    return(x,y)

def data_xcsegment_to_pyplot(XCsegment):
    '''Return the lists [x1,x2] and [y1,y2] with the coordinates 
    of the start and end points of the XCsegment passed as parameter.
    '''
    stPt=XCsegment.getFromPoint()
    endPt=XCsegment.getToPoint()
    return ([stPt[0],endPt[0]],[stPt[1],endPt[1]])

def data_axis_to_pyplot(axis,contour):
    '''Return the lists [x1,x2] and [y1,y2] with the coordinates 
    of the start and end points of the segment of axis inside the fiberSect contour
    passed as parameters.
    '''
    sg=contour.clipLine(axis)
    return data_xcsegment_to_pyplot(sg)

    
    
