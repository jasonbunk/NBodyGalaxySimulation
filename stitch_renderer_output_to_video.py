import os
import sys

if len(sys.argv) <= 1:
	print("usage:  {video-out-filename}  {add-black-bg?}  {generate-new-frames?}")
	quit()

videoOutFilename = str(sys.argv[1])

AddBlackBGToFrames = False
if len(sys.argv) > 2:
	if str(sys.argv[2]) == "True" or str(sys.argv[2]) == "true" or str(sys.argv[2]) == "1":
		AddBlackBGToFrames = True

GenerateNewRenderFrames = False
if len(sys.argv) > 3:
	if str(sys.argv[3]) == "True" or str(sys.argv[3]) == "true" or str(sys.argv[3]) == "1":
		GenerateNewRenderFrames = True

#-------------------------------------------------------------

if GenerateNewRenderFrames:
	os.system("./Renderer3D/Renderer3D out_opencl.data 596 1 0 1 Renderer3D/cposfile.txt")


if AddBlackBGToFrames:
	prefixed = sorted([filename for filename in os.listdir('frames') if filename.startswith("__")])
	for filename in prefixed:
		os.system("composite -compose plus frames/"+str(filename)+" frames/black.png frames/"+str(filename))


print("Making movie "+str(videoOutFilename)+" - this make take a while")
os.system("ffmpeg -r 30 -f image2 -i 'frames/__%05d.png' -qscale 0 '"+str(videoOutFilename)+"'")
#os.system("rm frames/*.png")
