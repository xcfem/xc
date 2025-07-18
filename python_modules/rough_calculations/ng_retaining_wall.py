# -*- coding: utf-8 -*-
'''Routines for cantilever retaining walls design.'''

from __future__ import division
from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2016, LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

import os
import sys
import math
from scipy.constants import g
from postprocess.reports import common_formats as fmt
from postprocess.reports import draw_wall_reinforcement_scheme as draw_schema
from postprocess import get_reactions
import scipy.interpolate
from materials import typical_materials
from materials.sections import section_properties
from materials.sections.fiber_section import def_simple_RC_section
from model.geometry import retaining_wall_geometry
from model.sets import sets_mng 
from rough_calculations import ng_rc_section
from misc_utils import log_messages as lmsg
from misc_utils import path_utils
import geom
import xc
from solution import predefined_solutions
from postprocess import RC_material_distribution

#
#       --  zTopWall
#       | \
#       |  \
#       |  \
#       |   \  stemSlope 
#       |   \  
#       |    \  
#       |    \  
#  +----     ----------+  zTopFooting
#  |Toe          Heel  |
#  +-------------------+
#

def filterRepeatedValues(yList,mList,vList):
    ''' Filter values that are repeated 
        in the list.

    :param yList: list of y coordinates.
    :param mList: list of corresponding bending moments.
    :param vList: list of corresponding shear forces.
    '''
    sz= len(yList)

    mapM={}
    mapV={}
    for i in range(0,sz):
      y= abs(yList[i])
      mapM[y]= mList[i]
      mapV[y]= vList[i]

    retY= list()
    retM= list()
    retV= list()
    for y in mapM:
      retY.append(y)
    retY.sort()
    for y in retY:
      retM.append(mapM[y])
      retV.append(mapV[y])
    return retY, retM, retV

class InternalForces(object):
    '''Internal forces for a retaining wall.

    :ivar ndEnvelope: stem axial force design values.
    :ivar mdEnvelope: stem bending moment design values.
    :ivar vdEnvelope: stem shear force design values.
    :ivar NdFooting: footing axial load design value.
    :ivar MdFooting: footing bending moment design value.
    :ivar VdFooting: footing shear force design value.
    '''
    def __init__(self,y, ndEnvelope, mdEnvelope, vdEnvelope, NdFooting, MdFooting, VdFooting):
        ''' Constructor.
        
        :param ndEnvelope: stem axial force design values.
        :param mdEnvelope: stem bending moment design values.
        :param vdEnvelope: stem shear force design values.
        :param NdFooting: footing axial load design value.
        :param MdFooting: footing bending moment design value.
        :param VdFooting: footing shear force design value.
        '''
        self.ndEnvelope= ndEnvelope
        self.mdEnvelope= mdEnvelope
        self.vdEnvelope= vdEnvelope
        self.ndEnvelope.filterRepeatedValues()
        self.mdEnvelope.filterRepeatedValues()
        self.vdEnvelope.filterRepeatedValues()
        self.interpolate()
        self.stemHeight= self.y[-1]
        self.NdFooting= NdFooting
        self.MdFooting= MdFooting
        self.VdFooting= VdFooting
        
    def interpolate(self):
        self.y= self.mdEnvelope.yValues
        self.ndMaxStem= scipy.interpolate.interp1d(self.y,self.ndEnvelope.positive)
        self.ndMinStem= scipy.interpolate.interp1d(self.y,self.ndEnvelope.negative)
        
        self.mdMaxStem= scipy.interpolate.interp1d(self.y,self.mdEnvelope.positive)
        self.mdMinStem= scipy.interpolate.interp1d(self.y,self.mdEnvelope.negative)
        
        self.vdMaxStem= scipy.interpolate.interp1d(self.y,self.vdEnvelope.positive)    
        self.vdMinStem= scipy.interpolate.interp1d(self.y,self.vdEnvelope.negative)
        
    def __imul__(self,f):
        for n in self.ndMax:
          n*=f
        for m in self.mdMax:
          m*=f
        for v in self.vdMax:
          v*=f
        self.interpolate()
        self.NdFooting*=f
        self.MdFooting*=f
        self.VdFooting*=f
        return self
    
    def clone(self):
        ''' Return a copy of this object.'''
        return InternalForces(y= self.y, ndEnvelope= self.ndEnvelope, mdEnvelope= self.mdEnvelope, vdEnvelope= self.vdEnvelope, NdFooting= self.NdFooting, MdFooting= self.MdFooting, VdFooting= self.VdFooting)
    
    def __mul__(self, f):
        retval= self.clone()
        retval*= f
        return retval
    
    def __rmul__(self,f):
        return self*f
    
    def NdMaxEncastrement(self,footingThickness):
        '''Bending moment (envelope) at stem base.

        :param footingThickness: thickness of the footing.
        '''
        yEncastrement= self.stemHeight-footingThickness/2.0
        return self.NdMax(yEncastrement)
    
    def NdMinEncastrement(self,footingThickness):
        '''Bending moment (envelope) at stem base.

        :param footingThickness: thickness of the footing.
        '''
        yEncastrement= self.stemHeight-footingThickness/2.0
        return self.NdMin(yEncastrement)
    
    def MdMaxEncastrement(self,footingThickness):
        '''Bending moment (envelope) at stem base.

        :param footingThickness: thickness of the footing.
        '''
        yEncastrement= self.stemHeight-footingThickness/2.0
        return self.MdMax(yEncastrement)
    
    def VdMaxEncastrement(self, footingThickness, epaisseurEncastrement):
        '''Shear force (envelope) at stem base.

        :param epaisseurEncastrement: thickness of the stem bottom.
        '''
        yV= self.stemHeight-footingThickness/2.0-epaisseurEncastrement
        return abs(self.vdMaxStem(yV))
    
    def MdMaxMidStem(self,footingThickness):
        '''Max. bending moment (envelope) at the middle of the stem.

        :param footingThickness: thickness of the footing.
        '''
        yMidStem= (self.stemHeight-footingThickness/2.0)/2.0
        return self.MdMax(yMidStem)
    
    def VdMaxMidStem(self,footingThickness):
        '''Max. shear (envelope) at the middle of the stem.

        :param footingThickness: thickness of the footing.
        '''
        yMidStem= (self.stemHeight-footingThickness/2.0)/2.0
        return self.VdMax(yMidStem)
    
    def MdMinMidStem(self,footingThickness):
        '''Min. bending moment (envelope) at the middle of the stem.

        :param footingThickness: thickness of the footing.
        '''
        yMidStem= (self.stemHeight-footingThickness/2.0)/2.0
        return self.MdMin(yMidStem)
    
    def VdMinMidStem(self,footingThickness):
        '''Min. shear (envelope) at the middle of the stem.

        :param footingThickness: thickness of the footing.
        '''
        yMidStem= (self.stemHeight-footingThickness/2.0)/2.0
        return self.VdMin(yMidStem)
    
    def NdMax(self, y):
        '''Max. axial force (envelope) at height y.
  
        :param y: height to compute the shear force at.
        '''
        return self.ndMaxStem(y)
    
    def NdMin(self, y):
        '''Max. axial force (envelope) at height y.
  
        :param y: height to compute the shear force at.
        '''
        return self.ndMinStem(y)
    
    def VdMax(self, y):
        '''Max. shear (envelope) at height y.
  
        :param y: height to compute the shear force at.
        '''
        return abs(self.vdMaxStem(y))
    
    def VdMin(self, y):
        '''Min. shear (envelope) at height y.
  
        :param y: height to compute the shear force at.
        '''
        return abs(self.vdMinStem(y))
    
    def MdMax(self, y):
        '''Max. bending moment (envelope) at height y.
  
        :param y: height to compute the shear force at.
        '''
        return abs(self.mdMaxStem(y))
    
    def MdMin(self, y):
        '''Min bending moment (envelope) at height y.
  
        :param y: height to compute the shear force at.
        '''
        return abs(self.mdMinStem(y))
    
    def getYStem(self, hCoupe):
        ''' Return the depth corresponding to the height argument.

        :param hCoupe: height with respect to the footing midplane.
        '''
        return self.stemHeight-hCoupe
    
    def writeGraphic(self,fileName):
        '''Draws a graphic of internal forces (envelopes) in
           the wall stem.

        :param fileName: name of the output file.
        '''
        z= []
        for yi in self.y:
            z.append(self.stemHeight-yi)
        mPos= []
        for mi in self.mdEnvelope.positive:
            mPos.append(mi/1e3)
        mNeg= []
        for mi in self.mdEnvelope.negative:
            mNeg.append(mi/1e3)
        vPos= []
        for vi in self.vdEnvelope.positive:
            vPos.append(vi/1e3)
        vNeg= []
        for vi in self.vdEnvelope.negative:
            vNeg.append(vi/1e3)
        import matplotlib.pyplot as plt
        plt.plot(mPos,z,'+', vPos, z,'^',mNeg,z,'-', vNeg, z,'v')
        plt.legend(['Md+ (kN m/m)', 'Vd+ (kN/m)','Md- (kN m/m)', 'Vd- (kN/m)'], loc='best')
        plt.title("Internal forces.")
        plt.savefig(fileName)
        plt.close()

class ReinforcementMap(dict):
    ''' Simplified reinforcement for a cantilever retaining wall.

    :ivar concreteCover: concrete cover for rebars.
    :ivar steel: reinforcement steel type.
    :ivar rebarFamilyTemplate: reinforcement template.
    '''
    def __init__(self,concreteCover=40e-3, steel= None,rebarFamilyTemplate= None):
        '''Constructor.
 
        :param concreteCover: concrete cover for rebars.
        :param steel: reinforcement steel type.
        :param rebarFamilyTemplate: reinforcement template.
        '''
        super(ReinforcementMap, self).__init__()
        self.concreteCover= concreteCover
        #Materials.
        self.steel= steel
        #Default reinforcement
        for i in range(1,15):
          self[i]= rebarFamilyTemplate
        # #Footing skin reinforcement
        # R= self.footingThickness-2*self.concreteCover-8e-3
        # n= math.ceil(R/0.15)+1
        # ecart= R/(n-1)
        # self[self.skinFootingIndex]= FamNBars(self.steel,n,8e-3,ecart,concreteCover)
        # #Stem top reinforcement
        # R= self.stemTopWidth-2*self.concreteCover-8e-3
        # n= math.ceil(R/0.15)+1
        # ecart= R/(n-1)
        # self[self.topSkinIndex]= FamNBars(self.steel,n,8e-3,ecart,concreteCover)

    def __eq__(self, other):
        '''Overrides the default implementation'''
        retval= super(ReinforcementMap, self).__eq__(other)
        if(retval):
            retval= self.concreteCover == other.concreteCover
        if(retval):
            retval= self.steel == other.steel
        if(retval):
            retval= retval.rebarFamilyTemplate == other.rebarFamilyTemplate
        return retval
            
    def setReinforcement(self,index,reinforcement):
        '''Set reinforcement.

        :param index: index of the reinforcement to set.
        :param reinforcement: reinforcement to set.
        '''
        self[index]= reinforcement

    def getReinforcement(self,index):
        '''Return reinforcement at index.

        :param index: index of the reinforcement to get.
        '''
        return self[index]

    def getBasicAnchorageLength(self,index, concrete):
        '''Returns basic anchorage length for the reinforcement at "index".

        :param index: key of the reinforcement bar in this map.
        :param concrete: concrete material.
        '''
        return self.getReinforcement(index).getBasicAnchorageLength(concrete)
    
