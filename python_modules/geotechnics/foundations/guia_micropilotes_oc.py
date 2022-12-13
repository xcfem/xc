# -*- coding: utf-8 -*-
'''Micropile modelling according to the document: "Guía para el proyecto y
   la ejecución de micropilotes en obras de carretera», by «Ministerio de Fomento" (Spain) 2005.'''

import sys
import math
from misc_utils import log_messages as lmsg
from geotechnics.foundations import pile
from scipy import interpolate

def alpha_p(soilType, soilState):
    ''' Return the value of the alpha_p coefficient according to the table
        A-6.5 of the "Guía..." document.

    :param soilType: "peat", "clay", "silt", "sand", "gravel" or "rock"
    :param soilState: "overConsolidated", "normallyConsolidated", 
                               "remolded", "highlyFractured",
                               "moderatelyFractured" (the two last only 
                               for rocks).
    '''
    retval= None
    # Check input values.
    if(not soilType in ['peat', 'clay', 'silt', 'sand', 'gravel', 'rock']):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; unknown soil type: '+str(soilType))
        return retval
    if(not soilState in ['overConsolidated', 'normallyConsolidated', 'remolded', 'highlyFractured', 'moderatelyFractured']):
        methodName= sys._getframe(0).f_code.co_name
        lmsg.warning(methodName+'; unknown soil consolidation state: '+str(soilState))
        return retval
    if(soilType=='peat'):
        if(soilState!='normallyConsolidated'):
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; soil state: '+str(soilState)+' cannot be used with soil type: '+str(soilType))
        return retval
    if(soilType=='rock'):
        if(soilState not in ['highlyFractured', 'moderatelyFractured']):
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; soil state: '+str(soilState)+' cannot be used with soil type: '+str(soilType))
            
            
    # Get value.
    if(soilState=='overConsolidated'):
        if(soilType=='peat'):
            retval= None
        elif(soilType=='clay'):
            retval= 1.0
        elif(soilType=='peat'):
            retval= 2/3.0
        elif(soilType=='sand'):
            retval= 1/2.0
        elif(soilType=='gravel'):
            retval= 1/3.0
    elif(soilState=='normallyConsolidated'):
        if(soilType=='peat'):
            retval= 1.0
        elif(soilType=='clay'):
            retval= 2/3.0
        elif(soilType=='peat'):
            retval= 1/2.0
        elif(soilType=='sand'):
            retval= 1/3.0
        elif(soilType=='gravel'):
            retval= 1/4.0
    elif(soilState=='remolded'): # remolded 
        if(soilType=='peat'):
            retval= None
        elif(soilType=='clay'):
            retval= 1/2.0
        elif(soilType=='peat'):
            retval= 1/2.0
        elif(soilType=='sand'):
            retval= 1/3.0
        elif(soilType=='gravel'):
            retval= 1/4.0
    elif(soilState=='highlyFractured'): # rock
        retval= 1/3.0
    else:
        retval= 2/3.0
    return retval

def pressuremeterModulusEstimation(relativeDensity= None, consistency= None):
    ''' Return an estimation of the pressuremeter modulus according to the table
        A-6.6 of the "Guía..." document.

    :param relativeDensity: if not None, relative density of the granular soil, 
                            expressed as 'veryLoose', 'loose', 'medium',
                            'dense' or 'veryDense'.
    :param consistency: if not None, consistency of the cohesive soil, expressed
                        as 'verySoft', 'soft', 'medium', 'stiff', veryStiff'
                        or 'hard'.
    '''
    retval= None
    if(relativeDensity): # soil is granular
        if(relativeDensity=='veryLoose'):
            retval= 0.0
        elif(relativeDensity=='loose'):
            retval= 3.5/2.0*1e6
        elif(relativeDensity=='medium'):
            retval= (12+3.5)/2.0*1e6
        elif(relativeDensity=='dense'):
            retval= (22.5+12)/2.0*1e6
        elif(relativeDensity=='veryDense'):
            retval= 22.5*1e6
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; unknown relative density: '+str(relativeDensity))
            retval= None
    elif(consistency): # cohesive soil
        if(consistency=='verySoft'):
            retval= 0.0
        elif(consistency=='soft'):
            retval= 2.5/2.0*1e6
        elif(consistency=='medium'):
            retval= (5+2.5)/2.0*1e6
        elif(consistency=='stiff'):
            retval= (12+5)/2.0*1e6
        elif(consistency=='veryStiff'):
            retval= (25+12)/2.0*1e6
        elif(consistency=='hard'):
            retval= 25e6
        else:
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(methodName+'; unknown consistency: '+str(consistency))
            retval= None
    return retval


