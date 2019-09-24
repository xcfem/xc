# -*- coding: utf-8 -*-

from __future__ import division

'''Classes related to prestressed concrete 
'''

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import xc_base
import geom
import xc
import math
import numpy as np
from scipy import interpolate
from scipy import optimize
from scipy.spatial import distance
import matplotlib.pyplot as plt
from miscUtils import LogMessages as lmsg
from mpl_toolkits.mplot3d import Axes3D

class PrestressTendon(object):
    '''Geometry and prestressing losses of a prestressing tendon

    :ivar lstRoughPnt: list of 3D-coordinate tuples defining the points from 
                       which to interpolate the cubic spline that will delineate
                       the tendon geometry. e.g. [(x1,y1,z1),(x2,y2,z2),...].
                       For straight tendon only two 3D-coordinate tuples are 
                       provided defining the two extremities of the tendon.
    :ivar roughCoordMtr: matrix 3*n_rough_ptos [[x1,x2, ..],[y1,y2,..],[z1,z2,..]] 

    '''
    def __init__(self,lstRoughPnts):
        self.lstRoughPnts=lstRoughPnts
        self.roughCoordMtr=np.array(lstRoughPnts).transpose()

    def pntsStraightTendon(self,eSize):
        '''Interpolates coordinates in the straight line defined by two points 
        in lstRoughPnt, spaced eSize. Creates attribute:
        
        - fineCoordMtr: matrix with coordinates of the interpolated points
        [[x1,x2, ..],[y1,y2,..],[z1,z2,..]]
        '''
        extr1=geom.Pos3d(self.lstRoughPnts[0][0],self.lstRoughPnts[0][1],self.lstRoughPnts[0][2])
        extr2=geom.Pos3d(self.lstRoughPnts[1][0],self.lstRoughPnts[1][1],self.lstRoughPnts[1][2])
        v=extr2-extr1
        L=v.getModulus()
        nPnt=int(L/eSize)+1
        eSize=L/(nPnt-1)
        vn=v.normalized()
        xCoor,yCoor,zCoor=list(),list(),list()
        for i in range(nPnt):
            v=i*eSize*vn
            xCoor.append(v.x)
            yCoor.append(v.y)
            zCoor.append(v.z)
        self.fineCoordMtr=np.array([xCoor,yCoor,zCoor])
        return
            

    def pntsInterpTendon(self,nPntsFine,smoothness,kgrade=3):
        '''Generates a cubic spline (default) or a spline of grade kgrade 
        interpolation from the rough points and calculates its value and 
        the value of its derivative in nPntsFine equispaced.
        Creates the following attributes:
        
        - fineCoordMtr: matrix with coordinates of the interpolated points
        [[x1,x2, ..],[y1,y2,..],[z1,z2,..]]
        - fineDerivMtr: matrix with the vector representing the derivative
        in each interpolated  
        - tck: tuple (t,c,k) containing the vector of knots, the B-spline 
               coefficients, and the degree of the spline.
        - fineScoord: curvilinear coordinate (cummulative length of curve 
                      for in each point)
        - fineProjXYcoord: coordinate by the projection of the curve on the XY 
        plane. Matrix 1*nPntsFine whose first elements is 0 and the rest the 
        cumulative distance to the first point
        '''
        tck, u = interpolate.splprep(self.roughCoordMtr, k=kgrade,s=smoothness)
        x_knots, y_knots,z_knots = interpolate.splev(np.linspace(0, 1, nPntsFine), tck,der=0)
        self.fineCoordMtr=np.array([x_knots, y_knots,z_knots])
        x_der, y_der,z_der = interpolate.splev(np.linspace(0, 1,nPntsFine), tck,der=1)
        self.fineDerivMtr=np.array([x_der, y_der,z_der])
        self.tck=tck
        self.fineScoord=self.getCumLength()
        x0,y0=(self.fineCoordMtr[0][0],self.fineCoordMtr[1][0])
        self.fineProjXYcoord=((self.fineCoordMtr[0]-x0)**2+(self.fineCoordMtr[1]-y0)**2)**0.5
        return

    def getLengthSequence(self):
        '''Return for each point in fineCoordMtr the distance to the preceding 
        point
        '''
        lseq=[0]+[distance.euclidean((self.fineCoordMtr[0][i],self.fineCoordMtr[1][i],self.fineCoordMtr[2][i]),(self.fineCoordMtr[0][i+1],self.fineCoordMtr[1][i+1],self.fineCoordMtr[2][i+1])) for i in range(len(self.fineCoordMtr[0])-1)]
        return lseq

    def getCumLength(self):
        '''Return for each point in fineCoordMtr the cumulative lenght of 
        tendon from its starting point
        '''
        length_sequence=self.getLengthSequence()
        return np.cumsum(length_sequence)


    def getReverseCumLength(self):
        '''Return for each point in fineCoordMtr the cumulative lenght of 
        tendon from its ending point
        '''
        lseq=self.getLengthSequence()
        lseq.append(lseq.pop(0))
        lseq.reverse()
        lcum_back=np.cumsum(lseq)
        return np.flipud(lcum_back)

    def getAngleSequence(self):
        '''Return for each point in fineCoordMtr the deviation deviation 
        (angle in rad) with respect to the preceding point
        '''
        aseq=[0]+ [angle_between((self.fineDerivMtr[0][i], self.fineDerivMtr[1][i], self.fineDerivMtr[2][i]),(self.fineDerivMtr[0][i+1], self.fineDerivMtr[1][i+1], self.fineDerivMtr[2][i+1])) for i in range(len(self.fineDerivMtr[0])-1)]
        return aseq
        
    def getCumAngle(self):
        '''Return for each point in fineCoordMtr the cumulative angular 
        deviation (angle in rad) of the tendon from its starting point
        '''
        angl_sequence=self.getAngleSequence()
        return np.cumsum(angl_sequence)

    def getReverseCumAngle(self):
        '''Return for each point in fineCoordMtr the cumulative angular 
        deviation (angle in rad) of the tendon from its ending point
        '''
        aseq=self.getAngleSequence()
        aseq.append(aseq.pop(0))
        aseq.reverse()
        acum_back=np.cumsum(aseq)
        return np.flipud(acum_back)

    def creaTendonElements(self,preprocessor,materialName,elemTypeName,crdTransfName,areaTendon,setName):
        '''Creates the nodes and elements of the tendon and appends them to the set returned. Creates also the attribute lstOrderedElems as a list with the elements of the tendon ordered from left to right.

        :param preprocessor: preprocessor
        :param materialName: name of the material
        :param elemTypeName: name of the type of element
        :param crdTransfName: name of the coordinate transformation
        :param areaTendon:   area of the cross-section of the tendon
        :param setName:      name of the set to which append the nodes and 
                             elements created (if the set doesn't exist, it's 
                             created)
        '''
        self.lstOrderedElems=list()
        setsMng=preprocessor.getSets
        if setsMng.exists(setName):
            tendonSet=setsMng.getSet(setName)
        else:
            tendonSet=setsMng.defSet(setName)
        nodes= preprocessor.getNodeHandler
        elems= preprocessor.getElementHandler
        elems.dimElem= preprocessor.getNodeHandler.dimSpace
        elems.defaultMaterial=materialName
        if crdTransfName:
            elems.defaultTransformation=crdTransfName
        nEnd2=nodes.newNodeXYZ(self.fineCoordMtr[0][0],self.fineCoordMtr[1][0],self.fineCoordMtr[2][0])
        tendonSet.nodes.append(nEnd2)
        for i in range(1,len(self.fineCoordMtr[0])):
            nEnd1=nodes.getNode(nEnd2.tag)
            nEnd2=nodes.newNodeXYZ(self.fineCoordMtr[0][i],self.fineCoordMtr[1][i],self.fineCoordMtr[2][i])
            elem=elems.newElement(elemTypeName,xc.ID([nEnd1.tag,nEnd2.tag]))
            elem.area=areaTendon
            tendonSet.nodes.append(nEnd2)
            tendonSet.elements.append(elem)
            self.lstOrderedElems.append(elem)
        return tendonSet
            
    def getLossFriction(self,coefFric,k,sigmaP0_extr1=0.0,sigmaP0_extr2=0.0):
        ''' Return an array that contains  for each point in fineCoordMtr the 
        cumulative immediate loss of prestressing due to friction.
        The method also creates the attribute stressAfterLossFriction, of 
        type array, that contains  for each point in fineCoordMtr the stress
        after after this loss (it is used in calculation of losses due to
        anchorage slip)

        :param sigmaP0_extr1: maximum stress applied at the extremity 1 of 
                        the tendon (starting point) (stress refers to its 
                        value at the end of the anchorage, in the contact
                        with concrete). Defaults to 0.0 (no prestress applied)
        :param sigmaP0_extr2: idem for prestress applied at extremity 2
                         (end point of the tendon). Defaults to 0.0 (no 
                         prestress applied)
        :param coefFric: coefficient of friction between the tendon and its 
                        sheathing
        :param k: wobble coefficient or coefficient for wave effect, that 
                  refers to the unintentional deviation of the position of 
                  the tendon along the duct. It's expressed by unit length 
                  of tendon. The value of k varies from 0.0015 to 0.0050 per 
                  meter length of the tendon depending on the type of tendon. 
        '''
        if (sigmaP0_extr1 != 0.0):
            cum_len=self.getCumLength()
            cum_angl=self.getCumAngle()
            loss_frict_ext1=np.array([sigmaP0_extr1*(1-math.exp(-coefFric*cum_angl[i]-k*cum_len[i])) for i in range(len(cum_angl))])
            self.stressAfterLossFrictionOnlyExtr1=sigmaP0_extr1-loss_frict_ext1
        if (sigmaP0_extr2 != 0.0):
            cum_len=self.getReverseCumLength()
            cum_angl=self.getReverseCumAngle()
            loss_frict_ext2=np.array([sigmaP0_extr2*(1-math.exp(-coefFric*cum_angl[i]-k*cum_len[i])) for i in range(len(cum_angl))])
            self.stressAfterLossFrictionOnlyExtr2=sigmaP0_extr2-loss_frict_ext2
        if (sigmaP0_extr1 != 0.0) and (sigmaP0_extr2 != 0.0):
            lossFriction=np.minimum(loss_frict_ext1,loss_frict_ext2)
            self.stressAfterLossFriction=np.maximum(self.stressAfterLossFrictionOnlyExtr1,self.stressAfterLossFrictionOnlyExtr2)
        elif (sigmaP0_extr1 != 0.0):
            lossFriction=loss_frict_ext1
            self.stressAfterLossFriction=self.stressAfterLossFrictionOnlyExtr1
        elif (sigmaP0_extr2 != 0.0):
            lossFriction=loss_frict_ext2
            self.stressAfterLossFriction=self.stressAfterLossFrictionOnlyExtr2
        else:
            lmsg.warning("No prestressing applied.")
        return lossFriction

    def getLossAnchor(self,Ep,anc_slip_extr1=0.0,anc_slip_extr2=0.0):
        '''Return an array that contains  for each point in fineCoordMtr 
        the cumulative loss of prestressing due to anchorage.
        Loss due to friction must be previously calculated

        :param Ep:      elasic modulus of the prestressing steel

        :param anc_slip_extr1: anchorage slip (data provided by the manufacturer
                        of the anchorage system) at extremity 1 of 
                        the tendon (starting point)  (= deltaL)
        :param anc_slip_extr2: anchorage slip at extremity 2 of 
                        the tendon (ending point) (= deltaL)
        '''
        self.projXYcoordZeroAnchLoss=[0,self.fineProjXYcoord[-1]] # projected coordinates of the
                                   # points near extremity 1 and extremity 2,
                                   #respectively, that delimite the lengths of
                                   # tendon affected by the loss of prestress
                                   # due to the anchorages slip
        #Initialization values
        lossAnchExtr1=np.zeros(len(self.fineScoord))
        lossAnchExtr2=np.zeros(len(self.fineScoord))
        if anc_slip_extr1 >0:
            self.slip1=Ep*anc_slip_extr1
            self.tckLossFric=interpolate.splrep(self.fineScoord,self.stressAfterLossFrictionOnlyExtr1,k=3)
            if self.fAnc_ext1(self.fineScoord[-1])<0:  #the anchorage slip  
                                             #affects all the tendon length
                lackArea=-2*self.fAnc_ext1(self.fineScoord[-1])
                excess_delta_sigma=lackArea/self.getCumLength().item(-1)
                sCoordZeroLoss=self.fineScoord[-1]
            else:
                # we use newton_krylov solver to find the zero of function
                # fAnc_ext1 that gives us the point from which the tendon is
                # not affected by the anchorage slip. Tolerance and relative
                # step is given as parameters in order to enhance convergence
                tol=self.slip1*1e-6
                sCoordZeroLoss=optimize.newton_krylov(F=self.fAnc_ext1,xin=self.fineScoord[-1]/2.0,rdiff=0.1,f_tol=tol)   #point from which the tendon is not
                       #affected by the anchorage slip
                self.projXYcoordZeroAnchLoss[0]=self.ScoordToXYprojCoord(sCoordZeroLoss.item(0))
                excess_delta_sigma=0
            stressSCoordZeroLoss=interpolate.splev(sCoordZeroLoss,self.tckLossFric,der=0)              #stress in that point (after loss due to friction)
            condlist=[self.fineScoord <= sCoordZeroLoss]
            choicelist = [2*(self.stressAfterLossFrictionOnlyExtr1-stressSCoordZeroLoss)+excess_delta_sigma]
            lossAnchExtr1=np.select(condlist,choicelist)
        if anc_slip_extr2 >0:
            self.slip2=Ep*anc_slip_extr2
            self.tckLossFric=interpolate.splrep(self.fineScoord,self.stressAfterLossFrictionOnlyExtr2,k=3)
            if self.fAnc_ext2(self.fineScoord[0])<0:  #the anchorage slip 
                                             #affects all the tendon length
                lackArea=-2*self.fAnc_ext2(self.fineScoord[0])
                excess_delta_sigma=lackArea/self.getCumLength().item(-1)
                sCoordZeroLoss=self.fineScoord[0]
            else:
                # we use newton_krylov solver to find the zero of function
                # fAnc_ext1 that gives us the point from which the tendon is
                # not affected by the anchorage slip
                tol=self.slip2*1e-6
                sCoordZeroLoss=optimize.newton_krylov(self.fAnc_ext2,self.fineScoord[-1]/2.0,rdiff=0.1,f_tol=tol)   #point from which the tendon is affected
                       #by the anchorage slip
                self.projXYcoordZeroAnchLoss[1]=self.ScoordToXYprojCoord(sCoordZeroLoss.item(0))
                excess_delta_sigma=0
            stressSCoordZeroLoss=interpolate.splev(sCoordZeroLoss,self.tckLossFric,der=0)              #stress in that point (after loss due to friction)
            condlist=[self.fineScoord >= sCoordZeroLoss]
            choicelist = [2*(self.stressAfterLossFriction-stressSCoordZeroLoss)+excess_delta_sigma]
            lossAnchExtr2=np.select(condlist,choicelist)
        lossAnch=lossAnchExtr1+lossAnchExtr2
        return lossAnch