class WallStabilityResults(object):
    ''' Results of the wall stability analysis.'''
    def __init__(self,wall,combinations,foundationSoilModel, toeFillDepth, sg_adm= None, gammaRSliding= 1.0, gammaRBearing= 1.0, ignoreAdhesion= True, NgammaCoef= 2.0):
        ''' Constructor.

        :param wall: retaining wall to analyze.
        :param foundationSoilModel: soil model foundation.
        :param sg_adm: maximum allowable soil pressure.
        :param gammaR: partial safety factor.
        :param gammaRSliding: partial factor for sliding resistance.
        :param gammaRBearing: partial factor for bearing resistance.
        :param ignoreAdhesion: if true don't consider the adhesion of the foundation to the soil for sliding resistance.
        :param NgammaCoef: Coefficient to compute Ngamma value with the
                           Brinch-Hansen formulas (see FrictionalCohesiveSoil
                           class). See also D.4 in EC7 part 1.
        '''
        self.Foverturning= 1e15
        self.FoverturningComb= ''
        self.Fsliding= 1e15
        self.FslidingComb= ''
        self.Fbearing= 1e15
        self.FbearingComb= ''
        self.FadmPressure= 1e15
        self.FadmPressureComb= ''
        for comb in combinations:
            reactions= wall.resultComb(comb)
            R= reactions.getResultantSVS()
            Foverturning= wall.getOverturningSafetyFactor(R ,gammaR= 1.0)
            if(Foverturning<self.Foverturning):
                self.Foverturning= Foverturning
                self.FoverturningComb= comb
            Fsliding= wall.getSlidingSafetyFactor(R,foundationSoilModel= foundationSoilModel,gammaR= gammaRSliding, ignoreAdhesion= ignoreAdhesion)
            if(Fsliding<self.Fsliding):
                self.Fsliding= Fsliding
                self.FslidingComb= comb
            Fbearing= wall.getBearingPressureSafetyFactor(R= R, foundationSoilModel= foundationSoilModel, toeFillDepth= toeFillDepth, q= 0.0, gammaR= gammaRBearing, NgammaCoef= NgammaCoef)
            if(Fbearing<self.Fbearing):
                self.Fbearing= Fbearing
                self.FbearingComb= comb
            if(sg_adm):
                FadmPressure= wall.getAdmissiblePressureSafetyFactor(R,sg_adm)
                if(FadmPressure<self.FadmPressure):
                    self.FadmPressure= FadmPressure
                    self.FadmPressureComb= comb

    def getDegreeOfUtilizationForSliding(self):
        ''' Return the degree of utilization obtained in the verification 
            of resistance to sliding.
        '''
        return 1.0/self.Fsliding

    def getWorstCombinationForSliding(self):
        ''' Return the worst case for the verification of resistance 
            to sliding.
        '''
        return self.FslidingComb    

    def getDegreeOfUtilizationForBearingResistance(self):
        ''' Return the degree of utilization obtained in the verification 
            of bearing resistance.
        '''
        return 1.0/self.Fbearing

    def getWorstCombinationForBearingResistance(self):
        ''' Return the worst case for the verification of bearing resistance.
        '''
        return self.FbearingComb
    
    def getDegreeOfUtilizationForAdmissiblePressure(self):
        ''' Return the degree of utilization obtained in the verification 
            of resistance to sliding.
        '''
        return 1.0/self.FadmPressure

    def getWorstCombinationForAdmissiblePressure(self):
        ''' Return the worst case for the verification of admissible pressure.
        '''
        return self.FadmPressureComb    
                    
    def getDegreeOfUtilizationForOverturning(self):
        ''' Return the degree of utilization obtained in the verification 
            of resistance to overturning.
        '''
        return 1.0/self.Foverturning
    

    def getWorstCombinationForOverturning(self):
        ''' Return the worst case for the verification for overturning.
        '''
        return self.FoverturningComb
    
    def writeOutput(self,outputFile,name):
        '''Write results in LaTeX format.

        :param outputFile: output file.
        :param name: wall name.
        '''
        outputFile.write("\\begin{center}\n")
        outputFile.write("\\begin{tabular}[H]{|l|c|c|c|}\n")
        outputFile.write("\\hline\n")
        outputFile.write("\\multicolumn{4}{|c|}{\\textsc{wall: "+name+" stability check}}\\\\\n")
        outputFile.write("\\hline\n")
        outputFile.write("Vérification:  & $F_{avail}$ & $F_{req}$ & Combination\\\\\n")
        outputFile.write("\\hline\n")
        outputFile.write("Overturning:  & " + fmt.Factor.format(self.Foverturning) +" & 1.00 & "+self.FoverturningComb+'\\\\\n')
        outputFile.write("Sliding:  & " + fmt.Factor.format(self.Fsliding) +" & 1.00 & "+self.FslidingComb+'\\\\\n')
        outputFile.write("Bearing capacity:  & " + fmt.Factor.format(self.Fbearing) +" & 1.00 & "+self.FbearingComb+'\\\\\n')
        if(self.FadmPressureComb!=''):
          outputFile.write("Adm. pressure:  & " + fmt.Factor.format(self.FadmPressure) +" & 1.00 & "+self.FadmPressureComb+'\\\\\n')
        outputFile.write("\\hline\n")
        outputFile.write("\\multicolumn{4}{|l|}{$F_{avail.}$: available security.}\\\\\n")
        outputFile.write("\\multicolumn{4}{|l|}{$F_{req}$: required security.}\\\\\n")

        outputFile.write("\\hline\n")
        outputFile.write("\\end{tabular}\n")
        outputFile.write("\\end{center}\n")

class WallULSResults(object):
    ''' Results for the ultimate limit state checking.'''
    def __init__(self,internalForces):
      self.internalForces= internalForces

class WallSLSResults(WallULSResults):
    ''' Results for the serviceability limit state checking.'''
    def __init__(self,internalForces,rotation, rotationComb):
        super(WallSLSResults,self).__init__(internalForces)
        self.rotation= rotation
        self.rotationComb= rotationComb
    def writeOutput(self,outputFile,name):
        '''Write results in LaTeX format.'''
        outputFile.write("\\begin{center}\n")
        outputFile.write("\\begin{tabular}[H]{|l|c|c|c|}\n")
        outputFile.write("\\hline\n")
        outputFile.write("\\multicolumn{3}{|c|}{\\textsc{Wall: "+name+" rotation check}}\\\\\n")
        outputFile.write("\\hline\n")
        outputFile.write("$\\beta_{avail} (\\permil)$ & $\\beta_{req}(\\permil)$ & Combination\\\\\n")
        outputFile.write("\\hline\n")
        outputFile.write(fmt.Factor.format(self.rotation*1000) +" & 2.00 & "+self.rotationComb+'\\\\\n')
        outputFile.write("\\hline\n")
        outputFile.write("\\multicolumn{3}{|l|}{$\\beta_{avail}$: wall maximum computed rotation.}\\\\\n")
        outputFile.write("\\multicolumn{3}{|l|}{$\\beta_{req}$: wall maximum admissible rotation.}\\\\\n")

        outputFile.write("\\hline\n")
        outputFile.write("\\end{tabular}\n")
        outputFile.write("\\end{center}\n")

class Envelope(object):
    ''' Store internal forces envelope.'''
    def __init__(self, yValues):
        '''Constructor.

        :param yValues: values of y coordinates.
        '''
        self.yValues= yValues
        size= len(self.yValues)
        self.positive= [-1.0e23]*size
        self.negative= [1.0e23]*size
        
    def __str__(self):
        retval= 'yi= '+str(self.yValues)+'\n'
        retval+= 'maxima: '+str(self.positive)+'\n'
        retval+= 'minima: '+str(self.negative)+'\n'
        return retval
      
    def update(self, values):
        '''Update envelopes.

        :param values: values to update the current envelope.
        '''
        self.positive= [max(l1, l2) for l1, l2 in zip(self.positive, values)]
        self.negative= [min(l1, l2) for l1, l2 in zip(self.negative, values)]
        
    def getMaximum(self):
        '''Return the maximum of the positive envelope'''
        retval= self.positive[0]
        for v in self.positive[1:]:
            retval= max(v,retval)
        return retval
      
    def getMinimum(self):
        '''Return the minimum of the negative envelope'''
        retval= self.negative[0]
        for v in self.negative[1:]:
            retval= min(v,retval)
        return retval
      
    def getAbsMaximum(self):
        '''Return the greatest value positive or negative'''
        retval= self.positive[0]
        for v in self.positive[1:]:
            if(abs(v)>abs(retval)):
                retval= v
        for v in self.negative:
            if(abs(v)>abs(retval)):
                retval= v
        return retval
      
    def filterRepeatedValues(self):
        '''Filter repeated values.'''
        sz= len(self.yValues)

        mapPos= dict()
        mapNeg= dict()
        for i in range(0,sz):
            y= abs(self.yValues[i])
            mapPos[y]= self.positive[i]
            mapNeg[y]= self.negative[i]

        retY= list()
        retPos= list()
        for y in mapPos:
            retY.append(y)
        retY.sort()
        for y in retY:
            retPos.append(mapPos[y])
        self.positive= retPos

        retY= list()
        retNeg= list()
        for y in mapNeg:
            retY.append(y)
        retY.sort()
        for y in retY:
            retNeg.append(mapNeg[y])
        self.negative= retNeg
        self.yValues= retY

