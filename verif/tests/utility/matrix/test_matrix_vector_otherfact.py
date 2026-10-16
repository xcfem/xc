# -*- coding: utf-8 -*-
''' Check Matrix and Vector otherFact.

Based on:
https://github.com/OpenSees/OpenSees/blob/master/tests/test_matrix_vector_otherfact.cpp
'''
__author__= "Luis C. Pérez Tato (LCPT) and Ana Ortega (AO_O)"
__copyright__= "Copyright 2026, LCPT and AO_O"
__license__= "GPL"
__version__= "3.0"
__email__= "l.pereztato@ciccp.es ana.ortega@ciccp.es"

import xc
import sys
from misc_utils import log_messages as lmsg


def nearly_equal(lhs, rhs):
    scale = 1.0 + max(abs(lhs), abs(rhs))
    return (abs(lhs - rhs) <= (1.0e-12 * scale))

def require(condition, message):
    if(not condition):
        lmsg.error("FAIL: "+message+"\n")
        sys.exit(1)
    else:
        return True

def make_matrix(rows, cols, offset):
    tmp= list()
    for i in range(0, rows):
        row= list()
        for j in range(0, cols):
            row.append(offset + 1.0 + 2.0 * i + 3.0 * j)
        tmp.append(row)
    return xc.Matrix(tmp)

def make_vector(size, offset):
    tmp= size*[0.0]
    for i in range(0, size):
        tmp[i] = offset + 1.0 + 2.0 * i
    return xc.Vector(tmp)

def scaled(value, factor):
    tmp= value.getList()
    for j in range(0, value.noCols):
        for i in range(0, value.noRows):
            tmp[i][j]*= factor
    return xc.Matrix(tmp)

def add(lhs, rhs, rhsFactor):
    tmp= xc.Matrix(lhs).getList()
    for j in range(0, lhs.noCols):
        for i in range(0, lhs.noRows):
            tmp[i][j]+= rhs(i, j) * rhsFactor
    return xc.Matrix(tmp)

def multiply(lhs, rhs):
    tmp= xc.Matrix(lhs.noRows, rhs.noCols).getList()
    for j in range(0, rhs.noCols):
        for i in range(0, lhs.noRows):
            for k in range(0, lhs.noCols):
                tmp[i][j]+= lhs(i, k) * rhs(k, j)
    return xc.Matrix(tmp)

def transpose(value):
    tmp= xc.Matrix(value.noCols, value.noRows).getList()
    for j in range(0, value.noCols):
        for i in range(0, value.noRows):
            tmp[j][i]= value(i, j)
    return xc.Matrix(tmp)

def require_matrix(actual, expected, message):
    retval= require((actual.noRows == expected.noRows) and (actual.noCols == expected.noCols), message)
    for j in range(0, actual.noCols):
        for i in range(0, actual.noRows):
            tmp= require(nearly_equal(actual(i, j), expected(i, j)), message)
            if(not tmp):
                retval= False
                break;
    return retval

def require_vector(actual, expected, message):
    retval= require(actual.size() == expected.size(), message)
    for i in range(0,actual.size()):
        tmp= require(nearly_equal(actual[i], expected[i]), message)
        if(not tmp):
            retval= False
            break;
    return retval

A= make_matrix(2, 2, 0.0)
B= make_matrix(2, 2, 10.0)
actual= xc.Matrix(A)
testOK= require(actual.addMatrix(1.0, B, 0.0) == 0, "addMatrix zero factor return")

testOK&= require_matrix(actual, A, "addMatrix zero factor preserves this")
actual= xc.Matrix(A)
actual.addMatrix(2.0, B, 0.0)
testOK&= require_matrix(actual, scaled(A, 2.0), "addMatrix zero factor scales this")
actual= xc.Matrix(A)
actual.addMatrix(1.0, B, -1.0)
testOK&= require_matrix(actual, add(A, B, -1.0), "addMatrix negative-one factor")