#        self.stressAfterLossAnch=self.stressAfterLossFriction-self.lossAnch

    def fAnc_ext1(self,s):
        '''Funtion to minimize when searching the point from which the tendon 
        is not affected by the anchorage slip in extremity 1
        '''
        y=interpolate.splint(0.0,s,self.tckLossFric)-s*interpolate.splev(s,self.tckLossFric,der=0)-self.slip1/2.0
        return y
        
    def fAnc_ext2(self,s):
        '''Funtion to minimize when searching the point from which the tendon 
        is not affected by the anchorage slip in extremity 2
        '''
        y=interpolate.splint(s,self.fineScoord[-1],self.tckLossFric)-(self.fineScoord[-1]-s)*interpolate.splev(s,self.tckLossFric,der=0)-self.slip2/2.0
        return y

    def ScoordToXYprojCoord(self,Scoord):
        '''return the projected XYcoordinate of the tendon point corresponding 
        to the value Scoord given as parameter
        '''
        index1=(np.abs(self.fineScoord-Scoord)).argmin()
        if Scoord < self.fineScoord[index1]:
            index2=index1-1
        else:
            index2=index1+1
        prop=(Scoord-self.fineScoord[index1])/(self.fineScoord[index2]-self.fineScoord[index1])
        XYcoor=self.fineProjXYcoord[index1]+prop*(self.fineProjXYcoord[index2]-self.fineProjXYcoord[index1])
        return XYcoor
        
    def applyStressToElems(self,stressMtr):
        '''Apply stress in each tendon element with the corresponding 
        value of stress defined in stressMtr

        :param stressMtr: matrix of dimension 1*number of nodes in the tendon                          with the stress to be applied to each of the elements 
                          (from left to right)
        '''
        stressMtrToElem=(stressMtr[1:]+stressMtr[:-1])/2.0
        for e, s in zip(self.lstOrderedElems,stressMtrToElem):
            m=e.getMaterial()
            m.prestress=s
            e.update()
        return
    
    def applyStressLossToElems(self,stressLossMtr):
        '''Apply stress loss in each tendon element with the corresponding 
        value of stress loss defined in stressMtr

        :param stressLossMtr: matrix of dimension 1*number of nodes in the 
               tendon with the loss of stress to be applied to each of the 
               elements (from left to right)
        '''
        stressLossMtrToElem=(stressLossMtr[1:]+stressLossMtr[:-1])/2.0
        for e, delta_stress in zip(self.lstOrderedElems,stressLossMtrToElem):
            m=e.getMaterial()
            m.prestress=m.getStress()-delta_stress
            e.update()
        return
        
    def plot3D(self,axisEqualScale='N',symbolRougPoints=None,symbolFinePoints=None,symbolTendon=None,resultsToPlot=list()):
        '''Return in a 3D graphic the results to which a symbol is assigned.
        Symbol examples: 'r-': red solid line, 'mo': magenta circle, 
        'b--': blue dashes, 'ks':black square,'g^' green triangle_up, 
        'c*': cyan star, ...
        :param axisEqualScale:  ='Y', 'y','yes' or 'Yes' for equal aspect ratio
                             (defaults to 'N')
        :param resultsToPlot: list of results to plot. Each result to be
               displayed is input with the following parameters:
               (matRes,symb,label), where:
               - matRes: is the matrix of dimension 1*number of elements in the 
                 tendon with the value of the result for each of those elements
               - symb: is the symbol to use for representing this result
               - label: is the text to label this result 
        '''
        fig = plt.figure()
        ax3d = fig.add_subplot(111, projection='3d')
        if symbolRougPoints:
            ax3d.scatter(self.roughCoordMtr[0],self.roughCoordMtr[1],self.roughCoordMtr[2],symbolRougPoints,label='Rough points')
        if symbolFinePoints:
            ax3d.plot(self.fineCoordMtr[0],self.fineCoordMtr[1],self.fineCoordMtr[2],symbolFinePoints,label='Interpolated points (spline)')
        if symbolTendon:
            ax3d.plot(self.fineCoordMtr[0],self.fineCoordMtr[1],self.fineCoordMtr[2],symbolTendon,label='Tendon')
        for res in resultsToPlot:
            ax3d.plot(self.fineCoordMtr[0],self.fineCoordMtr[1],res[0],res[1],label=res[2])
        ax3d.legend()
        ax3d.set_xlabel('X')
        ax3d.set_ylabel('Y')
        ax3d.set_zlabel('Z')
        if str.lower(axisEqualScale[0])=='y':