class StemReinforcement(ReinforcementMap):
    '''Stem reinforcement. '''
    extStemBottomIndex= 1 # exterior reinforcement at stem bottom.
    extStemIndex= 2 # exterior reinforcement at stem.
    intStemBottomIndex= 4 # interior reinforcement at stem bottom.
    intStemIndex= 5 # interior reinforcement at stem.
    topStemIndex= 6 # reinforcement at stem top.
    longExtStemIndex= 11 # exterior longitudinal reinforcement at stem.
    longIntStemIndex= 12 # interior longitudinal reinforcement at stem.
    topSkinIndex= 13 # longitudinal skin reinforcement at stem top.

    def __init__(self,wallGeom, concreteCover, steel):
        '''Constructor.'''
        super(StemReinforcement,self).__init__(concreteCover, steel)
        self.wallGeom= wallGeom

    def __eq__(self, other):
        '''Overrides the default implementation'''
        retval= super(StemReinforcement, self).__eq__(other)
        if(retval):
            retval= self.wallGeom == other.wallGeom
        return retval

    def getStemMemberType(self):
        ''' Return the member type of the stem ('wall' or 'short wall')
            depending on the joint spacing.
        '''
        jointSpacing= self.wallGeom.jointSpacing
        if(jointSpacing<7.5):
            return 'short_wall'
        else:
            return 'wall'
        
    # def getRCSections(self,stemSets):
    #     '''Create reinforced concrete sections for the stem.

    #     :param stemSets: sets of elements along the stem.
    #     '''
    #     numberOfSets= len(stemSets)
    #     stemDepthInc= (self.wallGeom.stemTopWidth-self.wallGeom.stemBottomWidth)/numberOfSets
    #     stemRCSections= dict()
    #     for i in range(0,4):
    #         h= self.wallGeom.stemBottomWidth
    #         sectName= 'stemRCSect'+str(h)
    #         stemRCSections[h]= element_section_map.RCSlabBeamSection(name=sectName,sectionDescr='stem',concrType= self.concrete, reinfSteelType= stemReinforcement.steel,width= self.b, depth= h,elemSet= stemSets[i])
    #         if(i==0):
    #             positvRebarRows1= rcs.LongReinfLayers([rcs.RebarRow2ReinfRow(self[self.extStemBottomIndex])])
    #             negatvRebarRows1= rcs.LongReinfLayers([rcs.RebarRow2ReinfRow(self[self.intStemBottomIndex])])
    #         else:
    #             positvRebarRows1= rcs.LongReinfLayers([rcs.RebarRow2ReinfRow(self[self.extStemIndex])])
    #             negatvRebarRows1= rcs.LongReinfLayers([rcs.RebarRow2ReinfRow(self[self.intStemIndex])])
    #         stemRCSections[h].dir1PositvRebarRows= positvRebarRows1
    #         stemRCSections[h].dir1NegatvRebarRows= negatvRebarRows1
    #         stemRCSections[h].dir2PositvRebarRows= positvRebarRows1
    #         stemRCSections[h].dir2NegatvRebarRows= negatvRebarRows1
    #         h-= stemDepthInc
    #     return stemRCSections
      
    def getSectionExtStemBottom(self):
        '''Returns RC section for exterior reinforcement at stem bottom.'''
        return ng_rc_section.RCSection(self[self.extStemBottomIndex],self.wallGeom.concrete,self.wallGeom.b,self.wallGeom.stemBottomWidth, memberType= self.getStemMemberType())
    
    def getSectionStem(self, y, reinforcementIndex):
        '''Returns RC section of the stem.

        :param y: depth of the section.
        :param reinforcementIndex: index of the reinforcement for the section.
        '''
        thickness= self.wallGeom.getSectionDepth(y)
        return ng_rc_section.RCSection(tensionRebars= self[reinforcementIndex], concrete= self.wallGeom.concrete,b= self.wallGeom.b, h= thickness, memberType= self.getStemMemberType())
              
    
    def getSectionIntStemBottom(self):
        '''Returns RC section for interior reinforcement at stem bottom.'''
        return ng_rc_section.RCSection(tensionRebars= self[self.intStemBottomIndex], concrete= self.wallGeom.concrete, b= self.wallGeom.b, h= self.wallGeom.stemBottomWidth, memberType= self.getStemMemberType())
    
    def getSectionStemTop(self):
        '''Returns RC section for reinforcement at stem top.'''
        return ng_rc_section.RCSection(tensionRebars= self[self.topStemIndex], concrete= self.wallGeom.concrete, b= self.wallGeom.b, h= self.wallGeom.stemTopWidth, memberType= self.getStemMemberType())
    
    def getSectionStemLongExt(self):
        '''Returns RC section for longitudinal reinforcement in stem exterior.'''
        reinforcementIndex= self.longExtStemIndex
        if(self.wallGeom.stemBackSteps is None):
            return ng_rc_section.RCSection(tensionRebars= self[reinforcementIndex], concrete= self.wallGeom.concrete, b= self.wallGeom.b, h= (self.wallGeom.stemTopWidth+self.wallGeom.stemBottomWidth)/2.0, memberType= self.getStemMemberType())
        else: # Compute the thickness at the middle of the lowest step. 
            depth= (self.wallGeom.stemBackSteps[-1][0]+self.wallGeom.stemHeight)/2.0
            return self.getSectionStem(y= depth, reinforcementIndex= reinforcementIndex) 
    
    def getSectionStemLongInt(self):
        '''Returns RC section for longitudinal reinforcement in stem interior.'''
        reinforcementIndex= self.longIntStemIndex
        if(self.wallGeom.stemBackSteps is None):
            return ng_rc_section.RCSection(tensionRebars= self[reinforcementIndex], concrete= self.wallGeom.concrete, b= self.wallGeom.b, h= (self.wallGeom.stemTopWidth+self.wallGeom.stemBottomWidth)/2.0, memberType= self.getStemMemberType())
        else: # Compute the thickness at the middle of the lowest step. 
            depth= (self.wallGeom.stemBackSteps[-1][0]+self.wallGeom.stemHeight)/2.0
            return self.getSectionStem(y= depth, reinforcementIndex= reinforcementIndex) 

    def getYExtReinfChange(self):
        ''' Returns the depth where the exterior reinforcement of the stem chantes.
        '''
        anchorageLength= self.getBasicAnchorageLength(self.extStemBottomIndex, self.wallGeom.concrete)
        return self.wallGeom.getYStem(anchorageLength)
        
    def writeResult(self, outputFile):
        '''Write stem reinforcement verification results in LaTeX format.'''

        # Exterior reinforcement in stem bottom.
        CExtStemBottom= self.getSectionExtStemBottom()
        VdMaxEncastrement= self.wallGeom.internalForcesULS.VdMaxEncastrement(self.wallGeom.footingThickness, self.wallGeom.stemBottomWidth)
        MdMaxEncastrement= self.wallGeom.internalForcesULS.MdMaxEncastrement(self.wallGeom.footingThickness)
        outputFile.write("\\textbf{Reinforcement "+str(self.extStemBottomIndex)+" (back reinforcement dowels) :} \\\\\n")
        NdEncastrement= self.wallGeom.internalForcesULS.NdMinEncastrement(self.wallGeom.footingThickness)
        CExtStemBottom.writeResultFlexion(outputFile, NdEncastrement, MdMaxEncastrement,VdMaxEncastrement)
        # CExtStemBottom.writeResultStress(outputFile,MdMaxEncastrement) # LP commented out 24/08/2023

        # Exterior reinforcement in stem.
        yCoupeExtStem= self.getYExtReinfChange()
        stemReinforcementIndex= self.extStemIndex
        CExtStem= self.getSectionStem(y= yCoupeExtStem, reinforcementIndex= stemReinforcementIndex)
        NdExtStem= self.wallGeom.internalForcesULS.NdMin(yCoupeExtStem) #
        VdExtStem= self.wallGeom.internalForcesULS.VdMax(yCoupeExtStem)
        MdExtStem= self.wallGeom.internalForcesULS.MdMax(yCoupeExtStem)
        outputFile.write("\\textbf{Reinforcement "+str(self.extStemIndex)+" (back vertical stem reinforcement):}\\\\\n")
        CExtStem.writeResultFlexion(outputFile,NdExtStem,MdExtStem,VdExtStem)
        # CExtStem.writeResultStress(outputFile,MdExtStem) # LP commented out 24/08/2023

        steps= None
        if(self.wallGeom.stemBackSteps):
            steps= self.wallGeom.stemBackSteps[::-1] # Reverse step list.
        if(steps):
            for i, step in enumerate(steps):
                depth= step[0]-0.01
                stemReinforcementIndex= (i+1)*100+self.extStemIndex
                CExtStem= self.getSectionStem(y= depth, reinforcementIndex= stemReinforcementIndex)
                NdExtStem= self.wallGeom.internalForcesULS.NdMin(depth) #
                VdExtStem= self.wallGeom.internalForcesULS.VdMax(depth)
                MdExtStem= self.wallGeom.internalForcesULS.MdMax(depth)
                outputFile.write("\\textbf{Reinforcement "+str(stemReinforcementIndex)+" (back vertical stem reinforcement step:" +str(i+1)+"):}\\\\\n")
                CExtStem.writeResultFlexion(outputFile,NdExtStem,MdExtStem,VdExtStem)
                # CExtStem.writeResultStress(outputFile,MdExtStem) # LP commented out 24/08/2023
                

        # Interior reinforcement at stem bottom 
        CIntStemBottom= self.getSectionIntStemBottom()
        CIntStem= CIntStemBottom
        CSectionStemTop= self.getSectionStemTop()
        CSectionStemLongExt= self.getSectionStemLongExt()
        CSectionStemLongInt= CSectionStemLongExt

        outputFile.write("\\textbf{Reinforcement "+str(self.intStemBottomIndex)+" (front reinforcement dowels):}\\\\\n")
        CIntStemBottom.writeResultCompression(outputFile,0.0,CSectionStemLongInt.tensionRebars.getAs())

        # Interior reinforcement at stem.
        outputFile.write("\\textbf{Reinforcement "+str(self.intStemIndex)+" (front vertical stem reinforcement):}\\\\\n")
        CIntStem.writeResultCompression(outputFile,0.0,CSectionStemLongInt.tensionRebars.getAs())
        
        if(steps):
            for i, step in enumerate(steps):
                depth= step[0]-0.01
                stemReinforcementIndex= (i+1)*100+self.intStemIndex
                CIntStem= self.getSectionStem(y= depth, reinforcementIndex= stemReinforcementIndex)
                outputFile.write("\\textbf{Reinforcement "+str(stemReinforcementIndex)+" (front vertical stem reinforcement step:" +str(i+1)+"):}\\\\\n")
                CIntStem.writeResultCompression(outputFile,0.0,CSectionStemLongInt.tensionRebars.getAs())
                

        # Reinforcement at stem top.
        outputFile.write("\\textbf{Reinforcement "+str(self.topStemIndex)+" (top stem reinforcement):}\\\\\n")
        CSectionStemTop.writeResultFlexion(outputFile,0.0,0.0,0.0)

        # Stem exterior longitudinal reinforcement.
        outputFile.write("\\textbf{Reinforcement "+str(self.longExtStemIndex)+" (back longitudinal stem reinforcement):}\\\\\n")
        CSectionStemLongExt.writeResultTraction(outputFile= outputFile, Nd= 0.0)
        
        if(steps):
            for i, step in enumerate(steps):
                depth= step[0]-0.01
                stemReinforcementIndex= (i+1)*100+self.longExtStemIndex
                CExtStem= self.getSectionStem(y= depth, reinforcementIndex= stemReinforcementIndex)
                outputFile.write("\\textbf{Reinforcement "+str(stemReinforcementIndex)+" (back longitudinal stem reinforcement step:" +str(i+1)+"):}\\\\\n")
                CExtStem.writeResultTraction(outputFile= outputFile, Nd= 0.0)

        # Stem interior longitudinal reinforcement.
        outputFile.write("\\textbf{Reinforcement  "+str(self.longIntStemIndex)+" (front longitudinal stem reinforcement):}\\\\\n")
        CSectionStemLongInt.writeResultTraction(outputFile= outputFile, Nd= 0.0)

        if(steps):
            for i, step in enumerate(steps):
                depth= step[0]-0.01
                stemReinforcementIndex= (i+1)*100+self.longIntStemIndex
                CIntStem= self.getSectionStem(y= depth, reinforcementIndex= stemReinforcementIndex)
                outputFile.write("\\textbf{Reinforcement "+str(stemReinforcementIndex)+" (front longitudinal stem reinforcement step:" +str(i+1)+"):}\\\\\n")
                CIntStem.writeResultTraction(outputFile= outputFile, Nd= 0.0)
                
        # Stem top skin reinforcement.
        outputFile.write("\\textbf{Reinforcement "+str(self.topSkinIndex)+" (top longitudinal stem reinforcement):}\\\\\n")
        outputFile.write("  --\\\\\n")
        #self[self.topSkinIndex].writeRebars(outputFile,self.wallGeom.concrete,1e-5)

    def drawSchema(self,defStrings):
        '''Stem data for wall scheme drawing in LaTeX format.'''

        defStrings[self.extStemBottomIndex]= self.getSectionExtStemBottom().tensionRebars.getDefStrings()
        yCoupeExtStem= self.getYExtReinfChange()
        defStrings[self.extStemIndex]= self.getSectionStem(y= yCoupeExtStem, reinforcementIndex= self.extStemIndex).tensionRebars.getDefStrings()
        defStrings[self.intStemBottomIndex]= self.getSectionIntStemBottom().tensionRebars.getDefStrings()
        defStrings[self.intStemIndex]= self.getSectionIntStemBottom().tensionRebars.getDefStrings() #CIntStem==CIntStemBottom
        defStrings[self.topStemIndex]= self.getSectionStemTop().tensionRebars.getDefStrings()
        defStrings[self.longExtStemIndex]= self.getSectionStemLongExt().tensionRebars.getDefStrings()
        defStrings[self.longIntStemIndex]= defStrings[self.longExtStemIndex]#self.getSectionStemLongInt().tensionRebars.getDefStrings() #CSectionStemLongInt==CSectionStemLongExt

