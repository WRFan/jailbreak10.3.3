import os
import shutil
import ipsw

def clean():
	if not os.path.exists('IPSW'):
		ipsw.removeFiles()
		pass

def yoshi():
	1==1