def soilReactionModulus(soilType, soilState, relativeDensity= None, consistency= None):
    ''' Return the value of the soil reaction modulus according to the clause
        A-6.5.2 of the "Guía..." document.

    :param soilType: "peat", "clay", "silt", "sand", "gravel" or "rock"
    :param soilState: "overConsolidated", "normallyConsolidated", 
                               "remolded", "highlyFractured",
                               "moderatelyFractured" (the two last only 
                               for rocks).
    :param relativeDensity: if not None, relative density of the granular soil, 
                            expressed as 'veryLoose', 'loose', 'medium',
                            'dense' or 'veryDense'.
    :param consistency: if not None, consistency of the cohesive soil, expressed
                        as 'verySoft', 'soft', 'medium', 'stiff', veryStiff'
                        or 'hard'.
    '''
    EM= pressuremeterModulusEstimation(relativeDensity= relativeDensity, consistency= consistency)
    a_p= alpha_p(soilType= soilType, soilState= soilState)
    return EM*6/(4/3.0*pow(2.65,a_p)+a_p)

class SandySoilLayers(pile.SoilLayers):
    ''' Class to compute the value of the soil reaction modulus according 
        to the table A-6.7 of the "Guía..." document.

    :ivar soilProfile: properties of the horizons of sandy soil, defined from 
                       top to bottom as a list: 
                       - [[zBottomSoil,relativeDensity], ...] where,
                       'zBottomSoil' is the global Z coordinate of the 
                        bottom level of the soil,
                       - relativeDensity: relative density of the granular soil,
                         expressed as 'veryLoose', 'loose', 'medium',
                        'dense' or 'veryDense'.
    '''
    def __init__(self, soilProfile, waterTableZ, groundLevel= 0.0):
        ''' Constructor.

        :param soilProfile: properties of the horizons of sandy soil, defined from 
                           top to bottom as a list: 
                           - [[zBottomSoil,relativeDensity], ...] where,
                           'zBottomSoil' is the global Z coordinate of the 
                            bottom level of the soil,
                           - relativeDensity: relative density of the granular soil,
                             expressed as 'veryLoose', 'loose', 'medium',
                            'dense' or 'veryDense'.
        :param waterTableZ: global Z coordinate of the water table.
        :param groundLevel: global Z coordinate of the ground level.
        '''
        super().__init__(waterTableZ= waterTableZ, groundLevel= groundLevel)
        self.soilProfile= soilProfile

    def getRelativeDensity(self, z):
        ''' Return the relative density of the soil at the depth argument.

        :param z: global Z coordinate of the point of interest.
        '''
        retval= None
        if(z<self.groundLevel):
            for layer in self.soilProfile:
                zBottom= layer[0]
                if(zBottom<z):
                    retval= layer[1]
                    break
        return retval

    def submerged(self, z):
        ''' Return true if z<waterTableZ.'''
        return z<self.waterTableZ

    def getReactionModulus(self, z):
        ''' Return the value of the soil reaction modulus according to the table
            A-6.7 of the "Guía..." document.

        :param z: global Z coordinate of the point of interest.
        '''
        submerged= self.submerged(z)
        relativeDensity= self.getRelativeDensity(z)
        if(not submerged): # not submerged
            if(relativeDensity=='veryLoose'):
                retval= 1
            elif(relativeDensity=='loose'):
                retval= 2
            elif(relativeDensity=='medium'):
                retval= 5
            elif(relativeDensity=='dense'):
                retval= 10
            elif(relativeDensity=='veryDense'):
                retval= 20
            else:
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(methodName+'; unknown relative density: '+str(relativeDensity))
                retval= None
        else: # submerged
            if(relativeDensity=='veryLoose'):
                retval= 0.6
            elif(relativeDensity=='loose'):
                retval= 1.2
            elif(relativeDensity=='medium'):
                retval= 3
            elif(relativeDensity=='dense'):
                retval= 6
            elif(relativeDensity=='veryDense'):
                retval= 12
            else:
                methodName= sys._getframe(0).f_code.co_name
                lmsg.warning(methodName+'; unknown relative density: '+str(relativeDensity))
                retval= None
        if(retval):
            retval*= (self.groundLevel-z)*1e6
        return retval
        
