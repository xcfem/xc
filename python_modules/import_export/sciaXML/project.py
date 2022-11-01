# -*- coding: utf-8 -*-

#Based on sXML-master projet on gitHub

__author__= "Luis C. Pérez Tato (LCPT)"
__copyright__= "Copyright 2015 LCPT"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@gmail.com"

# from import_export.sciaXML.xml_basics import scxml_object as obj
# from import_export.sciaXML.xml_basics import scxml_table_container as ctr
from import_export.sciaXML.xml_basics import scxml_header as hdr
from import_export.sciaXML.xml_basics import scxml_header_item as hdr_item
# from import_export.sciaXML.xml_basics import scxml_object_item as obj_item
from import_export.sciaXML import node_container
import xml.etree.cElementTree as ET

class Project:
    
    def __init__(self,xmlns, defn, containers):
        ''' Constructor:

            :param xmlns:  attribute that defines the XML namespace. 
            :param defn:   definition file name (i.e.: 'foo.def')
            :param containers:   
        '''
        self.xmlns= xmlns
        self.defn= defn
        self.containers= containers
        self.fileName= ''
        self.uuid= ''

    # def getContainerNos(self, sumarioDados):
    #     nos= sumarioDados.getListaDeNos()
    #     containerNos= None
    #     if(len(nos) > 0):
    #       objects= list()
    #       for coordinate in nos:
    #         o= obj.SCXMLObject()
    #         o.setId(coordinate.getId() + self.uuid)
    #         o.setNm(coordinate.getName()  + self.uuid)
    #         o.setP0(obj_item.SCXMLObjectItem(String.valueOf(coordinate.getName()  + self.uuid), None, None, None, None, None))
    #         o.setP1(obj_item.SCXMLObjectItem(String.valueOf(coordinate.getX()), None, None, None, None, None))
    #         o.setP2(obj_item.SCXMLObjectItem(String.valueOf(coordinate.getY()), None, None, None, None, None))
    #         o.setP3(obj_item.SCXMLObjectItem(String.valueOf(coordinate.getZ()), None, None, None, None, None))
    #         objects.append(o)


    #       header= getDefaultStructNodeHeader()
    #       tabelaNos=TableXMLNodes(SciaXMLConstantes.STRUCT_NODE_TABLE_ID,SciaXMLConstantes.STRUCT_NODE_TABLE, SciaXMLConstantes.STRUCT_NODE_TABLE_NAME, header, objects)
    #       containerNos= Container(SciaXMLConstantes.STRUCT_NODE_ID, SciaXMLConstantes.STRUCT_NODE_TITLE, tabelaNos)
    #       return containerNos


    # def getContainerPecas(self, sumarioDados):
    #     containerPecas= None
    #     objects= list()
    #     objects.appendAll(getPecas(sumarioDados.getPecasFinais()))

    #     header= getDefaultBeamHeader()
    #     tabelaPecas= TableXMLNodes(SciaXMLConstantes.BEAM_TABLE_ID, SciaXMLConstantes.BEAM_TABLE, SciaXMLConstantes.BEAM_TABLE_NAME, header, objects)

    #     containerPecas= ctr.SCXMLTableContainer(SciaXMLConstantes.BEAM_ID, SciaXMLConstantes.BEAM_TITLE, tabelaPecas)

    #     return containerPecas


    # def getContainerCamadas(self, sumarioDados):
    #     containerPecas= None

    #     objects= list()
    #     objects.appendAll(getCamadas(sumarioDados.getPecasFinais()))

    #     header= getDefaultDataLayerNodeHeader()
    #     tabelaPecas= TableXMLNodes(SciaXMLConstantes.DATA_LAYER_TABLE_ID,SciaXMLConstantes.DATA_LAYER_TABLE, SciaXMLConstantes.DATA_LAYER_TABLE_NAME, header, objects)
    #     containerPecas= Container(SciaXMLConstantes.DATA_LAYER_ID, SciaXMLConstantes.DATA_LAYER_TITLE, tabelaPecas)
    #     return containerPecas


    # def getCamadas(pecas):
    #     retval= list()

    #     pecasUnicas= set()

    #     # Removendo os itens repetidos
    #     for peca in pecas:
    #       pecasUnicas.append(peca.getType())


    #     if(len(pecasUnicas) > 0):
    #       counterID= 1
    #       for peca in pecasUnicas:
    #         pecaOrigem= RepositorioPecas.pecas.get(peca)
    #         o= Object()                
    #         containerOrigem= None
    #         listaContainer= pecaOrigem.getContainers()

    #         for container in listaContainer:
    #           if(container.getT().contains(SciaXMLConstantes.DATA_LAYER)):
    #             containerOrigem= container
    #             break


    #       # Um arquivo de peça só possui um object/peça no container Beam
    #       objectOrigem= containerOrigem.getTables()[0].getObjects().get(0)

    #       o.setId(String.valueOf(counterID))
    #       o.setNm(peca)
    #       o.setP0(obj_item.SCXMLObjectItem(peca, None, None, None, None, None))
    #       o.setP1(objectOrigem.getP1())
    #       retval.append(o)
    #       counterID+= 1

    #     return retval

    # def getPecas(self, pecas):
    #     retval= list()
    #     for peca in pecas:
    #       o= Object()
    #       pecaOrigem= RepositorioPecas.pecas.get(peca.getType())

    #       containerOrigem= None
    #       listaContainer= pecaOrigem.getContainers()
    #       for container in listaContainer:
    #         if(container.getT().contains(SciaXMLConstantes.BEAM)):
    #           containerOrigem= container
    #           break

    #       # Um arquivo de peça só possui um object/peça no container Beam
    #       objectOrigem= containerOrigem.getTables()[0].getObjects().get(0)

    #       o.setId(peca.getId() + self.uuid)
    #       o.setNm(peca.getName() + self.uuid)
    #       o.setP0(obj_item.SCXMLObjectItem(peca.getId()  + self.uuid, None, None, None, None, None))
    #       o.setP1(obj_item.SCXMLObjectItem(None,String.valueOf(peca.getNoInicial().getId()  + self.uuid),String.valueOf(peca.getNoInicial().getId() + self.uuid),None,None,None))
    #       o.setP2(obj_item.SCXMLObjectItem(None,String.valueOf(peca.getNoFinal().getId() + self.uuid),String.valueOf(peca.getNoFinal().getId() + self.uuid),None,None,None))

    #       item= obj_item.SCXMLObjectItem()
    #       item.setV(peca.getId())
    #       if(objectOrigem.getP3() is not None):
    #         valor= objectOrigem.getP3().getN().substring(0, objectOrigem.getP3().getN().indexOf("-")-1)
    #         item.setN(valor)

    #       o.setP3(item)            
    #       o.setP4(objectOrigem.getP4())
    #       o.setP5(objectOrigem.getP5())
    #       o.setP6(objectOrigem.getP6())
    #       o.setP7(objectOrigem.getP7())
    #       o.setP8(objectOrigem.getP8())
    #       o.setP9(objectOrigem.getP9())
    #       o.setP10(objectOrigem.getP10())
    #       o.setP11(objectOrigem.getP11())
    #       o.setP12(objectOrigem.getP12())

    #     # TODO: Se necessário atualizar os valores da tabela de geometria
    #     # o.getXX().setT(String())
    #     retval.append(o)
    #     return retval

    def getDefaultBeamHeader(self):
        retval= hdr.SCXMLHeader()
        # retval.setH0(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H0))
        # retval.setH1(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H1))
        # retval.setH2(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H2))
        # retval.setH3(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H3))
        # retval.setH4(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H4))
        # retval.setH5(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H5))
        # retval.setH6(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H6))
        # retval.setH7(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H7))
        # retval.setH8(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H8))
        # retval.setH9(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H9))
        # retval.setH10(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H10))
        # retval.setH11(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H11))
        # retval.setH12(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.BEAM_HEADER_H12))
        return retval

    def getDefaultStructNodeHeader(self):
        retval= hdr.SCXMLHeader()
        retval.setH0(hdr_item.SCXMLHeaderItem(node_container.STRUCT_NODE_HEADER_H0))
        retval.setH1(hdr_item.SCXMLHeaderItem(node_container.STRUCT_NODE_HEADER_H1))
        retval.setH2(hdr_item.SCXMLHeaderItem(node_container.STRUCT_NODE_HEADER_H2))
        retval.setH3(hdr_item.SCXMLHeaderItem(node_container.STRUCT_NODE_HEADER_H3))
        return retval

    def getDefaultDataLayerNodeHeader(self):
        retval= hdr.SCXMLHeader()
        # retval.setH0(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.DATA_LAYER_HEADER_H0))
        # retval.setH1(hdr_item.SCXMLHeaderItem(SciaXMLConstantes.DATA_LAYER_HEADER_H1))
        return retval

    def getDef(self):
        return self.defn
    def setDef(self, defn):
        self.defn= defn


    def getContainers(self):
        return self.containers

    def setContainers(self, containers):
        self.containers= containers


    def getXmlns(self):
        return self.xmlns
    def setXmlns(self, xmlns):
        self.xmlns= xmlns

    def toString(self):
        return "Project [xmlns=" + self.xmlns + ", def=" + self.defn + ", container=" + self.containers + "]"


    def getFileName(self):
        return self.fileName
      
    def setFileName(self, fileName):
        self.fileName= fileName


    def getXMLElement(self, defFileName):
        '''Returns the corresponding XML element for the object.

           :param defFileName: XML definition file name.
        '''
        project= ET.Element("project")
        project.set("xmlns",self.xmlns)
        df= ET.SubElement(project,"def")
        df.set("uri",defFileName)
        for c in self.containers:
            elem= c.getXMLElement(project)
            if __debug__:
                if(not elem):
                    AssertionError('Couldn\'t get XML element.')
        return project

    def getXMLTree(self,defFileName):
        '''Returns the corresponding XML tree.'''
        project= self.getXMLElement(defFileName)
        tree = ET.ElementTree(project)
        return tree

    def getDefFileName(self):
        return self.fileName + ".def"

    def writeXMLFile(self): 
        '''Write XML files for the object.'''
        tree= self.getXMLTree(self.getDefFileName())
        tree.write(self.fileName,encoding="UTF-8", xml_declaration=None, default_namespace=None, method="xml")
