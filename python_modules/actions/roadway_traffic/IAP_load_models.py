# -*- coding: utf-8 -*-
from __future__ import division

'''Roadway traffic load model definition according to spanish IAP standard.
'''

__author__= "Ana Ortega (AO_O) "
__copyright__= "Copyright 2015, AO_O "
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com"

import xc_base
import geom
import xc
from actions.roadway_traffic import load_model_base as lmb
from actions import load_cases as lcases
from actions import loads

# pos: (transversal,longitudinal)
IAP_carril_virt1=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=150e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=150e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=150e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=150e3,lx=0.4,ly=0.4)])

IAP_carril_virt2=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=100e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=100e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=100e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=100e3,lx=0.4,ly=0.4)])

IAP_carril_virt3=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=50e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=50e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=50e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=50e3,lx=0.4,ly=0.4)])

#Carro IAP concomitante con frenado (0.75*Q)
IAP_carril_virt1_fren=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=112.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=112.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=112.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=112.5e3,lx=0.4,ly=0.4)])

IAP_carril_virt2_fren=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=75e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=75e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=75e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=75e3,lx=0.4,ly=0.4)])

IAP_carril_virt3_fren=lmb.LoadModel(wLoads=
[lmb.WheelLoad(pos=geom.Pos2d(1.0,-0.6),ld=37.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,-0.6),ld=37.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(1.0,0.6),ld=37.5e3,lx=0.4,ly=0.4),
lmb.WheelLoad(pos=geom.Pos2d(-1.0,0.6),ld=37.5e3,lx=0.4,ly=0.4)])


