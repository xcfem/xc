# -*- coding: utf-8 -*-
''' Nonlinear non-linear spring model to analyze pile wall structutres.'''

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2024, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import xc
from geotechnics import earth_pressure
from scipy.constants import g
from model import predefined_spaces
from solution import predefined_solutions

from operator import itemgetter
from scipy.interpolate import interp1d
from postprocess.reports import common_formats as cf
import matplotlib.pyplot as plt
import matplotlib.ticker as plticker

def get_results_table(resultsDict):
    ''' Return the given results in tabular format.

    :param resultsDict: dictionary containing the results.
    '''
    headerRow= ['#', 'fixed node', 'depth (m)', 'Ux (mm)', 'M (kN.m)', 'V (kN)', 'pres. dif. (kN/m)', 'Rx (kN)', 'Ea (kN)', 'E0 (kN)', 'Ep (kN)']
    retval= list()
    for nodeTag in resultsDict:
        nodeResults= resultsDict[nodeTag]
        outputRow= [str(nodeTag)] # node tag of the pile wall.
        outputRow.append(str(nodeResults['fixed_node'])) # identifier of the fixed node.
        outputRow.append(cf.Length.format(nodeResults['depth'])) # depth of the node.
        outputRow.append(cf.Length.format(nodeResults['Ux']*1e3)) # displacement of the node.    
        outputRow.append(cf.Force.format(nodeResults['M']/1e3)) # bending moment at the node.
        outputRow.append(cf.Force.format(nodeResults['V']/1e3)) # shear force at the node.
        outputRow.append(cf.Force.format(nodeResults['pDif']/1e3)) # shear force at the node.
        outputRow.append(cf.Force.format(nodeResults['Rx']/1e3)) # reaction of the soil.
        outputRow.append(cf.Force.format(nodeResults['leftEa']/1e3)) # active force at the left side.
        outputRow.append(cf.Force.format(nodeResults['leftE0']/1e3)) # at-rest force at the left side.
        outputRow.append(cf.Force.format(nodeResults['leftEp']/1e3)) # passive force at the left side.
        outputRow.append(cf.Force.format(nodeResults['rightEa']/1e3)) # active force at the right side.
        outputRow.append(cf.Force.format(nodeResults['rightE0']/1e3)) # at-rest force at the right side.
        outputRow.append(cf.Force.format(nodeResults['rightEp']/1e3)) # passive force at the right side.
        retval.append(outputRow)
    # Sort on depth
    retval= sorted(retval, key=lambda x: float(x[2]))
    retval.insert(0, headerRow)
    return retval

