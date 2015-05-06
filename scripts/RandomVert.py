import maya.cmds as pm
import maya.mel as mel
import random as random

mesh = pm.ls(selection=True)

verts  = mel.eval("getVerts;")

for vert in verts:
    min = -0.01
    max = 0.01
    
    randNumX = random.uniform(min * 2, max * 2)
    randNumY = random.uniform(min, max)
    randNumZ = random.uniform(min * 6, max * 6)

    pm.select(vert, replace=True)
    pm.move(randNumX, randNumY, randNumZ, relative=True)
    