class FootingReinforcement(ReinforcementMap):
    '''Footing reinforcement. '''
    topFootingIndex= 3 # transverse reinforcement at wall footing top.
    bottomFootingIndex= 7 # transverse reinforcement at wall footing bottom.
    longBottomFootingIndex= 8 # longitudinal reinforcement at wall footing bottom.
    longTopFootingIndex= 9 # longitudinal reinforcement at wall footing bottom.
    skinFootingIndex= 10 # skin reinforcement at wall footing.

    def __init__(self,wallGeom, concreteCover, steel):
        '''Constructor.'''
        super(FootingReinforcement,self).__init__(concreteCover, steel)
        self.wallGeom= wallGeom
        
    def __eq__(self, other):
        '''Overrides the default implementation'''
        retval= super(FootingReinforcement, self).__eq__(other)
        if(retval):
            retval= self.wallGeom == other.wallGeom
        return retval

    # def getRCSections(self, footingSet):
    #     '''Create reinforced concrete sections for the footing.
        
    #     :param footingSet: set of the footing elements.
    #     '''
    #     sectName= 'footingRCSet'
    #     footingRCSection= element_section_map.RCSlabBeamSection(name=sectName,sectionDescr='stem',concrType= self.concrete, reinfSteelType= stemReinforcement.steel,width= self.b, depth= h,elemSet= footingSet)
        
    #     positvRebarRows1= rcs.LongReinfLayers([rcs.RebarRow2ReinfRow(self[self.bottomFootingIndex])])
    #     negatvRebarRows1= rcs.LongReinfLayers([rcs.RebarRow2ReinfRow(self[self.topFootingIndex])])
    #     footingRCSection.dir1PositvRebarRows= positvRebarRows1
    #     footingRCSection.dir1NegatvRebarRows= negatvRebarRows1
    #     footingRCSection.dir2PositvRebarRows= positvRebarRows1
    #     footingRCSection.dir2NegatvRebarRows= negatvRebarRows1
    #     return footingRCSection
      
    def getSectionTopFooting(self):
        '''Returns RC section for reinforcement on footing top.'''
        return ng_rc_section.RCSection(tensionRebars= self[self.topFootingIndex], concrete= self.wallGeom.concrete, b= self.wallGeom.b, h= self.wallGeom.footingThickness, memberType='footing')
      
    def getSectionFootingBottom(self):
        '''Returns RC section for reinforcement at footing bottom.'''
        return ng_rc_section.RCSection(tensionRebars= self[self.bottomFootingIndex], concrete= self.wallGeom.concrete, b= self.wallGeom.b, h= self.wallGeom.footingThickness, memberType='footing')
      
    def getSectionFootingBottomLongitudinal(self):
        '''Returns RC section for longitudinal reinforcement at footing bottom.'''
        return ng_rc_section.RCSection(tensionRebars= self[self.longBottomFootingIndex], concrete= self.wallGeom.concrete, b= self.wallGeom.b, h= self.wallGeom.footingThickness, memberType='footing')

    def getShearForceAtFootingStemCorner(self):
        ''' Return the shear force at the vertical plane that passes through
            the corner formed by the footing top surface and the stem internal
            face.
        '''
        # It will be much better to get the internal forces envelopes at the
        # footing instead of using the following trick:
        VdTopFooting= abs(self.wallGeom.internalForcesULS.VdFooting.getAbsMaximum())
        # Assume shear decreases linearly to reach zero at the end of the heel.
        interpolationFactor= VdTopFooting/self.wallGeom.bHeel
        retval= VdTopFooting - interpolationFactor*self.wallGeom.stemBottomWidth/2.0
        return retval
    
    def writeResult(self, outputFile):
        '''Write reinforcement verification results in LaTeX format.'''

        # Reinforcement on footing top
        CTopFooting= self.getSectionTopFooting()
        # Consider the minimum compressive axial force (favorable effect):
        NdTopFooting= self.wallGeom.internalForcesULS.NdFooting.getAbsMaximum()
        VdTopFooting= self.getShearForceAtFootingStemCorner()
        MdTopFooting= self.wallGeom.internalForcesULS.MdFooting.getAbsMaximum() # Concomitant??
        outputFile.write("\\textbf{Reinforcement "+str(self.topFootingIndex)+" (top transverse footing reinforcement):}\\\\\n")
        CTopFooting.writeResultFlexion(outputFile, NdTopFooting, MdTopFooting, VdTopFooting)
        # CTopFooting.writeResultStress(outputFile,self.wallGeom.internalForcesSLS.MdFooting.getAbsMaximum()) # LP commented out 24/08/2023

        CSectionFootingBottom= self.getSectionFootingBottom()
        CSectionFootingBottomLongitudinal= self.getSectionFootingBottomLongitudinal()
        CSectionFootingTopLongitudinal= CSectionFootingBottomLongitudinal

        # Reinforcemnt at footing bottom.
        outputFile.write("\\textbf{Reinforcement "+str(self.bottomFootingIndex)+" (bottom transverse footing reinforcement):}\\\\\n")
        CSectionFootingBottom.writeResultCompression(outputFile,0.0,CSectionFootingBottomLongitudinal.tensionRebars.getAs())

        # Longitudinal reinforcement at footing bottom.
        outputFile.write("\\textbf{Reinforcement "+str(self.longBottomFootingIndex)+" (bottom longitudinal footing reinforcement):}\\\\\n")
        CSectionFootingBottomLongitudinal.writeResultTraction(outputFile,0.0)

        # Longitudinal reinforcement at footing top.
        outputFile.write("\\textbf{Reinforcement "+str(self.longTopFootingIndex)+" (top longitudinal footing reinforcement):}\\\\\n")
        CSectionFootingTopLongitudinal.writeResultTraction(outputFile,0.0)

        # Footing skin reinforcement.
        outputFile.write("\\textbf{Reinforcement "+str(self.skinFootingIndex)+" (skin footing reinforcement):}\\\\\n")
        outputFile.write("  --\\\\\n")
        #self[self.skinFootingIndex].writeRebars(outputFile,self.wallGeom.concrete,1e-5)

    def drawSchema(self,defStrings):
        '''Footing scheme drawing in LaTeX format.'''

        defStrings[self.topFootingIndex]= self.getSectionTopFooting().tensionRebars.getDefStrings()
        defStrings[self.bottomFootingIndex]= self.getSectionFootingBottom().tensionRebars.getDefStrings()
        defStrings[self.longBottomFootingIndex]= self.getSectionFootingBottomLongitudinal().tensionRebars.getDefStrings()
        defStrings[self.longTopFootingIndex]= self.getSectionFootingBottomLongitudinal().tensionRebars.getDefStrings() #CSectionFootingTopLongitudinal==CSectionFootingBottomLongitudinal
        #defStrings[self.skinFootingIndex]= self.getSectionFootingSkin().tensionRebars.getDefStrings()

 