# Commonly used tubes
# (ascii art from ascii.co.uk)
# 
#              =-=   
#           =   _   = 
#          =  /   \  =
#          |    o    |
#          =  \ _ /  =
#           =       = 
#              = =    
#      
class Micropile(pile.CircularPile):
    '''Micropile foundation model according to "Guía para el proyecto y la 
      ejecución de micropilotes en obras de carreteras." by «Ministerio 
      de Fomento" 2009. Spain.
    
    :ivar pileDiam: diameter of the pile
    :ivar pipeSection: cross-section of the tubular reinforcement.
    :ivar axialRebar: reinforcement steel bar in the axis of the micropile.
    '''
    def __init__(self, pileSet, pileDiam, soilLayers, mortarMaterial, pipeSection, axialRebar= None, pileType= 'endBearing', soilAggressivity= 'aggresiveBackfill', designLife= 100, Fe= 1.5, Fuc= 0.5):
        ''' Constructor.

        :param pileSet: set of nodes and elements defining a single pile.
        :param pileDiam: diameter of the micropile.
        :param soilLayers: properties of the soil horizons.
        :param mortarMaterial: mortar material.
        :param pipeSection: cross-section of the tubular reinforcement.
        :param axialRebar: reinforcement steel bar in the axis of the micropile.
        :param pileType: "endBearing" for end bearing piles 
                        "friction" for friction piles
        :param soilAggressivity: soil aggresivity descriptor
                                 ('unalteredNatural', 'pollutedNatural',
                                  'aggresiveNatural', 'backfill',
                                  'aggresiveBackfill') according to table 2.4.
        :param designLife: design service life of the micropile (in years).
        :param Fe: influence of the execution technique taken from table 3.5.
        :param Fuc: reduction factor of the cross-section area due to the 
                    type of the union taken from table 3.4.
        '''
        E= 0.0
        if(pipeSection):
            E= pipeSection.steelType.E
        elif(mortarMaterial):
            E= mortarMaterial.concrete.getEcm()
        elif(axialRebar):
            E= axialRebar.steel.Es
        super(Micropile, self).__init__(E= E, pileType= pileType, soilLayers= soilLayers, diameter= pileDiam, pileSet= pileSet)
        self.mortarMaterial= mortarMaterial
        self.pipeSection= pipeSection
        self.axialRebar= axialRebar
        self.soilAggressivity= soilAggressivity
        self.designLife= designLife
        self.Fe= Fe
        self.Fuc= Fuc

    def getCorrosionThicknessReduction(self):
        ''' Return the thickness reduction of the steel pipe according
            to table 2.4 of the Guia.

        :param designLife: design service life of the micropile (in years).
        '''
        xi= [5, 25, 50, 75, 100] # years.
        if(self.soilAggressivity=='unalteredNatural'):
            yi= [0.00, 0.30, 0.60, 0.90, 1.20]
        elif(self.soilAggressivity=='pollutedNatural'):
            yi= [0.15, 0.75, 1.50, 2.25, 3.00]
        elif(self.soilAggressivity=='aggresiveNatural'):
            yi= [0.20, 1.00, 1.75, 2.50, 3.25]
        elif(self.soilAggressivity=='backfill'):
            yi= [0.18, 0.70, 1.20, 1.70, 2.20]
        elif(self.soilAggressivity=='aggresiveBackfill'):
            yi= [0.50, 2.00, 3.25, 4.50, 5.75]
        else:
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.warning(className+'.'+methodName+'; unknown soil aggresivity descriptor: '+str(self.soilAggressivity))
        f= interpolate.interp1d(xi, yi, kind='linear', fill_value="extrapolate")
        return float(f(self.designLife))*1e-3 # mm->m
 
    def getSteelPipeDesignArea(self):
        ''' Return the design value of the steel pipe cross-section area
            according to clause 3.6.1 of the Guía.
        '''
        retval= 0.0
        if(self.pipeSection):
            re= self.getCorrosionThicknessReduction()
            de= self.pipeSection.getOutsideDiameter()
            di= self.pipeSection.getInsideDiameter()
            retval= max(math.pi/4.0*((de-2.0*re)**2-di**2)*self.Fuc, 0.0)
        return retval

    def getNcRd(self, C_R= 10):
        ''' Return the compressive structural strength of the micropile
            according to the clause 3.6.1 of the Guía.

        :param C_R: adimensional coefficient from the table 3.6.
        '''
        retval= 0.0
        As= 0.0 # Reinforcing steel area.
        if(self.axialRebar):
            As= self.axialRebar.getArea()
            fsd= min(self.axialRebar.steel.fyd(), 400e6)
            retval+= As*fsd
        Aa= 0.0 # Steel pipe area.
        if(self.pipeSection):
            Aa= self.getSteelPipeDesignArea()
            fyd= min(self.pipeSection.steelType.fyd(), 400e6)
            retval+= Aa*fyd
        Ac= self.getCrossSectionArea()-Aa-As
        fcd= abs(self.mortarMaterial.fcd())
        retval+= 0.85*Ac*fcd 
        R= min(1.07-0.027*C_R, 1.0) # Empiric buckling factor.
        retval*= R/1.2/self.Fe
        return retval

    def getVcRd(self):
        ''' Return the shear strength of the micropile according to clause
            A-4.2 of the "Guía".'''
        retval= 0.0
        if(self.pipeSection):
            Aa= self.getSteelPipeDesignArea() # Steel pipe remaining area.
            fyd= self.pipeSection.steelType.fyd()
            retval= 2*Aa/math.pi/math.sqrt(3)*fyd
        return retval

    def getWpl(self):
        ''' Return the plastic modulus of the remaining steel section
            (reduced due to the corrosion).
        '''
        retval= 0.0
        if(self.pipeSection):
            re= self.getCorrosionThicknessReduction() # Corroded steel.
            de= self.pipeSection.getOutsideDiameter() # External diameter.
            di= self.pipeSection.getInsideDiameter() # Internal diameter.
            dr= de-2*re # Reduced external diameter.
            retval= (dr**3-di**3)/6.0
        return retval
    
    def getWel(self):
        ''' Return the elastic modulus of the remaining steel section
            (reduced due to the corrosion).
        '''
        retval= 0.0
        if(self.pipeSection):
            re= self.getCorrosionThicknessReduction() # Corroded steel.
            de= self.pipeSection.getOutsideDiameter() # External diameter
            di= self.pipeSection.getInsideDiameter() # Internal diameter.
            dr= de-2*re # Reduced external diameter.
            retval= math.pi*(dr**4-di**4)/(32*dr)
        return retval

    def getMcRd(self, Fuf= 0.5):
        ''' Return the bending strength of the micropile according to clause
            A-4.2 of the "Guía".

        :param Fuf: bending modulus reduction factor depending of the type
                    of the connection. Defaults to 0.5 
        '''
        retval= 0.0
        if(self.pipeSection):
            re= self.getCorrosionThicknessReduction() # Corroded steel.
            de= self.pipeSection.getOutsideDiameter() # External diameter
            t= self.t() # Pipe wall thickness.
            dr= de-2*re # Reduced external diameter.
            tr= t-re # Reduced thickness.
            ratio= dr/tr
            fy= self.pipeSection.steelType.fy() # elastic limit stress.
            if(ratio<=16450e6/fy):
                Wpl= self.getWpl()
                retval= Wpl
            elif(ratio<=21150e6/fy):
                Wel= self.getWpl()
                retval= Wel
            else:
                retval= 0.0

        fyd= self.pipeSection.steelType.fyd() # design value of the elastic limit stress.
        retval*= fyd*Fuf
        return retval

    def getBendingEfficiency(self, Nd, Md, Vd= 0.0, Fuf= 0.5):
        '''Return bending efficiency

        :param Nd: required axial strength.
        :param Mzd: required bending strength.
        :param Vyd: required shear strength.
        :param Fuf: bending modulus reduction factor depending of the type
                    of the connection. Defaults to 0.5 
        '''
        if(Nd!=0.0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+': for compressed sections not implemented yet.')
        McRd= self.getMcRd(Fuf= Fuf)
        VcRd= self.getVcRd()
        if(abs(Vd)<=0.5*VcRd):
            retval= Md/McRd
        else:
            ro= (2*Vd/VcRd-1)**2
            retval= (1-ro)*McRd
        return retval
        
    def defElasticSection3d(self,preprocessor, overrideRho= None):
        ''' Return an elastic section appropriate for 3D beam analysis

        :param  preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return self.pipeSection.defElasticSection3d(preprocessor, overrideRho= overrideRho)
    
    def defElasticShearSection3d(self,preprocessor, overrideRho= None):
        '''elastic section appropriate for 3D beam analysis, including shear 
           deformations

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return self.pipeSection.defElasticShearSection3d(preprocessor, overrideRho= overrideRho)

    def defElasticSection1d(self,preprocessor, overrideRho= None):
        ''' Return an elastic section appropriate for truss analysis.

        :param preprocessor: preprocessor object.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return self.pipeSection.defElasticSection1d(preprocessor, overrideRho= overrideRho)
    
    def defElasticSection2d(self,preprocessor, majorAxis= True, overrideRho= None):
        ''' Return an elastic section appropriate for 2D beam analysis

        :param preprocessor: preprocessor object.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return self.pipeSection.defElasticSection2d(preprocessor, majorAxis= majorAxis, overrideRho= overrideRho)
    
    def defElasticShearSection2d(self,preprocessor, majorAxis= True, overrideRho= None):
        '''elastic section appropriate for 2D beam analysis, including shear deformations

        :param  preprocessor: preprocessor object.
        :param majorAxis: true if bending occurs in the section major axis.
        :param overrideRho: if defined (not None), override the value of 
                            the material density.
        '''
        return self.pipeSection.defElasticShearSection2d(preprocessor, majorAxis= majorAxis, overrideRho= overrideRho)

    def getEstimatedTipReactionModulus(self, NcEk_Rcd= 1.0):
        ''' Return an estimation of the tip reaction modulus according to the expression
            in the clause 3.7.3 of the "Guía".

        :param NcEk_Rcd: quotient between the solliciting axial load and the design value
                         of the micropile strength.
        '''
        sN= (9*NcEk_Rcd-2)*self.getDiameter()/90
        NcRd= self.getNcRd()
        k= NcRd/sN
        return k        

    def getLinearSpringsConstants(self, alphaKh_x= 1.0, alphaKh_y= 1.0, alphaKv_z= 1.0):
        '''Compute the spring contants that simulate the soils along the pile

        :param alphaKh_x: coefficient to be applied to the horizontal stiffness
                          of a single pile in X direction
        :param alphaKh_y: coefficient to be applied to the horizontal stiffness
                          of a single pile in Y direction
        :param alphaKh_Z: coefficient to be applied to the vertical stiffness of
                          a single pile in Z direction
        '''
        # Soil reaction modulus at each node.
        lstNodPile= self.getNodeZs()
        self.computeTributaryLengths(False)
        retval= dict()
        for (n,z) in lstNodPile:
            if(z<self.soilLayers.groundLevel):
                 soilReactionModulus= self.soilLayers.getReactionModulus(z= z)
                 assert(soilReactionModulus>=0)
                 springStiffness= soilReactionModulus*n.getTributaryLength()*self.getDiameter()
                 retval[n.tag]= [alphaKh_x*springStiffness, alphaKh_x*springStiffness, 1e-5]
        # Compute vertical stiffness on the tip.
        tipNode= lstNodPile[-1][0]
        tipNodeStiffness= retval[tipNode.tag]
        tipStiffness= self.getEstimatedTipReactionModulus()
        tipNodeStiffness[2]= alphaKv_z*tipStiffness
        return retval