def plot_results(resultsDict, title= None):
    ''' Return the given results in tabular format.

    :param resultsDict: dictionary containing the results.
    :param title: title.
    '''
    def get_results(resultsDict, x_field:str, x_scale_factor:float, y_field= 'depth'):
        ''' Extract the results from the given dictionary and sort them on
            depth.

        :param resultsDict: dictionary containing the analysis results.
        :param x_field: values for the x axis.
        :param x_scale_factor: scale factor on the x axis.
        :param y_field: values por the y axis.
        '''
        xy= list()
        for nodeTag in resultsDict:
            nodeResults= resultsDict[nodeTag]
            depth= nodeResults[y_field]
            xValue= nodeResults[x_field]
            xy.append((xValue*x_scale_factor, depth))
        ## Sort on depth
        xy= sorted(xy, key=lambda x: float(x[1]))
        ## Unzip the tuples.
        return zip(*xy)

    def plot_diagram(ax, resultsDict, title:str, x_field:str, x_scale_factor:float, x_label:str, y_field= 'depth', y_label= None):
        ''' Extract the results from the given dictionary and sort them on
            depth.

        :param ax: matplotlib Axes object.
        :param resultsDict: dictionary containing the analysis results.
        :param title: title for the diagram.
        :param x_field: values for the x axis.
        :param x_scale_factor: scale factor on the x axis.
        :param x_label: label for the x axis.
        :param y_field: values por the y axis.
        :param y_label: label for the y axis.
        '''
        x, y= get_results(resultsDict= resultsDict, x_field= x_field, x_scale_factor= x_scale_factor, y_field= y_field)
        ax.plot(x, y, diagramsColor)
        ax.invert_yaxis()  # Reverse y-axis
        topPoint= (0.0, y[0])
        bottomPoint= (0.0, y[-1])
        ax.plot([bottomPoint[0], topPoint[0]], [bottomPoint[1], topPoint[1]], pileWallColor) # plot pile wall
        ratio= 4
        xleft, xright = ax.get_xlim()
        ybottom, ytop = ax.get_ylim()
        ax.set_aspect(abs((xright-xleft)/(ybottom-ytop))*ratio)
        if(x_field=='Ux'):
            loc = plticker.MultipleLocator(base=40.0) # this locator puts ticks at regular intervals
            ax.xaxis.set_major_locator(loc)

        if(y_label):
            ax.set(xlabel= x_label, ylabel= y_label)
        else:
            ax.set(xlabel= x_label)
        ax.set_title(title)
        ax.grid()
        
    fig, (disp, moment, shear, presDif, soilReact) = plt.subplots(1, 5)
    pileWallColor= 'tab:blue'
    diagramsColor= 'tab:red'
    # Plot displacements.
    plot_diagram(ax= disp, resultsDict= resultsDict, title= 'Displacements', x_field= 'Ux', x_scale_factor= 1e3, x_label= 'Ux (mm)', y_field= 'depth', y_label= 'Depth (m)')
    
    # Plot bending moment.
    plot_diagram(ax= moment, resultsDict= resultsDict, title= 'Moment', x_field= 'M', x_scale_factor= 1e-3, x_label= '$M (kN \cdot m)$', y_field= 'depth', y_label= None)
    
    # Plot shear forces.
    plot_diagram(ax= shear, resultsDict= resultsDict, title= 'Shear', x_field= 'V', x_scale_factor= 1e-3, x_label= '$V (kN)$', y_field= 'depth', y_label= None)
    
    # Plot pDif.
    plot_diagram(ax= presDif, resultsDict= resultsDict, title= 'Pres. Dif.', x_field= 'pDif', x_scale_factor= 1e-3, x_label= '$pD (kN/m)$', y_field= 'depth', y_label= None)
    
    # Plot soil reactions.
    plot_diagram(ax= soilReact, resultsDict= resultsDict, title= 'Soil Reac.', x_field= 'Rx', x_scale_factor= 1e-3, x_label= '$Rx (kN)$', y_field= 'depth', y_label= None)

    if(title):
        fig.suptitle(title)
    plt.show()

