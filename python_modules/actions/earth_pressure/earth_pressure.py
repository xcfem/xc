# -*- coding: utf-8 -*-
'''Calculation of lateral earth pressures on vertical walls (retaining, 
basement, earth support system, ...) exerced by a given back soil with 
or without water level

Calculation of lateral earth pressures on vertical walls (retaining, 
basement, earth support system, ...) due to different kind of loads 
on the backfill.
'''

from __future__ import division
from __future__ import print_function

__author__= "Ana Ortega (AO_O) and Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015, AO_O and LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com ana.Ortega.Ort@gmail.com" 

import sys
import math
import xc
import geom
from misc_utils import log_messages as lmsg
from geotechnics import mononobe_okabe
from geotechnics import iskander_method
from model.sets import sets_mng as sets
import bisect

class PressureModelBase(object):
    '''Base class for objects defining earth pressures.'''
    
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global 
           coordinate z.
        '''
        className= type(self).__name__
        methodName= sys._getframe(0).f_code.co_name
        lmsg.error(className+'.'+methodName+'; error: getPressure must be overloaded in derived classes.')
        return 0.0

    def getForces2D(self, segment2d, numDiv= 10, beta= 0.0):
        ''' Return the sliding vector system which is equivalent to the 
            pressures acting on the surface represented by the 2D segment
            argument.

        :param segment2d: two-dimensional segment that represents the surface
                          under pressure.
        :param numDiv: number of sample points along the segment.
        :param beta: slope inclination of backfill.
        '''
        points= segment2d.Divide(numDiv)
        divLength= segment2d.getLength()/numDiv
        # Compute sample points.
        samplePoints= list()
        p0= points[0]
        for p in points[1:]:
            v= (p-p0)*0.5 # vector to division midpoint.
            samplePoints.append(p0+v) # append midpoint.
            p0= p
        dirVector= geom.Vector2d(-math.cos(beta), -math.sin(beta)) # direction of the pressure.
        tributaryArea= divLength
        # Compute sliding vector system.
        retval= geom.SlidingVectorsSystem2d()
        for p in samplePoints:
            pressure= self.getPressure(z= p.y)
            forceVector= geom.SlidingVector2d(p, pressure*tributaryArea*dirVector)
            retval+= forceVector
        return retval

    def appendLoadToCurrentLoadPattern(self, xcSet, vDir,iCoo= 2,delta= 0.0):
        '''Append earth thrust on a set of elements to the current
        load pattern.

        :param xcSet: set that contains the elements (shells and/or beams)
        :param vDir: unit xc vector defining pressures direction
        :param iCoo: index of the coordinate that represents depth.
        :param delta: soil-wall friction angle (usually: delta= 2/3*Phi).
        '''
        tanDelta= math.tan(delta)
        tanVector= xc.Vector([-vDir[1],vDir[0]]) #iCoo= 1 => 2D
        if(iCoo==2): #3D
            tanVector= xc.Vector([vDir[2],vDir[1],-vDir[0]])
        if(len(vDir)==3): #3D load.
            retval= geom.SlidingVectorsSystem3d()  # checking purposes.
            for e in xcSet.elements:
                centroid= e.getCooCentroid(False)
                presElem= self.getPressure(centroid[iCoo])
                loadVector= presElem*(vDir+tanDelta*tanVector)
                if(presElem!=0.0):
                    e.vector3dUniformLoadGlobal(loadVector)
                    area= e.getArea(False)
                    totalLoad= loadVector*area
                    retval+= geom.SlidingVector3d(geom.Pos3d(centroid[0], centroid[1], centroid[2]), geom.Vector3d(totalLoad[0], totalLoad[1], totalLoad[2]))
        else: #2D load.
            retval= geom.SlidingVectorsSystem2d()  # checking purposes.
            for e in xcSet.elements:
                centroid= e.getCooCentroid(False)
                presElem= self.getPressure(centroid[iCoo])
                loadVector= presElem*(vDir+tanDelta*tanVector)
                if(presElem!=0.0):
                    e.vector2dUniformLoadGlobal(loadVector)
                    length= e.getLength(False)
                    totalLoad= loadVector*length
                    retval+= geom.SlidingVector2d(geom.Pos2d(centroid[0], centroid[1]), geom.Vector2d(totalLoad[0], totalLoad[1]))
        return retval # Sliding vector system (checking purposes)
    
    def appendVerticalLoadToCurrentLoadPattern(self, vLoad, xcSet, vDir, iXCoo= 0,iZCoo= 2, alph= math.radians(30)):
        '''Append to the current load pattern the vertical pressures on 
           a set of elements due to the strip load. According to
           11.3.4 in the book "Mecánica de suelos" of Llano, J.J.S.
           isbn= 9788471461650 (https://books.google.ch/books?id=oQFZRKlix\_EC)

        :param vLoad: vertical load.
        :param xcSet: set that contains the elements.
        :param vDir: unit xc vector defining pressures direction.
        :param iXCoo: index of the horizontal coordinate.
        :param iZCoo: index of the vertical coordinate.
        :param alph: angle of stress spreading.
        '''
        sigma_v= vLoad
        if(sigma_v!= 0.0):
            loadVector= sigma_v*vDir
            if(len(vDir)==3): #3D load.
                retval= geom.SlidingVector3d()
                for e in xcSet.elements:
                    centroid= e.getCooCentroid(False)
                    area= e.getArea(False)
                    totalLoad= loadVector*area
                    e.vector3dUniformLoadGlobal(loadVector)
                    retval+= geom.SlidingVector3d(geom.Pos3d(centroid[0], centroid[1], centroid[2]), geom.Vector3d(totalLoad[0], totalLoad[1], totalLoad[2]))
            else: #2D load.
                retval= geom.SlidingVector2d()
                for e in xcSet.elements:
                    centroid= e.getCooCentroid(False)
                    length= e.getLength(False)
                    totalLoad= loadVector*length
                    e.vector2dUniformLoadGlobal(loadVector)
                    retval+= geom.SlidingVector2d(geom.Pos2d(centroid[0], centroid[1]), geom.Vector2d(totalLoad[0], totalLoad[1]))
        return retval

class UniformPressureOnBackfill(PressureModelBase):
    '''Parameters to define a uniform pressure over the backfill surface
        type earth pressure

    :ivar zGround: global Z coordinate of ground level
    :ivar zBottomSoils: list of global Z coordinates of the bottom level
          for each soil (from top to bottom)
    :ivar KSoils: list of pressure coefficients for each soil (from top 
          to bottom)
    :ivar qUnif: uniform load over the backfill surface (defaults to 0)
    '''
    def __init__(self, zGround, zBottomSoils, KSoils, qUnif, xcSet= None, vDir= None):
        ''' Constructor.

        :param zGround: global Z coordinate of ground level
        :param zBottomSoils: list of global Z coordinates of the bottom level
              for each soil (from top to bottom)
        :param KSoils: list of pressure coefficients for each soil (from top 
              to bottom)
        :param qUnif: uniform load over the backfill surface (defaults to 0)
        :ivar xcSet: set that contains the elements (shells and/or beams)
        :ivar vDir: unit xc vector defining pressures direction
        '''
        super(UniformPressureOnBackfill,self).__init__()
        self.zGround= zGround
        self.zBottomSoils= zBottomSoils
        self.zTopLev= [zGround]+zBottomSoils
        self.KSoils= KSoils
        self.qUnif=qUnif
        self.xcSet=xcSet
        self.vDir=vDir

    def getLayerIndex(self, z):
        ''' Return the soil layer that corresponds to z.

        :param z: global z coordinate.
        '''
        self.zTopLev.reverse()
        retval=len(self.zTopLev)-bisect.bisect_left(self.zTopLev,z)-1
        self.zTopLev.reverse()
        return retval
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.
        :param z: global z coordinate.
        '''
        ret_press= 0.0
        if z <= self.zGround:
            ind= self.getLayerIndex(z) # index of the soil layer that correspond to z.
            ret_press= self.KSoils[ind]*self.qUnif # Compute pressure.
        # else:
        #     className= type(self).__name__
        #     methodName= sys._getframe(0).f_code.co_name
        #     lmsg.warning(className+'.'+methodName+'; asking for pressure of point above zGround (z= '+str(z)+', zGround= '+str(self.zGround)+')')
        return ret_press
    
    def appendLoadToCurrentLoadPattern(self, iCoo=2, delta=0):
        '''Append earth thrust on a set of elements to the current
        load pattern.


        :param iCoo: index of the coordinate that represents depth.
        :param delta: soil-wall friction angle (usually: delta= 2/3*Phi).
        '''
        return super(UniformPressureOnBackfill,self).appendLoadToCurrentLoadPattern(xcSet= self.xcSet, vDir= self.vDir, iCoo= iCoo, delta= delta)
        
    def appendVerticalLoadToCurrentLoadPattern(self, xcSet, vDir, iXCoo= 0,iZCoo= 2, alph= math.radians(30)):
        '''Append to the current load pattern the vertical pressures on 
           a set of elements due to the strip load. According to
           11.3.4 in the book "Mecánica de suelos" of Llano, J.J.S.
           isbn= 9788471461650 (https://books.google.ch/books?id=oQFZRKlix\_EC)

        :param xcSet: set that contains the elements.
        :param vDir: unit xc vector defining pressures direction.
        :param iXCoo: index of the horizontal coordinate.
        :param iZCoo: index of the vertical coordinate.
        :param alph: angle of stress spreading.
        '''
        sigma_v= self.qUnif
        return super(UniformPressureOnBackfill,self).appendVerticalLoadToCurrentLoadPattern(vLoad= self.qUnif, xcSet= xcSet, vDir= vDir, iXCoo= iXCoo, iZCoo= iZCoo, alph= alph)
        
class UniformLoadOnBackfill(UniformPressureOnBackfill): # Probably to DEPRECATE LP 20/07/2023.
    '''Lateral earth pressure on a retaining wall due to a uniform indefinite
       load on the backfill.

    :ivar K: pressure coefficient.
    '''
    def __init__(self, K, qLoad):
        ''' Constructor.

        :param K: pressure coefficient.
        :param qLoad: surcharge load (force per unit area).
        '''
        super(UniformLoadOnBackfill,self).__init__(zGround=0, zBottomSoils=[-1e3],KSoils=[K], qUnif= qLoad, xcSet= None, vDir= None)
        
class EarthPressureModel(UniformPressureOnBackfill):
    '''Parameters to define a load of type earth pressure

      :ivar zGround: global Z coordinate of ground level
      :ivar gammaSoils: list of weight density for each soil (from top to
            bottom)
      :ivar zWater: global Z coordinate of groundwater level 
            (if zGroundwater<minimum z of model => there is no groundwater)
      :ivar gammaWater: weight density of water
      :ivar qUnif: uniform load over the backfill surface (defaults to 0)
    '''
    def __init__(self, zGround, zBottomSoils, KSoils, gammaSoils, zWater, gammaWater,qUnif=0,xcSet=None,vDir=None):
        ''' Constructor.

        :param zGround: global Z coordinate of ground level
        :param zBottomSoils: list of global Z coordinates of the bottom level
              for each soil (from top to bottom)
        :param KSoils: list of pressure coefficients for each soil (from top 
              to bottom)
        :param gammaSoils: list of weight density for each soil (from top to
              bottom)
        :param zWater: global Z coordinate of groundwater level 
              (if zGroundwater<minimum z of model => there is no groundwater)
        :param gammaWater: weight density of water
        :param qUnif: uniform load over the backfill surface (defaults to 0)
        :ivar xcSet: set that contains the elements (shells and/or beams)
        :ivar vDir: unit xc vector defining pressures direction
        Note: xcSet and vDir have a default value None for compatibility with
          old definitions (through loads module)
        '''
        super(EarthPressureModel,self).__init__(zGround= zGround, zBottomSoils= zBottomSoils, KSoils= KSoils, qUnif= qUnif, xcSet= xcSet, vDir= vDir)
        self.gammaSoils= gammaSoils
        self.zWater= zWater # global Z coordinate of groundwater level 
        # Insert zWater level at the proper position.
        self.zTopLev.reverse()
        bisect.insort_left(self.zTopLev,zWater)
        self.zTopLev.reverse()
        if(self.zWater>self.zBottomSoils[-1]):
            indWat= self.zTopLev.index(zWater)
            self.KSoils.insert(indWat-1,self.KSoils[indWat-1])
            self.gammaSoils.insert(indWat-1,self.gammaSoils[indWat-1])
            for i in range(indWat,len(self.gammaSoils)):
                self.gammaSoils[i]-=gammaWater
            self.gammaWater=[0]*indWat+[gammaWater]*(len(self.gammaSoils)-indWat)                

    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.
        :param z: global z coordinate.
        '''
        ret_press= super(EarthPressureModel,self).getPressure(z)
        if z <= self.zGround:
            ind= self.getLayerIndex(z) # soil layer that correspond to z.
            # Compute pressure.
            ## Pressure of the upper soil layers.
            for i in range(ind):
                ret_press+= self.KSoils[i]*self.gammaSoils[i]*(self.zTopLev[i]-self.zTopLev[i+1])
            ## Pressure of the layer that correspond to z.
            ret_press+=self.KSoils[ind]*self.gammaSoils[ind]*(self.zTopLev[ind]-z)
            ## Water pressure.
            if(self.zWater>self.zBottomSoils[-1]):
                for i in range(ind):
                    ret_press+= self.gammaWater[i]*(self.zTopLev[i]-self.zTopLev[i+1])
                ret_press+= self.gammaWater[ind]*(self.zTopLev[ind]-z)
        return ret_press

class EarthPressureBase(PressureModelBase):
    '''Parameters to define a load of type earth pressure

      :ivar zGround:   global Z coordinate of ground level
      :ivar gammaSoil: weight density of soil .
    '''
    def __init__(self, zGround, gammaSoil):
        ''' Constructor.

        :param zGround:   global Z coordinate of ground level
        :param gammaSoil: weight density of soil 
        '''
        super(EarthPressureBase,self).__init__()
        self.zGround= zGround
        self.gammaSoil= gammaSoil
        
class PeckPressureEnvelope(EarthPressureBase):
    ''' Envelope of apparent lateral pressure diagrams for design 
        of cuts in sand. See 10.2 in the book "Principles of Foundation
        Engineering" from Braja M. Das.

      :ivar H: height of the cut.
      :ivar phi: effective friction angle of sand.
    '''
    def __init__(self,phi , zGround, gammaSoil, zWater, gammaWater, H):
        ''' Constructor.

        :param phi: effective friction angle of sand.
        :param zGround: global Z coordinate of ground level.
        :param gammaSoil: weight density of soil.
        :param zWater: global Z coordinate of groundwater level.
        :param gammaWater: weight density of water
        :param H: height of the cut.
        '''
        self.K= math.tan(math.pi/4.0-phi/2.0)**2 # Rankine active pressure coefficient.
        self.zWater=zWater
        self.gammaWater=gammaWater
        super(PeckPressureEnvelope,self).__init__( zGround, gammaSoil)
        self.H= H
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.'''
        ret_press= 0.0
        if(z<self.zGround):
            ret_press=0.65*self.K*self.gammaSoil*self.H
            if(z<self.zWater):
                className= type(self).__name__
                methodName= sys._getframe(0).f_code.co_name
                lmsg.error(className+'.'+methodName+'; pressures under water table not implemented.')
        return ret_press

class UniformLoadOnStem(PressureModelBase):
    '''Uniform lateral earth pressure on a retaining wall.

    :ivar qLoad: surcharge load (force per unit area).
    '''
    def __init__(self,qLoad):
        ''' Constructor.

        :param qLoad: surcharge load (force per unit area).
        '''
        super(UniformLoadOnStem,self).__init__()
        self.qLoad=qLoad
        
    def getPressure(self, z):
        '''Return the earth pressure acting on the points at global coordinate z.
        :param z: global z coordinate.
        '''
        return self.qLoad

    def appendLoadToCurrentLoadPattern(self,iCoo=2,delta=0):
        '''Append earth thrust on a set of elements to the current
        load pattern.


        :param iCoo: index of the coordinate that represents depth.
        :param delta: soil-wall friction angle (usually: delta= 2/3*Phi).
        '''
        return super(UniformLoadOnStem, self).appendLoadToCurrentLoadPattern(xcSEt= self.xcSet, vDir= self.vDir, iCoo= iCoo, delta= delta)

class StripLoadOnBackfill(UniformLoadOnStem):
    '''Lateral earth pressure on a retaining wall due to a strip surcharge 
    load on the backfill. (J.Calavera, pg.40)

    :ivar zLoad: global Z coordinate where the surcharge load acts
    :ivar distWall: minimal horizontal distance between the wall and the 
                    surcharge load
    :ivar stripWidth: width of the strip
    :ivar coef: is a coefficient = 1.5 (default) for the usual case of non-rigid
                walls. It can be redefined =2 for rigid walls
    '''
    def __init__(self,qLoad, zLoad,distWall, stripWidth):
        ''' Constructor.

        :param qLoad: surcharge load (force per unit area).
        :param distWall: minimal horizontal distance between the wall and the 
                        surcharge load
        :param stripWidth: width of the strip
        '''
        super(StripLoadOnBackfill,self).__init__(qLoad)
        self.zLoad=zLoad
        self.distWall=abs(distWall)
        self.stripWidth=stripWidth
        self.coef=1.5
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.

        :param z: global z coordinate.
        '''
        ret_press=0.0
        difZ=self.zLoad-z
        if difZ>0:
            bet1=math.atan(self.distWall/difZ)
            bet2=math.atan((self.distWall+self.stripWidth)/difZ)
            beta=bet2-bet1
            omega=bet1+beta/2.
            ret_press=self.coef*self.qLoad/math.pi*(beta-math.sin(beta)*math.cos(2*omega))
        return ret_press

    def appendVerticalLoadToCurrentLoadPattern(self,xcSet,vDir,iXCoo= 0,iZCoo= 2,alph= math.radians(30)):
        '''Append to the current load pattern the vertical pressures on 
           a set of elements due to the strip load. According to
           11.3.4 in the book "Mecánica de suelos" of Llano, J.J.S.
           isbn= 9788471461650 (https://books.google.ch/books?id=oQFZRKlix\_EC)

        :param xcSet: set that contains the elements.
        :param vDir: unit xc vector defining pressures direction.
        :param iXCoo: index of the horizontal coordinate.
        :param iZCoo: index of the vertical coordinate.
        :param alph: angle of stress spreading.
        '''
        tanAlph= math.tan(alph)
        avgZCoo= 0.0
        length= 0.0
        for e in xcSet.elements:
            z= e.getCooCentroid(False)[iZCoo]
            l= e.getLineSegment(False).getLength()
            avgZCoo+=z*l
            length+= l
        avgZCoo/= length
        H= self.zLoad-avgZCoo
        xMin= self.distWall-self.stripWidth-H*tanAlph
        xMax= self.distWall+self.stripWidth+H*tanAlph
        L= xMax-xMin
        sigma_v= self.qLoad*self.stripWidth/L
        if(len(vDir)==3): #3D load.
          for e in xcSet.elements:
              xElem= e.getCooCentroid(False)[iXCoo]
              if (sigma_v!=0.0) and (xElem>xMin) and (xElem<xMax):
                  e.vector3dUniformLoadGlobal(sigma_v*vDir)
        else: #2D load.
          for e in xcSet.elements:
              xElem= e.getCooCentroid(False)[iXCoo]
              if (sigma_v!=0.0) and (xElem>xMin) and (xElem<xMax):
                  e.vector2dUniformLoadGlobal(sigma_v*vDir)

    def getMaxMagnitude(self,xcSet):
        '''Return an estimation of the maximum magnitude of the vector loads 
        (it's supposed to occur in a point placed 1/3L from the top)

        :param xcSet: set containing the elements.
        '''
        zmin= sets.get_min_coo_nod(xcSet,2)
        zmax= sets.get_max_coo_nod(xcSet,2)
        zcontrol=zmin+2/3.*(zmax-zmin)
        maxEstValue=self.getPressure(zcontrol)
        return maxEstValue


class LineVerticalLoadOnBackfill(PressureModelBase):
    '''Lateral earth pressure on a retaining wall due to line surcharge 
    load acting in vertical direction on the backfill. (J.Calavera, pg.41)

    :ivar qLoad: surcharge load (force per unit length)
    :ivar zLoad: global Z coordinate where the line load acts
    :ivar distWall: horizontal distance between the wall and the line 
                    surcharge load
    :ivar xcSet: set that contains the elements (shells and/or beams)
    :ivar vDir: unit xc vector defining pressures direction
    Note: xcSet and vDir have a default value None for compatibility with
          old definitions (through loads module)
    '''
    def __init__(self,qLoad, zLoad,distWall,xcSet=None,vDir=None):
        ''' Constructor.

        :param qLoad: surcharge load (force per unit length)
        :param zLoad: global Z coordinate where the line load acts
        :param distWall: horizontal distance between the wall and the line 
                        surcharge load
        '''
        super(LineVerticalLoadOnBackfill,self).__init__()
        self.qLoad=qLoad
        self.zLoad=zLoad
        self.distWall=abs(distWall)
        self.xcSet=xcSet
        self.vDir=vDir
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.
        :param z: global z coordinate.
        '''
        ret_press=0.0
        difZ=self.zLoad-z
        if difZ>0:
            omega=math.atan(self.distWall/difZ)
            ret_press=self.qLoad/math.pi/difZ*(math.sin(2*omega))**2
        return ret_press

    def getMaxMagnitude(self,xcSet):
        '''Return an estimation of the maximum magnitude of the vector loads 
        (it's supposed to occur in a point placed 1/3L from the top).

        :param xcSet: set containing the elements.
        '''
        zmin=sets.get_min_coo_nod(xcSet,2)
        zmax=sets.get_max_coo_nod(xcSet,2)
        zcontrol=zmin+2/3.*(zmax-zmin)
        maxEstValue=self.getPressure(zcontrol)
        return maxEstValue
    
    def appendLoadToCurrentLoadPattern(self):
        return super(LineVerticalLoadOnBackfill,self).appendLoadToCurrentLoadPattern(xcSet= self.xcSet, vDir= self.vDir)

class PointVerticalLoadOnBackfill(object):
    '''Lateral earth pressure on a retaining wall due to a point 
    load acting in vertical direction on the backfill. (J.Calavera)

    :ivar xcSet: set of elements to which apply the loads
    :ivar Qload: value of the point load 
    :ivar loadAppPnt: load application point (xc.Vector([x,y,z])
    :ivar zBaseWall: global Z coordinate of the base of the wall
    :ivar distWall: horizontal distance between the wall and the point 
                    load
    :ivar vdir: xc unit vector prependicular to the wall pointing to its back
                (xc.Vector([ux,uy,0])
    '''
    def __init__(self, xcSet, Qload, loadAppPnt, zBaseWall, distWall, vdir):
        ''' Constructor.

        :param xcSet: set of elements to which apply the loads
        :param Qload: value of the point load 
        :param loadAppPnt: load application point (xc.Vector([x,y,z])
        :param zBaseWall: global Z coordinate of the base of the wall
        :param distWall: horizontal distance between the wall and the point 
                        load
        :param vdir: xc unit vector prependicular to the wall pointing to its
                     back (xc.Vector([ux,uy,0])
        '''
        self.xcSet=xcSet
        self.Qload=Qload
        self.loadAppPnt=loadAppPnt
        self.zBaseWall=zBaseWall
        self.distWall=distWall
        self.vdir=vdir
        
    def getAngPsi(self,x,y):
        '''Return the angle used to distribute pressures horizontally'''
        auxV=geom.Vector2d(x-self.loadAppPnt[0],y-self.loadAppPnt[1])
        auxVperp=geom.Vector2d(self.vdir[0],self.vdir[1])
        angPsi=auxV.getAngle(auxVperp)
        return angPsi 
        
    def getPressure(self,x,y,z):
        '''Return the earth pressure acting on the point of the wall
           placed at global coordinates (x,y,z).
        '''
        ret_press=0.0
        difZ=self.loadAppPnt[2]-z
        if difZ>0:
            H=self.loadAppPnt[2]-self.zBaseWall
            n=difZ/H
            m=abs(self.distWall)/H
            if m < 0.4:
                PN0=0.28*(self.Qload/(H)**2)*(n**2/(0.16+n**2)**3)
            else:
                PN0=1.77*(self.Qload/(H)**2)*(m**2*n**2/(m**2+n**2)**3)
            psi=self.getAngPsi(x,y)
            ret_press=PN0*(math.cos(psi))**2
        return ret_press
    
    def appendLoadToCurrentLoadPattern(self):
        '''Append earth thrust on a set of elements to the current
           load pattern.'''
        retval= geom.SlidingVector3d() # checking purposes.
        for e in self.xcSet.elements:
            coo=e.getCooCentroid(False)
            presElem=self.getPressure(coo[0],coo[1],coo[2])
            if(presElem!=0.0):
                loadVector= presElem*self.vdir
                e.vector3dUniformLoadGlobal(loadVector)
                area= e.getArea(False)
                totalLoad= loadVector*area
                retval+= geom.SlidingVector3d(geom.Pos3d(coo[0], coo[1], coo[2]), geom.Vector3d(totalLoad[0], totalLoad[1], totalLoad[2]))
        return retval # Sliding vector system (checking purposes)

class HorizontalLoadOnBackfill(PressureModelBase):
    '''Lateral earth pressure on a retaining wall due to a surcharge 
    load acting in horizontal direction on the backfill.

     See: https://estructurando.net/2017/04/18/empujes-muros-sobrecarga-horizontal/


    :ivar soilIntFi: agle of internal friction of the soil (º)
    :ivar qLoad: surcharge load (force per unit length)
    :ivar zLoad: global Z coordinate where the line load acts
    :ivar distWall: minimal horizontal distance between the wall and
          the area where the surcharge load acts (e.g.: a foundation)
    :ivar widthLoadArea: width (perperdicular to wall) of the area on
          which the horizontal load acts (e.g.: a foundation). 
    :ivar lengthLoadArea: length (normal to wall) of the area on
          which the horizontal load acts (e.g.: a foundation). We can
          take lengthLoadArea=1 (default) for a continous load  
    :ivar horDistrAngle: angle to distribute load in the direction of
          the lengthLoadArea (defaults to 0 to apply all the load to a
          length of wall =lengthLoadArea, which would be the case of a
          continous load)
    '''
    def __init__(self,soilIntFi, qLoad, zLoad,distWall,widthLoadArea,lengthLoadArea=1,horDistrAngle=0):
        ''' Constructor.

        :param soilIntFi: agle of internal friction of the soil (º)
        :param qLoad: surcharge load (force per unit length)
        :param zLoad: global Z coordinate where the line load acts
        :param distWall: minimal horizontal distance between the wall and
              the area where the surcharge load acts (e.g.: a foundation)
        :param widthLoadArea: width (perperdicular to wall) of the area on
              which the horizontal load acts (e.g.: a foundation). 
        :param lengthLoadArea: width (parallel to wall) of the area on
              which the horizontal load acts (e.g.: a foundation). We can
              take lengthLoadArea=1 (default) for a continous load  
        :param horDistrAngle: angle to distribute load in the direction of
              the lengthLoadArea (defaults to 0 to apply all the load to a
              length of wall =lengthLoadArea, which would be the case of a
              continous load)
        '''
        super(HorizontalLoadOnBackfill,self).__init__()
        self.soilIntFi=soilIntFi
        self.qLoad=qLoad
        self.zLoad=zLoad
        self.distWall=abs(distWall)
        self.widthLoadArea=widthLoadArea
        self.lengthLoadArea=lengthLoadArea
        self.horDistrAngle=horDistrAngle

    def setup(self):
        '''Calculate basic parameters.'''
        IntFiRad=math.radians(self.soilIntFi)
        psi=math.pi/4+IntFiRad/2
        self.zpresmax=self.zLoad-self.distWall*math.tan(IntFiRad)
        self.zpresmin=self.zLoad-(self.distWall+self.widthLoadArea)*math.tan(psi)
        horDistrAngleRad=math.radians(self.horDistrAngle)
        self.presmax=(self.qLoad*self.lengthLoadArea)/(self.lengthLoadArea+2*self.distWall*math.tan(horDistrAngleRad))*2/(self.zpresmax-self.zpresmin)
                                                  
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.
        :param z: global z coordinate.
        '''
        ret_press=0
        if z<=self.zpresmax and z>=self.zpresmin:
            ret_press=self.presmax/(self.zpresmax-self.zpresmin)*(z-self.zpresmin)
        return ret_press

    def appendLoadToCurrentLoadPattern(self,xcSet,vDir,iCoo= 2, delta= 0.0):
        '''Append to the current load pattern the earth thrust on a set of 
        elements due to the horizontal load.

        :param xcSet: set that contains the elements (shells and/or beams)
        :param vDir: unit xc vector defining pressures direction
        :param iCoo: index of the coordinate that represents depth.
        :param delta: soil-wall friction angle (usually: delta= 2/3*Phi).
        '''
        self.setup()
        return super(HorizontalLoadOnBackfill,self).appendLoadToCurrentLoadPattern(xcSet= xcSet, vDir= vDir, iCoo= iCoo, delta= delta)

    def getMaxMagnitude(self):
        '''Return the maximum magnitude of the vector loads'''
        self.setup()
        maxValue=self.getPressure(self.zpresmax)
        return maxValue

class SeismicPressureDistribution(EarthPressureBase):
    '''Base class for soil overpressure due to seismic action

      :ivar H: height of the structure.
      :ivar kv: seismic coefficient of vertical acceleration.
      :ivar kh: seismic coefficient of horizontal acceleration.
      :ivar psi: back face inclination of the structure (<= PI/2)
      :ivar phi: angle of internal friction of soil.
      :ivar beta: slope inclination of backfill.
    '''
    def __init__(self, zGround, gammaSoil, H, kv, kh, psi, phi, beta):
        ''' Constructor.

        :param zGround: global Z coordinate of ground level.
        :param gammaSoil: weight density of soil.
        :param H: height of the structure.
        :param kv: seismic coefficient of vertical acceleration.
        :param kh: seismic coefficient of horizontal acceleration.
        :param psi: back face inclination of the structure (<= PI/2)
        :param phi: angle of internal friction of soil.
        :param beta: slope inclination of backfill.
        '''
        
        super(SeismicPressureDistribution,self).__init__(zGround, gammaSoil)
        self.H= H
        self.kv= kv
        self.kh= kh
        self.psi= psi
        self.phi= phi
        self.beta= beta

    def setPhi(self, phi):
        ''' Assigns the value of the angle of internal friction of soil.

        :param phi: angle of internal friction of soil.
        '''
        self.phi= phi
        self.update()

    def setKh(self, kh):
        ''' Assigns the value of the horizontal seismic coefficient.

        :param kh: seismic coefficient of horizontal acceleration.
        '''
        self.kh= kh
        self.update()
        
    def setKv(self, kv):
        ''' Assigns the value of the vertical seismic coefficient.

        :param kv: seismic coefficient of vertical acceleration.
        '''
        self.kv= kv
        self.update()
        
    def getThrust(self):
        ''' Return the value of the earthquake thrust.'''
        return 0.5*self.max_stress*self.H
                                                  
class MononobeOkabePressureDistribution(SeismicPressureDistribution):
    '''Overpressure due to seismic action according to Mononobe-Okabe

      :ivar delta_ad: angle of friction soil - structure.
      :ivar Kas: static earth pressure coefficient 
    '''
    def __init__(self, zGround, gammaSoil, H, kv, kh, psi, phi, delta_ad, beta, Kas):
        ''' Constructor.

        :param zGround: global Z coordinate of ground level.
        :param gammaSoil: weight density of soil.
        :param H: height of the structure.
        :param kv: seismic coefficient of vertical acceleration.
        :param kh: seismic coefficient of horizontal acceleration.
        :param psi: back face inclination of the structure (<= PI/2)
        :param phi: angle of internal friction of soil.
        :param delta_ad: angle of friction soil - structure.
        :param beta: slope inclination of backfill.
        :param Kas: static earth pressure coefficient.
        '''
        
        super(MononobeOkabePressureDistribution,self).__init__(zGround= zGround, gammaSoil= gammaSoil, H= H, kv= kv, kh= kh, psi= psi, phi= phi, beta= beta)
        self.delta_ad= delta_ad
        self.Kas= Kas
        self.update()
        
    def update(self):
        self.overpressure_dry= mononobe_okabe.overpressure_dry(self.H, self.gammaSoil, self.kv, self.kh, self.psi, self.phi, self.delta_ad, self.beta, self.Kas)
        self.max_stress= 2*self.overpressure_dry/self.H
        
    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.
        :param z: global z coordinate.
        '''
        zSup= self.zGround
        zInf= self.zGround-self.H
        retval= 0.0
        if((z>=zInf) and (z<=zSup)):
            retval= (z-zInf)/(zSup-zInf)*self.max_stress
        return retval

    def appendLoadToCurrentLoadPattern(self, elements, vDir, iCoo= 2):
        '''Append earth thrust on a set of elements to the current
        load pattern.

        :param elements: elements to apply the load on.
        :param vDir: unit xc vector defining pressures direction
        :param iCoo: index of the coordinate that represents depth.
        '''
        if(vDir.getDimension()==3): #3D load.
            retval= geom.SlidingVector3d()  # checking purposes.
            for e in elements:
                centroid= e.getCooCentroid(False)
                presElem= self.getPressure(centroid[iCoo])
                if(presElem!=0.0):
                    loadVector= presElem*e.getKVector3d(True)
                    orientation= loadVector.dot(vDir)
                    if(orientation<0.0):
                        loadVector= -loadVector
                    e.vector3dUniformLoadGlobal(xc.Vector(loadVector))
                    area= e.getArea(False)
                    totalLoad= loadVector*area
                    retval+= geom.SlidingVector3d(geom.Pos3d(centroid[0], centroid[1], centroid[2]), totalLoad)
        else: # 2D load.
            retval= geom.SlidingVector2d()  # checking purposes.
            for e in elements:
                centroid= e.getCooCentroid(False)
                presElem=self.getPressure(centroid[iCoo])
                if(presElem!=0.0):
                    loadVector= presElem*e.getJVector2d(True)
                    orientation= loadVector.dot(vDir)
                    if(orientation<0.0):
                        loadVector= -loadVector
                    e.vector2dUniformLoadGlobal(xc.Vector(loadVector))
                    length= e.getLength(False)
                    totalLoad= loadVector*length
                    retval+= geom.SlidingVector2d(geom.Pos2d(centroid[0], centroid[1]), totalLoad)
        return retval # Sliding vector system (checking purposes)

class IskanderPressureDistribution(SeismicPressureDistribution):
    '''Overpressure due to seismic action according to Iskander et al.
       (Active static and seismic earth pressure for c–φ soils Magued Iskander, 
        Zhibo (Chris) Chen , Mehdi Omidvar, Ivan Guzman, Omar Elsheri. 
        Polytechnic Institute of New York University, USA)

    :ivar c: cohesion
    '''
    def __init__(self, zGround, gammaSoil, H, kv, kh, psi, phi, beta, c):
        ''' Constructor.

        :param zGround: global Z coordinate of ground level.
        :param gammaSoil: weight density of soil.
        :param H: height of the structure.
        :param kv: seismic coefficient of vertical acceleration.
        :param kh: seismic coefficient of horizontal acceleration.
        :param psi: back face inclination of the structure (<= PI/2)
        :param phi: angle of internal friction of soil.
        :param beta: slope inclination of backfill.
        :param c: cohesion
        '''
        
        super(IskanderPressureDistribution,self).__init__(zGround= zGround, gammaSoil= gammaSoil, H= H, kv= kv, kh= kh, psi= psi, phi= phi, beta= beta)
        self.c= c
        self.update()

    def update(self):
        omega= math.pi/2.0-self.psi # Angle with vertical.
        self.iskander= iskander_method.Iskander(kh= self.kh, kv= self.kv, omega= omega, beta= self.beta, phi= self.phi, gamma= self.gammaSoil, c= self.c, H= self.H)
        # Compute pressure with earthquake.
        self.hl= self.iskander.Hl()
        pressureA= self.iskander.sigma_AEH(self.hl)
        # Compute pressure with earthquake.
        self.iskander.kh= 0.0
        self.iskander.kv= 0.0
        pressureB= self.iskander.sigma_AEH(self.hl)
        self.max_stress= pressureA-pressureB
        # Restore earthquake coefficients.
        self.iskander.kh= self.kh
        self.iskander.kv= self.kv

    def getPressure(self,z):
        '''Return the earth pressure acting on the points at global coordinate z.
        :param z: global z coordinate.
        '''
        zSup= self.zGround
        zInf= self.zGround-self.hl
        retval= 0.0
        if((z>=zInf) and (z<=zSup)):
            retval= (zSup-z)/(zSup-zInf)*self.max_stress
        return retval
        
        
class EarthPressureSlopedBackfill(object):
    '''Earth pressure on a wall with sloped backfill. A single soil type with no water table.

    :ivar Ksoil: pressure coefficient of the soil.
    :ivar gammaSoil: weight density of the soil
    :ivar zGroundPnt1: global Z coordinate of ground level at point of 
                      coordinates XYpnt1
    :ivar XYpnt1: (x,y) coordinates of point 1   
    :ivar zGroundPnt2: global Z coordinate of ground level at point of 
                      coordinates XYpnt2
    :ivar XYpnt2: (x,y) coordinates of point 2   
    '''
    
    def __init__(self, Ksoil, gammaSoil, zGroundPnt1, XYpnt1, zGroundPnt2, XYpnt2):
        ''' Constructor.

        :param Ksoil: pressure coefficient of the soil
        :param gammaSoil: weight density of the soil
        :param zGroundPnt1: global Z coordinate of ground level at point of 
                          coordinates XYpnt1
        :param XYpnt1: (x,y) coordinates of point 1   
        :param zGroundPnt2: global Z coordinate of ground level at point of 
                          coordinates XYpnt2
        :param XYpnt2: (x,y) coordinates of point 2   
        '''
        self.Ksoil=Ksoil
        self.gammaSoil=gammaSoil
        self.zGroundPnt1=zGroundPnt1
        self.XYpnt1=XYpnt1
        self.zGroundPnt2=zGroundPnt2
        self.XYpnt2=XYpnt2
        self.slope=(self.zGroundPnt2-self.zGroundPnt1)/math.sqrt((self.XYpnt2[0]-self.XYpnt1[0])**2+(self.XYpnt2[1]-self.XYpnt1[1])**2)
        
    def getPressure(self,x,y,z):
        zGround=self.zGroundPnt1+self.slope*math.sqrt((x-self.XYpnt1[0])**2+(y-self.XYpnt1[1])**2)
        if z<zGround:
            ret_press=self.Ksoil*self.gammaSoil*(zGround-z)
        else:
            ret_press=0.0
        return ret_press
        
    def appendLoadToCurrentLoadPattern(self,xcSet,vDir):
        retval= geom.SlidingVector3d()  # checking purposes.
        for e in xcSet.elements:
            coo= e.getCooCentroid(False)
            presElem= self.getPressure(coo[0],coo[1],coo[2])
            if(presElem!=0.0):
                loadVector= presElem*vDir
                e.vector3dUniformLoadGlobal(loadVector)
                area= e.getArea(False)
                totalLoad= loadVector*area
                retval+= geom.SlidingVector3d(geom.Pos3d(coo[0], coo[1], coo[2]), geom.Vector3d(totalLoad[0], totalLoad[1], totalLoad[2]))
        return retval # Sliding vector system (checking purposes)

class WeightDistrEmbankment(object):
    '''Distribution of pressure on a set of shells due to the weight of a
       the soil on the shell elements. 
       The surface of the embankment is a a strip defined
       by the angle that its transversal section forms with the X global axis 
       (counterclockwise) and a list of coordinates [[xp1,z1],[xp2,z2], ...],
       where xp coordinates are expressed in a coordinate system obtained 
       rotating theta radians the global system.

    :ivar xcSet: set of elements to which apply the loads
    :ivar gammaSoil: weight density of the soil
    :ivar theta: angle counterclockwise in radians that forms the transversal
                 section of the embankment with the global X-axis
    :ivar coordSoilSurf: list of coordinates [[xp1,z1],[xp2,z2], ...] 
          that defines the soil surface, where xp are the x coordinates of the
          vertices of a transversal section in the surface, expressed in 
          the rotated reference system.
    '''
    def __init__(self, xcSet, gammaSoil, theta, coordSoilSurf):
        ''' Constructor.

        :param xcSet: set of elements to which apply the loads
        :param gammaSoil: weight density of the soil
        :param theta: angle counterclockwise in radians that forms the transversal
                     section of the embankment with the global X-axis
        :param coordSoilSurf: list of coordinates [[xp1,z1],[xp2,z2], ...] 
              that defines the soil surface, where xp are the x coordinates of
              the vertices of a transversal section in the surface, expressed
              in the rotated reference system.
        '''
        self.xcSet=xcSet
        self.gammaSoil=gammaSoil
        self.theta=theta
        self.coordSoilSurf=sorted(coordSoilSurf)
        self.thetarad=math.radians(theta)
        self.xpmin=coordSoilSurf[0][0]
        self.xpmax=coordSoilSurf[-1][0]
        self.xpList=[self.coordSoilSurf[i][0] for i in range(len(self.coordSoilSurf))]

    def getPressure(self,x,y,z):
        xp=x*math.cos(self.thetarad)+y*math.sin(self.thetarad)
        if self.xpmin < xp < self.xpmax:
            indXp=bisect.bisect(self.xpList,xp)
            xp1,xp2=self.coordSoilSurf[indXp-1][0],self.coordSoilSurf[indXp][0]
            z1,z2=self.coordSoilSurf[indXp-1][1],self.coordSoilSurf[indXp][1]
            zGround=z1+(z2-z1)/(xp2-xp1)*(xp-xp1)
            ret_press=self.gammaSoil*(zGround-z)
        else:
            ret_press=0.0
        return ret_press

    def appendLoadToCurrentLoadPattern(self):
        '''Append pressure on a set of elements to the current
           load pattern.'''
        retval= geom.SlidingVector3d() # checking purposes.
        for e in self.xcSet.elements:
            coo=e.getCooCentroid(False)
            presElem=max(0,self.getPressure(coo[0],coo[1],coo[2]))
            if(presElem!=0.0):
                loadVector= xc.Vector([0,0,-presElem])
                e.vector3dUniformLoadGlobal(loadVector)
                area= e.getArea(False)
                totalLoad= loadVector*area
                retval+= geom.SlidingVector3d(geom.Pos3d(coo[0], coo[1], coo[2]), geom.Vector3d(totalLoad[0], totalLoad[1], totalLoad[2]))
        return retval # Sliding vector system (checking purposes)
                
            
            
                    
