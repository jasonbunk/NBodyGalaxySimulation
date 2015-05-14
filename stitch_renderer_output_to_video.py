import os
import sys

if len(sys.argv) <= 1:
	print("usage:  {results-filename}  {video-out-filename}  {add-black-bg?}  {generate-new-frames?}")
	quit()

inputPositionsFileoutName = str(sys.argv[1])

videoOutFilename = str(sys.argv[2])

AddBlackBGToFrames = False
if len(sys.argv) > 3:
	if str(sys.argv[3]) == "True" or str(sys.argv[3]) == "true" or str(sys.argv[3]) == "1":
		AddBlackBGToFrames = True

GenerateNewRenderFrames = False
if len(sys.argv) > 4:
	if str(sys.argv[4]) == "True" or str(sys.argv[4]) == "true" or str(sys.argv[4]) == "1":
		GenerateNewRenderFrames = True

#-------------------------------------------------------------

if GenerateNewRenderFrames:
	os.system("./Renderer3D/Renderer3D data/results/"+inputPositionsFileoutName+" 596 1 0 1 Renderer3D/cposfile.txt")


if AddBlackBGToFrames:
	prefixed = sorted([filename for filename in os.listdir('frames') if filename.startswith("__")])
	for filename in prefixed:
		os.system("composite -compose plus data/videoframes/"+str(filename)+" data/videoframes/black.png data/videoframes/"+str(filename))


print("Making movie "+str(videoOutFilename)+" - this make take a while")
os.system("ffmpeg -r 30 -f image2 -i 'data/videoframes/__%05d.png' -qscale 0 '"+str(videoOutFilename)+"'")
#os.system("rm data/videoframes/*.png")