actual= xc.Matrix(A)
actual.addMatrix(2.0, B, 2.5)
testOK&= require_matrix(actual, add(scaled(A, 2.0), B, 2.5), "addMatrix general factors")

actual= xc.Matrix(A)
actual.addMatrixTranspose(1.0, B, 0.0)
testOK&= require_matrix(actual, A, "addMatrixTranspose zero factor")

actual= xc.Matrix(A)
actual.addMatrixTranspose(1.0, B, -1.0)
testOK&= require_matrix(actual, add(A, transpose(B), -1.0), "addMatrixTranspose negative-one factor")

actual= xc.Matrix(A)
actual.addMatrixTranspose(2.0, B, 2.5)
testOK&= require_matrix(actual, add(scaled(A, 2.0), transpose(B), 2.5), "addMatrixTranspose general factors")

B23= make_matrix(2, 3, 1.0)
C32= make_matrix(3, 2, 2.0)
actual= xc.Matrix(A)
actual.addMatrixProduct(1.0, B23, C32, 0.0)
testOK&= require_matrix(actual, A, "addMatrixProduct zero factor")

actual= xc.Matrix(A)
actual.addMatrixProduct(0.0, B23, C32, 1.0)
product= multiply(B23, C32)
testOK&= require_matrix(actual, product, "addMatrixProduct positive-one factor")

actual= xc.Matrix(A)
actual.addMatrixProduct(1.0, B23, C32, -1.0)
testOK&= require_matrix(actual, add(A, product, -1.0), "addMatrixProduct negative-one factor")

actual= xc.Matrix(A)
actual.addMatrixProduct(2.0, B23, C32, 2.5)
testOK&= require_matrix(actual, add(scaled(A, 2.0), product, 2.5), "addMatrixProduct general factors")

A32= make_matrix(3, 2, 3.0)
B32= make_matrix(3, 2, 4.0)
transposeProduct= multiply(transpose(A32), B32)
actual= xc.Matrix(A)
actual.addMatrixTransposeProduct(1.0, A32, B32, 0.0)
testOK&= require_matrix(actual, A, "addMatrixTransposeProduct zero factor")

actual= xc.Matrix(A)
actual.addMatrixTransposeProduct(1.0, A32, B32, -1.0)
testOK&= require_matrix(actual, add(A, transposeProduct, -1.0), "addMatrixTransposeProduct negative-one factor")

actual= xc.Matrix(A)
actual.addMatrixTransposeProduct(2.0, A32, B32, 2.5)
testOK&= require_matrix(actual, add(scaled(A, 2.0), transposeProduct, 2.5), "addMatrixTransposeProduct general factors")

T32= make_matrix(3, 2, 5.0)
B33= make_matrix(3, 3, 6.0)
triple= multiply(transpose(T32), multiply(B33, T32))
actual= xc.Matrix(A)
actual.addMatrixTripleProduct(1.0, T32, B33, 0.0)
testOK&= require_matrix(actual, A, "addMatrixTripleProduct zero factor")

actual= xc.Matrix(A)
actual.addMatrixTripleProduct(1.0, T32, B33, -1.0)
testOK&= require_matrix(actual, add(A, triple, -1.0), "addMatrixTripleProduct negative-one factor")

actual= xc.Matrix(A)
actual.addMatrixTripleProduct(2.0, T32, B33, 2.5)
testOK&= require_matrix(actual, add(scaled(A, 2.0), triple, 2.5), "addMatrixTripleProduct general factors")


C32b= make_matrix(3, 2, 7.0)
tripleGeneral= multiply(transpose(A32), multiply(B33, C32b))
actual= xc.Matrix(A)
actual.addMatrixTripleProduct(1.0, A32, B33, C32b, 0.0)
testOK&= require_matrix(actual, A, "general triple product zero factor")

actual= xc.Matrix(A)
actual.addMatrixTripleProduct(1.0, A32, B33, C32b, -1.0)
testOK&= require_matrix(actual, add(A, tripleGeneral, -1.0), "general triple product negative-one factor")

