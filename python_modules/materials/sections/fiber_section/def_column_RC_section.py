# -*- coding: utf-8 -*-
''' Definition of the variables that make up a reinforced concrete section 
with reinforcement symmetric in both directions (as usual in columns)
'''

__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (A_OO)"
__copyright__= "Copyright 2015, LCPT and A_OO"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com" "ana.Ortega.Ort@gmail.com"


import sys
import math
from materials.ehe import EHE_materials
from materials.sections.fiber_section import def_simple_RC_section
from materials.sections import section_properties
from misc_utils import log_messages as lmsg
import geom

class ColumnMainReinforcement(object):
    ''' Main (longitudinal) rebars of a column.

    :ivar cover:          concrete clear cover of main reinforcement
    :ivar nRebarsWidth:   number of rebars in the width direction of the 
                      section (each face)
    :ivar areaRebarWidth: cross sectional area of each rebar in  width direction
    :ivar nRebarsDepth:   number of rebars in the depth direction of the 
                      section (each face)
    :ivar areaRebarDepth: cross sectional area of each rebar in  depth direction
    '''
    def __init__(self):
        self.cover= 0.030 
        self.nRebarsWidth= 5 
        self.areaRebarWidth= EHE_materials.Fi12 

        self.nRebarsDepth= 2 
        self.areaRebarDepth= EHE_materials.Fi10 


class RCRectangularColumnSection(def_simple_RC_section.BasicRectangularRCSection):
    '''Definition of the variables that make up a reinforced concrete section 
    with reinforcement symmetric in both directions (as usual in columns)

    :ivar mainBars: object of type ColumnMainReinforcement defining
                    longitudinal reinforcement.
    '''

    def __init__(self, sectionDescr= None):
      super(RCRectangularColumnSection,self).__init__(sectionDescr= sectionDescr)
      self.mainBars= ColumnMainReinforcement()

    def __eq__(self, other):
        ''' Redefines equal operator.'''
        if(self is not other):
            retval= super(RCRectangularColumnSection, self).__eq__(other)
            if(retval):
                retval= (self.mainBars == other.mainBars)
        else:
            retval= True
        return retval
        

    def defSectionGeometry(self, preprocessor, matDiagType):
        '''Returns a reinforced concrete section with reinforcement 
        symmetric in both directions (as usual in columns)

        :param preprocessor: XC preprocessor for the finite element problem.
        :param matDiagType: type of stress-strain diagram (="k" for characteristic diagram, ="d" for design diagram)
        '''
        self.defDiagrams(preprocessor,matDiagType)

        self.geomSection= preprocessor.getMaterialHandler.newSectionGeometry(self.gmSectionName())
        self.defConcreteRegion(self.geomSection)

        reinforcement= self.geomSection.getReinfLayers

        self.reinforcementInf= reinforcement.newStraightReinfLayer(self.fiberSectionParameters.reinfDiagName)
        self.reinforcementInf.code= "infWidth"
        self.reinforcementInf.numReinfBars= self.mainBars.nRebarsWidth
        self.reinforcementInf.barArea= self.mainBars.areaRebarWidth

        self.reinforcementSup= reinforcement.newStraightReinfLayer(self.fiberSectionParameters.reinfDiagName)
        self.reinforcementSup.code= "supWidth"
        self.reinforcementSup.numReinfBars= self.mainBars.nRebarsWidth
        self.reinforcementSup.barArea= self.mainBars.areaRebarWidth
        cover= self.mainBars.cover
        width= self.b
        depth= self.h
        self.reinforcementSup.p1= geom.Pos2d(-width/2+cover,depth/2+cover) # top layer (side +).
        self.reinforcementSup.p2= geom.Pos2d(width/2-cover,depth/2+cover)

        rebarsSpacingCanto= (depth-2*cover)/(self.mainBars.nRebarsDepth+1)

        self.reinforcementCIzq= reinforcement.newStraightReinfLayer(self.fiberSectionParameters.reinfDiagName)
        self.reinforcementCIzq.code= "leftDepth"
        self.reinforcementCIzq.numReinfBars= self.mainBars.nRebarsDepth
        self.reinforcementCIzq.barArea= self.mainBars.areaRebarDepth
        self.reinforcementCIzq.p1= geom.Pos2d(-width/2+cover,-depth/2+cover+rebarsSpacingCanto) # Left side reinforcement.
        self.reinforcementCIzq.p2= geom.Pos2d(-width/2+cover,depth/2-cover-rebarsSpacingCanto)

        self.reinforcementCDer= reinforcement.newStraightReinfLayer(self.fiberSectionParameters.reinfDiagName)
        self.reinforcementCDer.code= "rightDepth"
        self.reinforcementCDer.numReinfBars= self.mainBars.nRebarsDepth
        self.reinforcementCDer.barArea= self.mainBars.areaRebarDepth
        self.reinforcementCDer.p1= geom.Pos2d(width/2-cover,-depth/2+cover+rebarsSpacingCanto) # Right side reinforcement.
        self.reinforcementCDer.p2= geom.Pos2d(width/2-cover,depth/2-cover-rebarsSpacingCanto)