#            ax3d.axis('equal')
            ax3d.set_aspect('equal')
            set_axes_equal(ax3d)
        return fig,ax3d

    def plot2D(self,XaxisValues='X',axisEqualScale='N',symbolRougPoints=None,symbolFinePoints=None,symbolTendon=None,resultsToPlot=list()):
        '''Return in a 2D graphic the results to which a symbol is assigned.
        Symbol examples: 'r-': red solid line, 'mo': magenta circle, 'b--': blue dashes, 'ks':black square,'g^' green triangle_up, 'c*': cyan star, ...
        :param XaxisValues: ='X' (default) to represent in the diagram X-axis
                              the X coordinates of the tendon,
                            ='Y' to represent in the diagram X-axis 
                              the Y coordinates of the tendon,
                            ='XY' to represent in the diagram X-axis 
                              the coordinate by the projection of the 
                              curve on the XY plane
        :param axisEqualScale:  ='Y', 'y','yes' or 'Yes' for equal aspect ratio
                             (defaults to 'N')
        :param resultsToPlot: list of results to plot. Each result to be
               displayed is input with the following parameters:
               (matRes,symb,label), where:
               - matRes: is the matrix of dimension 1*number of elements in the 
                 tendon with the value of the result for each of those elements
               - symb: is the symbol to use for representing this result
               - label: is the text to label this result 
        '''
        if XaxisValues.upper()=='X':
            XaxisCoord=self.fineCoordMtr[0]
            XaxisRoughCoord=self.roughCoordMtr[0]
            xLab='X'
        elif XaxisValues.upper()=='Y':
            XaxisCoord=self.fineCoordMtr[1]
            YaxisRoughCoord=self.roughCoordMtr[1]
            xLab='Y'
        elif XaxisValues.upper()=='XY':
            XaxisCoord=self.fineProjXYcoord
            x0,y0=(self.roughCoordMtr[0][0],self.roughCoordMtr[1][0])
            XaxisRoughCoord=((self.roughCoordMtr[0]-x0)**2+(self.roughCoordMtr[1]-y0)**2)**(0.5)
            xLab='XYproj'
        else:
            lmsg.warning("Wrong value for XaxisValues ('X','Y','S'): X coordinates are represented in the figure")

        fig = plt.figure()
        ax2d=plt.subplot(111)
        if symbolRougPoints:
            ax2d.plot(XaxisRoughCoord,self.roughCoordMtr[2],symbolRougPoints,label='Rough points')
        if symbolFinePoints:
            ax2d.plot(XaxisCoord,self.fineCoordMtr[2],symbolFinePoints,label='Interpolated points (spline)')
        if symbolTendon:
            ax2d.plot(XaxisCoord,self.fineCoordMtr[2],symbolTendon,label='Tendon')
        for res in resultsToPlot:
            ax2d.plot(XaxisCoord,res[0],res[1],label=res[2])
        ax2d.legend()
        ax2d.set_xlabel(xLab)
        if str.lower(axisEqualScale[0])=='y':
            ax2d.axis('equal')
        return fig,ax2d

