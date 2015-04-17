import maya.cmds as pm
import maya.mel as mel
import random as random

mesh = pm.ls(selection=True)

verts  = mel.eval("getVerts;")

for vert in verts:
    min = -0.3
    max = 0.3
    
    randNumX = random.uniform(min, max)
    randNumY = random.uniform(min, max)
    randNumZ = random.uniform(min, max)

    pm.select(vert, replace=True)
    pm.move(randNumX, randNumY, randNumZ, relative=True)
    