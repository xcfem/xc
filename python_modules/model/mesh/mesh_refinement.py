# -*- coding: utf-8 -*-
''' Mesh 3-refinement code as defined in: "Algorithms for Quadrilateral and Hexahedral Mesh Generation" Robert Schneiders.'''

from __future__ import print_function

__author__= "Luis C. Pérez Tato (LCPT) , Ana Ortega (AO_O) "
__copyright__= "Copyright 2024, LCPT, AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es, ana.Ortega@ciccp.es "

import xc

# Refinement templates.

# Point arrangement:
# 
#            13    14     15
#   12 +-----+-----+-----+
#      |                 |
#      |     9     10    |
#    8 +     +     +     + 11
#      |                 |
#      |     5     6     |
#    4 +     +     +     + 7
#      |                 |
#      |                 |
#   0  +-----+-----+-----+ 3
#            1     2

## Points.
refinementTemplatePoints_0= list()
for y in [-1, -1/3.0, 1/3.0, 1]:
    row= list()
    for x in [-1, -1/3.0, 1/3.0, 1]:
        row.append((x,y))
    refinementTemplatePoints_0+=row

### Rotated 90 degrees:
refinementTemplatePoints_90= list()
for p in refinementTemplatePoints_0:
    refinementTemplatePoints_90.append((-p[1], p[0]))
### Rotated 180 degrees:
refinementTemplatePoints_180= list()
for p in refinementTemplatePoints_0:
    refinementTemplatePoints_180.append((-p[0], -p[1]))
### Rotated 270 degrees:
refinementTemplatePoints_270= list()
for p in refinementTemplatePoints_180:
    refinementTemplatePoints_270.append((-p[1], p[0]))
refinementTemplatePoints= [refinementTemplatePoints_0, refinementTemplatePoints_90, refinementTemplatePoints_180, refinementTemplatePoints_270]
    
