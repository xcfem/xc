# -*- coding: utf-8 -*-
''' Reinforced concrete frame pushover analysis model migrated from OpenSees.

See https://openseespydoc.readthedocs.io/en/latest/src/RCFramePushOver.html
'''

from materials import typical_materials

def def_rc_push_over_frame(modelSpace, columnFiberSection, width, height):
    ''' Define the RC frame and return the elements and nodes of interest in
        a dictionary.

    :param modelSpace: wrapper of the FE problem preprocessor.
    :param columnFiberSection: fiber section for the frame columns.
    :param width: width of the frame.
    :param height: height of the frame.
    '''
    # Create nodes
    #    tag, X, Y
    n1= modelSpace.newNode(0.0, 0.0)
    n2= modelSpace.newNode(width, 0.0)
    n3= modelSpace.newNode(0.0, height)
    n4= modelSpace.newNode(width, height)
    
    retval= dict()
    retval['n3']= n3
    retval['n4']= n4

    # Constraints.
    # Fix supports at base of columns
    #   tag, DX, DY, RZ
    for n in [n1, n2]:
        modelSpace.fixNode('000', n.tag)


    # Define column elements
    # ----------------------

    # Geometry of column elements
    #                tag
    pDelta= modelSpace.newPDeltaCrdTransf("pDelta")

    # Lobatto integration
    preprocessor= modelSpace.preprocessor
    beamIntegratorHandler= preprocessor.getBeamIntegratorHandler
    lobattoBeamIntegration= beamIntegratorHandler.newBeamIntegrator('Lobatto','lobattoBeamIntegration')

    # Create the columns using Beam-column elements
    modelSpace.setDefaultCoordTransf(pDelta) # Coordinate transformation for the new elements.
    modelSpace.setDefaultMaterial(columnFiberSection) # Section material for the new elements.
    modelSpace.setNumSections(5) # Number of integration points along length of element.
    modelSpace.setDefaultIntegrator(lobattoBeamIntegration)
    column1= modelSpace.newElement("ForceBeamColumn2d", [n1.tag, n3.tag])
    column2= modelSpace.newElement("ForceBeamColumn2d", [n2.tag, n4.tag])
    retval['column1']= column1
    retval['column2']= column2
    

    # Define beam element
    ## Coordinate transformation.
    lin= modelSpace.newLinearCrdTransf("lin")
    ## Linear elastic 2D section.
    A= 360
    E= 4030
    I= 8640
    nu= 0.3
    G= E/(2.0*(1+nu))
    beamSection= typical_materials.defElasticShearSection2d(preprocessor, "section",A= A, E= E, G= G, I= I, alpha= 1.0)
    modelSpace.setDefaultCoordTransf(lin) # Coordinate transformation for the new elements.
    modelSpace.setDefaultMaterial(beamSection) # Section material for the new elements.
    beam= modelSpace.newElement("ElasticBeam2d", [n3.tag, n4.tag]) # Create the beam element
    return retval
