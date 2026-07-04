import os
print("Rendering frames...");
os.system('C:/blender/blender.exe --background D:/dsEngine/data/characters/character_upper.blend --scene SOUTH -s 0 -e 59 -a')
os.system('C:/blender/blender.exe --background D:/dsEngine/data/characters/character_upper.blend --scene NORTH -s 0 -e 59 -a')
os.system('C:/blender/blender.exe --background D:/dsEngine/data/characters/character_upper.blend --scene LEFT -s 0 -e 59 -a')
os.system('C:/blender/blender.exe --background D:/dsEngine/data/characters/character_upper.blend --scene RIGHT -s 0 -e 59 -a')
os.system('C:/blender/blender.exe --background D:/dsEngine/data/characters/character_lower.blend --scene SOUTH -s 0 -e 59 -a')
os.system('C:/blender/blender.exe --background D:/dsEngine/data/characters/character_lower.blend --scene NORTH -s 0 -e 59 -a')
os.system('C:/blender/blender.exe --background D:/dsEngine/data/characters/character_lower.blend --scene LEFT -s 0 -e 59 -a')
os.system('C:/blender/blender.exe --background D:/dsEngine/data/characters/character_lower.blend --scene RIGHT -s 0 -e 59 -a')
print("Moving rendered image files to target destination...");
os.system('move D:\\render\\*.png D:\\dsEngine\\data\\render')
print("Compiling spritesheets...");
os.system('D:/dsEngine/data/render/create_spritesheets.bat')
print("Cleaning Up...");
os.system('erase D:\\dsEngine\\data\\render\\north????.png')
os.system('erase D:\\dsEngine\\data\\render\\south????.png')
os.system('erase D:\\dsEngine\\data\\render\\left????.png')
os.system('erase D:\\dsEngine\\data\\render\\right????.png')
os.system('erase D:\\dsEngine\\data\\render\\north_lower????.png')
os.system('erase D:\\dsEngine\\data\\render\\south_lower????.png')
os.system('erase D:\\dsEngine\\data\\render\\left_lower????.png')
os.system('erase D:\\dsEngine\\data\\render\\right_lower????.png')