class SoilLayers(object):
    '''Layers of different soils.

    Soil with layers of different properties as described in
    4.5.5.7 "Guía de cimentaciones en obras de carreteras"
    (https://books.google.ch/books?id=a0eoygAACAAJ)
    2009

    :ivar depths: (float list) layer depths.
    :ivar soils: soil objects for each layer.
    :ivar leftWaterTableDepthIndex: index of the left water table depth in self.depths.
    :ivar rightWaterTableDepthIndex: index of the right water table depth in self.depths.
    :ivar excavationDepthIndexes: index of the excavation depth in self.depths.
    '''
    def __init__(self, depths, soils, waterTableDepth= [None, None], excavationDepths= None):
        '''Constructor.

        :param depths: (float list) layer depths.
        :param soils: soil objects for each layer.
        :param waterTableDepth: depth of the water table.
        '''
        self.depths= depths
        self.soils= soils
        self.setWaterTableDepth(waterTableDepth= waterTableDepth)
        self.setExcavationDepths(excavationDepths= excavationDepths)

    def getTotalDepth(self):
        ''' Return the distance from the deepest to the shallwest depth in the
            contariner.
        '''
        return self.depths[-1]-self.depths[0]
        
    def splitAtDepth(self, newDepth= None):
        ''' Recomputes the depths and soils list to take account of
            the given one.
        :param newDepth: depth of interest.
        '''
        retval= -1
        if(newDepth):
            calculationDepths= list()
            calculationSoils= list()
            newDepthIndex= self.getSoilIndexAtDepth(depth= newDepth)
            # Check if already in depths list.
            soilDepth= self.depths[newDepthIndex]
            totalDepth= self.getTotalDepth()
            tol= totalDepth/1e3
            if(abs(soilDepth-newDepth)>tol): # not in depths list.
                for i, (d, soil) in enumerate(zip(self.depths, self.soils)):
                    calculationDepths.append(d)
                    calculationSoils.append(soil)
                    if(i==newDepthIndex):
                        calculationDepths.append(newDepth)
                        calculationSoils.append(soil)
                        retval= newDepthIndex+1
                self.depths= calculationDepths
                self.soils= calculationSoils
            else:
                retval= newDepthIndex
        return retval
            
    def setWaterTableDepth(self, waterTableDepth= [None, None]):
        ''' Recomputes the depths and soils list to take account of
            the water table.

        :param waterTableDepth: depth of the water table.
        '''
        self.leftWaterTableDepthIndex= self.splitAtDepth(waterTableDepth[0])
        self.rightWaterTableDepthIndex= self.splitAtDepth(waterTableDepth[1])
        
    def setExcavationDepths(self, excavationDepths= None):
        ''' Recomputes the depths and soils list to take account of
            the water table.

        :param excavationDepth: depth of the water table.
        '''
        if(excavationDepths):
            self.excavationDepthIndexes= list()
            for excavationDepth in excavationDepths:
                excavationDepthIndex= self.splitAtDepth(excavationDepth)
                self.excavationDepthIndexes.append(excavationDepthIndex)
        
    def getLeftWaterTableDepth(self):
        ''' Return the index that corresponds to the depth of the water table at            the left of the pile wall.
        '''
        retval= 6378e3
        if(self.leftWaterTableDepthIndex>0):
            retval= self.depths[self.leftWaterTableDepthIndex]
        return retval
    
    def getRightWaterTableDepth(self):
        ''' Return the index that corresponds to the depth of the water table at            the right of the pile wall.
        '''
        retval= 6378e3
        if(self.rightWaterTableDepthIndex>0):
            retval= self.depths[self.rightWaterTableDepthIndex]
        return retval
    
    def getWaterTableDepths(self):
        ''' Return he depth of the water table  at both sides [left, right] of
            the pile wall.
        '''

        return [self.getLeftWaterTableDepth(), self.getRightWaterTableDepth()]

    def getWaterTableDepth(self, rightSide:bool):
        ''' Return he depth of the water table at the given side of the pile
            wall.

        :param rightSide: if true consider the water pressure at the right side 
                          of the wall, otherwise, consider the water pressure at
                          the left side.
        '''        
        if(rightSide): 
            retval= self.getRightWaterTableDepth()
        else:
            retval= self.getLeftWaterTableDepth()
        return retval
        
    def getExcavationDepth(self, i):
        ''' Return the index that corresponds to the i-th excavation depth.
        '''
        retval= 6378e3
        sz= len(self.excavationDepthIndexes)
        if((sz>0) and (i<sz)):
            excavationDepthIndex= self.excavationDepthIndexes[i]
            if(excavationDepthIndex>0):
                retval= self.depths[excavationDepthIndex]
        return retval

    def getDepths(self):
        ''' Return the depths of the different soil strata.'''
        return self.depths

    def getSoilIndexAtDepth(self, depth):
        ''' Return the index of the soil corresponding to the given depth.

        :param depth: depth of interest.
        '''
        retval= -1
        for i, d in enumerate(self.depths):
            if(depth>=d):
                retval= i
        return retval

    def getSoilAtDepth(self, depth):
        ''' Return the soil corresponding to the given depth.

        :param depth: depth of interest.
        '''
        retval= None
        soilIndex= self.getSoilIndexAtDepth(depth= depth)
        if(soilIndex>=0):
            retval= self.soils[soilIndex]
        return retval
    
    def getNetHydrostaticPressureAtDepth(self, depth):
        ''' Returns the net hydrostatic presure at the given depth.

        :param depth: depth to compute the pressure at.
        '''
        retval= 0.0
        waterUnitWeight= 1e3*g
        # Pressure to the right.
        leftWaterTableDepth= self.getLeftWaterTableDepth()
        if(depth>leftWaterTableDepth):
            retval+= waterUnitWeight*(depth-leftWaterTableDepth) # to the right.
        # Pressure to the left.
        rightWaterTableDepth= self.getRightWaterTableDepth()
        if(depth>rightWaterTableDepth):
            retval-= waterUnitWeight*(depth-rightWaterTableDepth) # to the left.
        return retval

    def getVerticalPressureAtDepth(self, depth, rightSide:bool):
        ''' Returns the vertical presure at the given depth.

        :param depth: depth to compute the pressure at.
        :param rightSide: if true consider the water pressure at the right side 
                          of the wall, otherwise, consider the water pressure at
                          the left side.
        '''
        retval= 0.0
        if(depth>self.depths[0]):
            lastDepth= self.depths[0]
            for i, d in enumerate(self.depths):
                soil= self.soils[i]
                gamma= soil.gamma()
                currentDepth= min(depth, d)
                if(currentDepth>self.getWaterTableDepth(rightSide= rightSide)):
                    gamma= soil.submergedGamma()
                soilThickness= currentDepth-lastDepth
                retval+= gamma*soilThickness
                if(abs(currentDepth-depth)<1e-6):
                    break
                lastDepth= d
        return retval
        
    def getHorizontalPressureAtDepth(self, K, depth, rightSide:bool):
        ''' Returns the horizontal presure at the given depth.

        :param K: pressure coefficient.
        :param depth: depth to compute the pressure at.
        :param rightSide: if true consider the water pressure at the right side 
                          of the wall, otherwise, consider the water pressure at
                          the left side.
        '''
        retval= self.getVerticalPressureAtDepth(depth= depth, rightSide= rightSide)*K
        retval+= getNetHydrostaticPressureAtDepth(depth= depth)
        return retval

    
