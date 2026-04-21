# -*- coding: utf-8 -*-
''' RCSectionBase class.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import geom
from misc_utils import log_messages as lmsg
from materials.sections.fiber_section import rc_section_base
from materials.sections import section_properties

class BasicRectangularRCSection(rc_section_base.RCSectionBase, section_properties.RectangularSection):
    '''
    Base class for rectangular reinforced concrete sections.

    :ivar shReinfZ:        object of type ShearReinforcement
                           defining the shear reinforcement in Z direction
    :ivar shReinfY:        object of type ShearReinforcement
                           defining the shear reinforcement in Y direction
    :ivar torsionReinf:    object of type TorsionReinforcement
                           defining the torsion reinforcement.
    :ivar swapReinforcementAxes: if true, swap the axes of reinforcement so the positive and negative reinforcement rows are placed rotated 90 degrees.
    '''
    def __init__(self, name= None, sectionDescr= None, width=0.25,depth=0.25,concrType=None, reinfSteelType=None, nDivIJ= 10, nDivJK= 10, swapReinforcementAxes= False):
        ''' Constructor.

        :param name: name of the section     
        :param sectionDescr: section description.
        :param width: section width.
        :param depth: section depth.
        :param concrType: type of concrete (e.g. EHE_materials.HA25)     
        :param reinfSteelType: type of reinforcement steel.
        :param swapReinforcementAxes: if true, swap the axes of reinforcement so the positive and negative reinforcement rows are placed rotated 90 degrees.
        '''
        RCSectionBase.__init__(self, sectionDescr= sectionDescr, concrType= concrType, reinfSteelType= reinfSteelType, nDivIJ= nDivIJ, nDivJK= nDivJK)
        section_properties.RectangularSection.__init__(self, name= name, b= width, h= depth)

            
        self.swapReinforcementAxes= swapReinforcementAxes
        # Transverse reinforcement (z direction)
        self.shReinfZ= ShearReinforcement(familyName= 'Vz')        

        # Transverse reinforcement (y direction)
        self.shReinfY= ShearReinforcement(familyName= 'Vy')
        # Torsion reinforcement.
        self.torsionReinf= TorsionReinforcement(familyName= 'T')
    
    def getDict(self):
        ''' Put member values in a dictionary.'''
        # retval= super(BasicRectangularRCSection, self).getDict()
        # retval.update(section_properties.RectangularSection.getDict(self))
        retval= dict()
        retval['rc_section_base']= RCSectionBase.getDict(self)
        retval['section_properties']= section_properties.RectangularSection.getDict(self)
        retval['swap_reinforcement_axes']= self.swapReinforcementAxes
        retval['shear_reinforcement_z']= self.shReinfZ
        retval['shear_reinforcement_y']= self.shReinfY
        retval['torsion_reinforcement']= self.torsionReinf
        return retval

    def setFromDict(self,dct):
        ''' Read member values from a dictionary.

        :param dct: Python dictionary containing the member values.
        '''
        # super(BasicRectangularRCSection, self).setFromDict(dct)
        # section_properties.RectangularSection.setFromDict(self, dct)
        RCSectionBase.setFromDict(self, dct['rc_section_base'])
        section_properties.RectangularSection.setFromDict(self, dct['section_properties'])
        self.swapReinforcementAxes= dct['swap_reinforcement_axes']
        self.shReinfZ= dct['shear_reinforcement_z']
        self.shReinfY= dct['shear_reinforcement_y']
        self.torsionReinf= dct['torsion_reinforcement']

    def clear(self):
        ''' Clears all the members of this object.'''
        self.swapReinforcementAxes= None
        self.shReinfZ.clear()
        self.shReinfY= None
        self.shReinfZ.clear()
        self.shReinfZ= None
        self.torsionReinf.clear()
        self.torsionReinf= None
        RCSectionBase.clear(self)
        section_properties.clear(self)
                
    def __eq__(self, other):
        '''Overrides the default implementation'''
        if(other is not None):
            if(self is not other):
                retval= super(BasicRectangularRCSection, self).__eq__(other)
                if(retval):
                    retval= section_properties.RectangularSection.__eq__(self, other)
                if(retval):
                    retval= (self.shReinfZ== other.shReinfZ)
                if(retval):
                    retval= (self.shReinfY== other.shReinfY)
                if(retval):
                    retval= (self.torsionReinf== other.torsionReinf)
            else:
                retval= True
        else:
            retval= False
        return retval

    def getCopy(self):
        ''' Returns a deep enough copy of the object.'''
        retval= BasicRectangularRCSection(name= self.name, sectionDescr= self.sectionDescr, concrType= self.getConcreteType(), reinfSteelType= self.getReinfSteelType(), width= self.b, depth= self.h, nDivIJ= self.getNDivIJ(), nDivJK= self.getNDivJK())
        return retval

    def getShearReinfY(self):
        '''Return the shear reinforcement for Vy.'''
        retval= self.shReinfY
        if(self.swapReinforcementAxes):
            retval= self.shReinfZ
        return retval

    def getShearReinfZ(self):
        '''Return the shear reinforcement for Vz.'''
        retval= self.shReinfZ
        if(self.swapReinforcementAxes):
            retval= self.shReinfY
        return retval

    def getShearReinforcementArea(self):
        '''Return the total shear reinforcement area by member unit length.'''
        retval= self.getShearReinfY().getAs()
        retval+= self.getShearReinfZ().getAs()
        return retval

    def getTorsionReinforcementArea(self):
        '''Return the total torsion reinforcement area by member unit length.'''
        return self.torsionReinf.getAs()

    def getTotalReinforcementArea(self):
        ''' Return the total reinforcement area by member unit length.'''
        retval= self.getMainReinforcementArea()
        retval+= self.getShearReinforcementArea()
        retval+= self.getTorsionReinforcementArea()
        return retval
        
    def getW1z(self):
        ''' Section modulus of the gross section with respect to the most tensioned fiber.'''
        b= self.getWidth()
        h= self.getDepth()
        return b*h**2/6.0
        
    def getW1y(self):
        ''' Section modulus of the gross section with respect to the most tensioned fiber.'''
        b= self.getWidth()
        h= self.getDepth()
        return h*b**2/6.0

    def getW1(self, bendingAxis= 'z'):
        ''' Section modulus of the gross section with respect to the most tensioned fiber.

        :param bendingAxis: bending axis ('z' or 'y').
        '''
        retval= None
        if(bendingAxis=='z'):
            retval= self.getW1z()
        elif(bendingAxis=='y'):
            retval= self.getW1y()
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            errMsg= className+'.'+methodName+"; bending axis must be 'z' or 'y'"
            lmsg.error(className+'.'+methodName+errMsg)
        return retval
    
    def getDepth(self):
        ''' Return the depth of the section that corresponds to the reinforcement axes: (width if the reinforcement axes are swapped).'''
        retval= self.h
        if(self.swapReinforcementAxes):
            retval= self.b
        return retval
        
    def getWidth(self):
        ''' Return the width of the section that corresponds to the reinforcement axes: (depth if the reinforcement axes are swapped).'''
        retval= self.b
        if(self.swapReinforcementAxes):
            retval= self.h
        return retval
    
    def getRespT(self, preprocessor):
        '''Material for modeling torsional response of section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.getRespT(self, preprocessor= preprocessor, G= self.fiberSectionParameters.concrType.Gcm()) # Torsional response of the section.

    def getRespVy(self, preprocessor):
        '''Material for modeling Y shear response of section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.getRespVy(self, preprocessor, self.fiberSectionParameters.concrType.Gcm())

    def getRespVz(self, preprocessor):
        '''Material for modeling Z shear response of section.

        :param preprocessor: preprocessor of the finite element problem.
        '''
        return section_properties.RectangularSection.getRespVz(self, preprocessor, self.fiberSectionParameters.concrType.Gcm())

    def getContour(self):
        ''' Return the vertices of the section contour.'''
        pMin= geom.Pos2d(-self.b/2,-self.h/2)
        pMax= geom.Pos2d(self.b/2,self.h/2)
        vertices= [pMin, geom.Pos2d(pMax.x, pMin.y), pMax, geom.Pos2d(pMin.x, pMax.y), pMin]
        return vertices

    def defConcreteRegion(self, twoDimensional= False):
        ''' Define a rectangular region filled with concrete.

        :param twoDimensional: if true set only one division on IJ direction.
        '''
        regions= self.geomSection.getRegions
        rg= regions.newQuadRegion(self.fiberSectionParameters.concrDiagName)
        if(twoDimensional):
            rg.nDivIJ= 1
        else:
            rg.nDivIJ= self.fiberSectionParameters.nDivIJ
        rg.nDivJK= self.fiberSectionParameters.nDivJK
        rg.pMin= geom.Pos2d(-self.b/2,-self.h/2)
        rg.pMax= geom.Pos2d(self.b/2,self.h/2)

    def clearConcreteRegions(self):
        ''' Clear previously defined concrete regions.'''
        if(self.geomSection):
            regions= self.geomSection.getRegions
            regions.clear()
    
    def defElasticSection1d(self, preprocessor, overrideRho= None, reductionFactor= 1.0):
        ''' Return an elastic section appropriate for truss analysis.

        :param preprocessor: preprocessor of the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticSection1d(preprocessor, material= mat, overrideRho= overrideRho, reductionFactor= reductionFactor)
    
    def defElasticSection3d(self, preprocessor, overrideRho= None, reductionFactor= 1.0):
        ''' Return an elastic section appropriate for 3D beam analysis

        :param preprocessor: preprocessor of the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticSection3d(preprocessor, material= mat, overrideRho= overrideRho, reductionFactor= reductionFactor)
    
    def defElasticShearSection3d(self, preprocessor, overrideRho= None, reductionFactor= 1.0):
        '''elastic section appropriate for 3D beam analysis, including shear 
           deformations

        :param preprocessor: XC preprocessor for the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
         '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticShearSection3d(preprocessor, material= mat, overrideRho= overrideRho, reductionFactor= reductionFactor)
    
    def defElasticSection2d(self, preprocessor, majorAxis= True, overrideRho= None, reductionFactor= 1.0):
        ''' Return an elastic section appropriate for 2D beam analysis

        :param preprocessor: XC preprocessor for the finite element problem.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticSection2d(preprocessor, material= mat, majorAxis= majorAxis, overrideRho= overrideRho, reductionFactor= reductionFactor)
        
    def defElasticShearSection2d(self, preprocessor, majorAxis= True, overrideRho= None, reductionFactor= 1.0):
        '''elastic section appropriate for 2D beam analysis, including 
           shear deformations.

        :param preprocessor: XC preprocessor for the finite element problem.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticShearSection2d(preprocessor, material= mat, majorAxis= majorAxis, overrideRho= overrideRho, reductionFactor= reductionFactor)

    def defElasticMembranePlateSection(self, preprocessor, overrideRho= None, reductionFactor= 1.0):
        '''Constructs an elastic isotropic section material appropriate 
           for plate and shell analysis.

        :param preprocessor: XC preprocessor of the finite element problem.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        :param reductionFactor: factor that divides the concrete elastic
                                modulus to simulate the effect of cracking,
                                normally between 1.0 and 7.0.
        '''
        mat= self.getElasticMaterialData(overrideRho= overrideRho)
        return super(BasicRectangularRCSection, self).defElasticMembranePlateSection(preprocessor= preprocessor, material= mat, overrideRho= overrideRho, reductionFactor= reductionFactor)
    
    def report(self, os= sys.stdout, indentation= ''):
        ''' Get a report of the object contents.'''
        section_properties.RectangularSection.report(self, os)
        super(BasicRectangularRCSection, self).report(os, indentation)
        indentation+= '  '
        os.write(indentation+'Transverse reinforcement (z direction):\n')
        self.getShearReinfZ().report(os, indentation+'  ')
        os.write(indentation+'Transverse reinforcement (y direction):\n')
        self.getShearReinfY().report(os, indentation+'  ')

    def latexReportGeometry(self, os= sys.stdout):
        ''' Write geometry data in LaTeX format.

        :param os: output stream.
        '''
        os.write('\\begin{tabular}{l}\n')
        os.write('width: \\\\\n')
        os.write('$b= '+'{0:.2f}'.format(self.b)+'\\ m$\\\\\n')
        os.write('depth: \\\\\n')
        os.write('$h= '+'{0:.2f}'.format(self.h)+'\\ m$\\\\\n')
        os.write('\\end{tabular} \\\\\n')

    def latexReportShearReinforcement(self, os= sys.stdout):
        ''' Write shear reinforcement report in LaTeX format.

        :param os: output stream.
        '''
        self.getShearReinfY().latexReport(width= self.b, os= os)
        self.getShearReinfZ().latexReport(width= self.h, os= os)       
