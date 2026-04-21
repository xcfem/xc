# -*- coding: utf-8 -*-
''' Reinforced concrete rectangular column section.'''

__author__= "Luis C. Pérez Tato (LCPT))"
__copyright__= "Copyright 2026, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import sys
import geom
from materials.sections.fiber_section import def_simple_RC_section
from misc_utils import log_messages as lmsg

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
        if(other is not None):
            if(self is not other):
                retval= super(RCRectangularColumnSection, self).__eq__(other)
                if(retval):
                    retval= (self.mainBars == other.mainBars)
            else:
                retval= True
        else:
            retval= False
        return retval
        

    def defSectionGeometry(self, preprocessor, matDiagType, twoDimensional= False):
        '''Returns a reinforced concrete section with reinforcement 
        symmetric in both directions (as usual in columns)

        :param preprocessor: XC preprocessor for the finite element problem.
        :param matDiagType: type of stress-strain diagram (="k" for characteristic diagram, ="d" for design diagram)
        :param twoDimensional: if true set only one division on IJ direction.
        '''
        self.defDiagrams(preprocessor, matDiagType)

        self.geomSection= preprocessor.getMaterialHandler.newSectionGeometry(self.gmSectionName())
        self.defConcreteRegion(twoDimensional= twoDimensional)

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
