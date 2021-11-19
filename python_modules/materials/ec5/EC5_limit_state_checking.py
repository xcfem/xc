# -*- coding: utf-8 -*-
''' Classes and functions for limit state checking according to Eurocode 5. '''

from __future__ import print_function
from __future__ import division

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2016, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.ortega@ciccp.es "

import sys
from misc_utils import log_messages as lmsg
from materials.sections import section_properties
from postprocess import def_vars_control
from postprocess import callback_controls
from postprocess import prop_statistics as ps


class EC5TimberRectCrossSection(section_properties.RectangularSection):
    '''Timber section with Eurocode 5 verification routines.'''
    def __init__(self,name,b,h,E,nu,fyd,taud):
        super(EC5TimberRectCrossSection,self).__init__(name,b,h,E,nu)
        self.fyd= fyd
        self.taud= taud

    def setupULSControlVars2d(self,elems):
        '''For each element creates the variables
           needed to check ultimate limit state criterion to satisfy.'''
        def_vars_control.defVarsControlTensRegElastico2d(elems)
        W= self.Wzel()
        for e in elems:
            e.setProp("fyd",self.fyd)
            e.setProp("fydV",self.taud)
            e.setProp("Wel",W)

    def setupULSControlVars3d(self,elems):
        '''For each element creates the variables
           needed to check ultimate limit state criterion to satisfy.'''
        def_vars_control.defVarsControlTensRegElastico3d(elems)
        Wz= self.Wzel()
        Wy= self.Wyel()
        for e in elems:
            e.setProp("fyd",self.fyd)
            e.setProp("fydV",self.taud)
            e.setProp("Wyel",Wy)
            e.setProp("AreaQy",0.9*self.A())
            e.setProp("Wzel",Wz)
            e.setProp("AreaQz",self.A()-e.getProp("AreaQy"))

    def installElementElasticStressesControlRecorder(self,recorderName, elemSet):
        preprocessor= elemSet.owner.getPreprocessor
        nodes= preprocessor.getNodeHandler
        domain= preprocessor.getDomain
        recorder= domain.newRecorder(recorderName,None)
        recorder.setElements(elemSet.getTags())
        if(nodes.numDOFs==3):
            self.setupULSControlVars2d(elemSet)
            recorder.callbackRecord= callback_controls.controTensRecElastico2d()
        else:
            self.setupULSControlVars3d(elemSet)
            recorder.callbackRecord= callback_controls.controTensRecElastico3d()

        recorder.callbackRestart= "print(\"Restart method called.\")"
        return recorder

def printResultsELU(elems,crossSection):
    '''print(ULS results.)'''
    fmt= "{:6.1f}"
    fmt2= "{:5.2f}"
    e= ps.getItemWithMaxProp(elems,"getProp",'FCTNCP')
    N= fmt.format(e.getProp("NCP")/1e3)
    My= fmt.format(e.getProp("MyCP")/1e3)
    Mz= fmt.format(e.getProp("MzCP")/1e3)
    sgMax= fmt.format(e.getProp("SgMax")/1e6)
    sgMin= fmt.format(e.getProp("SgMin")/1e6)
    sgAdm= fmt.format(crossSection.fyd/1e6)
    fctnCP= fmt2.format(e.getProp("FCTNCP"))
    print("tag= ", e.tag, " N= ", N , "kN  My= ", My , "kN.m  Mz= ", Mz , "kN-m   SgMax= ", sgMax , "MPa  SgMin= ", sgMin, "MPa  sgAdm= ", sgAdm , "MPa  FCTNCP= ", fctnCP , "HIPCPTN= ",e.getProp("HIPCPTN"))
    e=  ps.getItemWithMaxProp(elems,"getProp",'FCVCP')
    Vy= fmt.format(e.getProp("VyCP")/1e3)
    Vz= fmt.format(e.getProp("VzCP")/1e3)
    tauMax=  fmt.format(e.getProp("TauMax")/1e6)
    tauAdm= fmt.format(crossSection.taud/1e6)
    fcvCP= fmt2.format(e.getProp("FCVCP"))
    print("tag= ", e.tag, " Vy= ", Vy , "kN  Vz= ", Vz , "kN  TauMax= ", tauMax , "MPa tauAdm= ", tauAdm, "MPa FCVCP= ", fcvCP , " HIPCPV= ", e.getProp("HIPCPV"))

# Annex B (informative) Vibrations caused by pedestrians.