class RetainingWall(retaining_wall_geometry.CantileverRetainingWallGeometry):
    '''Cantilever retaining wall.

    :ivar b: wall lenght (defaults to 1.0).
    :ivar numberOfStemSets: number of element sets along the stem height.
    :ivar stemBottomWidth: (float) Stem width at his contact with the footing.
    :ivar stemTopWidth: (float) Stem width at his top.
    :ivar stemBackSlope: (float) Stem back slope expressed as H/V ratio. 
    :ivar footingThickness: (float) Thickness of the footing.
    :ivar smoothPrecastFoundation: true if the wall has a precast foundation with a smooth contact with the soil.    
    '''
    b= 1.0
    numberOfStemSets= 4
    
    def __init__(self,name= 'prb',concreteCover= 40e-3, stemHeight= 2.5, stemBottomWidth=0.25, stemTopWidth=0.25, stemBackSlope= 0.0, footingThickness= 0.25, bToe= 0.5, bHeel= 1.0, concrete= None, steel= None,title=None, smoothPrecastFoundation= False):
        '''Constructor

        :param name: identifier for the retaining wall.
        :param concreteCover: cover of the concrete reinforcement.
        :param stemHeight: (float) Height of the stem.
        :param stemBottomWidth: (float) Stem width at his contact with the footing.
        :param stemTopWidth: (float) Stem width at his top.
        :param stemBackSlope: (float) Stem back slope expressed as H/V ratio. 
        :param footingThickness: (float) Thickness of the footing.
        :param bToe: (float) Toe length.
        :param bHeel: (float) Heel length.
        :param title: title for the report tables.
        :param smoothPrecastFoundation: true if the wall has a precast foundation with a smooth contact with the soil.    
        '''
        super(RetainingWall,self).__init__(name= name, stemHeight= stemHeight, stemBottomWidth= stemBottomWidth, stemTopWidth= stemTopWidth, footingThickness= footingThickness, bToe= bToe, bHeel= bHeel, stemBackSlope= stemBackSlope)
        #Materials.
        self.concrete= concrete
        self.stemReinforcement= StemReinforcement(self,concreteCover, steel)
        self.footingReinforcement= FootingReinforcement(self,concreteCover, steel)
        self.title=title if title else name.replace('_',' ')
        self.smoothPrecastFoundation= smoothPrecastFoundation
        

    def setULSInternalForcesEnvelope(self, wallInternalForces):
        '''Assigns the ultimate limit state infernal forces envelope 
           for the stem.

        :param wallInternalForces: container for the wall internal forces.
        '''
        if(hasattr(self,'stemHeight')):
            if(self.getWFStemHeigth()!=wallInternalForces.stemHeight):
                lmsg.warning('stem height (' + str(self.getWFStemHeigth()) + ' m) different from length of internal forces envelope law ('+ str(wallInternalForces.stemHeight)+ ' m)') 
        else:
            self.stemHeight= wallInternalForces.stemHeight-self.footingThickness/2.0
        self.internalForcesULS= wallInternalForces

    def setSLSInternalForcesEnvelope(self, wallInternalForces):
        '''Assigns the serviceability limit state infernal forces envelope 
           for the stem.

        :param wallInternalForces: container for the wall internal forces.
        '''
        if(hasattr(self,'stemHeight')):
            if(self.getWFStemHeigth()!=wallInternalForces.stemHeight):
                lmsg.warning('stem height (' + str(self.getWFStemHeigth()) + ' m) different from length of internal forces envelope law('+ str(wallInternalForces.stemHeight)+ ' m)') 
        else:
            self.stemHeight= wallInternalForces.stemHeight
        self.internalForcesSLS= wallInternalForces

    def writeDef(self, pth:str, outputFile, includeGraphicsPath:str, convertToEPS= False):
        '''Write wall definition in LaTeX format.

        :param pth: output path.
        :param outputFile: LaTeX output file.
        :param includeGraphicsPath: path for the includegraphics LaTeX command. 
        :param convertToEPS: if true, create a postscript version of the 
                             graphic output.
        '''
        valid_filename= path_utils.get_valid_filename(self.name)
        figurePath= pth+valid_filename
        figurePathPNG= figurePath+".png"
        self.internalForcesULS.writeGraphic(figurePathPNG)
        if(convertToEPS):
            figurePathEPS= figurePath+".eps"
            os.system("convert "+figurePathPNG+" "+figurePathEPS)
        outputFile.write("\\begin{table}\n")
        outputFile.write("\\begin{center}\n")
        outputFile.write("\\begin{tabular}[H]{|l|}\n")
        outputFile.write("\\hline\n")
        outputFile.write("\\multicolumn{1}{|c|}{\\textsc{"+self.title+"}}\\\\\n")
        outputFile.write("\\hline\n")
        outputFile.write("\\begin{tabular}{c|l}\n")
        outputFile.write("\\begin{minipage}{85mm}\n")
        outputFile.write("\\vspace{2mm}\n")
        outputFile.write("\\begin{center}\n")
        outputFile.write("\\includegraphics[width=80mm]{"+includeGraphicsPath+valid_filename+"}\n")
        outputFile.write("\\end{center}\n")
        outputFile.write("\\vspace{1pt}\n")
        outputFile.write("\\end{minipage} & \n")
        self.writeGeometry(outputFile)
        outputFile.write("\\end{tabular} \\\\\n")

        outputFile.write("\\hline\n")
        outputFile.write("\\begin{tabular}{llll}\n")
        outputFile.write("\\multicolumn{3}{c}{\\textsc{Materials}}\\\\\n")
        outputFile.write("  Concrete: " + self.concrete.materialName +" & ")
        outputFile.write("  Steel: " + self.stemReinforcement.steel.materialName +" & ")
        outputFile.write("  Concrete cover: "+ fmt.Diam.format(self.stemReinforcement.concreteCover*1e3)+ " mm\\\\\n")
        outputFile.write("\\end{tabular} \\\\\n")
        outputFile.write("\\hline\n")
        outputFile.write("\\end{tabular}\n")
        outputFile.write("\\caption{Wall materials and dimensions "+ self.title +"} \\label{tb_def_"+valid_filename+"}\n")
        outputFile.write("\\end{center}\n")
        outputFile.write("\\end{table}\n")

    def writeStrengthResults(self, outputFile):
        ''' Write reinforced concrete strength results.

        :param outputFile: LaTeX output file.
        '''
        outputFile.write("\\bottomcaption{Wall "+ self.title +" reinforcement} \\label{tb_"+self.name+"}\n")
        outputFile.write("\\tablefirsthead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{"+self.title+" wall reinforcement}}\\\\\\hline\n}\n")
        outputFile.write("\\tablehead{\\hline\n\\multicolumn{1}{|c|}{\\textsc{"+self.title+" (suite)}}\\\\\\hline\n}\n")
        outputFile.write("\\tabletail{\\hline \\multicolumn{1}{|r|}{../..}\\\\\\hline}\n")
        outputFile.write("\\tablelasttail{\\hline}\n")
        outputFile.write("\\begin{center}\n")
        outputFile.write("\\begin{supertabular}[H]{|l|}\n")
        outputFile.write("\\hline\n")

        self.stemReinforcement.writeResult(outputFile)
        self.footingReinforcement.writeResult(outputFile)

        outputFile.write("\\hline\n")
        outputFile.write("\\end{supertabular}\n")
        outputFile.write("\\end{center}\n")
        outputFile.close()
        
    def writeResult(self, pth:str, includeGraphicsPath= None, convertToEPS= False):
        '''Write reinforcement verification results in LaTeX format.

        :param pth: output path.
        :param includeGraphicsPath: path for the includegraphics LaTeX command. 
        :param convertToEPS: if true, create a postscript version of the 
                             graphic output.
        '''
        if(includeGraphicsPath is None):
            includeGraphicsPath= pth
        file_name_string= path_utils.get_valid_filename(self.name)
        outputFile= open(pth+file_name_string+".tex","w") # LaTeX output file.
        self.writeDef(pth= pth, outputFile= outputFile, includeGraphicsPath= includeGraphicsPath, convertToEPS= convertToEPS)
        # Stability ULS results.
        self.stability_results.writeOutput(outputFile,self.title)
        # Serviceability limit states results.
        self.sls_results.writeOutput(outputFile,self.title)
        # Structural strength results.
        self.writeStrengthResults(outputFile)

    def drawSchema(self, pth):
        '''Retaining wall scheme drawing in LaTeX format.
 
        :param pth: path for the output file.
        '''
        file_name_string= path_utils.get_valid_filename(self.name)
        outputFile= open(pth+'schema_'+file_name_string+".tex","w")
        outputFile.write("\\begin{figure}\n")
        outputFile.write("\\begin{center}\n")
        outputFile.write(draw_schema.hdr)
        for l in draw_schema.lines:
            outputFile.write(l)

        defStrings= {}
        self.stemReinforcement.drawSchema(defStrings)
        self.footingReinforcement.drawSchema(defStrings)

        rebarAnno= draw_schema.getRebarAnnotationLines(defStrings)
        for l in rebarAnno:
            outputFile.write(l)
        outputFile.write(draw_schema.tail)
        outputFile.write("\\end{center}\n")
        outputFile.write("\\caption{Wall "+ self.title +" reinforcement scheme} \\label{fg_"+self.name+"}\n")
        outputFile.write("\\end{figure}\n")

    # def createRCSections(self,stemSets):
    #     '''Create reinforced concrete sections.

    #     :param stemSets: sets of elements along the stem.
    #     '''
    #     stemRCSections= self.stemReinforcement.getRCSections(stemSets)
    #     footingRCSection= self.footingReinforcement.getRCSections(footingSet)
    #     if(__debug__):
    #         if(not stemRCSections):
    #             AssertionError('Can\'t stem footing sections.')
    #         if(not footingRCSection):
    #             AssertionError('Can\'t create footing section.')
        
    def createFEProblem(self, title):
        '''Create finite element problem.'''
        self.feProblem= xc.FEProblem()
        self.feProblem.title= title
        return self.feProblem

    def clearFEProblem(self):
        self.modelSpace.clearAll()
        self.feProblem= None
        self.modelSpace= None

    def getFoundationSections(self):
        ''' Create the XC section material for the foundation.
        '''
        rcSection= def_simple_RC_section.RCRectangularSection(name= self.name+"FoundationSection", width= self.b, depth= self.footingThickness, concrType= self.concrete, reinfSteelType= self.footingReinforcement.steel)
        preprocessor= self.modelSpace.preprocessor 
        elasticSection= rcSection.defElasticShearSection2d(preprocessor) #Foundation elements material.
        if(__debug__):
            if(not elasticSection):
                AssertionError('Can\'t create section for foundation.')
        return rcSection, elasticSection
    
    def getStemSections(self, y):
        ''' Create the XC section material for the foundation.

        :param y: distance from the top of the stem.
        '''
        depth= self.getSectionDepth(y)
        name= self.name+"StemSection"+str(y)
        rcSection= def_simple_RC_section.RCRectangularSection(name= name, width= self.b, depth= depth, concrType= self.concrete, reinfSteelType= self.stemReinforcement.steel)

        elasticSection= rcSection.defElasticShearSection2d(self.modelSpace.preprocessor) #Foundation elements material.
        if(__debug__):
            if(not elasticSection):
                AssertionError('Can\'t create section for foundation.')
        return rcSection, elasticSection
        
    def genMesh(self, springMaterials):
        '''Generate finite element mesh.

        :param springMaterials: spring materials for foundation winkler model.
        '''
        preprocessor= self.feProblem.getPreprocessor
        nodeHandler= preprocessor.getNodeHandler
        self.defineWireframeModel(nodeHandler)
        trfs= preprocessor.getTransfCooHandler
        transformationName= self.name+'LinearTrf'
        self.trf= trfs.newLinearCrdTransf2d(transformationName)
        foundationRCSection, foundationXCSection= self.getFoundationSections() # foundation elements XC section material.
        elementSize= 0.2
        seedElemHandler= preprocessor.getElementHandler.seedElemHandler
        seedElemHandler.defaultMaterial= foundationXCSection.name
        seedElemHandler.defaultTransformation= transformationName
        seedElem= seedElemHandler.newElement("ElasticBeam2d")
        if(__debug__):
            if(not seedElem):
                AssertionError('Can\'t create seed element.')
        self.wallSet= preprocessor.getSets.defSet("wallSet")
        self.heelSet= preprocessor.getSets.defSet("heelSet")
        self.toeSet= preprocessor.getSets.defSet("toeSet")
        self.foundationSet= preprocessor.getSets.defSet("foundationSet")
        for lineName in ['heel','toe']:
            l= self.wireframeModelLines[lineName]
            l.setElemSize(elementSize)
            l.genMesh(xc.meshDir.I)
            for e in l.elements:
                e.setProp("baseSection", foundationRCSection)
                self.foundationSet.elements.append(e)
                self.wallSet.elements.append(e)
                if(lineName=='heel'):
                    self.heelSet.elements.append(e)
                else:
                    self.toeSet.elements.append(e)

        self.heelSet.fillDownwards()
        self.toeSet.fillDownwards()
        self.foundationSet.fillDownwards()

        # Stem mesh.
        stemRCSection, stemXCSection= self.getStemSections(self.stemHeight/2.0) # stem elements XC section material.
        self.stemSet= preprocessor.getSets.defSet("stemSet")
        for lineName in ['stem']:
            for l in self.wireframeModelLines[lineName]:
                l.setElemSize(elementSize)
                seedElemHandler.defaultMaterial= stemXCSection.name
                l.genMesh(xc.meshDir.I)
                for e in l.elements:
                    y= -e.getPosCentroid(True).y
                    stemRCSection, stemXCSection= self.getStemSections(y) # stem elements XC section material.
                    e.setMaterial(stemXCSection.name)
                    self.stemSet.elements.append(e)
                    self.wallSet.elements.append(e)
                    e.setProp("baseSection", stemRCSection)
        # Springs on nodes.
        tributaryLengths= sets_mng.get_tributary_lengths(xcSet= self.foundationSet, initialGeometry= False) # tributary areas.
        self.fixedNodes= []
        elasticBearingNodes= self.foundationSet.nodes
        kX= springMaterials[0] #Horizontal
        kSx= kX.E
        kY= springMaterials[1] #Vertical
        kSy= kY.E
        lngTot= 0.0
        for n in elasticBearingNodes:
            lT= tributaryLengths[n.tag]
            lngTot+= lT
            #print("tag= ", n.tag, " lT= ", lT)
            #print("before k= ", kY.E)
            kX.E= kSx*lT
            kY.E= kSy*lT
            fixedNode, newElem= self.modelSpace.setBearing(n.tag,["kX","kY"])
            self.fixedNodes.append(fixedNode)
        self.stemSet.fillDownwards()
        self.wallSet.fillDownwards()
        
    def createLinearElasticFEModel(self, prbName, kS):
        ''' Create a linear elastic FE model.

        :param prbName: name of the finite element problem.
        :param kS: subgrade reaction modulus.
        '''
        retval= self.createFEProblem(prbName)
        preprocessor= retval.getPreprocessor

        #Soils
        kX= typical_materials.defElasticMaterial(preprocessor, "kX",kS/10.0)
        kY= typical_materials.defElasticMaterial(preprocessor, "kY",kS)
        #kY= typical_materials.defElastNoTensMaterial(preprocessor, "kY",kS)
        
        #Mesh.
        self.genMesh([kX,kY])
        return retval

    def getFoundationReinforcement(self):
        ''' Return the reinforcement of the foundation in the form of LongReinfLayers
            objects that will be used the check the reinforcement of the finite
            elements.
        '''
        bottomReinf= self.footingReinforcement.getReinforcement(self.footingReinforcement.bottomFootingIndex)
        if(bottomReinf):
            foundationBottomReinforcement= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.RebarRow2ReinfRow(bottomReinf)])
        else:
            foundationBottomReinforcement= def_simple_RC_section.LongReinfLayers([])
        topReinf= self.footingReinforcement.getReinforcement(self.footingReinforcement.topFootingIndex)
        if(topReinf):
            foundationTopReinforcement= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.RebarRow2ReinfRow(topReinf)])
        else:
            foundationTopReinforcement= def_simple_RC_section.LongReinfLayers([])
        return foundationBottomReinforcement, foundationTopReinforcement

    def getStemReinforcement(self):
        ''' Return the reinforcement of the wall stem in the form of LongReinfLayers
            objects that will be used the check the reinforcement of the finite
            elements.
        '''
        extBottomReinf= self.stemReinforcement.getReinforcement(self.stemReinforcement.extStemBottomIndex)
        extReinf= self.stemReinforcement.getReinforcement(self.stemReinforcement.extStemIndex)
        if(extBottomReinf and extReinf):
            stemExtBottomReinforcement= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.RebarRow2ReinfRow(extBottomReinf), def_simple_RC_section.RebarRow2ReinfRow(extReinf)])
            stemExtReinforcement= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.RebarRow2ReinfRow(extReinf)])
        else:
            stemExtBottomReinforcement= def_simple_RC_section.LongReinfLayers([])
            stemExtReinforcement= def_simple_RC_section.LongReinfLayers([])
        intReinf= self.stemReinforcement.getReinforcement(self.stemReinforcement.intStemIndex)
        if(intReinf):
            stemIntReinforcement= def_simple_RC_section.LongReinfLayers([def_simple_RC_section.RebarRow2ReinfRow(intReinf)])
        else:
            stemIntReinforcement= def_simple_RC_section.LongReinfLayers([])
        return stemExtBottomReinforcement, stemExtReinforcement, stemIntReinforcement
        
    
    def transferReinforcementToFEModel(self):
        ''' Transfer the reinforcement definition in foundationReinforcement 
            and stemReinforcement to the corresponding properties of the
            model elements.
        '''
        reinforcementUpVector= geom.Vector3d(0,1,0) # Y+. This vector defines
                                                    # the meaning of
                                                    # "top reinforcement"
                                                    # or "bottom reinforcement".
        # foundation reinforcement.
        foundationBottomReinforcement, foundationTopReinforcement= self.getFoundationReinforcement()
        for e in self.foundationSet.elements:
            e.setProp("reinforcementUpVector", reinforcementUpVector) # Y+
            e.setProp("bottomReinforcement", foundationBottomReinforcement)
            e.setProp("topReinforcement", foundationTopReinforcement)
        # stem reinforcement.
        reinforcementUpVector= geom.Vector3d(-1,0,0) # X-. This vector defines
                                                    # the meaning of
                                                    # "top reinforcement"
                                                    # or "bottom reinforcement".
        stemExtBottomReinforcement, stemExtReinforcement, stemIntReinforcement= self.getStemReinforcement()
        yExtReinfChange= self.stemReinforcement.getYExtReinfChange()
        for e in self.stemSet.elements:
            y= -e.getPosCentroid(True).y
            e.setProp("reinforcementUpVector", reinforcementUpVector) # X-
            if(y>yExtReinfChange):
                e.setProp("bottomReinforcement", stemExtBottomReinforcement)
            else:
                e.setProp("bottomReinforcement", stemExtReinforcement)
            e.setProp("topReinforcement", stemIntReinforcement)

    def createConcreteSectionDistribution(self):
        ''' Define spatial distribution of reinforced concrete sections from the
            properties assingned to the elements.'''
        self.transferReinforcementToFEModel()
        retval= RC_material_distribution.RCMaterialDistribution()
        retval.assignFromElementProperties(elemSet= self.wallSet.elements)
        return retval
        

    def createSelfWeightLoads(self, rho= 2500, grav= g):
        '''Create the loads of the concrete weight.

        :param rho: density of the concrete material.
        :param grav: gravitational acceleration.
        '''
        retval= geom.SlidingVectorsSystem2d() # With checking purposes only.
        for e in self.wallSet.elements:
            selfWeightLoad= xc.Vector([0.0, -grav*rho*e.sectionProperties.A])
            e.vector2dUniformLoadGlobal(selfWeightLoad)
            pos= e.getPosCentroid(False)
            length= e.getLength(False)
            retval+= geom.SlidingVector2d(geom.Pos2d(pos.x, pos.y), length*geom.Vector2d(selfWeightLoad[0], selfWeightLoad[1]))
        return retval
        
    def createDeadLoad(self, heelFillDepth, toeFillDepth, rho= 2000, grav= g):
        '''Create the loads of earth self weigth.

        :param heelFillDepth: depth of the fill that rests on the wall heel.
        :param toeFillDepth: depth of the fill that rests on the wall toe.
        :param rho: density of the concrete material.
        :param grav: gravitational acceleration.
        '''
        retval= geom.SlidingVectorsSystem2d() # With checking purposes only.
        heelLengthFactor= self.bHeel/self.wireframeModelLines['heel'].getLength()
        heelFillLoad= xc.Vector([0.0, -grav*rho*heelFillDepth*heelLengthFactor])
        for e in self.heelSet.elements:
            e.vector2dUniformLoadGlobal(heelFillLoad)
            pos= e.getPosCentroid(False)
            length= e.getLength(False)
            retval+= geom.SlidingVector2d(geom.Pos2d(pos.x, pos.y), length*geom.Vector2d(heelFillLoad[0], heelFillLoad[1]))
        toeLengthFactor= self.bToe/self.wireframeModelLines['toe'].getLength()
        toeFillLoad= xc.Vector([0.0, -grav*rho*toeFillDepth*toeLengthFactor])
        for e in self.toeSet.elements:
            e.vector2dUniformLoadGlobal(toeFillLoad)
            pos= e.getPosCentroid(False)
            length= e.getLength(False)
            retval+= geom.SlidingVector2d(geom.Pos2d(pos.x, pos.y), length*geom.Vector2d(toeFillLoad[0], toeFillLoad[1]))
        return retval

    def createEarthPressureLoadOnStem(self, pressureModel, vDir= xc.Vector([-1.0,0.0]), Delta= 0.0):
        '''Create the loads of the earth pressure over the stem.

        :param pressureModel: (obj) earth pressure model.
        :param vDir: (xc.Vector) direction for the pressures.
        :param Delta: angle of the earth pressure with respect to the stem back face.
        '''
        return pressureModel.appendLoadToCurrentLoadPattern(iCoo= 1, xcSet= self.stemSet, vDir= vDir, delta= Delta)
    
    def getStemTopNode(self):
        ''' Return the node at the top of the stem.'''
        return self.wireframeModelPoints['stemTop'].getNode()
        
    def getHeelEndNode(self):
        ''' Return the node at the heel end.'''
        return self.wireframeModelPoints['heelEnd'].getNode()

    def getHeelEndNodePosition(self):
        ''' Return the position of the node at the heel end.'''
        return self.getHeelEndNode().getInitialPos2d
    
    def getToeEndNode(self):
        ''' Return the node at the toe end.'''
        return self.wireframeModelPoints['toeEnd'].getNode()

    def getToeEndNodePosition(self):
        ''' Return the position of the node at the toe end.'''
        return self.getToeEndNode().getInitialPos2d

    def createEarthPressureLoadOnHeelEnd(self, pressureModel, Delta= 0.0):
        '''Create the loads of the earth pressure over the vertical face
           at the end of the heel.

        :param pressureModel: (obj) earth pressure model.
        :param Delta: angle of the earth pressure with respect to the stem back face.
        '''
        n= self.wireframeModelPoints['heelEnd'].getNode()
        nodePos= n.getInitialPos2d
        z= nodePos.y
        force= pressureModel.getPressure(z)*self.footingThickness
        loadVector= force*xc.Vector([-math.cos(Delta),-math.sin(Delta),0.0])
        n.newLoad(loadVector)
        return geom.SlidingVector2d(nodePos, geom.Vector2d(loadVector[0], loadVector[1]))

    def createEarthPressureLoadOnToeEnd(self,pressureModel):
        '''Create the loads of the earth pressure over the vertical face
           at the end of the toe.

           :param pressureModel: (obj) earth pressure model.
        '''
        n= self.wireframeModelPoints['toeEnd'].getNode()
        nodePos= n.getInitialPos2d
        z= nodePos.y
        force= pressureModel.getPressure(z)*self.footingThickness
        loadVector= force*xc.Vector([1.0,0.0,0.0])
        n.newLoad(loadVector)
        return geom.SlidingVector2d(nodePos, geom.Vector2d(loadVector[0], loadVector[1]))

    def createBackfillPressures(self, pressureModel, Delta= 0.0, beta= 0):
        '''Create backfill earth pressures over the wall.

        :param pressureModel: (obj) earth pressure model for the backfill.
        :param Delta: angle of the earth pressure with respect to the stem back face.
        :param beta: slope inclination of backfill.
        '''
        if(beta!=0.0): 
            # Compute virtual back.
            virtualBack= self.getVirtualBack(beta= beta, footingIncluded= True)
            virtualBackSVS= pressureModel.getForces2D(virtualBack, beta= beta)
            zeroMomentLine= virtualBackSVS.zeroMomentLine()
            zeroMomentPt= virtualBack.getIntersection(zeroMomentLine)[0]
            R= virtualBackSVS.getResultant()
            H= geom.SlidingVectorsSystem2d(zeroMomentPt, geom.Vector2d(R.x, 0), 0.0) # Horizontal component.
            V= geom.SlidingVectorsSystem2d(zeroMomentPt, geom.Vector2d(0, R.y),0.0) # Vertical component.
            # Distribute horizontal compoent on the stem.
            ptList= list()
            nodeList= list()
            for n in self.stemSet.nodes:
                ptList.append(n.getInitialPos2d)
                nodeList.append(n)
            loadVectors= H.distribute(ptList)
            retval= geom.SlidingVectorsSystem2d(zeroMomentPt, geom.Vector2d(), 0.0)
            for n, v in zip(nodeList, loadVectors):
                f= v.getVector2d()
                retval+= geom.SlidingVector2d(n.getInitialPos2d, f)
                n.newLoad(xc.Vector([f.x,f.y,0.0]))
            # Distribute vertical component on the heel
            ptList= list()
            nodeList= list()
            # heelEndPosition= self.getHeelEndNodePosition()
            # limit= 1/2*self.bHeel
            for n in self.heelSet.nodes:
                nPos= n.getInitialPos2d
                #if(nPos.dist(heelEndPosition)<limit):
                ptList.append(nPos)
                nodeList.append(n)
            loadVectors= V.distribute(ptList)
            for n, v in zip(nodeList, loadVectors):
                f= v.getVector2d()
                retval+= geom.SlidingVector2d(n.getInitialPos2d, f)
                n.newLoad(xc.Vector([f.x,f.y,0.0]))            
        else:
            retval= self.createEarthPressureLoadOnStem(pressureModel, Delta= Delta)
            retval+= self.createEarthPressureLoadOnHeelEnd(pressureModel, Delta= Delta)
        return retval

    def createFrontFillPressures(self, pressureModel,Delta= 0.0):
        '''Create front fill earth pressures over the wall.

        :param pressureModel: (obj) earth pressure model for the backfill.
        :param Delta: angle of the earth pressure with respect to the stem back face.
        '''
        retval= self.createEarthPressureLoadOnStem(pressureModel,xc.Vector([1.0,0.0]),Delta= Delta)
        retval+= self.createEarthPressureLoadOnToeEnd(pressureModel)
        return retval

    def createVerticalLoadOnHeel(self,loadOnBackfill):
        '''Create the loads over the heel dues to a load acting on the backfill.

           :param loadOnBackfill: (obj) load acting on the backfill.
        '''
        heelLengthFactor= self.bHeel/self.wireframeModelLines['heel'].getLength()
        return heelLengthFactor*loadOnBackfill.appendVerticalLoadToCurrentLoadPattern(xcSet= self.heelSet, vDir= xc.Vector([0.0,-1.0]), iXCoo= 0, iZCoo= 1, alph= 0.0)

    def createPressuresFromLoadOnBackfill(self, loadOnBackfill,Delta= 0.0):
        '''Create the pressures on the stem and on the heel dues to 
           a load acting on the backfill.

        :param loadOnBackfill: (obj) load acting on the backfill.
        :param Delta: angle of the earth pressure with respect to the stem back face.
        '''
        self.createEarthPressureLoadOnStem(loadOnBackfill,Delta= Delta) #Pressures on stem.
        self.createEarthPressureLoadOnHeelEnd(loadOnBackfill) #Force on heel end.
        self.createVerticalLoadOnHeel(loadOnBackfill) #Vertical load on heel.

    def createLoadOnTopOfStem(self,loadVector):
        '''Create a load acting on the node at the top of the stem.

           :param loadVector: (vector) vector defining the load.
        '''
        n= self.wireframeModelPoints['stemTop'].getNode()
        n.newLoad(loadVector)
        
    def getMononobeOkabeDryOverpressure(self,backfillModel,kv,kh,delta_ad= 0,beta= 0, Kas= None, g= g):
        ''' Return overpressure due to seismic action according to Mononobe-Okabe

          :param backfillModel: back fill terrain model
          :param kv: seismic coefficient of vertical acceleration.
          :param kh: seismic coefficient of horizontal acceleration.
          :param delta_ad: angle of friction soil - structure.
          :param beta: slope inclination of backfill.
        '''
        H= self.getTotalHeight()
        psi= math.radians(90) #back face inclination of the structure (< PI/2)
        return backfillModel.getMononobeOkabeDryOverpressure(H, kv, kh, psi, delta_ad, beta, Kas,g)/H

    def getReactions(self):
        '''Return the reactions on the foundation.'''
        return get_reactions.Reactions(self.modelSpace.preprocessor,self.fixedNodes)

    def getEccentricity(self,R):
        '''Return the eccenctricity of the loads acting on the retaining wall.

         :param R: (SlidingVectorsSystem3d) resultant of the loads acting on the retaining wall.
        '''
        foundationPlane= self.getFoundationPlane()
        zml= R.zeroMomentLine(1e-5).getXY2DProjection() #Resultant line of action.
        p= foundationPlane.getIntersection(zml)[0] # Intersection with
                                                   # foundation plane.
        foundationCenterPos2D= self.getFoundationCenterPosition()
        return p.x-foundationCenterPos2D.x #eccentricity

    def getFoundationRotation(self):
        '''Returns the rotation of the foundation.'''
        n0= self.wireframeModelPoints['toeEnd'].getNode()
        n1= self.wireframeModelPoints['heelEnd'].getNode()
        b= self.getFootingWidth()
        delta= n1.getDisp[1]-n0.getDisp[1]
        return math.atan(delta/b)

    def getOverturningSafetyFactor(self,R,gammaR):
        '''Return the factor of safety against overturning.

         :param R: (SlidingVectorsSystem3d) resultant of the loads acting on the retaining wall.
         :param gammaR: (float) partial resistance reduction factor.
        '''
        e= self.getEccentricity(R) #eccentricity
        b= self.getFootingWidth()
        if(e>0.0): # Overturning not possible.
            e= -.001*b
        return b/(3*(-e)*gammaR)

    def getSlidingSafetyFactor(self, R, gammaR, foundationSoilModel, ignoreAdhesion= True):
        '''Return the factor of safety against sliding.

        :param R: (SlidingVectorsSystem3d) resultant of the loads acting on the retaining wall.
        :param gammaR: partial resistance reduction factor.
        :param foundationSoilModel: (FrictionalCohesiveSoil) soil model.
        :param ignoreAdhesion: if true don't consider the adhesion of the foundation to the soil.
        '''
        foundationPlane= self.getFoundationPlane()
        alphaAngle= math.atan(foundationPlane.getSlope())
        F= R.getResultant()
        F2D= geom.Vector2d(F.x,F.y)
        Ftang= foundationPlane.getVector2dProj(F2D)
        Fnormal= F2D-Ftang
        #Sliding strength
        # Get the critical state (constant volume) angle of shearing
        # resistance of the soil.
        if(foundationSoilModel.phi_cv): # if explicitly defined.
            phi_cv= foundationSoilModel.phi_cv
        else: # otherwise assume it's equal to the internal friction angle.
            phi_cv= foundationSoilModel.getDesignPhi()
        Rd= Fnormal.getModulus()*math.tan(phi_cv)
        if(self.smoothPrecastFoundation):
            Rd*= 2/3.0
        if(not ignoreAdhesion):
            e= self.getEccentricity(R) #eccentricity
            b= self.getFootingWidth()
            bReduced= 2*(b/2.0+e)
            Rd+= +foundationSoilModel.getDesignC()*bReduced/math.cos(alphaAngle)
        retval= Rd/Ftang.getModulus()/gammaR
        return retval

    def getBearingPressure(self,R):
        ''' Return the bearing pressure.

         :param R: force on the bearing plane.
        '''
        e= self.getEccentricity(R) #eccentricity
        b= self.getFootingWidth()
        bReduced= 2*(b/2.0+e)
        F= R.getResultant()
        sigma= F.y/bReduced
        return sigma
      
    def getBearingPressureSafetyFactor(self, R, foundationSoilModel, toeFillDepth, gammaR= 1.0, q= 0.0, NgammaCoef= 2.0):
        ''' Return the factor of safety against bearing capacity of the soil.

        :param R: force on the bearing plane.
        :param foundationSoilModel: soil model for the Brinch-Hansen analysis.
        :param toeFillDepth: (float) depht of the soil filling over the toe.
        :param gammaR: partial factor for bearing resistance.
        :param q: (float) overburden uniform load over the foundation plane.
        :param NgammaCoef: Coefficient to compute Ngamma value with the
                           Brinch-Hansen formulas (see FrictionalCohesiveSoil
                           class).
        '''
        D= self.getFoundationDepth(toeFillDepth)
        e= self.getEccentricity(R) #eccentricity
        b= self.getFootingWidth()
        bReduced= 2*(b/2.0+e)
        F= R.getResultant()
        sigma= F.y/bReduced
        Leff= 100*self.b # for an infnitely long footing.
        # Overburden load.
        overburdenLoad= q+toeFillDepth*foundationSoilModel.gamma()
        qu= foundationSoilModel.qu(q= overburdenLoad, D= D,Beff= bReduced, Leff= Leff, Vload= F.y, HloadB= F.x, HloadL= 0.0, NgammaCoef= NgammaCoef)/gammaR
        return qu/sigma

    def getAdmissiblePressureSafetyFactor(self,R,sg_adm):
        ''' Return the factor of safety against bearing capacity of the soil.

         :param R: force on the bearing plane.
         :param q_adm: (float) admissible bearing pressure.
        '''
        sigma= self.getBearingPressure(R)
        return sg_adm/sigma

    def getStemYCoordinates(self):
        y= list()
        for e in self.stemSet.elements:
            n1= e.getNodes[0]
            y.append(n1.getInitialPos2d.y)
        n2= e.getNodes[1]
        y.append(n2.getInitialPos2d.y)
        return y

    def getStemInternalForces(self):
        nd= list()
        md= list()
        vd= list()
        for e in self.stemSet.elements:
            nd.append(e.getN1)
            md.append(e.getMz1)
            vd.append(e.getVy1)
        nd.append(e.getN2)
        md.append(e.getMz2)
        vd.append(e.getVy2)
        return nd, md, vd

    def getHeelInternalForces(self):
        ''' Return the minimum axial force and the maximum bending moment 
            and shear force at the wall heel.
        '''
        retvalNd= 0.0
        retvalMd= 0.0
        retvalVd= 0.0
        heelElements= self.heelSet.elements.getPythonList()
        # Values from the first element.
        if(len(heelElements)>0):
            e0= heelElements[0]
            retvalNd= e0.getN1
            if(abs(e0.getN2)>abs(retvalMd)): # max. value (minimum compresion).
                retvalMd= e0.getN2
            retvalMd= e0.getMz1
            if(abs(e0.getMz2)>abs(retvalMd)): # maximum value.
                retvalMd= e0.getMz2
            retvalVd= e0.getVy1
            if(abs(e0.getVy2)>abs(retvalVd)): # maximum value.    
                retvalVd= e0.getVy2
            for e in heelElements[1:]:
                if(abs(e.getN1)>abs(retvalMd)): # max. value (minimum compresion).
                    retvalMd= e.getN1
                if(abs(e.getN2)>abs(retvalMd)): # max. value (minimum compresion).
                    retvalMd= e.getN2
                if(abs(e.getMz1)>abs(retvalMd)): # maximum value.
                    retvalMd= e.getMz1
                if(abs(e.getMz2)>abs(retvalMd)): # maximum value.
                    retvalMd= e.getMz2
                if(abs(e.getVy1)>abs(retvalVd)): # maximum value.    
                    retvalVd= e.getVy1
                if(abs(e.getVy2)>abs(retvalVd)): # maximum value.    
                    retvalVd= e.getVy2
        return retvalNd, retvalMd, retvalVd

    def resultComb(self,nmbComb):
        '''Solution and result retrieval routine.'''
        preprocessor= self.feProblem.getPreprocessor   
        self.modelSpace.removeAllLoadPatternsFromDomain()
        preprocessor.resetLoadCase()
        preprocessor.getLoadHandler.addToDomain(nmbComb)
        #Solution
        solution= predefined_solutions.SimpleStaticLinear(self.feProblem)
        result= solution.solve()
        if(result!=0):
            className= type(self).__name__
            methodName= sys._getframe(0).f_code.co_name
            lmsg.error(className+'.'+methodName+'; could not solve for combination: '+str(nmbComb))
        reactions= self.getReactions()
        #preprocessor.getLoadHandler.removeFromDomain(nmbComb)
        return reactions

    def performStabilityAnalysis(self,combinations, foundationSoilModel, toeFillDepth, sg_adm= None, ignoreAdhesion= True, NgammaCoef= 2.0): 
        ''' Perform stability limit state analysis.

        :param combinations: load combinations to use in the analysis.
        :param foundationSoilModel: model of the foundation soil.
        :param toeFillDepth: depth of the fill that rests on the wall toe.
        :param sg_adm: admissible stress of the terrain (optional).
        :param ignoreAdhesion: if true don't consider the adhesion of the foundation to the soil for sliding resistance.
        :param NgammaCoef: Coefficient to compute Ngamma value with the
                           Brinch-Hansen formulas (see FrictionalCohesiveSoil
                           class). See also D.4 in EC7 part 1.
         '''
        self.stability_results= WallStabilityResults(self, combinations, foundationSoilModel= foundationSoilModel, toeFillDepth= toeFillDepth, sg_adm= sg_adm, ignoreAdhesion= ignoreAdhesion, NgammaCoef= NgammaCoef)
        return self.stability_results

    def performGEOVerifications(self,combinations, foundationSoilModel, toeFillDepth, gammaRSliding, gammaRBearing, sg_adm= None, ignoreAdhesion= True, NgammaCoef= 2.0): 
        ''' Perform stability limit state analysis.

        :param combinations: load combinations to use in the analysis.
        :param foundationSoilModel: model of the foundation soil.
        :param toeFillDepth: depth of the fill that rests on the wall toe.
        :param gammaRSliding: partial factor for sliding resistance.
        :param gammaRBearing: partial factor for bearing resistance.
        :param sg_adm: admissible stress of the terrain (optional).
        :param ignoreAdhesion: if true don't consider the adhesion of the foundation to the soil for sliding resistance.
        '''
        self.stability_results= WallStabilityResults(self, combinations, foundationSoilModel, toeFillDepth= toeFillDepth, sg_adm= sg_adm, gammaRSliding= gammaRSliding, gammaRBearing= gammaRBearing, ignoreAdhesion= ignoreAdhesion, NgammaCoef= NgammaCoef)
        return self.stability_results

    def getEnvelopeInternalForces(self, envelopeNd, envelopeMd, envelopeVd, envelopeNdHeel, envelopeMdHeel, envelopeVdHeel):
        nd, md, vd= self.getStemInternalForces()
        envelopeNd.update(nd)
        envelopeMd.update(md)
        envelopeVd.update(vd)
        ndHeel, mdHeel, vdHeel= self.getHeelInternalForces()
        envelopeNdHeel.update([ndHeel])
        envelopeMdHeel.update([mdHeel])
        envelopeVdHeel.update([vdHeel])
        return envelopeNd, envelopeMd, envelopeVd, envelopeNdHeel, envelopeMdHeel, envelopeVdHeel 

    def performSLSAnalysis(self,combinations):
        ''' Perform serviceability limit state analysis.

        :param combinations: load combinations to use in the analysis.
        '''
        rotation= 1e15
        rotationComb= ''
        y= self.getStemYCoordinates()
        envelopeNd= Envelope(yValues= y)
        envelopeMd= Envelope(yValues= y)
        envelopeVd= Envelope(yValues= y)
        envelopeNdHeel= Envelope(yValues= [0.0])
        envelopeMdHeel= Envelope(yValues= [0.0])
        envelopeVdHeel= Envelope(yValues= [0.0])
        for comb in combinations:
            reactions= self.resultComb(comb)
            if(__debug__):
                if(not reactions):
                    AssertionError('Can\'t obtain the reactions.')
            envelopeNd, envelopeMd, envelopeVd, envelopeNdHeel, envelopeMdHeel, envelopeVdHeel= self.getEnvelopeInternalForces(envelopeNd, envelopeMd, envelopeVd, envelopeNdHeel, envelopeMdHeel, envelopeVdHeel)
            rot= self.getFoundationRotation()
            if(rot<rotation):
                rotation= rot
                rotationComb= comb
        internalForces= InternalForces(y,envelopeNd, envelopeMd, envelopeVd, envelopeNdHeel, envelopeMdHeel, envelopeVdHeel)
        self.sls_results= WallSLSResults(internalForces,rotation, rotationComb)
        return self.sls_results

    def performULSAnalysis(self,combinations):
        ''' Perform ultimate limit state analysis.

        :param combinations: load combinations to use in the analysis.
        '''
        y= self.getStemYCoordinates()
        envelopeNd= Envelope(yValues= y)
        envelopeMd= Envelope(yValues= y)
        envelopeVd= Envelope(yValues= y)
        envelopeNdHeel= Envelope(yValues= [0.0])
        envelopeMdHeel= Envelope(yValues= [0.0])
        envelopeVdHeel= Envelope(yValues= [0.0])
        for comb in combinations:
            reactions= self.resultComb(comb)
            if(__debug__):
                if(not reactions):
                    AssertionError('Can\'t obtain the reactions.')
            envelopeNd, envelopeMd, envelopeVd, envelopeNdHeel, envelopeMdHeel, envelopeVdHeel= self.getEnvelopeInternalForces(envelopeNd, envelopeMd, envelopeVd, envelopeNdHeel, envelopeMdHeel, envelopeVdHeel)
        internalForces= InternalForces(y, envelopeNd, envelopeMd, envelopeVd, envelopeNdHeel, envelopeMdHeel, envelopeVdHeel)
        self.uls_results= WallULSResults(internalForces)
        return self.uls_results
