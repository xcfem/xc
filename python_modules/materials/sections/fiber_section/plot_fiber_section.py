# -*- coding: utf-8 -*-

import math
import cairo
import xc_base
import geom
from geom_utils import auxCairoPlot

import matplotlib.pyplot as plt
from matplotlib.path import Path
import matplotlib.patches as patches
from miscUtils import LogMessages as lmsg

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"

def plotArmaduras(reinforcement, ctx):
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

def plotGeomSeccion(geomSection, path):
    ''' draws section geometry in a postscript file'''
    WIDTH, HEIGHT = 256, 256
    surface = cairo.PSSurface(path, WIDTH, HEIGHT)
    ctx = cairo.Context(surface)
    regiones= geomSection.getRegions
    bnd= regiones.getBnd
    trf= auxCairoPlot.TransformParams(WIDTH, HEIGHT,auxCairoPlot.Boundary(bnd.getXMin,bnd.getYMin,bnd.getXMax,bnd.getYMax))
    trf.applyTransform(ctx)# Normalizing the canvas
    ctx.set_line_width(trf.scale/100000)
    ctx.set_source_rgb(0.0, 0.0, 0.0) # Black solid color
    for r in regiones:
        trf.plotPolygon(r.getPolygon(),ctx)
    ctx.stroke()
    ctx.set_line_width(trf.scale/300000)
    reinforcement= geomSection.getReinfLayers
    plotArmaduras(reinforcement,ctx)
    trf.plotEjesYZ(ctx)
    surface.set_eps(True)
    ctx.show_page()
    surface.finish()

class fibSectFeaturesToplot(object):
    '''Class to generate python plots of the selected features associated with 
    a fiber section.

    :ivar 
    '''
    def __init__(self,fiberSection):
        self.fiberSection=fiberSection
        self.colorNeutralAxis=None
        self.colorBendingPlane=None
        self.colorCompressionPlane=None
        self.colorTensionPlane=None
        self.colorIntForcAxis=None
        self.colorLeverArm=None
        self.colorEffDepth=None
        self.colorEffConcrArea=None
        self.MaxEffHeight=None
  
    def generatePlot(self):
        self.contour=self.fiberSection.getRegionsContour()
        fig = plt.figure()
        ax2d=plt.subplot(111)
        ax2d.axis('equal')
        #Fiber section contour
        (y,z)=data_xcpolygon_to_pyplot(self.contour)
        ax2d.plot(y,z,'k')
        #Neutral axis 
        if self.colorNeutralAxis != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getNeutralAxis(),self.contour)
            ax2d.plot(y,z,self.colorNeutralAxis,label='Neutral axis')
        #Bending plane
        if self.colorBendingPlane != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getTrazaPlanoFlexion(),self.contour)
            ax2d.plot(y,z,self.colorBendingPlane,label='Bending plane')
        #Compression plane
        if self.colorCompressionPlane != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getTrazaPlanoCompresion(),self.contour)
            ax2d.plot(y,z,self.colorCompressionPlane,label='Compression plane')
        #Tension plane
        if self.colorTensionPlane != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getTrazaPlanoTraccion(),self.contour)
            ax2d.plot(y,z,self.colorTensionPlane,label='Tension plane')
        #Internal forces axis
        if self.colorIntForcAxis != None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getInternalForcesAxis(),self.contour)
            ax2d.plot(y,z,self.colorIntForcAxis ,label='Internal forces axis')
        #Lever arm
        if self.colorLeverArm != None:
            (y,z)= data_xcsegment_to_pyplot(self.fiberSection.getSegmentoBrazoMecanico())
            ax2d.plot(y,z,self.colorLeverArm,label='Lever arm')
        #Effective depth
        if self.colorEffDepth != None:
            (y,z)= data_xcsegment_to_pyplot(self.fiberSection.getSegmentoCantoUtil())
            ax2d.plot(y,z,self.colorEffDepth,label='Effective depth')
        #Limit of concrete effective area
        if self.colorEffConcrArea != None:
            if self.MaxEffHeight==None:
                lmsg.error("I can't plot the limit of the effective concrete area, a value to MaxEffHeight must be provided \n")
            else:
                (y,z)=data_axis_to_pyplot(self.fiberSection.getEffectiveConcreteAreaLimitLine(self.MaxEffHeight),self.contour)
                ax2d.plot(y,z,self.colorEffConcrArea,label='Limit of effective concrete area')
         
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
    stPt=XCsegment.getOrigen()
    endPt=XCsegment.getDestino()
    return ([stPt[0],endPt[0]],[stPt[1],endPt[1]])

def data_axis_to_pyplot(axis,contour):
    '''Return the lists [x1,x2] and [y1,y2] with the coordinates 
    of the start and end points of the segment of axis inside the fiberSect contour
    passed as parameters.
    '''
    sg=contour.recortaRecta(axis)
    return data_xcsegment_to_pyplot(sg)

    
    