actual= xc.Matrix(A)
actual.addMatrixTripleProduct(2.0, A32, B33, C32b, 2.5)
expected= add(scaled(A, 2.0), tripleGeneral, 2.5)
testOK&= require_matrix(actual, expected, "general triple product general factors")

v= make_vector(2, 8.0)
w= make_vector(2, 9.0)
vector= xc.Vector(v)
vector.addVector(1.0, w, 0.0)
testOK&= require_vector(vector, v, "addVector zero factor")

vector= xc.Vector(v)
vector.addVector(0.0, w, -1.0)
expectedVector= xc.Vector(w)
expectedVector *= -1.0
testOK&= require_vector(vector, expectedVector, "addVector negative-one factor")

vector= xc.Vector(v)
vector.addVector(2.0, w, 2.5)
tmp= expectedVector.size()*[0.0]
for i in range(0,expectedVector.size()):
    tmp[i]= 2.0 * v[i] + 2.5 * w[i]
expectedVector= xc.Vector(tmp)
testOK&= require_vector(vector, expectedVector, "addVector general factors")


M23= make_matrix(2, 3, 11.0)
x3= make_vector(3, 12.0)
matrixVector= xc.Vector(v)
matrixVector.addMatrixVector(1.0, M23, x3, 0.0)
testOK&= require_vector(matrixVector, v, "addMatrixVector zero factor")

Mx= [0.0, 0.0]
for i in range(0,M23.noRows):
    Mx[i]= 0.0
    for j in range(0,M23.noCols):
        Mx[i]+= M23(i, j) * x3[j]
Mx= xc.Vector(Mx)

matrixVector= xc.Vector(v)
matrixVector.addMatrixVector(0.0, M23, x3, -1.0)
Mx*= -1.0
testOK&= require_vector(matrixVector, Mx, "addMatrixVector negative-one factor")

Mx*= -1.0
matrixVector= xc.Vector(v)
matrixVector.addMatrixVector(2.0, M23, x3, 2.5)
tmp= expectedVector.size()*[0.0]
for i in range(0, expectedVector.size()):
    tmp[i]= 2.0 * v[i] + 2.5 * Mx[i]
expectedVector= xc.Vector(tmp)
testOK&= require_vector(matrixVector, expectedVector, "addMatrixVector general factors")

M32= make_matrix(3, 2, 13.0)
x3b= make_vector(3, 14.0)
transposeVector= xc.Vector(v)
transposeVector.addMatrixTransposeVector(1.0, M32, x3b, 0.0)
testOK&= require_vector(transposeVector, v, "addMatrixTransposeVector zero factor")

Mtx= [0.0, 0.0]
for j in range(0, M32.noCols):
    Mtx[j]= 0.0
    for i in range(0,M32.noRows):
        Mtx[j]+= M32(i, j) * x3b[i]
Mtx= xc.Vector(Mtx)

transposeVector= xc.Vector(v) # copy.
transposeVector.addMatrixTransposeVector(0.0, M32, x3b, -1.0)
Mtx*= -1.0
testOK&= require_vector(transposeVector, Mtx, "addMatrixTransposeVector negative-one factor")

Mtx*= -1.0
transposeVector= xc.Vector(v) # copy.
transposeVector.addMatrixTransposeVector(2.0, M32, x3b, 2.5)
tmp= expectedVector.size()*[0.0]
for i in range(0, expectedVector.size()):
    tmp[i]= 2.0 * v[i] + 2.5 * Mtx[i]
expectedVector= xc.Vector(tmp)
testOK&= require_vector(transposeVector, expectedVector, "addMatrixTransposeVector general factors")

'''
if(testOK):
    print("matrix/vector otherFact regression tests passed.")
'''

import os
from misc_utils import log_messages as lmsg
fname= os.path.basename(__file__)
if (testOK):
    print('test '+fname+': ok.')
else:
    lmsg.error(fname+' ERROR.')