def IAP_traffic_LC(lcName,deckSet,virtLane1Set,xyCentPL1,hDistrPL,slopeDistrPL=1.0,vQbraking=None,virtLane2Set=None,xyCentPL2=None,virtLane3Set=None,xyCentPL3=None,restDrivewaySet=None,sidewalkSet=None):
    '''Return a traffic load case according to IAP.

    :param lcName: load case name
    :param deckSet: deck set of elements (used to distribute point loads)
    :param virtLane1Set: virtual lane 1 set of elements
    :param xyCentPL1: [xCent,yCent] global coord. X,Y where to place the centroid of the 
                     vehicle on virtual lane 1
    :param hDistrPL: height considered to distribute each point load with
                  slope slopeDistr 
    :param slopeDistrPL: slope (H/V) through hDistr to distribute the load of 
                  a wheel (defaults to 1)
    :param vQbraking: [qfrenX,qfrenY] x, y components of the uniform load due to braking
                      on virtual lane 1 (defaults to None)
    :param virtLane2Set: virtual lane 2 set of elements
    :param xyCentPL2: [xCent,yCent] global coord. X,Y where to place the centroid of the 
                     vehicle on virtual lane 2
    :param virtLane3Set: virtual lane 3 set of elements (defaults to None, not considered)
    :param xyCentPL3: [xCent,yCent] global coord. X,Y where to place the centroid of the 
                     vehicle on virtual lane 3 (defaults to None)
    :param restDrivewaySet: rest of driveway set of elements (defaults to None, not considered)
    :param sidewalkSet: sidewalk set of elements (defaults to None, not considered)
 
    '''
    preprocessor=deckSet.getPreprocessor
    if vQbraking:
        qunifVL1,qunifVL2,qunifVL3,qunifRest,qunifSidewalk=[0.4*9e3]+4*[0.4*2.5e3] #uniform load on virtual lanes, rest of driveway and sidewalks when braking forceis considered
        QpointVL1,QpointVL2,QpointVL3=[IAP_carril_virt1_fren,IAP_carril_virt2_fren,IAP_carril_virt3_fren]
    else:
        qunifVL1,qunifVL2,qunifVL3,qunifRest,qunifSidewalk=[9e3]+4*[2.5e3]    
        QpointVL1,QpointVL2,QpointVL3=[IAP_carril_virt1,IAP_carril_virt2,IAP_carril_virt3]
    lc=lcases.LoadCase(preprocessor,lcName,"default","constant_ts")
    lc.create()
    # add uniform loads
    lc.addLstLoads([loads.UniformLoadOnSurfaces(name= lcName+'quVL1',xcSet=virtLane1Set,loadVector=xc.Vector([0,0,-qunifVL1,0,0,0]),refSystem='Global')])
    if virtLane2Set:
        lc.addLstLoads([loads.UniformLoadOnSurfaces(name= lcName+'quVL2',xcSet=virtLane2Set,loadVector=xc.Vector([0,0,-qunifVL2,0,0,0]),refSystem='Global')])
    if virtLane3Set:
        lc.addLstLoads([loads.UniformLoadOnSurfaces(name= lcName+'quVL3',xcSet=virtLane3Set,loadVector=xc.Vector([0,0,-qunifVL3,0,0,0]),refSystem='Global')])
    if restDrivewaySet:
        lc.addLstLoads([loads.UniformLoadOnSurfaces(name= lcName+'quVL3',xcSet=restDrivewaySet,loadVector=xc.Vector([0,0,-qunifRest,0,0,0]),refSystem='Global')])
    if sidewalkSet:
        lc.addLstLoads([loads.UniformLoadOnSurfaces(name= lcName+'quVL3',xcSet=sidewalkSet,loadVector=xc.Vector([0,0,-qunifSidewalk,0,0,0]),refSystem='Global')])
    #add point loads
    lc.addLstLoads([lmb.VehicleDistrLoad(name=lcName+'QpVL1',xcSet=deckSet,loadModel=QpointVL1, xCentr=xyCentPL1[0],yCentr=xyCentPL1[1],hDistr=hDistrPL,slopeDistr=slopeDistrPL)])
    if virtLane2Set:
        lc.addLstLoads([lmb.VehicleDistrLoad(name=lcName+'QpVL2',xcSet=deckSet,loadModel=QpointVL2, xCentr=xyCentPL2[0],yCentr=xyCentPL2[1],hDistr=hDistrPL,slopeDistr=slopeDistrPL)])
    if virtLane3Set:
        lc.addLstLoads([lmb.VehicleDistrLoad(name=lcName+'QpVL3',xcSet=deckSet,loadModel=QpointVL3, xCentr=xyCentPL3[0],yCentr=xyCentPL3[1],hDistr=hDistrPL,slopeDistr=slopeDistrPL)])
    #add braking load
    if vQbraking:
        lc.addLstLoads([loads.UniformLoadOnSurfaces(name= lcName+'qfren',xcSet=virtLane1Set,loadVector=xc.Vector([vQbraking[0],vQbraking[1],0,0,0,0]),refSystem='Global')])
    return lc

            
def dead_LC(lcName,drivewaySet,qAsphalt,sidewalkSet=None,qSidewalk=None,barrierSet=None,qBarrier=None,deckEdgeSet=None,qDeckEdge=None):
    '''Return the dead load case (asphalt, sidewalk, barriers, ...)

    :param lcName: load case name
    :param drivewaySet: driveway set of elements (where to apply asphalt load)
    :param qAsphalt: uniform load due to asphalt weight
    :param sidewalkSet:sidewalk set of elements (defaults to None)
    :param qSidewalk: uniform surface dead load on sidewalks (defaults to None).
    :param barrierSet: set of lines where to apply barrier linear loads (defaults to None).
    :param qBarrier: uniform linear load due to barrier weight (defaults to None)
    :param deckEdgeSet: set of lines in the edges of the deck (defaults to None).
    :param qDeckEdge: uniform linear load to apply on the edges of the deck (defaults to None).
    '''
    preprocessor=drivewaySet.getPreprocessor
    lc=lcases.LoadCase(preprocessor,lcName,"default","constant_ts")
    lc.create()
    # add surface loads
    lc.addLstLoads([loads.UniformLoadOnSurfaces(name= lcName+'asph',xcSet=drivewaySet,loadVector=xc.Vector([0,0,-qAsphalt,0,0,0]),refSystem='Global')])
    if sidewalkSet:
        lc.addLstLoads([loads.UniformLoadOnSurfaces(name= lcName+'sidew',xcSet=sidewalkSet,loadVector=xc.Vector([0,0,-qSidewalk,0,0,0]),refSystem='Global')])
    #add linear loads
    if barrierSet:
        lc.addLstLoads([loads.UniformLoadOnLines(name= lcName+'barr', xcSet=barrierSet, loadVector=xc.Vector([0,0,-qBarrier,0,0,0]))])
    if deckEdgeSet:
        lc.addLstLoads([loads.UniformLoadOnLines(name= lcName+'deckEdg', xcSet=deckEdgeSet, loadVector=xc.Vector([0,0,-qDeckEdge,0,0,0]))])
    return lc
    
