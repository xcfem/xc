# -*- coding: utf-8 -*-
# Utils to prestress elements

def pre_stress_elem_line(preprocessor,tag_line, tension):
    '''Introduces a prestressing tension on the elements in a line

    :param preprocessor: preprocessor
    :param tag_line: line number
    :param tension: tension value
    '''
    line= preprocessor.getMultiBlockTopology.getLine(tag_line)
    elems= line.getElems()
    for e in elems:
        e.getMaterial().prestress(tension)

def post_stress_elem_line(preprocessor,tag_line, tension):
    '''Introduces a post-tension on the elements in a line

    :param preprocessor: preprocessor
    :param tag_line: line number
    :param tension: tension value
    '''

    line= preprocessor.getMultiBlockTopology.getLine(tag_line)
    elems= line.getElems()
    for e in elems:
        pret= e.getPrestress
        e.getMaterial().prestress(tension+pret)

def re_stress_elem_lines(preprocessor, set_lines, umbral, tension):
    '''Increments stress on the elmenents contained in the lines if its
       current one is lower than threshold.

    :param preprocessor: preprocessor
    :param set_lines: line container
    :param umbral: stress threshold.
    :param tension: tension value
    '''
    setsLines= preprocessor.getSets.getSet(set_lines)
    for s in setsLines:
        elems= s.getElems()
        for e in elems:
            pret= e.getPrestress()
            if(pret<umbral):
                e.prestress(tension+pret)
