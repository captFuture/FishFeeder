Import("env", "projenv")
import os
import sys
from shutil import copyfile
my_flags = env.ParseFlags(env['BUILD_FLAGS'])

print(my_flags.get("CPPDEFINES"))

defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}

script_path = os.path.dirname(os.path.realpath('__file__'))
#print("-------------------------------"+script_path)
script_dir = os.path.split(script_path)[0]
#print("-------------------------------"+script_dir)

def before_upload(source, target, env):
    print("--------------------------------before_upload")
    # do some actions

def after_upload(source, target, env):
    print("--------------------------------after_upload")
    # do some actions

def after_build(source, target, env):
    print("--------------------------------after_build")
    # do some actions
    if defines.get("FRELEASE") == "true":
        os.makedirs(script_path+"\\releases\\"+defines.get("FNAME").lower(), exist_ok=True)
        copyfile(script_path+"\\.pio\\build\\nodemcuv2\\firmware.bin", script_path+"\\releases\\"+defines.get("FNAME").lower()+"\\firmware_"+defines.get("FVERSION")+".bin")
    else:
        os.makedirs(script_path+"\\releases\\"+defines.get("FNAME").lower(), exist_ok=True)
        copyfile(script_path+"\\.pio\\build\\nodemcuv2\\firmware.bin", script_path+"\\releases\\"+defines.get("FNAME").lower()+"\\firmware"+defines.get("FVERSION")+"_dev.bin")



def before_build(source, target, env):
    print("--------------------------------after_build")
    # do some actions

env.AddPostAction("buildprog", after_build)
env.AddPreAction("buildprog", before_build)