## Connectivity.
refinementTemplateConnectivity= dict()
refinementTemplateConnectivity['1']= [(0, 1, 5, 4), (1, 3, 15, 5), (4, 5, 15, 12)]
refinementTemplateConnectivity['2a']= [(0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (4, 5, 9, 12), (5, 6, 10, 9), (6, 7, 15, 10), (9, 10, 15, 12)] 
refinementTemplateConnectivity['2b']= [(0, 1, 5, 4),  (1, 3, 6, 5), (4, 5, 9, 12), (5, 6, 10, 9), (6, 3, 11, 10), (10, 11, 15, 14), (9, 10, 14, 12)]
refinementTemplateConnectivity['3']= [(0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (4, 5, 9, 12), (5, 6, 10, 9), (6, 7, 11, 10), (9, 10, 14, 12), (10, 11, 15, 14)]
refinementTemplateConnectivity['4']= [(0, 1, 5, 4), (1, 2, 6, 5), (2, 3, 7, 6), (4, 5, 9, 8), (5, 6, 10, 9), (6, 7, 11, 10), (8, 9, 13, 12), (9, 10, 14, 13),  (10, 11, 15, 14)]

def set_refinement_templates(xcSet):
    ''' Set the refinement templates to the elements of the given set.

    :param xcSet: set whose elements will be given a refinement template.
    '''
    ## Assign the refinement template (figure 53).
    for e in xcSet.elements:
        markedNodes= [0, 0, 0, 0]
        markedNodesIndices= list()
        for i, n in enumerate(e.nodes):
            nodeSubdivisionLevel= 0
            if(n.hasProp('subdivisionLevel')):
                nodeSubdivisionLevel= n.getProp('subdivisionLevel')
            if(nodeSubdivisionLevel>0):
                markedNodes[i]= 1
                markedNodesIndices.append(i)
        markedNodesSum= sum(markedNodes)
        refinementTemplateCode= str(markedNodesSum)
        if(markedNodesSum == 2):
            if(markedNodesIndices==[0,3]): # if last side.
                markedNodesIndices= [3,0]
            indicesDist= abs(markedNodesIndices[1]-markedNodesIndices[0])
            diagonal= (indicesDist==2)
            if(diagonal):
                refinementTemplateCode= '2b'
            else:
                refinementTemplateCode= '2a'
        elif(markedNodesSum == 3):
            if(markedNodesIndices==[0, 2, 3]):
                markedNodesIndices= [2, 3, 0]
            elif(markedNodesIndices==[0, 1, 3]):
                markedNodesIndices= [3, 0, 1]
        e.setProp('refinementTemplateCode', refinementTemplateCode)
        e.setProp('markedNodesIndices', markedNodesIndices)

class EdgeCornerNodesDict(object):
    ''' Dictionary containing the nodes on the edges of the already existing
        elements.

    '''
    def __init__(self):
        ''' Constructor.'''
        self.edgeCornerNodesDict= dict()

    @staticmethod
    def getEdgeKey(edgeCornerNodes):
        ''' Compute the edge key from the given end nodes.
        
        :param edgeCornerNodes: list containing the two end nodes of the edge.
        '''
        retval= None
        if(len(edgeCornerNodes)>1):
            cornerNode0Tag= edgeCornerNodes[0].tag
            cornerNode1Tag= edgeCornerNodes[1].tag
            if(cornerNode0Tag<cornerNode1Tag):
                cornerNode0Tag, cornerNode1Tag= cornerNode1Tag, cornerNode0Tag
            retval= (cornerNode0Tag, cornerNode1Tag)
        return retval
        
    def findNodesOnEdge(self, edgeCornerNodes):
        ''' Return the nodes on the edge with the given end nodes.
        
        :param edgeCornerNodes: list containing the two end nodes of the edge.
        '''
        retval= list()
        edgeKey= EdgeCornerNodesDict.getEdgeKey(edgeCornerNodes)
        if(edgeKey):
            if(edgeKey in self.edgeCornerNodesDict):
                retval= self.edgeCornerNodesDict[edgeKey]
        return retval

    def findMatchingNodeOnEdge(self, edgeCornerNodes, cartesianCoordinates, nodeDict):
        ''' Return the node on the same edge with the same position (if any).

        :param edgeCornerNodes: list containing the two end nodes of the edge.
        :param cartesianCoordinates: cartesian coordinates of the matching node.
        :param nodeDict: dictionary containing the nodes already created.
        '''
        retval= None
        edgeCornerNodesIDs= self.findNodesOnEdge(edgeCornerNodes)
        for otherNodeId in edgeCornerNodesIDs:
            otherNodeData= nodeDict[otherNodeId]
            otherCartesianCoord= otherNodeData['cartesian_coord']
            dist2= otherCartesianCoord.dist2(cartesianCoordinates)
            if(dist2<1e-6):
                retval= otherNodeId
                break
        return retval

    def newEdgeNode(self, edgeCornerNodes, nodeId):
        ''' Append the node to the list of nodes of the corresponding edge.

        :param edgeCornerNodes: list containing the two end nodes of the edge.
        :param nodeId: identifier of the node.
        '''
        edgeKey= EdgeCornerNodesDict.getEdgeKey(edgeCornerNodes)
        if(edgeKey):
            if(edgeKey in self.edgeCornerNodesDict):
                self.edgeCornerNodesDict[edgeKey].append(nodeId)
            else:
                self.edgeCornerNodesDict[edgeKey]= [nodeId]

    def computeNodesOnEdgeSubdivisionLevels(self, xcSet, nodeDict):
        ''' Compute the subdivision levels of the edges.

        :param xcSet: set whose subdivision levels will be computed.
        :param nodeDict: dictionary containing the data for the new nodes.
        '''
        domain= xcSet.getPreprocessor.getDomain
        for edgeKey in self.edgeCornerNodesDict:
            firstNodeTag= edgeKey[0]
            firstNode= domain.getNode(firstNodeTag)
            firstNodeSubdivisionLevel= 0
            if(firstNode.hasProp('subdivisionLevel')):
                firstNodeSubdivisionLevel= firstNode.getProp('subdivisionLevel')
            secondNodeTag= edgeKey[1]
            secondNode= domain.getNode(secondNodeTag)
            secondNodeSubdivisionLevel= 0
            if(secondNode.hasProp('subdivisionLevel')):
                secondNodeSubdivisionLevel= secondNode.getProp('subdivisionLevel')
            edgeSubdivisionLevel= min(firstNodeSubdivisionLevel, secondNodeSubdivisionLevel)
            edgeNodes= self.edgeCornerNodesDict[edgeKey]
            for nodeID in edgeNodes:
                nodeData= nodeDict[nodeID]
                nodeData['subdivision_level']= edgeSubdivisionLevel
        
def compute_refined_mesh(xcSet):
    ''' Compute the refined mesh for the elements of the given set.

    :param xcSet: set whose elements will be refined.
    '''
    # Compute positions of the new nodes.
    newNodeDict= dict()
    newNodeId= 0
    newElementDict= dict()
    newElementId= 0
    edgeCornerNodesDict= EdgeCornerNodesDict()
    elementsToRemove= list()
    cornerNodesTags= dict()
    for e in xcSet.elements:
        refinementTemplateCode= '0'
        if(e.hasProp('refinementTemplateCode')):
            refinementTemplateCode= e.getProp('refinementTemplateCode')
        if(refinementTemplateCode!='0'): # Element to be refined.
            elementsToRemove.append(e)
            # Get element nodes.
            elementNodes= list(e.nodes)
            markedNodesIndices= e.getProp('markedNodesIndices')
            templateRotation= markedNodesIndices[0]
            # Compute positios of the new nodes.
            rotatedRefinementTemplatePoints= refinementTemplatePoints[templateRotation]
            nIndex= dict()
            nIndex[0]= templateRotation # pos. 0 in the point arrangement.
            nIndex[3]= (1+templateRotation) % 4 # pos. 3 in the pt. arrangement.
            nIndex[15]= (2+templateRotation) % 4 # pos. 15 in the pt. arrangement.
            nIndex[12]= (3+templateRotation) % 4 # pos. 12 in the pt. arrangement.
            # Corner nodes
            cornerNodes= dict()
            cornerNodes[0]= elementNodes[nIndex[0]]
            cornerNodes[3]= elementNodes[nIndex[3]]
            cornerNodes[15]= elementNodes[nIndex[15]]
            cornerNodes[12]= elementNodes[nIndex[12]]
            # Edge nodes.
            edge_0_3_nodes= (cornerNodes[0], cornerNodes[3])
            edge_3_15_nodes= (cornerNodes[3], cornerNodes[15])
            edge_15_12_nodes= (cornerNodes[15], cornerNodes[12])
            edge_12_0_nodes= (cornerNodes[12], cornerNodes[0])
            # Node positions.
            markedNodesLocalCoordinates= list()
            for mn in markedNodesIndices:
                markedNodesLocalCoordinates.append(e.getLocalCoordinatesOfNode(mn))
            connectivity= refinementTemplateConnectivity[refinementTemplateCode]
            indexesAlreadyVisited= dict()
            for indexes in connectivity:
                newElementNodeIDs= list()
                for i in indexes:
                    if(i not in indexesAlreadyVisited):
                        existingNode= None
                        edgeCornerNodes= list()
                        matchingNode= None
                        interiorNode= False
                        naturalCoord= rotatedRefinementTemplatePoints[i]
                        elementDimension= e.getDimension
                        if(elementDimension==2):
                            naturalCoord= xc.ParticlePos2d(naturalCoord[0], naturalCoord[1])
                            cartesianCoord= e.getCartesianCoordinates(naturalCoord, True)
                        elif(elementDimension==3):
                            naturalCoord= xc.ParticlePos3d(naturalCoord[0], naturalCoord[1], naturalCoord[2])
                            cartesianCoord= e.getCartesianCoordinates(naturalCoord, True)
                        else:
                            className= type(self).__name__
                            methodName= sys._getframe(0).f_code.co_name
                            lmsg.error(className+'.'+methodName+'; not implemented for elements of dimension: '+str(elementDimension))
                            
                        if( i in [0, 3, 12, 15]): # corner nodes.
                            existingNode= cornerNodes[i]
                            # Search for corner matching node.
                            existingNodeTag= existingNode.tag
                            if(existingNodeTag in cornerNodesTags):
                                matchingNode= cornerNodesTags[existingNodeTag]
                            else:
                                cornerNodesTags[existingNodeTag]= newNodeId
                        elif( i in [5, 6, 9, 10]): # interior nodes.
                            interiorNode= True
                        else: # edge nodes.
                            if(i in [1, 2]):
                                edgeCornerNodes= [cornerNodes[0], cornerNodes[3]]
                            elif(i in [7, 11]):
                                edgeCornerNodes= [cornerNodes[3], cornerNodes[15]]
                            elif(i in [13, 14]):
                                edgeCornerNodes= [cornerNodes[15], cornerNodes[12]]
                            elif(i in [4, 8]):
                                edgeCornerNodes= [cornerNodes[12], cornerNodes[0]]

                            # Check for matching node on edge.
                            matchingNode= edgeCornerNodesDict.findMatchingNodeOnEdge(edgeCornerNodes= edgeCornerNodes, cartesianCoordinates= cartesianCoord, nodeDict= newNodeDict)
                        if(matchingNode is None): # No matching node.
                            edgeCornerNodesDict.newEdgeNode(edgeCornerNodes= edgeCornerNodes, nodeId= newNodeId)
                            newNodeDict[newNodeId]= {'from_element':e, 'natural_coord':naturalCoord, 'cartesian_coord':cartesianCoord, 'existing_node':existingNode, 'interior_node':interiorNode}
                            newElementNodeIDs.append(newNodeId)
                            indexesAlreadyVisited[i]= newNodeId
                        else:
                            newElementNodeIDs.append(matchingNode)
                            indexesAlreadyVisited[i]= matchingNode
                        newNodeId+= 1
                    else:
                        existingNodeId= indexesAlreadyVisited[i]
                        newElementNodeIDs.append(existingNodeId)
                newElementDict[newElementId]= {'from_element':e, 'node_ids':newElementNodeIDs}
                newElementId+= 1
    # Compute subdivision levels.
    ## Update subdivision levels of the existing nodes.
    alreadyVisitedNodes= set()
    for nodeKey in newNodeDict:
        nodeData= newNodeDict[nodeKey]
        existingNode= nodeData['existing_node']
        if(existingNode): # node already exists.
            existingNodeTag= existingNode.tag
            if(not existingNodeTag in alreadyVisitedNodes): # don't update twice.
                alreadyVisitedNodes.add(existingNodeTag)
                oldSubdivisionLevel= 0
                if(existingNode.hasProp('subdivisionLevel')):
                    oldSubdivisionLevel= existingNode.getProp('subdivisionLevel')
                newSubdivisionLevel= max(oldSubdivisionLevel-1, 0)
                existingNode.setProp('subdivisionLevel', newSubdivisionLevel)
            else:
                newSubdivisionLevel= existingNode.getProp('subdivisionLevel')
            nodeData['subdivision_level']= newSubdivisionLevel

    ## Compute subdivision levels of the new nodes on element edges.
    edgeCornerNodesDict.computeNodesOnEdgeSubdivisionLevels(xcSet= xcSet, nodeDict= newNodeDict)
    ## Update subdivision levels of the existing elements.
    for e in xcSet.elements:
        elementSubdivisionLevel= 0
        for n in e.nodes:
            nodeSubdivisionLevel= 0
            if(n.hasProp('subdivisionLevel')):
                nodeSubdivisionLevel= n.getProp('subdivisionLevel')
                elementSubdivisionLevel= max(nodeSubdivisionLevel, elementSubdivisionLevel)
        refinementTemplateCode= None
        if(e.hasProp('refinementTemplateCode')):
            refinementTemplateCode= e.getProp('refinementTemplateCode')
        if(refinementTemplateCode=='4'):
            elementSubdivisionLevel+= 1
        e.setProp('subdivisionLevel', elementSubdivisionLevel-1)
    ## Compute subdivision levels of the new nodes on element interior.
    for nodeKey in newNodeDict:
        nodeData= newNodeDict[nodeKey]
        interiorNode= nodeData['interior_node']
        if(interiorNode):
            fromElement= nodeData['from_element']
            elementSubdivisionLevel= 0
            if(fromElement.hasProp('subdivisionLevel')):
                elementSubdivisionLevel= fromElement.getProp('subdivisionLevel')
            nodeData['subdivision_level']= max(elementSubdivisionLevel, 0)
    return {'new_nodes':newNodeDict, 'new_elements':newElementDict, 'elements_to_remove':elementsToRemove, 'edges':edgeCornerNodesDict}

def create_new_nodes(modelSpace, refinedMesh):
    ''' Create the nodes needed to refine the mesh.

    :param modelSpace: wrapper of the finite element model.
    :param refinedMesh: dictionary returned by compute_refined_mesh 
                        containing the information about the position
                        of the new nodes.
    '''
    # Create new nodes.
    nodeSubstitutions= dict()
    newNodeDict= refinedMesh['new_nodes']
    for nodeKey in newNodeDict:
        if(nodeKey not in nodeSubstitutions):
            nodeData= newNodeDict[nodeKey]
            existingNode= nodeData['existing_node']
            if(not existingNode): # new node.
                cartesianCoord= nodeData['cartesian_coord']
                spaceDimension= cartesianCoord.getDimension()
                if(spaceDimension==2):
                    newNode= modelSpace.newNodeXY(cartesianCoord[0], cartesianCoord[1])
                elif(spaceDimension==3):
                    newNode=  modelSpace.newNodeXYZ(cartesianCoord[0], cartesianCoord[1], cartesianCoord[2])
                subdivisionLevel= nodeData['subdivision_level']
                if(subdivisionLevel!=0):
                    newNode.setProp('subdivisionLevel', subdivisionLevel)
                nodeTag= newNode.tag
            else:
                nodeTag= existingNode.tag
        nodeSubstitutions[nodeKey]= nodeTag
    return nodeSubstitutions
          
def create_new_elements(modelSpace, refinedMesh, nodeSubstitutions):
    ''' Create the elements needed to refine the mesh.

    :param modelSpace: wrapper of the finite element model.
    :param refinedMesh: dictionary returned by compute_refined_mesh 
                        containing the information about the new
                        elements.
    :param nodeSubstitutions: dictionary containing the node identifiers
                              corresponding to each of the node keys in
                              newNodesDict.
    '''
    # Create new elements.
    newElementDict= refinedMesh['new_elements']
    elementsToRemove= dict()
    for eId in newElementDict:
        elementData= newElementDict[eId]
        nodeIds= elementData['node_ids']
        nodeTags= list()
        for nId in nodeIds:
            nodeTag= nodeSubstitutions[nId]
            nodeTags.append(nodeTag)
        fromElement= elementData['from_element']
        elementType= fromElement.tipo().removeprefix('XC::')
        newElement= modelSpace.newElement(elementType, nodeTags)
        # Transfer material.
        newElement.copyMaterialFrom(fromElement, True)
        # Transfer properties.
        newElement.copyPropsFrom(fromElement)
        # Transfer sets.
        modelSpace.copyElementSets(fromElement, newElement)
        # Transfer loads.
        modelSpace.copyElementalLoads(fromElement, newElement)
        # Mark the old element.
        if(fromElement.tag not in elementsToRemove):
            elementsToRemove[fromElement.tag]= fromElement
    for eleTag in elementsToRemove:
        oldElement= elementsToRemove[eleTag]
        modelSpace.removeElement(oldElement)

def compute_subdivision_levels(xcSet):
    ''' Compute the subdivision levels of the elements of the given set.

    :param xcSet: set whose subdivision levels will be computed.
    '''
    maxSubdivisionLevel= 0
    for e in xcSet.elements:
        elementSubdivisionLevel= 0
        for n in e.nodes:
            nodeSubdivisionLevel= 0
            if(n.hasProp('subdivisionLevel')):
                nodeSubdivisionLevel= n.getProp('subdivisionLevel')
            elementSubdivisionLevel= max(elementSubdivisionLevel, nodeSubdivisionLevel)
        e.setProp('subdivisionLevel', elementSubdivisionLevel)
        maxSubdivisionLevel= max(maxSubdivisionLevel, elementSubdivisionLevel)
    return maxSubdivisionLevel

def get_max_node_subdivision_level(xcSet):
    ''' Compute the maximum node subdivision level among the nodes of the 
        given set.

    :param xcSet: set whose maximum subdivision level among its nodes will be computed.
    '''
    maxSubdivisionLevel= 0
    for n in xcSet.nodes:
        nodeSubdivisionLevel= 0
        if(n.hasProp('subdivisionLevel')):
            nodeSubdivisionLevel= n.getProp('subdivisionLevel')
        maxSubdivisionLevel= max(maxSubdivisionLevel, nodeSubdivisionLevel)
    return maxSubdivisionLevel

def refinement_step(modelSpace, xcSet):
    ''' Perform a refinement step on the given set.

    :param modelSpace: wrapper of the finite element model.
    :param xcSet: set whose elements will be refined.
    '''
    set_refinement_templates(xcSet)

    refinedMesh= compute_refined_mesh(xcSet)
    nodeSubstitutions= create_new_nodes(modelSpace= modelSpace, refinedMesh= refinedMesh)
    create_new_elements(modelSpace= modelSpace, refinedMesh= refinedMesh, nodeSubstitutions= nodeSubstitutions)
    
def compute_node_subdivision_level(xcSet):
    ''' Compute the subdivision level of the nodes from the subdivision level
        of the elements according to expression (3) of the article.

    :param xcSet: set whose elements will be refined.
    '''
    ## Compute node subdivision level according with expression (3) of the article.
    maxSubdivisionLevel= 0
    for n in xcSet.nodes:
        connectedElements= n.getConnectedElements()
        nodeSubdivisionLevel= 0
        for e in connectedElements:
            elementSubdivisionLevel= 0
            if(e.hasProp('subdivisionLevel')):
               elementSubdivisionLevel= e.getProp('subdivisionLevel')
            nodeSubdivisionLevel= max(elementSubdivisionLevel,nodeSubdivisionLevel)
        n.setProp('subdivisionLevel', nodeSubdivisionLevel)
        maxSubdivisionLevel= max(maxSubdivisionLevel, nodeSubdivisionLevel)
    return maxSubdivisionLevel
    
def refine_mesh(modelSpace, xcSet):
    ''' Perform a mesh refinement using the so called "3-refinement" procedure
        as described in the article "Algorithms for Quadrilateral and 
        Hexahedral Mesh Generation" Robert Schneiders.'

    :param modelSpace: wrapper of the finite element model.
    :param xcSet: set whose elements will be refined.
    '''
    retval= compute_node_subdivision_level(xcSet= xcSet)
    while retval>0:
        refinement_step(modelSpace= modelSpace, xcSet= xcSet)
        compute_subdivision_levels(xcSet= xcSet)
        retval= get_max_node_subdivision_level(xcSet= xcSet)
    return retval
    
