# -*- coding: utf-8 -*-

'''Classes related to prestressed concrete 
'''

__author__= "Ana Ortega (AO_O)"
__copyright__= "Copyright 2017, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "ana.Ortega@ciccp.es"

import math
import numpy as np
from scipy import interpolate
from scipy.spatial import distance
import matplotlib.pyplot as plt
from miscUtils import LogMessages as lmsg
from mpl_toolkits.mplot3d import Axes3D

class PrestressTendon(object):
    '''Geometry and prestressing losses of a prestressing tendon

    :ivar lstRoughPnt: list of 3D-coordinate tuples defining the points from 
                       which to interpolate the cubic spline that will delineate
                       the tendon geometry. e.g. [(x1,y1,z1),(x2,y2,z2),...]
    :ivar roughCoordMtr: matrix 3*n_rough_ptos [[x1,x2, ..],[y1,y2,..],[z1,z2,..]] 

    '''
    def __init__(self,lstRoughPnts):
        self.lstRoughPnts=lstRoughPnts
        self.roughCoordMtr=np.array(lstRoughPnts).transpose()

    def pntsInterpTendon(self,nPntsFine,smoothness,kgrade=3):
        '''Generates a cubic spline (default) or a spline of grade kgrade 
        interpolation from the rough points and calculates its value and 
        the value of its derivative in nPntsFine equispaced.
        '''
        tck, u = interpolate.splprep(self.roughCoordMtr, k=kgrade,s=smoothness)
        x_knots, y_knots,z_knots = interpolate.splev(np.linspace(0, 1, nPntsFine), tck,der=0)
        self.fineCoordMtr=np.array([x_knots, y_knots,z_knots])
        x_der, y_der,z_der = interpolate.splev(np.linspace(0, 1,nPntsFine), tck,der=1)
        self.fineDerivMtr=np.array([x_der, y_der,z_der])
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
        '''Creates the nodes and elements of the tendon and appends them to a set.

        :param preporcessor: preprocessor
        :param materialName: name of the material
        :param elemTypeName: name of the type of element
        :param crdTransfName: name of the coordinate transformation
        :param areaTendon:   area of the cross-section of the tencon
        :param setName:      name of the set to which append the nodes and elements
                             created (if the set doesn't exist, it's created)
        '''
        setsMng=preprocessor.getSets
        if setsMng.exists(setName):
            tendonSet=setsMng.getSet(setName)
        else:
            tendonSet=setsMng.defSet(setName)
        nodes= preprocessor.getNodeLoader
        elems= preprocessor.getElementLoader
        elems.dimElem= preprocessor.getNodeLoader.dimSpace
        elems.defaultMaterial=tendonMaterialName
        elems.defaultTransformation=crdTransfName
        nEnd2=nodes.newNodeXYZ(fineCoordMtr[0][0],fineCoordMtr[1][0],fineCoordMtr[2][0])
        tendonSet.getNodes.append(nEnd2)
        for i in range(1,len(self.fineCoordMtr[0])):
            nEnd1=nodes.getNode(nEnd2.tag)
            nEnd2=nodes.newNodeXYZ(fineCoordMtr[0][0],fineCoordMtr[1][0],fineCoordMtr[2][0])
            elem=elements.newElement(elemTypeName,xc.ID([nEnd1.tag,nEnd2.tag]))
            elem.area=areaTendon
            tendonSet.getNodes.append(nEnd2)
            tendonSet.getElements.append(elem)
        return tendonSet
            

    def getLossFriction(self,coefFric,uninDev,sigmaP0_extr1=0.0,sigmaP0_extr2=0.0):
        '''Return for each point in fineCoordMtr the cumulative immediate loss 
        of prestressing due to friction.

        :param sigmaP0_extr1: maximum stress applied at the extremity 1 of 
                        the tendon (starting point) (stress refers to its 
                        value at the end of the anchorage, in the contact
                        with concrete). Defaults to 0.0 (no prestress applied)
        :param sigmaP0_extr2: idem for prestress applied at extremity 2
                         (end point of the tendon). Defaults to 0.0 (no 
                         prestress applied)
        :param coefFric: coefficient of friction between the tendon and its 
                        sheathing
        :param uninDev: unintentional angular deviation (rad/m tendon)
        '''
        if (sigmaP0_extr1 != 0.0):
            cum_len=self.getCumLength()
            cum_angl=self.getCumAngle()
            loss_frict_ext1=np.array([sigmaP0_extr1*(1-math.exp(-coefFric*(cum_angl[i]+uninDev*cum_len[i]))) for i in range(len(cum_angl))])
        if (sigmaP0_extr2 != 0.0):
            cum_len=self.getReverseCumLength()
            cum_angl=self.getReverseCumAngle()
            loss_frict_ext2=np.array([sigmaP0_extr2*(1-math.exp(-coefFric*(cum_angl[i]+uninDev*cum_len[i]))) for i in range(len(cum_angl))])
        if (sigmaP0_extr1 != 0.0) and (sigmaP0_extr2 != 0.0):
            self.lossFriction=np.maximum(loss_frict_ext1,loss_frict_ext2)
        elif (sigmaP0_extr1 != 0.0):
            self.lossFriction=loss_frict_ext1
        elif (sigmaP0_extr2 != 0.0):
            self.lossFriction=loss_frict_ext2
        else:
            lmsg.warning("No prestressing applied.")
        return

    def plot3D(self,fileName='plot.png',symbolRougPoints=None,symbolFinePoints=None,symbolTendon=None,symbolLossFriction=None):
        '''Plot in a 3D graphic the results to which a symbol is assigned.
        Symbol examples: 'r-': red solid line, 'mo': magenta circle, 'b--': blue dashes, 'ks':black square,'g^' green triangle_up, 'c*': cyan star, ...
        '''
        fig = plt.figure()
        ax3d = fig.add_subplot(111, projection='3d')
        if symbolRougPoints:
            ax3d.scatter(self.roughCoordMtr[0],self.roughCoordMtr[1],self.roughCoordMtr[2],symbolRougPoints,label='Rough points')
        if symbolFinePoints:
            ax3d.plot(self.fineCoordMtr[0],self.fineCoordMtr[1],self.fineCoordMtr[2],symbolFinePoints,label='Interpolated points (spline)')
        if symbolTendon:
            ax3d.plot(self.fineCoordMtr[0],self.fineCoordMtr[1],self.fineCoordMtr[2],symbolTendon,label='Tendon')
        if symbolLossFriction:
            ax3d.plot(self.fineCoordMtr[0],self.fineCoordMtr[1],self.lossFriction,symbolLossFriction,label='Immediate loss due to friction')
        ax3d.legend()
        ax3d.set_xlabel('X')
        ax3d.set_ylabel('Y')
        ax3d.set_zlabel('Z')
        fig.savefig(fileName)
        return

    def plot2D(self,XaxisValues='X',fileName='plot.png',symbolRougPoints=None,symbolFinePoints=None,symbolTendon=None,symbolLossFriction=None):
        '''Plot in a 2D graphic the results to which a symbol is assigned.
        Symbol examples: 'r-': red solid line, 'mo': magenta circle, 'b--': blue dashes, 'ks':black square,'g^' green triangle_up, 'c*': cyan star, ...
        :param XaxisValues: ='X' (default) to represent in the diagram X-axis
                              the X coordinates of the tendon,
                            ='Y' to represent in the diagram X-axis 
                              the Y coordinates of the tendon,
                            ='S' to represent in the diagram X-axis 
                              the curviline coordinates of the tendon 
                              (sqrt(X**2+Y**2)
        '''
        if XaxisValues.upper()=='X':
            XaxisCoord=self.fineCoordMtr[0]
            XaxisRoughCoord=self.roughCoordMtr[0]
            xLab='X'
        elif XaxisValues.upper()=='Y':
            XaxisCoord=self.fineCoordMtr[1]
            YaxisRoughCoord=self.roughCoordMtr[1]
            xLab='Y'
        elif XaxisValues.upper()=='S':
            XaxisCoord=(self.fineCoordMtr[0]**2+self.fineCoordMtr[1]**2)**(0.5)
            XaxisRoughCoord=(self.roughCoordMtr[0]**2+self.roughCoordMtr[1]**2)**(0.5)
            xLab='S'
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
        if symbolLossFriction:
            ax2d.plot(XaxisCoord,self.lossFriction,symbolLossFriction,label='Immediate loss due to friction')
        ax2d.legend()
        ax2d.set_xlabel(xLab)
        fig.savefig(fileName)
        return

def angle_between(a,b):
  arccosInput = np.dot(a,b)/np.linalg.norm(a)/np.linalg.norm(b)
  arccosInput = 1.0 if arccosInput > 1.0 else arccosInput
  arccosInput = -1.0 if arccosInput < -1.0 else arccosInput
  return math.acos(arccosInput)