## B.2 Vertical vibrations.
def getVerticalAccel1Person(f_vert:float, M:float, dampingRatio:float):
    ''' Return the vertical acceleration of the bridge according to
        paragraph (1) of clause B.2 of EC5 annex B.

    :param f_vert: fundamental natural frequency for vertical 
                   deformation of the bridge.
    :param M: total mass of the bridge.
    :param dampingRatio: damping ratio.
    '''
    retval= 100/M/dampingRatio
    if(f_vert<=2.5): # Expression (B.1)
        retval*= 2
    elif(f_vert>5): # out of range
        funcName= sys._getframe(0).f_code.co_name
        lmsg.warning(funcName+'; frequency: '+str(f_vert)+' out of range [0,5] Hz. Normally a higer frequency means that there is no vibration problem.')
        retval= 0.0
    return retval
        

def kvert(f_vert:float):
    ''' Return the value of the k_vert coefficient according to figure
        B.1 of EC5 annex B.

    :param f_vert: fundamental natural frequency for vertical 
                   deformation of the bridge.
    '''
    retval= 1
    if(f_vert<=1):
        retval= 0.33
    elif(f_vert<=1.5):
        retval= 1.34*(f_vert-1)+0.33
    elif(f_vert<=5.0):
        retval= 1-0.4*(f_vert-2.5)
    else:
        retval= 0.0
    return retval
        
def getVerticalAccelNPersons(n:int, f_vert:float, M:float, dampingRatio:float):
    ''' Return the vertical acceleration of the bridge according to
        paragraph (2) of clause B.2 of EC5 annex B.

    :param n: number of pedestrians.
    :param f_vert: fundamental natural frequency for vertical 
                   deformation of the bridge.
    :param M: total mass of the bridge.
    :param dampingRatio: damping ratio.
    '''
    a_vert1= getVerticalAccel1Person(f_vert, M, dampingRatio)
    k_vert= kvert(f_vert)
    return 0.23*a_vert1*n*k_vert # Expression (B.2)

def getVerticalAccel1Runner(f_vert:float, M:float, dampingRatio:float):
    ''' Return the vertical acceleration of the bridge according to
        paragraph (3) of clause B.2 of EC5 annex B.

    :param f_vert: fundamental natural frequency for vertical 
                   deformation of the bridge.
    :param M: total mass of the bridge.
    :param dampingRatio: damping ratio.
    '''
    if(f_vert>=2.5 and f_vert<=3.5): # Expression (B.3)
        retval*= 600/M/dampingRatio
    else: # out of range
        funcName= sys._getframe(0).f_code.co_name
        lmsg.warning(funcName+'; frequency: '+str(f_vert)+' out of range [2.5,3.5] Hz. Normally a higer frequency means that there is no vibration problem.')
        retval= 0.0
    return retval


## B.3 Horizontal vibrations.
def getHorizontalAccel1Person(f_hor:float, M:float, dampingRatio:float):
    ''' Return the horizontal acceleration of the bridge according to
        paragraph (1) of clause B.3 of EC5 annex B.

    :param f_hor: fundamental natural frequency for horizontal 
                   deformation of the bridge.
    :param M: total mass of the bridge.
    :param dampingRatio: damping ratio.
    '''
    retval= 50/M/dampingRatio # Expression B.4
    if(f_hor>2.5): # out of range
        funcName= sys._getframe(0).f_code.co_name
        lmsg.warning(funcName+'; frequency: '+str(f_hor)+' out of range [0,2.5] Hz. Normally a higer frequency means that there is no vibration problem.')
        retval= 0.0
    return retval

def khor(f_hor:float):
    ''' Return the value of the k_hor coefficient according to figure
        B.2 of EC5 annex B.

    :param f_hor: fundamental natural frequency for horizontal 
                   deformation of the bridge.
    '''
    retval= 1
    if(f_hor>1.25 and f_hor<2.5):
        retval= 1-0.8*(f_hor-1.25)
    else:
        retval= 0.0
    return retval

def getHorizontalAccelNPersons(n:int, f_hor:float, M:float, dampingRatio:float):
    ''' Return the horizontal acceleration of the bridge according to
        paragraph (2) of clause B.2 of EC5 annex B.

    :param n: number of pedestrians.
    :param f_hor: fundamental natural frequency for horizontal 
                   deformation of the bridge.
    :param M: total mass of the bridge.
    :param dampingRatio: damping ratio.
    '''
    a_hor1= getHorizontalAccel1Person(f_hor, M, dampingRatio)
    k_hor= khor(f_hor)
    return 0.18*a_hor1*n*k_hor # Expression (B.2)