def angle_between(a,b):
    '''Return the angle between vectors a and b
    '''
    arccosInput = np.dot(a,b)/np.linalg.norm(a)/np.linalg.norm(b)
    arccosInput = 1.0 if arccosInput > 1.0 else arccosInput
    arccosInput = -1.0 if arccosInput < -1.0 else arccosInput
    return math.acos(arccosInput)

def set_axes_equal(ax):
    '''Make axes of 3D plot have equal scale so that spheres appear as spheres,
    cubes as cubes, etc..  This is one possible solution to Matplotlib's
    ax.set_aspect('equal') and ax.axis('equal') not working for 3D.

    Input
      ax: a matplotlib axis, e.g., as output from plt.gca().
    '''

    x_limits = ax.get_xlim3d()
    y_limits = ax.get_ylim3d()
    z_limits = ax.get_zlim3d()

    x_range = abs(x_limits[1] - x_limits[0])
    x_middle = np.mean(x_limits)
    y_range = abs(y_limits[1] - y_limits[0])
    y_middle = np.mean(y_limits)
    z_range = abs(z_limits[1] - z_limits[0])
    z_middle = np.mean(z_limits)

    # The plot bounding box is a sphere in the sense of the infinity
    # norm, hence I call half the max range the plot radius.
    plot_radius = 0.5*max([x_range, y_range, z_range])

    ax.set_xlim3d([x_middle - plot_radius, x_middle + plot_radius])
    ax.set_ylim3d([y_middle - plot_radius, y_middle + plot_radius])
    ax.set_zlim3d([z_middle - plot_radius, z_middle + plot_radius])
