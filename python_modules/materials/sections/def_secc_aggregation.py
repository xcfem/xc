# -*- coding: utf-8 -*-

from materials import typical_materials

def def_secc_aggregation3d(preprocessor, defSecc, defMat):
    ''' Definition of an elastic material section for 3D elements

    :param preprocessor: preprocessor of the FE problem.
    :param defSecc:  object with the mechanical properties of the section (A, Iy, Iz, ...)
    :param defMat:   object with the properties of the material (E, G)  
    '''
    nmbRigF= defSecc.name+ "_rigF" # Bending stiffness.
    typical_materials.defElasticSection3d(preprocessor,nmbRigF,defSecc.A(),defMat.E,defMat.G(),defSecc.Iz(),defSecc.Iy(),defSecc.J())
    nmbRigVy= defSecc.name+"_rigVy" # Y shear stiffness.
    typical_materials.defElasticMaterial(preprocessor,nmbRigVy,defSecc.alphaY()*defMat.G()*defSecc.A())
    nmbRigVz= defSecc.name+"_rigVz" # Z shear stiffness.
    typical_materials.defElasticMaterial(preprocessor,nmbRigVz,defSecc.alphaY()*defMat.G()*defSecc.A())
    nmbRigT= defSecc.name+"_rigT" # Torsional stiffness.
    typical_materials.defElasticMaterial(preprocessor,nmbRigT,defMat.G()*defSecc.J())
    materialHandler= preprocessor.getMaterialHandler
    agg= materialHandler.newMaterial("section_aggregator",defSecc.name)
    agg.setSection(nmbRigF)
    agg.setAdditions(["T","Vy","Vz"],[nmbRigT,nmbRigVy,nmbRigVz])
    #agg.setAddition("Vy",nmbRigVy)
    #agg.setAddition("Vz",nmbRigVz)

def def_secc_aggregation2d(preprocessor, defSecc, defMat):
    ''' Definition of a elastic material-section for 2D elements

    :param preprocessor: preprocessor of the FE problem.
    :param defSecc:  object with the mechanical properties of the 
                     section (A, Iy, Iz, ...)
    :param defMat:   object with the properties of the material (E, G)  
    '''
    nmbRigF= defSecc.name+ "_rigF" # Bending stiffness.
    typical_materials.defElasticSection2d(preprocessor,nmbRigF,defSecc.A(),defMat.E,defSecc.Iz())
    nmbRigVy= defSecc.name+"_rigVy" # Y shear stiffness.
    typical_materials.defElasticMaterial(preprocessor,nmbRigVy,defSecc.alphaY()*defMat.G()*defSecc.A())
    materialHandler= preprocessor.getMaterialHandler
    agg= materialHandler.newMaterial("section_aggregator",defSecc.name)
    agg.setSection(nmbRigF)
    agg.setAdditions(["Vy"],[nmbRigVy])
    #agg.setAddition("Vy",nmbRigVy)

def def_fiber_section_aggregation3d(preprocessor, fiberSection3d, respT, respVy, respVz):
    ''' Return a section aggregation with the bending response of the given
        fiber section and the torsional and shear respones of the given
        materials.

    :param preprocessor: preprocessor of the FE problem.
    :param fiberSection3d: 3D fiber section.
    :param respT: uniaxial material defining torsional response.
    :param respVy: uniaxial material defining shear response along y axis.
    :param respVz: uniaxial material defining shear response along z axis.
    '''
    aggregationName= fiberSection3d.name+'_'+respT.name+'_'+respVy.name+'_'+respVz.name
    materialHandler= preprocessor.getMaterialHandler

    retval= materialHandler.newMaterial("section_aggregator", aggregationName)
    retval.setSection(fiberSection3d.name)
    retval.setAdditions(["T","Vy","Vz"],[respT.name, respVy.name, respVz.name])
    return retval