class PileWall(object):
    '''Pile wall analysis using non-linear spring to represent soil-structure
       interaction.

    :ivar pileSection: 2D elastic shear section for the pile wall beam elements.
    :ivar soilLayers: SoilLayers object.
    '''
    
    def __init__(self, pileSection, soilLayersDepths, soilLayers, excavationDepths, pileSpacing= 1.0, waterTableDepth= [None, None]):
        '''Constructor.

        :param pileSection: 2D elastic shear section for the pile wall beam
                            elements.
        :param soilLayersDepths: (float list) layer depths.
        :param soilLayers: soil object for each layer.
        :param excavationDepths: list of excavation depths.
        :param pileSpacing: distance between pile axes.
        :param waterTableDepth: [float, float] depth of the water table at both
                                sides of the pile wall [left, right].
        '''
        self.pileSection= pileSection
        self.soilLayers= SoilLayers(depths= soilLayersDepths, soils= soilLayers, waterTableDepth= waterTableDepth, excavationDepths= excavationDepths)
        self.pileSpacing= pileSpacing
        self.solProc= None

    def defineFEProblem(self):
        ''' Define the FE problem.

        '''
        self.feProblem= xc.FEProblem()
        preprocessor=  self.feProblem.getPreprocessor   
        nodes= preprocessor.getNodeHandler
        ## Problem type
        self.modelSpace= predefined_spaces.StructuralMechanics2D(nodes)
        
    def defineSolutionProcedure(self, convergenceTestTol= 1e-5):
        ''' Define the solution procedure.

        :param convergenceTestTol: tolerance for the convergence test of the
                                   solver.
        '''
        # Solution procedure.
        #solProc= predefined_solutions.PenaltyKrylovNewton(prb= feProblem, numSteps= numSteps, maxNumIter= 300, convergenceTestTol= convergenceTestTol, printFlag= 0)
        self.solProc= predefined_solutions.PenaltyNewtonRaphson(prb= self.feProblem, numSteps= 10, maxNumIter= 300, convergenceTestTol= convergenceTestTol, printFlag= 0)
        
    def setNodeSoils(self):
        ''' Compute the soil corresponding to each node.'''
        self.soilsAtNodes= dict()
        for n in self.pileSet.nodes:
            nodeDepth= -n.getInitialPos3d.y
            nodeSoil= self.soilLayers.getSoilAtDepth(nodeDepth)
            self.soilsAtNodes[n.tag]= nodeSoil
            
    def genMesh(self):
        '''Define the FE mesh.
        '''
        # Define finite element problem.
        self.defineFEProblem()
        preprocessor= self.feProblem.getPreprocessor
        
        # Problem geometry
        lines= list()
        kPoints= list()
        for depth in self.soilLayers.getDepths():
            kPoints.append(self.modelSpace.newKPoint(0,-depth,0))
        kPt0= kPoints[0]
        for kPt1 in kPoints[1:]:
            newLine= self.modelSpace.newLine(kPt0, kPt1)
            newLine.setElemSize(0.25)
            kPt0= kPt1
            lines.append(newLine)
        
        # Mesh generation
        ## FE material.
        xcPileSection= self.pileSection.defElasticShearSection2d(preprocessor)
        ## Geometric transformations
        lin= self.modelSpace.newLinearCrdTransf("lin")
        ## Seed element
        seedElemHandler= preprocessor.getElementHandler.seedElemHandler
        seedElemHandler.dimElem= 2 # Bars defined in a two-dimensional space.
        seedElemHandler.defaultMaterial= xcPileSection.name
        seedElemHandler.defaultTransformation= lin.name
        beam2d= seedElemHandler.newElement("ElasticBeam2d")
        # beam2d.h= diameter
        self.pileSet= self.modelSpace.defSet('pileSet')
        for ln in lines:
            ln.genMesh(xc.meshDir.I)
            self.pileSet.lines.append(ln)
        self.pileSet.fillDownwards()

        ## Constraints.
        bottomNode= kPoints[-1].getNode()
        self.modelSpace.fixNodeF0F(bottomNode.tag) # Fix vertical displacement.

        ### Define soil response diagrams.
        soilResponseMaterials= dict()
        self.tributaryAreas= dict()
        #### Compute tributary lengths.
        self.pileSet.resetTributaries()
        self.pileSet.computeTributaryLengths(False) # Compute tributary lenghts.
        #### Compute soils at nodes.
        self.setNodeSoils()
        #### Minimum depth.
        minimumDepth= -self.getTopPoint().getPos.y
        #### Define non-linear springs.
        for n in self.pileSet.nodes:
            nodeDepth= -n.getInitialPos3d.y
            left_sg_v= self.soilLayers.getVerticalPressureAtDepth(depth= nodeDepth, rightSide= False) # Vertical pressure at left.
            right_sg_v= self.soilLayers.getVerticalPressureAtDepth(depth= nodeDepth, rightSide= True) # Vertical pressure at right.
            leftNonLinearSpringMaterial= None
            rightNonLinearSpringMaterial= None
            tributaryArea= 0.0
            if(nodeDepth>minimumDepth): # Avoid zero soil response.
                tributaryLength= n.getTributaryLength()
                tributaryArea= tributaryLength*self.pileSpacing
                materialName= 'soilResponse_z_'+str(n.tag)
                nodeSoil= self.soilsAtNodes[n.tag]
                leftNonLinearSpringMaterial= nodeSoil.defHorizontalSubgradeReactionNlMaterial(preprocessor, name= materialName+'_left', sg_v= left_sg_v, tributaryArea= tributaryArea, Kh= nodeSoil.Kh)
                rightNonLinearSpringMaterial= nodeSoil.defHorizontalSubgradeReactionNlMaterial(preprocessor, name= materialName+'_right', sg_v= right_sg_v, tributaryArea= tributaryArea, Kh= nodeSoil.Kh)

            self.tributaryAreas[n.tag]= tributaryArea
            soilResponseMaterials[n.tag]= (leftNonLinearSpringMaterial, rightNonLinearSpringMaterial)

        ### Duplicate nodes below ground level.
        self.springPairs= list()
        for n in self.pileSet.nodes:
            nodeTag= n.tag
            if(nodeTag in soilResponseMaterials):
                newNode= self.modelSpace.duplicateNode(n)
                self.modelSpace.fixNode000(newNode.tag)
                self.springPairs.append((n, newNode))

        ### Define Spring Elements
        elements= preprocessor.getElementHandler
        elements.dimElem= 2 #Element dimension.
        self.leftZLElements= dict()
        self.rightZLElements= dict()
        for i, pair in enumerate(self.springPairs):
            nodeTag= pair[0].tag
            (leftSoilResponseMaterial, rightSoilResponseMaterial)= soilResponseMaterials[nodeTag]
            if(leftSoilResponseMaterial): # Spring at left defined for this node.
                # Material for the left spring
                elements.defaultMaterial= leftSoilResponseMaterial.name
                # Springs on the left side of the beam
                zlLeft= elements.newElement("ZeroLength",xc.ID([pair[1].tag, pair[0].tag]))
                zlLeft.setupVectors(xc.Vector([-1,0,0]),xc.Vector([0,-1,0]))
                self.leftZLElements[nodeTag]= zlLeft
            if(rightSoilResponseMaterial): # Spring at right defined for this node.
                # Material for the right spring
                elements.defaultMaterial= rightSoilResponseMaterial.name
                # Springs on the right side of the beam
                zlRight= elements.newElement("ZeroLength",xc.ID([pair[0].tag, pair[1].tag]))
                zlRight.setupVectors(xc.Vector([1,0,0]),xc.Vector([0,1,0]))
                self.rightZLElements[nodeTag]= zlRight

    def getTopPoint(self):
        ''' Return the top point of the pile wall model.'''
        retval= None
        points= list(self.pileSet.points)
        sz= len(points)
        if(sz>0):
            retval= points[0]
            yMax= retval.getPos.y
            for pt in points[1:]:
                y= pt.getPos.y
                if(y>yMax):
                    retval= pt
                    yMax= y
        return retval
        
    def getTopNode(self):
        ''' Return the top node of the pile wall mesh.'''
        topPoint= self.getTopPoint()
        return topPoint.getNode()
    
    def getNodeAtDepth(self, depth):
        ''' Return the nearest node to the given depth.'''
        pileNodes= list(self.pileSet.nodes)
        retval= None
        sz= len(pileNodes)
        if(sz>0):
            retval= pileNodes[0]
            nodeDepth= -retval.getInitialPos3d.y
            dMin= abs(nodeDepth-depth)
            for n in pileNodes[1:]:
                nodeDepth= -n.getInitialPos3d.y
                d= abs(nodeDepth-depth)
                if(d<dMin):
                    retval= n
                    dMin= d
        return retval, dMin
        
    def updateSpringStiffness(self, remainingLeftElements, currentExcavationDepth, excavationSide):
        ''' Update the stiffness of the remaining materials after each 
            excavation step.

        :param remainingLeftElements: elements that remain "alive".
        :param currentExcavationDepth: current excavation depth.
        :param excavationSide: side for the excavation ('left' or 'right')
        '''
        updatedElements= list()
        rightSide= (excavationSide=='right')
        sg_v0= self.soilLayers.getVerticalPressureAtDepth(depth= currentExcavationDepth, rightSide= rightSide) # Vertical stress to remove at the excavation side.
        for nodeTag in remainingLeftElements:
            leftElement= remainingLeftElements[nodeTag]
            elemNodes= leftElement.nodes
            # Get the node depth.
            nodeIndex= 1
            if(nodeTag==elemNodes[0].tag):
                nodeIndex= 0
            pileNode= elemNodes[nodeIndex]
            nodeDepth= -pileNode.getInitialPos3d.y
            newDepth= nodeDepth-currentExcavationDepth
            # Compute new soil response.
            soil= self.soilsAtNodes[nodeTag]
            sg_v= self.soilLayers.getVerticalPressureAtDepth(depth= nodeDepth, rightSide= rightSide)-sg_v0 # Vertical pressure at the excavation side.
            newEa, newE0, newEp= soil.getEarthThrusts(sg_v= sg_v, tributaryArea= self.tributaryAreas[nodeTag])
            # Update soil response.
            leftElementInitStrainMaterial= leftElement.getMaterials()[0]
            leftElementEyBasicMaterial= leftElementInitStrainMaterial.material

            leftElementEyBasicMaterial.setParameters(soil.Kh, -newEp, -newEa)
            leftElementInitStrainMaterial.setInitialStress(-newE0)
            updatedElements.append(leftElement)
            #print('node: ', nodeTag, ' node depth: ', '{:.2f}'.format(nodeDepth), ' left node depth: ', '{:.2f}'.format(newDepth), ' tributary area: ', '{:.2f}'.format(tributaryAreas[nodeTag]), 'strains: ', oldInitStrain, -newInitStrain, newInitStrain+oldInitStrain, ' elementTag= ', leftElement.tag)
        return updatedElements
    
    def excavationProcess(self, excavationSide, excavationDepthIndex):
        ''' Deactivates the excavated elements and updates the stiffness of the
            remaining ones.

        :param excavationSide: side for the excavation ('left' or 'right')
        :param excavationDepthIndex: index of the excavation depth to reach.
        '''
        self.nodesToExcavate= list() # Nodes in the excavation depth.
        self.currentExcavationDepth= self.soilLayers.getExcavationDepth(i= excavationDepthIndex)
        for n in self.pileSet.nodes:
            nodeDepth= -n.getInitialPos3d.y
            if(nodeDepth<=self.currentExcavationDepth):
                self.nodesToExcavate.append((nodeDepth, n))
        ## Sort nodes to excavate on its depth
        self.nodesToExcavate.sort(key=itemgetter(0))
        ## Elements on excavation side.
        elementsOnExcavationSide= None
        if(excavationSide=='left'):
            elementsOnExcavationSide= self.leftZLElements
        elif(excavationSide=='right'):
            elementsOnExcavationSide= self.rightZLElements
        else:
            lmsg.error("Excavation side can be 'left' or 'right'.")
            exit(1)
        ## Elements to deactivate.
        remainingLeftElements= elementsOnExcavationSide
        for tp in self.nodesToExcavate:
            currentExcavationDepth= tp[0]
            if(currentExcavationDepth>self.currentExcavationDepth): # if excavation depth is reached, stop.
                break
            node= tp[1]
            nodeTag= node.tag
            leftSpring= None
            if(nodeTag in remainingLeftElements): # left spring still exists.
                leftSpring= remainingLeftElements[nodeTag]
                if(leftSpring):
                    # remove the spring.
                    toKill= self.modelSpace.defSet('kill'+str(leftSpring.tag))
                    toKill.getElements.append(leftSpring)
                    toKill.killElements()
                    remainingLeftElements.pop(nodeTag) # remove it from the dictionary.
                    ok= self.solProc.solve()
                    if(ok!=0):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.error(className+'.'+methodName+'; can\'t solve.')
                        exit(1)
                    # Update left springs.
                    updatedElements= self.updateSpringStiffness(remainingLeftElements, currentExcavationDepth= currentExcavationDepth, excavationSide= excavationSide)
                    # Solve again.
                    ok= self.solProc.solve()
                    if(ok!=0):
                        className= type(self).__name__
                        methodName= sys._getframe(0).f_code.co_name
                        lmsg.error(className+'.'+methodName+'; can\'t solve.')
                        exit(1)
        return updatedElements
                
    def solve(self, excavationDepthIndex= 0, convergenceTestTol= 1e-5, reactionCheckTolerance= 1e-6, excavationSide= 'left'):
        '''Compute the solution.

        :param excavationDepthIndex: index of the excavation depth to reach.
        :param convergenceTestTol: tolerance for the convergence test of the
                                   solver.
        :param reactionCheckTolerance: tolerance when checking nodal reactions.
        :param excavationSide: side for the excavation ('left' or 'right')
        '''
        if(not self.solProc):
            self.defineSolutionProcedure(convergenceTestTol= convergenceTestTol)
        ok= self.solProc.solve()
        if(ok!=0):
            lmsg.error('Can\'t solve')
            exit(1)
            
        updatedElements= self.excavationProcess(excavationSide= excavationSide, excavationDepthIndex= excavationDepthIndex)
        self.modelSpace.calculateNodalReactions(reactionCheckTolerance= reactionCheckTolerance)

    def computeExcavationSide(self):
        ''' Returns the excavation side according to the sizes of the element
            containers at each side of the pile wall.'''
        retval= None
        if(len(self.leftZLElements)<len(self.rightZLElements)):
            retval= 'left'
        else:
            retval= 'right'
        return retval
    
    def getEarthThrustsAtNode(self, node):
        ''' Extracts the values of the earth thrusts at the given node.

        :param node: node of interest.
        '''
        nodeTag= node.tag
        tributaryArea= self.tributaryAreas[nodeTag]
        leftMaterial= None
        if(nodeTag in self.leftZLElements):
            leftElement= self.leftZLElements[nodeTag]
            leftMaterial= leftElement.getMaterials()[0].material
        rightMaterial= None
        if(nodeTag in self.rightZLElements):
            rightElement= self.rightZLElements[nodeTag]
            rightMaterial= rightElement.getMaterials()[0].material
        if(leftMaterial):
            leftEp= -leftMaterial.getLowerYieldStress()*tributaryArea
            leftEa= -leftMaterial.getUpperYieldStress()*tributaryArea
        else:
            leftEa= 0.0
            leftEp= 0.0
        if(rightMaterial):
            rightEp= -rightMaterial.getLowerYieldStress()*tributaryArea
            rightEa= -rightMaterial.getUpperYieldStress()*tributaryArea
        else:
            rightEa= 0.0
            rightEp= 0.0
        excavationSide= self.computeExcavationSide()
        depth= -node.getInitialPos3d.y
        if(excavationSide=='left'):
            leftDepth= max(depth-self.currentExcavationDepth, 0.0)
            rightDepth= depth
            left_sg_v0= self.soilLayers.getVerticalPressureAtDepth(depth= self.currentExcavationDepth, rightSide= False) # Vertical stress to remove at the excavation side.
            right_sg_v0= 0.0
        else: # excavation at right.
            rightDepth= max(depth-self.currentExcavationDepth, 0.0)
            sg_v0= self.soilLayers.getVerticalPressureAtDepth(depth= self.currentExcavationDepth, rightSide= True) # Vertical stress to remove at the excavation side.
            leftDepth= depth
            left_sg_v0= 0.0
            right_sg_v0= self.soilLayers.getVerticalPressureAtDepth(depth= self.currentExcavationDepth, rightSide= True) # Vertical stress to remove at the excavation side.
        soil= self.soilsAtNodes[nodeTag]
        if(leftDepth>0):
            left_sg_v= self.soilLayers.getVerticalPressureAtDepth(depth= depth, rightSide= False)-left_sg_v0 # Vertical pressure at left.
            leftE0= soil.getAtRestPressure(sg_v= left_sg_v)
        else:
            leftE0= 0.0
        if(rightDepth>0.0):
            right_sg_v= self.soilLayers.getVerticalPressureAtDepth(depth= depth, rightSide= True)-right_sg_v0 # Vertical pressure at right.
            rightE0= soil.getAtRestPressure(sg_v= right_sg_v)
        else:
            rightE0= 0.0
        
        return (leftEa, leftE0, leftEp), (rightEa, rightE0, rightEp)
        
    def getResultsDict(self):
        ''' Extracts earth pressures and internal forces from the model.'''
        retval= dict()
        for sp in self.springPairs:
            fixedNode= sp[1]
            Rx= fixedNode.getReaction[0]
            pileNode= sp[0]
            Ux= pileNode.getDisp[0]
            depth= -fixedNode.getInitialPos3d.y
            soil= self.soilsAtNodes[pileNode.tag]
            tributaryArea= self.tributaryAreas[pileNode.tag]
            
            (leftEa, leftE0, leftEp), (rightEa, rightE0, rightEp)= self.getEarthThrustsAtNode(pileNode)
            nodeResults= {'depth': depth, 'fixed_node':fixedNode.tag, 'Rx':Rx, 'leftE0':leftE0, 'leftEa':leftEa, 'leftEp':leftEp, 'rightE0':rightE0, 'rightEa':rightEa, 'rightEp':rightEp, 'Ux':Ux}
            retval[pileNode.tag]= nodeResults
        # Get internal forces.
        for ln in self.pileSet.lines: # for lines in list
            for e in ln.elements: # for elements in line.
                nodeTag= e.getNodes[0].tag
                nodeResults= retval[nodeTag]
                depth= nodeResults['depth']
                # M2= e.getM2
                nodeResults['M']= e.getM1 # bending moment.
                nodeResults['V']= e.getV1 # shear force.    
        # Get the moment in the deepest node.
        nodeTag= e.getNodes[1].tag
        retval[nodeTag]['M']= e.getM2 # bending moment.
        retval[nodeTag]['V']= e.getV2 # shear force.

        # Compute pres. dif.
        x= list()
        y= list()
        for ln in self.pileSet.lines: # for lines in list
            for e in ln.elements: # for elements in line.
                topNodeTag= e.getNodes[0].tag
                topDepth= retval[topNodeTag]['depth']
                V1= retval[topNodeTag]['V'] # shear force at top node.
                bottomNodeTag= e.getNodes[1].tag
                bottomDepth= retval[bottomNodeTag]['depth']
                V2= retval[bottomNodeTag]['V'] # shear force at bottom node.
                presDif= (V2-V1)/(bottomDepth-topDepth)
                avgDepth= (bottomDepth+topDepth)/2.0
                x.append(avgDepth)
                y.append(presDif)
        presDif= interp1d(x, y, kind='linear', fill_value= 'extrapolate', assume_sorted=True)
        for nodeTag in retval:
            nodeResults= retval[nodeTag]
            depth= nodeResults['depth']
            pDif= presDif(depth)
            nodeResults['pDif']= pDif
        return retval

