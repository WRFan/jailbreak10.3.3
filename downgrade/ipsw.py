# vieux -> ipsw.py -> restore.py -> checkm8.py -> rmsigchks.py -> usbexec2.py
import os
import shutil
import bsdiff4
import plistlib
import sys
import device as localdevice
from pathlib import Path
from zipfile import ZipFile, is_zipfile
from restore import restore64, pwndfumode

def readmanifest(path, flag):
	fn = path

	with open(fn, 'rb') as f:
		pl = plistlib.load(f)

	if flag:
		result = pl['ProductVersion']
	else:
		supportedModels = str(pl['SupportedProductTypes'])

		supportedModels1 = supportedModels.replace("[", "")

		supportedModels2 = supportedModels1.replace("'", "")

		result = supportedModels2.replace("]", "")

	return result

def removeFiles():
	randomfiles = [
	'errorlogshsh.txt', 'errorlogrestore.txt', 'ibss', 'ibec', 'resources/other/baseband.bbfw',
	'resources/other/sep.im4p', 'resources/other/apnonce.shsh'
	]

	for item in randomfiles:
		if os.path.isfile(item):
			os.remove(item)

	if os.path.exists("IPSW"):
		shutil.rmtree("IPSW")

	if os.path.exists("Firmware"):
		shutil.rmtree("Firmware")

	if os.path.exists("custom"):
		shutil.rmtree('custom')

	if os.path.exists("igetnonce"):
		os.remove("igetnonce")

	if os.path.exists("tsschecker"):
		os.remove("tsschecker")

	if os.path.exists("futurerestore"):
		os.remove("futurerestore")

	if os.path.exists("irecovery"):
		os.remove("irecovery")

	if os.path.exists("custom.ipsw"):
		os.remove("custom.ipsw")

	dir_name = os.getcwd()

	test = os.listdir(dir_name)

	for item in test:
		if item.endswith(".im4p"):
			os.remove(os.path.join(dir_name, item))
		elif item.endswith(".plist"):
			os.remove(os.path.join(dir_name, item))
		elif item.endswith(".dmg"):
			os.remove(os.path.join(dir_name, item))
		elif item.endswith(".shsh"):
			os.remove(os.path.join(dir_name, item))
		elif item.endswith(".shsh2"):
			os.remove(os.path.join(dir_name, item))
		elif item.endswith(".dfu"):
			os.remove(os.path.join(dir_name, item))

	print("Files cleaned")

def touch(path):
	with open(path, 'a'):
		os.utime(path, None)

def unzipIPSW():
	devicemodel = str(localdevice.getmodel())

	arm64check = ('iPhone6,2')

	if any(ext in devicemodel for ext in arm64check):
		pwndfumode()

		restore64(devicemodel)
	else:
		print("ERROR: Unsupported model or device not connected!")
		exit(82)

def unzipIPSW_ORG(fname): # from: vieux my1
	armv7 = ['iPhone4,1', 'iPad2,1', 'iPad2,2', 'iPad2,3', 'iPad2,4', 'iPad2,5', 'iPad2,6', 'iPad2,7', 'iPod5,1']
	armv7s = ['iPhone5,1', 'iPhone5,2', 'iPad3,4', 'iPad3,5', 'iPad3,6', 'iPad3,1', 'iPad3,2', 'iPad3,3']

	#if os.path.exists("custom.ipsw"):
		#os.remove("custom.ipsw")

	outputFolder = "IPSW"

	newpath = fname.rstrip()

	fname = str(newpath)

	testFile = os.path.exists(fname)

	if not os.path.exists('IPSW'):
		os.mkdir('IPSW')

	while not testFile or not fname.endswith!=(".ipsw"):
		print("Invalid filepath/filename.\nPlease try again with a valid filepath/filename.")

		fname = input("Enter the path to the IPSW file (Or drag and drop the IPSW into this window):\n")

		newpath = fname.rstrip()

		fname = str(newpath)

		testFile = os.path.exists(fname)

	if testFile and fname.endswith(".ipsw"):
		if not os.path.exists('IPSW'):
			removeFiles()

			with ZipFile(fname, 'r') as zip_ref:
				zip_ref.extractall(outputFolder)

			source = ("IPSW/Firmware/dfu/")

			dest1 = os.getcwd()

			files = os.listdir(source)

			for f in files:
				shutil.move(source + f, dest1)

		devicemodel = str(localdevice.getmodel())

		version = False
		supportedModels = str(readmanifest("IPSW/BuildManifest.plist", version))

		arm64check = ('iPhone6,2')

		if any(ext in devicemodel for ext in arm64check): # devicemodel == NULL if something wrong
			pwndfumode() # my1

			createCustomIPSW64(devicemodel)
		else:
			print("ERROR: Unsupported model or device not connected!")
			exit(82)
	else:
		print('\033[91m' + "Invalid filepath!")

def createCustomIPSW64(devicemodel): # my2
	print("Starting iBSS/iBEC patching")

	patch_folder = Path("resources/patches/")

	phoneibec = patch_folder / "ibec5s.patch" #

	phoneibss = patch_folder / "ibss5s.patch" #

	version = True
	versionManifest = readmanifest("IPSW/BuildManifest.plist", version)
	version = False

	deviceManifest = readmanifest("IPSW/BuildManifest.plist", version)

	#--------------------------------------------------------------------------
	bsdiff4.file_patch_inplace("iBEC.iphone6.RELEASE.im4p", phoneibec)

	bsdiff4.file_patch_inplace("iBSS.iphone6.RELEASE.im4p", phoneibss)

	device = "iPhone5s"

	print("iBSS/iBEC patched")

	#--------------------------------------------------------------------------
	print("Re-building IPSW")

	shutil.move("iBEC.iphone6.RELEASE.im4p", "IPSW/Firmware/dfu/")
	shutil.move("iBSS.iphone6.RELEASE.im4p", "IPSW/Firmware/dfu/")

	shutil.move("IPSW/Firmware/Mav7Mav8-7.60.00.Release.bbfw", "resources/other/baseband.bbfw")

	shutil.move("IPSW/Firmware/all_flash/sep-firmware.n53.RELEASE.im4p", "resources/other/sep.im4p")

	touch("IPSW/Firmware/usr/local/standalone/blankfile")

	with ZipFile('custom.ipsw', 'w') as zipObj2:
		os.chdir("IPSW")

		zipObj2.write('Restore.plist')

		zipObj2.write('kernelcache.release.iphone8b')

		zipObj2.write('kernelcache.release.iphone6')

		zipObj2.write('BuildManifest.plist')

		zipObj2.write('058-75381-062.dmg')

		zipObj2.write('058-74940-063.dmg')

		zipObj2.write('058-74917-062.dmg')

		zipObj2.write('._058-74917-062.dmg') # !!!

		for folderName, subfolders, filenames in os.walk("Firmware"):
			for filename in filenames:
				filePath = os.path.join(folderName, filename)

				zipObj2.write(filePath)

		os.chdir("..")

		if os.path.exists("IPSW/custom.ipsw"):
			shutil.move("IPSW/custom.ipsw", "custom.ipsw") # main dir

	restore64(devicemodel)
