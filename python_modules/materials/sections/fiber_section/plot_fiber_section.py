# -*- coding: utf-8 -*-

import math
import cairo
from geom_utils import aux_cairo_plot

import matplotlib as mpl
mpl.rc('figure', max_open_warning = 0)
import matplotlib.pyplot as plt
from misc_utils import log_messages as lmsg
from PIL import Image
from pathlib import Path

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2015, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es" "ana.Ortega@ciccp.es"


def eps2png(inputFileName, outputFileName= None):
    ''' Convert the encapsulated Postscript input image into a PNG image
        with a "decent" resolution.

    :param inputFileName: name of the input file.
    :param outputFileName: name of the output file.
    '''
    eps_image = Image.open(inputFileName)
    # Rasterise onto 4x higher resolution grid
    eps_image.load(scale=4)
    if(outputFileName is None):
        pth= Path(inputFileName)
        outputFileName= str(pth.with_suffix(''))+'.png'
    eps_image.save(outputFileName)
    

def mplot_section_reinforcement(ax, reinforcement):
    ''' Plot the geometry of the section using matplotlib.

    :param ax: matplotlib subplot.
    :param reinforcement: reinforcement layers to draw.
    '''
    # Plot reinforcement.
    reinfLayersColors= ['black', 'blue', 'darkblue', 'red', 'darkred', 'darkgreen', 'purple']
    numColors= len(reinfLayersColors)
    for idx, reinfLayer in enumerate(reinforcement):
        rebars= reinfLayer.getReinfBars
        rebarColor= reinfLayersColors[idx % numColors]
        for b in rebars:
            ptPlot= b.getPos2d # bar position.
            rPlot= b.diameter/2.0 # bar radius.
            labelPlot= str(int(round(b.diameter*1e3))) # bar label.
            circle= plt.Circle((ptPlot.x, ptPlot.y), rPlot, color= rebarColor)
            ax.add_patch(circle)
            ax.annotate(labelPlot, (ptPlot.x+rPlot, ptPlot.y+rPlot))
    
def mplot_section_geometry(ax, sectionGeometry):
    ''' Plot the geometry of the section using matplotlib.

    :param ax: matplotlib subplot.
    :param sectionGeometry: geometry of the RC section.
    '''
    ax.axis('equal')
    ax.set_title('Section: '+sectionGeometry.name)
    ax.grid(visible= True, linestyle='dotted')
    # Plot contour.
    contour= sectionGeometry.getRegionsContour()
    x= list(); y= list()
    for p in contour.getVertices():
        x.append(p.x)
        y.append(p.y)
    ax.fill(x,y,'tab:gray')
    # Plot reinforcement.
    mplot_section_reinforcement(ax, sectionGeometry.getReinfLayers)    

def plot_reinforcement(reinforcement, ctx):
    '''draw section rebars in a postcript file.

    :param reinforcement: reinforcement to plot.
    :param ctx: cairo context.
    '''
    for reinfLayer in reinforcement:
        rebars= reinfLayer.getReinfBars
        for b in rebars:
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

def plot_section_geometry(geomSection, path):
    ''' draws section geometry in a postscript file.

    :param geomSection: section geometry to draw.
    :param path: output file path.
    '''
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
    plot_reinforcement(reinforcement,ctx)
    trf.plotYZAxis(ctx)
    surface.set_eps(True)
    ctx.show_page()
    surface.finish()

class FibSectFeaturesToplot(object):
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
        for reinfLayer in self.fiberSection.getSectionGeometry.getReinfLayers:
            bars=reinfLayer.getReinfBars
            for b in bars:
                cent= b.getPos2d
                R= b.diameter/2.0
                ax2d.add_artist(plt.Circle((cent[0],cent[1]),R,color='black'))
        #Neutral axis 
        if self.colorNeutralAxis is not None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getNeutralAxis(),self.contour)
            ax2d.plot(y,z,self.colorNeutralAxis,label='Neutral axis')
        #Bending plane
        if self.colorBendingPlane is not None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getBendingPlaneTrace(),self.contour)
            ax2d.plot(y,z,self.colorBendingPlane,label='Bending plane')
        #Compression plane
        if self.colorCompressionPlane is not None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getCompressedPlaneTrace(),self.contour)
            ax2d.plot(y,z,self.colorCompressionPlane,label='Compression plane')
        #Tension plane
        if self.colorTensionPlane is not None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getTensionedPlaneTrace(),self.contour)
            ax2d.plot(y,z,self.colorTensionPlane,label='Tension plane')
        #Internal forces axis
        if self.colorIntForcAxis is not None:
            (y,z)=data_axis_to_pyplot(self.fiberSection.getInternalForcesAxes(),self.contour)
            ax2d.plot(y,z,self.colorIntForcAxis ,label='Internal forces axis')
        #Lever arm
        if self.colorLeverArm is not None:
            (y,z)= data_xcsegment_to_pyplot(self.fiberSection.getLeverArmSegment())
            ax2d.plot(y,z,self.colorLeverArm,label='Lever arm')
        #Effective depth
        if self.colorEffDepth is not None:
            (y,z)= data_xcsegment_to_pyplot(self.fiberSection.getEffectiveDepthSegment())
            ax2d.plot(y,z,self.colorEffDepth,label='Effective depth')
        #Limit of concrete effective area
        if self.colorEffConcrArea is not None:
            if self.MaxEffHeight is None:
                lmsg.error("I can't plot the limit of the effective concrete area, a value to MaxEffHeight must be provided \n")
            else:
                (y,z)=data_axis_to_pyplot(self.fiberSection.getEffectiveConcreteAreaLimitLine(self.MaxEffHeight),self.contour)
                ax2d.plot(y,z,self.colorEffConcrArea,label='Limit of effective concrete area')
        if self.colorGrossEffConcrAreaContours is not None:
            if self.MaxEffHeight is None:
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
    sg= contour.clip(axis)
    return data_xcsegment_to_pyplot(sg)

    
    
