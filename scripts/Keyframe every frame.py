import pymel.core as pm
import pymel.core.datatypes as dt

selected1 = pm.ls(selection = True)
node = pm.PyNode(selected1[0])

def animation(kid):
    kid.translate.setKey()
    kid.rotate.setKey()
    kid.scale.setKey()

def SourceAnimation(keygiver):        
    ##Gets the first and last keyFrames
    first = pm.findKeyframe(keygiver, which='first')
    last = pm.findKeyframe(keygiver, which='last')
    
    for i in range(int(last+1)):
        pm.setCurrentTime(i)
        keygiver.translate.setKey()
        keygiver.rotate.setKey()
        keygiver.scale.setKey()
        
        for child in keygiver.getChildren():
            print child
            animation(child)
    
        ##Finds the next keyframe once the first keyframe has been set!
        i = pm.findKeyframe(keygiver, time=i, which='next')

SourceAnimation(node)