def thermal_rheological_LC(lcName,lstUnifThStrnData):
    '''Return a thermal or rheological load case (shrinkage, creep, 
       thermal expansion, ...)

    :param lcName: load case name
    :param lstUnifThStrnData: list of instances of 'unifStrain' or 
                  'unifThermalStrain' classes that defines the data
                  parameters (set, DOF, strain, temperature, ...) 

    '''
    preprocessor=lstUnifThStrnData[0].elemSet.getPreprocessor
    lc=lcases.LoadCase(preprocessor,lcName,"default","constant_ts")
    lc.create()
    for sd in lstUnifThStrnData:
        lc.addLstLoads([loads.StrainLoadOnShells(name=lcName+sd.elemSet.name,xcSet=sd.elemSet,DOFstrain=sd.DOF,strain=sd.strain)])
    return lc
    
def gradient_thermal_LC(lcName,lstGradThStrnData):
    '''Return a thermal or rheological load case (shrinkage, creep, 
       thermal expansion, ...)

    :param preprocessor: preprocessor
    :param lcName: load case name
    :param lstGradThStrnData: list of instances of gradThermalStrain class
                  that defines the data parameters (set, element thickness,
                  DOF, alpha, temperatures on top and bottom faces)
    '''
    preprocessor=lstGradThStrnData[0].elemSet.getPreprocessor
    lc=lcases.LoadCase(preprocessor,lcName,"default","constant_ts")
    lc.create()
    for sd in lstGradThStrnData:
        lc.addLstLoads([loads.StrainGradientThermalLoadOnShells(name=lcName+sd.elemSet.name,elemSet=sd.elemSet,elThick=sd.elThick,DOF=sd.DOF,alpha=sd.alpha,Ttop=sd.Ttop,Tbottom=sd.Tbottom)])
    return lc
 
def wind_LC(lcName,deckLineSet,vectWindDeck,windwardPileSet,vectWindwardPile,leewardPileSet=None,vectLeewardPile=None):
    '''Return the dead load case (asphalt, sidewalk, barriers, ...)

    :param lcName: load case name
    :param deckLineSet: set of lines to apply wind on deck.
    :param vectWindDeck: components [vx,vy] of the  uniform linear load due to wind on deck.
    :param windwardPileSet:set of lines to apply on piles windward.
    :param vectWindwardPile: components [vx,vy] of the  uniform linear load due to wind on piles (windward side).
    :param leewardPileSet:set of lines to apply wind on pilesleeward (defaults to None).
    :param vectLeewardPile: components [vx,vy] of the  uniform linear load due to wind on piles (leeward side) (defaults to None).
    '''
    preprocessor=deckLineSet.getPreprocessor
    lc=lcases.LoadCase(preprocessor,lcName,"default","constant_ts")
    lc.create()
    #add linear loads
    lc.addLstLoads([loads.UniformLoadOnLines(name= lcName+'deck', xcSet=deckLineSet, loadVector=xc.Vector([vectWindDeck[0],vectWindDeck[1],0,0,0,0]))])
    lc.addLstLoads([loads.UniformLoadOnLines(name= lcName+'pilewindward', xcSet=windwardPileSet, loadVector=xc.Vector([vectWindwardPile[0],vectWindwardPile[1],0,0,0,0]))])
    if leewardPileSet:
        lc.addLstLoads([loads.UniformLoadOnLines(name= lcName+'pileleeward', xcSet=leewardPileSet, loadVector=xc.Vector([vectLeewardPile[0],vectLeewardPile[1],0,0,0,0]))])
    return lc