#  Circular RC section
#  (ascii art from ascii.co.uk)
#                _ _
#             =  o  o =
#           = o        o =
#          = o          o =
#          = o          o =
#           = o        o =
#             = o  o =
#                ~~


        
class RCCircularSection(def_simple_RC_section.RCSectionBase, section_properties.CircularSection):
    '''
    Base class for rectangular reinforced concrete sections.

    :ivar mainReinf: layers of main reinforcement.
    :ivar shReinf:  record of type ShearReinforcement
                    defining the shear reinforcement.
    '''
    def __init__(self, name= 'noName', sectionDescr= None, Rext= 0.25, Rint= 0.0, concrType=None, reinfSteelType=None):
        ''' Constructor.

        :param name: name of the section.
        :param Rext: external radius (defaults to 0.25).
        :param Rint: internal radius (defaults to 0.0).
        :param concrType: type of concrete (e.g. EHE_materials.HA25)     
        :param reinfSteelType: type of reinforcement steel.
        '''
        super(RCCircularSection, self).__init__(sectionDescr= sectionDescr, concrType= concrType,reinfSteelType= reinfSteelType, nDivIJ= 20, nDivJK= 5)
        section_properties.CircularSection.__init__(self,name,Rint= Rint,Rext= Rext)

        # Longitudinal reinforcement.
        self.mainReinf= def_simple_RC_section.LongReinfLayers()  #list of ReinfRow data (positive face)
        
        # Transverse reinforcement.
        self.shReinf= def_simple_RC_section.ShearReinforcement()
        self.shReinf.familyName= "V"
        
    def __eq__(self, other):
        ''' Redefines equal operator.'''
        if(self is not other):
            retval= super(RCCircularSection, self).__eq__(other)
            if(retval):
                retval= section_properties.CircularSection.__eq__(self, other)
            if(retval):
                retval= (self.mainReinf == other.mainReinf)
            if(retval):
                retval= (self.shReinf == other.shReinf)
        else:
            retval= True
        return retval
    
    def isCircular(self):
        ''' Return true if it's a circular section.'''
        return True
    
    def getShearReinfY(self):
        '''Return the shear reinforcement for Vy.'''
        return self.shReinf

    def getShearReinfZ(self):
        '''Return the shear reinforcement for Vz.'''
        return self.shReinf
    
    def getRespT(self,preprocessor):
        '''Material for modeling torsional response of section'''
        return section_properties.CircularSection.getRespT(self,preprocessor,self.getConcreteType().Gcm()) # Torsional response of the section.

    def getRespVy(self,preprocessor):
        '''Material for modeling Y shear response of section'''
        return section_properties.CircularSection.getRespVy(self,preprocessor,self.getConcreteType().Gcm())

    def getRespVz(self,preprocessor):
        '''Material for modeling Z shear response of section'''
        return section_properties.CircularSection.getRespVz(self,preprocessor,self.getConcreteType().Gcm())
        
    def defConcreteRegion(self,geomSection):
        regions= geomSection.getRegions
        rg= regions.newCircularRegion(self.fiberSectionParameters.concrDiagName)
        rg.nDivCirc= self.fiberSectionParameters.nDivCirc()
        rg.nDivRad= self.fiberSectionParameters.nDivRad()
        rg.extRad= self.Rext
        rg.intRad= self.Rint
        rg.initAngle= 0.0
        rg.finalAngle= 2*math.pi

    def getMinCover(self):
        ''' return the minimal cover of the reinforcement.'''
        return self.mainReinf.getMinCover()

    def getContour(self):
        ''' Return the vertices of the section contour.'''
        vertices= list()
        numSegments= 32
        angleIncrement= 2.0*math.pi/numSegments
        theta= 0.0
        for i in range(0,numSegments):
            x= self.Rext * math.cos(theta) # calculate the x component
            y= self.Rext * math.sin(theta) # calculate the y component
            vertices.append(geom.Pos2d(x,y))
            theta+= angleIncrement 
        if(self.Rint!= 0.0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; hollow sections not implemented yet. Internal contour ignored.')
        return vertices
        
    def defSectionGeometry(self, preprocessor, matDiagType):
        '''
        Define the XC section geometry object for this reinforced concrete section 

        :param preprocessor: XC preprocessor for the finite element problem.
        :param matDiagType: type of stress-strain diagram 
                     ("k" for characteristic diagram, "d" for design diagram)
        '''
        self.defDiagrams(preprocessor,matDiagType)
        self.geomSection= preprocessor.getMaterialHandler.newSectionGeometry(self.gmSectionName())
        self.defConcreteRegion(self.geomSection)
        reinforcement= self.geomSection.getReinfLayers
        self.mainReinf.defCircularLayers(reinforcement, "reinf", self.fiberSectionParameters.reinfDiagName, self.Rext)

        self.minCover= self.getMinCover()

    def getTorsionalThickness(self):
        '''Return the section thickness for torsion.'''
        return section_properties.CircularSection.getThickness(self)
        
    def report(self, os= sys.stdout, indentation= ''):
        '''Get a report of the object contents.

        :param os: output stream.
        '''
        super(RCCircularSection, self).report(os= os, indentation= indentation)
        indentation+= '  '
        os.write(indentation+'main reinforcement: \n')
        self.mainReinf.report(os= os, indentation= indentation)
        os.write(indentation+'shear reinforcement: \n')        
        self.shReinf.report(os= os, indentation= indentation)
    
    def latexReportGeometry(self, os= sys.stdout):
        ''' Write geometry data in LaTeX format.

        :param os: output stream.
        '''
        os.write('\\begin{tabular}{l}\n')
        os.write('ext. radius: \\\\\n')
        os.write('$r_{ext}= '+'{0:.2f}'.format(self.Rext)+'\\ m$\\\\\n')
        os.write('int. radius: \\\\\n')
        os.write('$r_{int}= '+'{0:.2f}'.format(self.Rint)+'\\ m$\\\\\n')
        os.write('\\end{tabular} \\\\\n')
        
    def latexReportShearReinforcement(self, os= sys.stdout):
        ''' Write shear reinforcement report in LaTeX format.

        :param os: output stream.
        '''
        self.shReinf.latexReport(width= 2.0*self.Rext, os= os)
        
    def defElasticSection1d(self, preprocessor, overrideRho= None):
        ''' Return an elastic section appropriate for truss analysis.

        :param preprocessor: preprocessor of the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super().defElasticSection1d(preprocessor, material= mat, overrideRho= overrideRho)
    
    def defElasticSection3d(self, preprocessor, overrideRho= None):
        ''' Return an elastic section appropriate for 3D beam analysis

        :param preprocessor: preprocessor of the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super().defElasticSection3d(preprocessor, material= mat, overrideRho= overrideRho)
    
    def defElasticShearSection3d(self, preprocessor, overrideRho= None):
        '''elastic section appropriate for 3D beam analysis, including shear 
           deformations

        :param preprocessor: XC preprocessor for the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
         '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super().defElasticShearSection3d(preprocessor, material= mat, overrideRho= overrideRho)
    
    def defElasticSection2d(self, preprocessor, majorAxis= True, overrideRho= None):
        ''' Return an elastic section appropriate for 2D beam analysis

        :param preprocessor: XC preprocessor for the finite element problem.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super().defElasticSection2d(preprocessor, material= mat, majorAxis= majorAxis, overrideRho= overrideRho)
        
    def defElasticShearSection2d(self, preprocessor, majorAxis= True, overrideRho= None):
        '''elastic section appropriate for 2D beam analysis, including 
           shear deformations.

        :param preprocessor: XC preprocessor for the finite element problem.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super().defElasticShearSection2d(preprocessor, material= mat, majorAxis= True, overrideRho= overrideRho)
