This repo provides some info on how to downgrade, jailbreak, and setup IOS 10.3.3 on an iPhone 5s.

The "install" script in this repo lists all post-jailbreak steps, so use that one in addition to this readme to guide you.

This repo provides sources only. The full package can be downloaded from the cloud:

https://cloud.mail.ru/public/RC9C/3hzpcTVcG

It contains everything from this repo, as well as anything else mentioned in the "install" script (debian install packages, iPhoneOS10.3.sdk, iPhoneOS10.3_headers, compiled files etc.)

Disclaimer:

Unless otherwise noted, all binary files are property of procurs.us + bingner.com + newosxbook.com ; all sources ("Packs" folder on this repo) are forked from the respective repos on github. I hope it's ok with you, guys; if you are the owner of a specific file I provide and want me to explicitly mention you, add a new topic to

	https://github.com/WRFan/jailbreak10.3.3/issues

Considering the dpkg packs, I wouldn't even provide them, but the problem is, the repos delete old files when updates are available, replacing them by new ones, which often contain symbols that are not available on IOS 10.3.3, so I'd like to keep the files online for the people who are still on 10.3.3.

1. Downgrade:

The files needed to downgrade are inside the "downgrade" directory on this repo. I was using a Mac Sierra for that. The files are a fork from:

	https://github.com/MatthewPierson/Vieux

You will need that repo to downgrade. However, this Vieux guy has some annoying errors in his python scripts. First, this guy seems to be unaware of the fact that an iPhone (at least, this particular model) will NOT stay in DFU mode forever. It will auto-exit this mode in like 10 minutes, so you have a limited window to install new firmware. This weird behaviour is not downgrade specific. The official Apple forums are full of complaints about this issue - people try to upgrade the iOS system, iTunes takes some time to download the firmware, then it attempts to install the downloaded firmware - too late! iPhone exits DFU mode. Apple specifically suggests to DOWNLOAD the system to your hard disk first to reduce the installation duration. Now look at what this guy is doing - his script will first put the device into pwned DFU Mode (which is a pain in the ars*, because it takes like 35 times average to succeed), THEN he creates the custom ipsw ("createCustomIPSW64" function inside "ipsw.py"). He thinks iPhone will patiently wait until his script is done! Of course, by the time his script is done, iPhone exits DFU, which means you have to start all over!

Not only that, he removes all created files on each attempt, meaning he re-creates the "custom.ipsw" all over again every time, and iPhone exits DFU right before the downgrade, again and again! Now, why is he deleting the custom ipsw?!! This file needs to be created only ONCE, it can be used for any iPhone 5s, so the entire "createCustomIPSW64" function needs to be executed only once, and BEFORE the iPhone is put into DFU mode.

The "createCustomIPSW64" also zips "._058-74917-062.dmg" file from the original ipsw at:

https://ipsw.me/download/iPhone6,2/14G60

Be careful about files containing a dot at the start of the name. If you create a backup of the downgrade folder by copying the folder through Mac Finder (that's the equivalent of Windows Explorer for us Winnies, lol), this file will NOT be copied, so this may cause the downgrade scripts to fail. I don't even think this file is necessary for the downgrade. I think you can comment out that line inside "createCustomIPSW64" function. If you insist on copying it, you need to copy the folder through the Mac terminal (== cmd)

Overall, it's best to create custom.ipsw once and be done with it.

After putting my iPhone like 1 trillion times into pwned DFU mode and failing to upgrade due to errors in the original scripts, I finally got a little further, only to be presented with a cryptic failure message. So I executed the individual steps manually through the terminal and... tsschecker and irecovery from:

	https://github.com/MatthewPierson/Vieux/tree/master/resources/bin

got

	Killed: 9

Oh. My. God. This. Is. NOT. Happening! This guy actually forgot to add the damn entitlements! Has he even tested his damn scripts?!!!

	ldid -S irecovery

	ldid -S tsschecker

If you want to edit linux or darwin scripts on Windows, do NOT forget the damn carriage return problem! "\r\n" is the standard Windows way of line breaking, but some programmes (yes, radvd and python, I'm looking at you!) will complain about improper syntax, because darwin expects "\r" and unix "\n" (if I remember correctly), so make sure you format your plain text files properly before uploading them to non-windows systems. Again, some files downloaded off the Mac will look extremely weird in most Windows text editors, Winword is the only programme that can format the darwin stuff properly. The carriage return is also a constant source of trouble when dealing with regex in ANY language. Will there EVER be an end to this relict of the past or are you going to drag it with us forever?!!

If you are going to edit the python scripts, be aware that this insane language uses indents (tabs and spaces) for statements following conditionals whereas sane languages use {} and that tabs and spaces are NOT interchangeable. You don't want to go through the pain of setting the device in pwned DFU mode just to find out there's an error in your script, because you mixed tabs and spaces or something.

Now let us go over the downgrade process step by step.

First, get the files from the repo I mentioned and edit the scripts; again, check the "downgrade" folder on this repo, but don't just stupidly overwrite the vanilla scripts by my versions, because I, e.g., disabled the function responsible for the creation of "custom.ipsw", since I already created it, but you still got to run it once. So my version of "ipsw.py" says:

	pwndfumode()

	restore64(devicemodel)

and you need to do once:

	pwndfumode()

	createCustomIPSW64(devicemodel)

then you can switch off the "createCustomIPSW64" function and proceed with "restore64" directly.

This folder contains the files required for downgrade:

	https://github.com/MatthewPierson/Vieux/tree/master/resources/bin

Now, this is weird, but the "futurerestore" executable can NOT be updated with a newer version from here:

	https://github.com/tihmstar/futurerestore/releases

The downgrade will simply fail. The other files can be updated, but again, check the damn entitlements!

The python scripts require, surprise! python + some additional python modules:

	https://github.com/MatthewPierson/Vieux/blob/master/requirements.txt

+ you need ldid as well, so most people will tell you to use homebrew to install this stuff on Mac. Don't. This programme is extremely annoying. It will refuse to run as root, took me like an hour to kill that stupid useless check. Most people say it's extremely important not to run homebrew as root. Important my ass! Plus it puts installed files in non-standard folders, like services usually go to

	/System/Library/LaunchDaemons

and homebrew puts them somewhere else and tells you to run services through itself, what, something wrong with "launchctl"? It will also fill your entire hard disk with useless crap files, makes it nearly impossible to downgrade to lower versions, has problems with non-standard repos etc. etc. Install programmes directly, your OS will be grateful and you'll save some nerves.

Second, put the device into DFU mode, google it, also check the specs if you are interested:

	https://usb.org/sites/default/files/DFU_1.1.pdf

Third, run the checkm8 exploit. I actually run that exploit separately:

1st terminal window:

	ipwndfu -p

then, if the device is pwned successfully, I execute the actual downgrade in a second terminal window through:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/downgrade/vieux

Prepare everything beforehand, because, as I said, the device will not stay in DFU mode forever!

Now, I'm not going to explain what the checkm8 exploit does, it's been all over the news. Everybody will tell you how great it is, but let me share some experience I had with it. First, the ipwndfu exploit takes ages to succeed (35-40 times average), so not so "extraordinary" from my point of view. Second, the checkra1n programme (based on this exploit):

	https://checkra.in

This damn programme is the very reason why I downgraded in the first place, because at that time unc0ver hasn't yet released an exploit for iOS 12.4.7 I had installed, so I was stuck with checkra1n. Now, I don't know the current state of development, nor do I give a damn about it any longer. The developer used to say "linux and mac supported", but that's a lie, linux was not supported, and probably still isn't. I tried to jailbreak through VMWare virtual machine, the app just crashed. Well, the developer will tell you not to use a virtual machine. So I cloned the virtual machine to an usb stick, booted from it, executed the programme, and it crashed again on the same step! So I checked github, the developer says, linux is not supported, but on his webpage he states, it's supported. Are we going to trust a guy who's lying in our faces?!

So, on Mac Sierra this jailbreak was running without problems... or so I thought, because after some time you'll notice the app will crash on a Mac very often too.

So since the jailbreak was working on Mac (well, sometimes), and since I don't own a Mac (the Mac I used belongs to a friend), I was thinking about creating a Hackintosh to run the app on PC. Now creating a Hackintosh is a science for itself, so it took me ages to create an usb stick with a Mac on it, there are also some github repos that provide such usb images, but then I actually booted the system, and you know, how long it takes for it to boot? 8 fuc*ing minutes! That's every time you need to jailbreak! Plus you need to plug the device into your computer, extremely inconvenient. Plus I never got the damn mouse working, and I use a standard Microsoft wired mouse.

Also, the press gets all euphoric, telling you how this exploit cannot be fixed, because it's iBoot magic, so not dependent on software. Really? Try to jailbreak an iPhone 5s (hardware supported by checkra1n) running iOS 10.3.3. checkra1n will say "unsupported". How interesting! So it IS dependent on software! So there's the super-duper checkm8 exploit for you. Back to business.

Also, I've got a 32-bit linux OS, so that . So I was looking for an "i386" version on his page. Nada. Instead, he lists "i486". Now, what's that? I know! it's the 486 pre-Pentium intel CPU! So no "i386"? Turns out, "i486" is his double-talk for "i386". Enough said.

Running ipwndfu separately from the actual downgrade process has the advantage that you can choose the ipwndfu repo you wanna use:

	https://github.com/axi0mX/ipwndfu

	https://github.com/geohot/ipwndfu

	https://github.com/LinusHenze/ipwndfu_public

the geohot fork is based on python 3 and is more verbose. It was running fine first on my Mac, then, all of a sudden, it stopped working (I didn't change a thing! Damn Apple! Their computers fall apart for no particular reason!), so I switched to the ipwndfu_public fork (python 2). In terms of success, there's really no difference, you are doomed to try many times until the operation succeeds. HOWEVER, be careful what ipwndfu outputs. If it says

	failed to enter pwned dfu mode

or something like that, that's ok, keep trying. But if it constantly complains that it can't detect a device in DFU mode (and your device is actually in DFU mode and connected to the computer), then you can stop trying, because there's something wrong with your setup. I heard of a guy who tried 200 times and failed. He's insane, you are guaranteed to succeed within 40 tries at most. For example, I created an Ubuntu usb stick, installed everything that is required by ipwndfu, but it still failed to pwn the device (the system itself was recognizing it). It always said "can't detect a device in DFU mode". So stop trying, fix your setup first:

- Use a Mac

- Don't use usb hubs, 99% probability it will constantly fail. Connect the device directly to the computer

- Don't run iTunes, if you started it, reboot the system before downgrading. I once started iTunes, then shut it down, tried to downgrade, but the process failed, rebooted the mac, downgrade ok, so iTunes messes something up permanently until next boot.

Another quirk of the iPhone 5s: ipwndfu will run ONCE, if it fails and you try again, the device will exit the DFU mode and you get the "cannot detect device in DFU" mode error. So this whole process is very annoying: DFU, try once, fail, get the device out of DFU, put it back into DFU mode, try again, now imagine doing this 40 times in a row!

So much for the downgrade process. Btw, the advantage you have with iPhone 5s, you don't need to have shsh blobs to restore. "What's shsh?" Exactly. If you just asked yourself this question, this downgrade is for you. You lucky guy!

2. Jailbreak:

I just downgraded my device and wanna jailbreak! What are gonna use? That's the 1 mio $ question. I suggest you take a peek into the "jailbreak" and "Docs" folders in this repo, primarily read the provided pdf files to get an idea about KPP bypass and the KPP-less jailbreak variants. There are actually two bugs that are exploitable on iOS 10.3.3:

V0rtex:

	CVE-2018-4233

	CVE-2017-13861

	https://support.apple.com/en-gb/HT208854

	https://siguza.github.io/v0rtex

and sock_port:

	CVE-2019-8591

	CVE-2019-8605

	https://support.apple.com/en-gb/HT210118

	https://github.com/jakeajames/sock_port

	https://github.com/jakeajames/sock_port/tree/sock_port_2

There are several jailbreaks, but it's basically the same stuff inside, check out the repos:

V0rtex KPP bypass:

	https://github.com/Siguza/doubleH3lix

	https://github.com/tihmstar/doubleH3lix

	https://github.com/Sticktron/g0blin

V0rtex KPP-less:

	https://github.com/PsychoTea/MeridianJB

sock_port KPP bypass:

	https://github.com/SongXiaoXi/sockH3lix

All these jailbreak apps are completely useless for us, however, because, in case you haven't figured it out yet, only native Apple apps and those certified by Apple are allowed to run on iOS, and Apple will obviously not sign a jailbreak, lol. Naughty Apple. So this is a vicious circle - you need an app to jailbreak, but precisely this particular app will not run on a non-jailbroken system. Now we could now consider provisioning:

	/private/var/MobileDevice/ProvisioningProfiles/

mention ReProvision:

	https://github.com/Matchstic/ReProvision

Xcode, Altstore, cydia impactor, AltDeploy:

	https://github.com/pixelomer/AltDeploy

Except, we won't, at least leave me out of it. Provisioning does not work. Has never worked, will never work. Apple constantly changes the signing process, all programmes created for signing go down one by one, some are updated, but it takes time, so people are stuck with their devices waiting for a jailbreak, lamentations all over the internet. The free certificate is only valid for 7 days, the developer one for a year (300 bucks!), plus Apple will kill the certificate prematurely if it detects a jailbreak, check:

	https://github.com/WRFan/jailbreak10.3.3/tree/main/dnsmasq

to block iOS from talking to Apple servers. btw, AltDeploy died on me a couple of days ago, another one bites the dust.

Now if you think it's funny, go ahead, upgrade to 12.x and unc0ver, but I'm staying right where I am, because the 10.3.3 is vulnerable to this cool Webkit (Safari engine) exploit. Safari is allowed to run on an unjailbroken system, so there's no need to worry about provisioning. That's the good news, the bad news is the code is partially closed:

	https://github.com/JakeBlair420/totally-not-spyware

Jailbreakers with a conscience? Well, but since there are only a few devices left running iOS 10.x, I think it's time to release the missing code - "genesis":

	https://github.com/WRFan/jailbreak10.3.3/blob/main/jailbreak/all.js

	https://github.com/WRFan/jailbreak10.3.3/blob/main/jailbreak/payload.pdf

Hope, Siguza releases it, I'd really like to compile the sock_port exploit (sockH3lix jailbreak above) as a payload for Safari (sockH3lix is just an app, means it requires provisioning).

The "jailbreak" folder in this repo is a fork of the github repo mentioned above +

	https://totally-not.spyware.lol

	http://jailbreakme.com

I basically got it safely sitting on my local apache server, just in case the web server goes down. Now, because the project is partially closed source, I had to hack edit the binary payload, which is just ridiculous. Please, Siguza, give us the code! Pleeeasse?

So if you check:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/jailbreak/payload

I changed the server address to 192.168.0.11, which is the address of my local server. So if intend to host the files at a different address, you need to hex-edit the "payload" file and adjust the ip. This ip is just required once, to get the bootstrap when you jailbreak for the first time.

All of the files in the "jailbreak" folder must be hosted on a server, not just the binaries. Waaait, why a server for html / javascript? It's client-side, why not jailbreak once by getting the files from a local server, then put the files on the iOS hard disk and execute index.html inside Safari? Hahaha, you are kidding, right? We are talking APPLE here! Is there anything Apple will allow us to do on OUR phones? Like executing locally stored html files in Safari? No, really, don't be naive.

The binaries used by the jailbreak are in the 7z file (cloud.mail.ru link above), or you can grab the originals from here:

	https://totally-not.spyware.lol/manifest.appcache

	https://totally-not.spyware.lol/payload

	https://totally-not.spyware.lol/bootstrap/DH.tar.xz

	https://totally-not.spyware.lol/bootstrap/Meridian.tar.xz

However, I adjusted the DH.tar.xz file (cloud link above) by adding some apps to be independent from Cydia (PATHETIC!) and Apt (CRAP!) right from the start:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/jailbreak/jailbreaker

Because you obviously need at least wget + openssh to get started in the first place, but totally-not.spyware.lol doesn't provide them. They do provide Cydia and Apt, though, as every other jailbreak out there does, plus Apt ships with every linux distro as well, something I will never comprehend, considering the pathetic nature of these programmes. As you can see, I'm using dpkg:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/install

which gives you total control over the installation of apps. You can check the install package, unpack the files, test them first, see if they are working, if there are dependencies, they are not downloaded and installed automatically, you have to provide them to dpkg, so again, you are in control, you can check those packages first, see if they are compatible with the system. Apt will simply install everything, you do it through Cydia, you see nothing of what's going on behind the scene, plus Cydia causes most problems itself, especially on older iOS systems, since the newest version is incompatible with iOS 10.x and the old version is incompatible with some repos. Most people end up with "white apple forever on boot", because they click "Install" in Cydia without considering the consequences.

So concerning the jailbreaks, there are not exactly many choices. We're stuck with the Siguza payload. Now to make this clear: This payload is basically a combination of the following two jailbreaks:

	https://github.com/Siguza/doubleH3lix

	https://github.com/PsychoTea/MeridianJB

No sock_port exploit, unfortunately. You will be presented with a choice the first time you jailbreak. Which to choose? Both of them have their problems. doubleH3lix just loves to crash both during the jailbreak process as well as later on - if you survive the first 20 minutes jailbroken, you are safe, but within the first 20 minutes there's a very high probability the device will bluescreen, plus if you execute the jailbreak right after boot, there's an extremely high chance it will panic, hence the cooldown delay in the app versions:

	https://github.com/Siguza/doubleH3lix/blob/master/doubleH3lix/ViewController.m

I tried to understand what causes the bluescreens, but nobody explains anything. From what I gathered, KPP detects an anomaly and immediately causes a panic. But why the delay after the boot? And why the increased probability of a bluescreen within the first 20 minutes after the jailbreak? From my understanding, KPP checks the integrity all the time, though at random intervals. So if the checks are random and constant, the probability for a bluescreen right after the reboot should be exactly the same as later... Am I missing something?

The bluescreens are problematic, because if they happen when you change some stuff on the hard disk, the device may not boot depending on what you were doing.

Yeah, but isn't there some precaution system? Like dism on Windows, to create a backup of your harddisk to restore it, in case something goes wrong? You are kidding, right? Haven't I told you to stop being naive? Remember? It's Apple! Well, sure, you got "dd" on iOS:

	https://manpages.debian.org/unstable/coreutils/dd.1.en.html

so go ahead and make an image of your hard disk, but restoring? Nada. Yeah, but iTunes can restore, right? STOP ASKING STUPID QUESTIONS! iTunes will install the newest version of iOS, your downgrade is gone, your jailbreak is gone, anything you installed is gone, unless you acquired it from Appstore! Hell, even some Appstore stuff is gone! Your music is gone! Half your preferences are gone! You have to start fresh. Check your iTunes folder:

	/Users/XXX/Library/Application Support/MobileSync/Backup

You have 4 GB data on your phone, yet the backup is 20 KB small. You think Apple compressed 4 GB into 20 KB? iTunes is not actually restoring anything, it downloads anew, installs anew, and forgets to restore 3/4 of your apps and preferences while doing it.

The Meridian jailbreak is kpp-less, so I wouldn't recommend it. It's not a real jailbreak, the jail is just a little larger. Now you'd think, it's more stable than KPP bypass, because it doesn't annoy KPP that much, but according to people who tested it, it causes the system to panic as well, maybe for different reasons, but a bluescreen is a bluescreen, the result is the same.

sock_port is supposed to be more stable, so maybe Siguza or somebody else can give us a sock_port version of the payload.

3. Post-jailbreak setup:

All right, so we jailbroke the device, for better or worse. Now what? Well, we install some cool apps! Lots of them! Tons of them! Except, there's a tiny problem. The repos on the internet:

	https://www.ios-repo-updates.com

	https://apt.procurs.us/pool/main/iphoneos-arm64/1600

	https://apt.bingner.com/debs/1443.00

don't really care about us poor shmoks still running 10.x iOS. They compile for later versions of iOS, means the apps rely on symbols not present on 10.x iOS. That's why I uploaded the 10.x iOS compatible apps to icloud, the files that work on iOS 10.x are very difficult to find, plus you need to check the symbols used by executables and dylibs (libraries) to determine if a programme is compatible. In particular, the following symbols are causing problems most problems:

	_chkstk_darwin

	_objc_alloc_init

	_utimensat

	_futimens

Plus, some apps compiled for later iOS versions try to access system directories not present on 10.x (check "uicache" provided by the uikittools, the procurs.us version, for an example), which makes such programmes useless.

Now you may argue, why not use the Saurik repo?

	http://apt.saurik.com/debs

Please, can we stop talking about Saurik? Is he some kind of panacea for all of our problems? Those repo files are ARM, means 32-bit, your device is 64-bit. Those files are old. Old is bad. Because files are updated and bugs are fixed. That's what updates are there for.

You can check for symbols on Windows using my binary search vbscript:

	https://github.com/WRFan/jailbreak10.3.3/tree/main/binarysearch

But that's stupid, check right on iOS the proper way:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/Packs/Work_IOS.txt

	jtool -arch arm64 -S XXX

	dyldinfo -export XXX

	nm XXX

In fact, I never managed to find some files working on iOS 10.x, so I had to compile myself:

	https://github.com/WRFan/jailbreak10.3.3/tree/main/Packs

The binaries are in the cloud (link above). I compile right on the phone, but here's the problem, there are two versions of the iPhoneOS10.3.sdk on the internet, one of them is totally broken, yet every single moron links to that repo and it appears first on google. The SDK I provide in the cloud is the proper version. I also provide the iPhoneOS10.3 headers. compiler installation instructions:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/install

As you can see, I'm using cmake to compile. Now I may be stupid, but what's the purpose using Theos if you already have a cross-syntax compilation wrapper, which uses the same syntax across all systems, so if you ever used cmake on Windows to control MSVC, you'll be able to compile darwin apps on iOS within seconds? Theos is large and obscure, the code is obfuscated, so code looks like it's been edited by a madman:

	https://github.com/pNre/NoAnnoyance/blob/master/SpringBoard.xm

	%ctor

What's a "%ctor"? A new jailbreak? No, really it's ridiculous. I'm staying with cmake.

Now if you take a look at the "Packs" directory, you'll notice a folder called "unzip-lzfse". You can get the compiled version from the cloud. I haven't found an lzfse capable iOS unzip binary anywhere, so I compiled it myself. What's lzfse? Well, you see, Apple used to package their Appstore apps using standard zip format, but they now switched to this new type of compression (by facebook?), so most programmes fail to extract the Appstore archives. 7z can do it, but you need the newest version. Plus, no 7z on iOS (I think?). That's where this repo comes into play:

	https://github.com/sskaje/unzip-lzfse

All right, so we've got an app to extract .ipa files distributed by Appstore, but how do we actually get a link to such a file? Well, that's easy, right? Add a proxy to your wifi, sniff the network traffic between the Appstore app and the Appstore servers. Easy peasy! Wait... it's not working. What the hell is going on?! Apple is going on, my friends, it's that damn Apple again! First Eve, then Adam, now we have Darwin! The apple is the arch-cause of the defect in our moral system, the Original sin of man, the seducer and liar, the paradise snake, Satan and Mephistopheles!

The key word here is "SSL Pinning". So basically, Apple will prevent certain programmes like Appstore as well as certain daemons from fetching stuff from Apple servers if the certificate compiled into those programmes/daemons does not correspond to the certificate returned by the server. Since a proxy passes its own self-signed certificate to the programme accessing the net, the certificate is not matched, connection refused.

Do you realise what this means? If you are on an university network, for example, there may be a proxy server in-between, so Appstore etc. will just fail to work there, since it would get the certificate of the university proxy. Security, lol. Apple is so secure, it's unusable. But there's Apple for you, it's all about control. Microsoft and Google like control too, but this kind of nonsense is limited to Apple only.

To circumvent SSL Pinning, install this (check my "install" script on this repo):

	https://github.com/nabla-c0d3/ssl-kill-switch2

There are several versions of this mobile substrate plugin as well as several versions of the preferenceloader required by this plugin, the ones mentioned in my "install" script are the only ones working, well, kind of...:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/Library/MobileSubstrate/DynamicLibraries/SSLKillSwitch2.plist

The plugin creator tells the mobile substrate to inject the plugin into all programmes linking to

	com.apple.AuthKit

	com.apple.UIKit

Pal, if you are reading this, this is just stuuuupid:

	grep -lir /AuthKit.framework/AuthKit /

	grep -lir /UIKit.framework/UIKit /

There are tons of programmes linking to these frameworks. In particular, this one:

	/Applications/InCallService.app/InCallService

Execute:

	lsof /Library/MobileSubstrate/DynamicLibraries/SSLKillSwitch2.dylib

We see, it's injected. Now wait like an hour, try to make a call:

	sbopenurl tel:+49XXX

	Call failed.

Call your cellular provider using /Applications/MobilePhone.app/MobilePhone:

	*#31#

Input the number, hit the green button, nothing happens. Now, this cost me some nerves and time to find out what was causing this. There are only so many programmes using SSL pinning, and only those programmes need to be targeted!

Also, I've never been able to actually install whatsapp over Appstore + proxy, even with SSL pinning disabled. I was able to sniff out the entire traffic, get the .ipa link, everything was downloaded properly, but during the actual installation I got some error, don't remember what it was. That's irrelevant, however, since once you got the traffic log, you can disable the proxy.

To actually disable SSL Pinning, the plugin must be injected into all apps/daemons used during the Appstore download process, and since Mobile Substrate is loaded AFTER boot, you need to restart the daemons:

	launchctl unload /System/Library/LaunchDaemons/com.apple.akd.plist && launchctl load /System/Library/LaunchDaemons/com.apple.akd.plist

	launchctl unload /System/Library/LaunchDaemons/com.apple.appstored.plist && launchctl load /System/Library/LaunchDaemons/com.apple.appstored.plist

	launchctl unload /System/Library/LaunchDaemons/com.apple.itunesstored.plist && launchctl load /System/Library/LaunchDaemons/com.apple.itunesstored.plist

But, as I said, I still run into problems during the actual install process.

Personally, I'm using Fiddler to sniff out http(s) access. Obviously, requires an Windows OS, set the iPhone proxy to the ip address of your Windows machine and port to the port of Fiddler. When downloading something off Appstore, it will display something like:

	http://iosapps.itunes.apple.com/itunes-assets/Purple124/v4/17/2b/59/172b590d-d8f5-a77f-870e-f0e3df064742/pre-thinned13567484966014256980.thinned.signed.dpkg.ipa?accessKey=XXX

where "accessKey" is obviously a string generated by the server to allow you to download the programme, short-lived so as to prevent people from posting direct download links on the internet.

This ipa is properly signed, can be also copied to other phones and whatsapp (or whatever you download) will work in an unjailbroken state. Get the whatsapp install dir:

	getappdir net.whatsapp.WhatsApp

	/private/var/containers/Bundle/Application/XXX/WhatsApp.app

delete WhatsApp.app folder, unzip .ipa, there's a "WhatsApp.app" folder inside, copy it to the original location, reboot (!!!) Reboot is important (even if Whatsapp wasn't running when you replaced the files!), otherwise you'll get tons of errors and Whatsapp will crash.

Talking about certificates, we still need a certificate for Safari. There's no SSL Pinning on Safari, but it still requires a valid certificate for https connections thru a proxy, as any other browser out there does. This poses a problem, which is not iPhone related. A HUGE problem. Every single browser out there (and every single browser version) has different requirements concerning security. So creating a certificate for your proxy that works across all browsers is nearly impossible. You have to consider IE 8 (WinXP), IE 11 (Win 10), Firefox, Chrome, and Safari. It took me some time to create a certificate that was working in all versions of IE + Firefox. Then I get this damn phone, Safari doesn't like the certificate. Apple and their damn security...

So first, how do you install a certificate on an iPhone? Just put it onto your local web server, navigate with Safari to it and click on it. It will be added, but it still needs to be enabled in the iPhone settings (Settings -> General -> About -> Certificate Trust Settings). Stupid iTunes does not backup this properly, so you restore iPhone, you may have to redownload the certificate.

So Safari was refusing to use my cert, while IE and Firefox had no problems with it. Generally speaking, IE requires least security, FF is annoying, but Safari is insane. Can't talk about Chrome.

So I messed around and got Safari to accept the cert by changing the md5 digest to sha1 and "basicConstraints" to "CA:true", but I think, it was md5 that was causing the problem. Newer iPhones have even tighter security, there's an Apple page that talks about this.

While checking the net on this issue I stumbled across this page:

	https://security.stackexchange.com/questions/68491

So I am like, I gotta consider this, FF may have problems with extendedKeyUsage. So I create my cert:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/openssl/config.cfg

	https://github.com/WRFan/jailbreak10.3.3/blob/main/openssl/readme.md

	openssl req -new -key "ca-key.pem" -out ca.pem.crt -x509 -days 25000 -sha1 -subj "/CN=WRFan" -set_serial 0x0

setting "extendedKeyUsage" to "anyExtendedKeyUsage" only. "Any" means "any", right? Well, Internet Explorer agrees, Safari agrees... FF doesn't:

	SEC_ERROR_INADEQUATE_CERT_TYPE

Really, I can't decide, which browser is more annoying, Safari or FF. They both suck.

So I added any extended key usages I could find (tons of them!) SPECIFICALLY to "config.cfg"... behold! FF is finally accepting the cert. Stupid FF! Ok, so IE 11 is working, FF is working, Safari's working. What about IE 8 on WinXP? So I boot WinXP, test it... not working. WTF? Seems not to like the "basicConstraints = CA:true". Fu*k that, I was sick and tired of this old system, so I ripped out the hard disk containing WinXP out of my PC, problem solved, lol.

So let's go over the

	https://github.com/WRFan/jailbreak10.3.3/blob/main/install

script:

First, we kill Cydia. Oh no, we killed Cydia! What are we gonna do? Well, I've said everything that needed to be said about Cydia above, and I am not going to repeat myself. You may wonder, why I added this:

	mkdir /Applications/Cydia.app

that's why:

	https://github.com/Siguza/doubleH3lix/blob/master/doubleH3lix/jailbreak.mm

It's checked, but just the folder itself, doesn't have to be anything inside. Also check:

	/private/var/installd/Library/Logs/MobileInstallation/mobile_installation.log.0

	<err> (0x16e077000) -[MIBundle _validateWithError:]: 37: Failed to load Info.plist from bundle at path /Applications/Cydia.app

lol.

Generally, it's a good idea to be aware of the files jailbreaks usually check:

	/.cydia_no_stash

	/Applications/Cydia.app/

	/.meridian_installed

	/meridian

	/bin/tar

	/bin/launchctl

Logic (pseudo-code):

	if !/.cydia_no_stash and !/meridian -> ask: Meridian or DoubleH3lix ?

	if /.cydia_no_stash and !/meridian -> use DoubleH3lix

There was a guy on reddit, who deleted /.cydia_no_stash , then suddenly, his jailbreak wasn't working. lol. Some people shouldn't be allowed to touch computers.

Next, we delete all Apt crap.

Install basic programmes like wget and ssh (I added them to my DH.tar.xz available on the cloud - see above, but you still need to install them to add them to dpkg properly). Concerning the binaries, some links in the "install" script still point to the original locations, others, which are not available online any longer, point to my local servers (Apache on linux + filza WebDav on my other phone). You obviously need to adjust them. Download my 7z archive from the cloud and put it on your local apache or something to wget the files onto the iPhone.

Again, be careful what files you install, always check the symbols (see above)! The files I provide are working, although there are still some incompatible symbols in some apps/libraries, but I haven't run into any problems.

The order in which you install apps is very important, because there are cross-dependencies all over, so strictly follow the order from my script!

To access the device over ssh, use:

	putty -load "iphone" -pw alpine

where "iphone" is your config file for putty. Check the putty and ssh man pages yourself. Also check:

	/private/etc/ssh/sshd_config

ssh is extemely annoying, because it won't allow you to connect if you don't have a password set on the guest system. I killed the password both on my linux and my Mac:

	/etc/sudoers

this file has extremely tight permissions, so you must chmod it even as root. If you look on the net, nobody will tell you how to kill the password and auto-login on linux / darwin (terminal), instead you just get responses about how important security is. Has everybody been infected by paranoia out there?! So here's a quick instruction:

/etc/shadow :

to remove root password, delete everything between the first and second colon:

	root:XXX:

/etc/sudoers:

	chmod 666 /etc/sudoers

	root ALL=(ALL:ALL) NOPASSWD:ALL

	%admin ALL=(ALL) NOPASSWD:ALL

	%sudo	ALL=(ALL:ALL) NOPASSWD:ALL

To login as root on Mac terminal, edit:

	/users/XXX/.profile

where XXX is your username

If you don't have a pass, you need to authenticate to ssh using a key. Damn security, it causes most problems on all systems.

/etc/ssh/sshd_config :

	PubkeyAuthentication yes

	AuthorizedKeysFile /etc/ssh/authorized_keys

	UsePAM no

You can get info on the net concerning the generation of ssh keys using "puttygen" or whatever you are using, and how to pass them to ssh (AuthorizedKeysFile). This isn't required for an iPhone, though, since you have a password there. Never bothered to remove it.

Note that Meridian installs dropbear, but I prefer openssh. You can tell Meridian not to start it and install openssh instead, but Meridian doesn't handle it properly, it just loads

	/Library/LaunchDaemons/com.openssh.sshd.plist

but this is not working on iOS 10.x. doubleH3lix takes this into account:

	https://github.com/Siguza/doubleH3lix/blob/master/doubleH3lix/jailbreak.mm

but not Meridian, so if you wanna use openssh on Meridian, you need to fix the com.openssh.sshd.plist file.

While at it, what services/files are actually loaded by specific jailbreaks AFTER the jailbreak?

doubleH3lix and Meridian launch services at:

	/Library/LaunchDaemons/

doubleH3lix ignores the com.openssh.sshd.plist properly, which Meridian does NOT.

doubleH3lix also executes all scripts at:

	/private/etc/rc.d

which for example, loads the mobile substrate:

	/private/etc/rc.d/substrate -> /usr/libexec/substrate -> /usr/libexec/substrated

I am using this folder too to run:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/private/etc/rc.d/zzz

The "install" script (this repo) changes some stuff in the

	/System/Library/Caches/com.apple.dyld/

cache. This is tested and working, but that doesn't mean it's safe to mess around with it - some changes will result in a permanent white apple on boot. This is true for any file whose integrity is checked on boot! Even if a file is not generally required to boot the iOS! So, e.g.:

	/usr/libexec/locationd

is not required to boot the system, but if the service is enabled and you change even a single byte in that file, then reboot the iPhone - congratulations, you just killed your device. Just compare - on Windows, you can load DRIVERS that are unsigned or signed with an invalid certificate:

	bcdedit /set nointegritychecks Yes

	bcdedit /set {current} testsigning Yes

but just try to run an altered pathetic useless "locationd" on iOS!

The primary reason I'd jailbroken was to prevent the iPhone from accessing the network and the internet. Fire up Wireshark and check what an iPhone is doing - it's hammering request after request, mDNS RAs on the local network, sending data to apple servers... It's exactly the same with Macs. No network can handle this amount of traffic. I've read complaints by admins, whose routers break down under the load, when too many iPhones sign into their wifi networks. iOS 12 is extremely annoying in terms of mDNS RAs, that's another reason I prefer iOS 10.x, although, for some completely incomprehensible reason, it will sometimes (extremely seldom) send

	MDNS Standard query PTR _sleep-proxy._udp.local, "QU" question OPT

I'm trying to combat this by:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/private/var/preferences/com.apple.mDNSResponder.plist

	https://github.com/WRFan/jailbreak10.3.3/blob/main/Library/LaunchDaemons2/com.apple.mDNSResponder2.plist

(the second one is loaded thru:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/private/etc/rc.d/zzz

but I'm still not sure if this actually helps, it's a solution for real Macs, not iOS.

Most internet access by iPhones can be stopped though by disabling the services. And here we are - the SERVICES! Services is the major source of trouble, yet the most surprising fact is that nobody knows what services actually do or if the system will boot if you disable a particular service. This is true for all systems - windows, linux, darwin. One might think, it is easier with linux, since it's open source, but unless you dive deep into the source code, you can't easily find out what a service does. Nobody cares about this, nobody knows anything, except the developers of course, and MS and Apple are not talking. Or they are talking, but nobody understands a word of their tech-babble. Now let me pose a question to you:

	HKLM\SYSTEM\CurrentControlSet\Services\ksecdd

Needed to boot, yes, no? What does the service do? Nothing? You don't know? Get the hell out of my class-room!

	/lib/systemd/system/systemd-udevd.service

Needed to boot, yes, no? What does the service do? You don't know?! Get into the corner of shame, you ignoramus!

	/System/Library/LaunchDaemons/com.apple.oscard.plist

Nothing? Failure, total failure! That's the kind of pupils we have nowadays!

Messing with services is extremely problematic on iOS, because Windows and Linux can be tested on a virtual machine, so if the system doesn't boot, you just restore a snapshot, but if iOS doesn't boot, you are done for. The only people who actually concerned themselves with iOS services to a greater extent are some Chinese people, translate.google.com is your friend. There are some pages, which will speak about some services, but not many, other internet resources will mention Mac services, but there's no guarantee a service that can be disabled on a Mac can be safely disabled on iOS either. A service may not be required on one system, but very well required on many other systems (mDNSResponder). The problem is, morons on the net state something, but nobody ever mentions, what system they use, this is true for any problem, not just services-specific. So a moron says, mDNSResponder can be disabled, because his system uses discoveryd for DNS. So you disable mDNSResponder on Sierra/iOS 10.x - 12.x, try to ping google.com, nada. Hey, but that guy said, mDNSResponder isn't required! Yeah, just listen to those morons. Other idiots are even worse, they just copy-paste info from other pages to their pages, without testing, so errors spread all over the net. So let me tell you some stuff I've found out about iOS services MYSELF, stuff, most of which nobody knows about... except Apple... and me... and now YOU! You lucky guy!

On darwin, launchctl is the equivalent of systemctl. You can get info about that on the net.

Services disabled by launchctl are saved to:

	/private/var/db/com.apple.xpc.launchd/disabled.plist

Check:
	https://github.com/WRFan/jailbreak10.3.3/blob/main/private/var/db/com.apple.xpc.launchd/disabled.plist

you must reboot the system right after changing this file to make changes permanent, because preferences are cached by:

	/System/Library/LaunchDaemons/com.apple.cfprefsd.xpc.daemon.plist

so your changes will simply be overwritten, or you need to kill "cfprefsd". Check for an example:

	https://github.com/SongXiaoXi/sockH3lix/blob/master/sockH3lix/jailbreak.mm

haven't tested this, I just reboot if I change some preferences.

The /System/Library/LaunchDaemons/ directory is completely and utterly IRRELEVANT on iOS. It's incredible that nobody seems to have noticed it. There are actually morons on the net, who suggest changing this or that plist. I was as naive first as well:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/System/Library/LaunchDaemons/com.apple.mDNSResponder.plist

There's actually a moron out there suggesting to edit

	/System/Library/LaunchDaemons/com.apple.backboardd.plist

	KeepAlive -> NO

	RunAtLoad -> NO

Blessed are the spiritually inept! Pal, if your "fix" actually worked, your springboard (== Windows desktop) would be DEAD!

Truth is, the services are cached inside XPC_PLIST_CACHE:

	https://opensource.apple.com/source/launchd/launchd-842.90.1/support/launchctl.c.auto.html

	/System/Library/Caches/com.apple.xpcd/xpcd_cache.dylib

It's open source, so, damn it, people, READ! You can read, do you? If you could mess around with the services that run on boot, it would be much easier to create untethered jailbreaks. Now look how many we have today - a whole lot of ZERO!

So about the actual services. Let's start with a complicated one:

	/System/Library/LaunchDaemons/com.apple.apsd.plist

Needed, yes, no? Again, you know shit. Figures. Let's take a look at what Apple has to say about this service:

	https://developer.apple.com/library/archive/documentation/NetworkingInternet/Conceptual/RemoteNotificationsPG/APNSOverview.html

	Apple Push Notification service (APNs) is the centerpiece of the remote notifications feature.

Huh? Bla bla bla? What the hell did that just say? You don't get it? Me neither. WHAT IS THIS DAMN SERVICE FOR?!!

All right, try the following: install Whatsapp, disable the service, let somebody call you over whatsapp. Your phone doesn't ring? Missed call? Enable the service, block internet access to

	init-p01st.push.apple.com

Enter Airplane mode, exit. Let somebody call you. Doesn't ring.

Block access to all domains containing:

	push.apple.com

Enter Airplane mode, exit. Let somebody call you. Doesn't ring.

So now you know what this service is for and what domains you have to unblock. As opposed to MS and Apple, I've told you this in plain English, which anybody can understand, not just tech freaks.

Probably required for iMessage / Facetime activation, not sure yet, have to test:

	/System/Library/LaunchDaemons/com.apple.idsremoteurlconnectionagent.plist

Required to create backup in iTunes, can be disabled otherwise, just re-enable if needed:

	/System/Library/LaunchDaemons/com.apple.atc.plist

	/System/Library/LaunchDaemons/com.apple.afcd.plist

	/System/Library/LaunchDaemons/com.apple.usbptpd.plist

ABSOLUTELY required, never ever stop this service, not even temporarily!:

	/System/Library/LaunchDaemons/com.apple.mobile.installd.plist

If you stop this service, delete the springboard caches, restart springboard, all your icons will be gone - empty screen! It's also required for iTunes backup, Appstore/.ipa installations and hell knows what else.

Required for provisioning (AltDeploy etc., I spoke about this above). Since provisioning never works properly, and since we jailbreak through Safari, we disable this crap:

	/System/Library/LaunchDaemons/com.apple.misagent.plist

	/System/Library/LaunchDaemons/com.apple.online-auth-agent.xpc.plist

	/System/Library/LaunchDaemons/com.apple.mobile_installation_proxy.plist

Now some people install .ipa packs even on jailbroken systems. That's presumedly what Appsync Unified is for. No idea. I tried once to install an .ipa and failed. It's completely pointless, just extract the .ipa and copy the .app filder to

	/Applications

then do

	uicache mobile

to update the springboard cache.

Required for apple maps:

	/System/Library/LaunchDaemons/com.apple.geod.plist

	/System/Library/LaunchDaemons/com.apple.locationd.plist

May be also required for apple maps, not sure:

	/System/Library/LaunchDaemons/com.apple.Maps.geocorrectiond.plist

	/System/Library/LaunchDaemons/com.apple.Maps.pushdaemon.plist

	/System/Library/LaunchDaemons/com.apple.navd.plist

Required for Recents view in MobilePhone:

	/System/Library/LaunchDaemons/com.apple.geod.plist

You disable this, there will be a slight delay when you scroll the "Recents" list. I strongly recommend disabling this crap service nevetheless, since it aggresively hammers the Apple servers, steals your bandwidth and increases the load on your router.

Now this is a difficult one, Appstore. First, the service needed to actually sign-in:

	/System/Library/LaunchDaemons/com.apple.akd.plist

Services needed for actual downloads on Appstore (REBOOT REQUIRED for com.apple.DuetHeuristic-BM !!!):

	/System/Library/LaunchDaemons/com.apple.appstored.plist

	/System/Library/LaunchDaemons/com.apple.itunesstored.plist

	/System/Library/LaunchDaemons/com.apple.nsurlsessiond.plist

	/System/Library/LaunchDaemons/com.apple.DuetHeuristic-BM.plist

	/System/Library/LaunchDaemons/com.apple.adid.plist

Now of course, nobody knows about this, just search for "com.apple.DuetHeuristic-BM" on google, nobody tells you it's required for Appstore downloads. Ask Google, right...

The urls that need to be un-blocked are listed in:

	https://github.com/WRFan/jailbreak10.3.3/tree/main/dnsmasq

Plus, check:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/private/etc/hosts

/etc/hosts is used only if you install this plugin:

	https://www.ios-repo-updates.com/search/?s=letmeblock&repository=all&section=all&price=all

In general, I recommend killing access to:

	init.itunes.apple.com

because it's the primary portal for iOS, and there's absolutely no need to allow anything on iOS to access the net, except APSD and programmes you actually want to access the net, like Safari. But Appstore needs this, so enable if you want to download something, then disable again.

Maybe I should mention that dnsmasq is basically similar to /etc/hosts , but first, /etc/hosts will not be used by mDNSResponder during the boot process (system not jailbroken yet), second, there are tons of apps/services on ALL systems that ignore /etc/hosts (nslookup on Windows, for example). So dnsmasq is better, since you set it as a DNS resolver, so system can't bypass it... unless, of course, a different DNS resolver is compiled into the programme itself, but that's what your firewall is for.

You may consider running dnsmasq on an iPhone, but, obviously, this is not a good idea. dnsmasq will not run until jailbroken, so neither the device itself, nor any other devices using its DNS service will be able to resolve domains until you jailbreak. It's much better to run it on a real computer, and here I recommend, surprise! linux. I am running it on my router, since it's online 24/7 anyway, but that depends on your router. If your router is one of those pathetic boxes you only have limited access to through snmp/web gui... Just, whatever you do, don't use a Mac! It's a childish system incapable of anything, except, maybe Minesweeper, lol. I spent three hours trying to get dnsmasq running on Sierra, and FAILED. It took me FIVE minutes to set it up on Ubuntu. It took me 6 hours to set up Apache + Webdav on Sierra and 10 minutes to set Apache up on Ubuntu. Btw, when setting Apache Webdav up on Sierra, the first thing you wanna do is:

	rm wfsctl

lol, then configure Apache DIRECTLY through its config files. Unless you like 500 Internal errors? Then stay with wfsctl.

Note that iOS versions of nslookup, host, dig, and delv are actually using

	https://github.com/WRFan/jailbreak10.3.3/blob/main/private/etc/resolv.conf

since they are a linux-ripoff. And while talking about resolv.conf, the resolv package is the most useless annoying package ever released for Linux. Kill it immediately and create a resolv.conf manually. In fact, the file above is a direct copy from my Ubuntu, I just copied it to iOS and it's working for nslookup etc.

This crap is required for for Cloud Sign-Out (NOT Appstore!), and only if service was running when signed-in:

	/System/Library/LaunchDaemons/com.apple.icloud.findmydeviced.plist

Never ever sign into ANY Apple services, except Appstore! ESPECIALLY, iCloud! If you do, iOS will hammer Apple servers continuously. Sure, you can block internet access, but that's just stupid - iOS hammers, you drive it back, it hammers again. Don't let iOS do it from the very start.

iOS preferences are confusing - there are actually TWO sign-in links in there, the first one at the very top of Preferences, the second:

	Settings -> iTunes & App Store -> Sign-in.

Only ever use the second one! If you don't want to type your username/password every time on this pathetic keyboard of your tiny iPhone, you can inject anything to iPhone clipboard over ssh:

	printf XXX | pbcopy

	pbpaste && echo

where XXX is the stuff to inject into iPhone clipboard (like your password for Appstore sign-in) and pbpaste will output the clipboard to your ssh app (putty, whatever), then you can simply copy it to the system clipboard where putty is running.

Required to make calls:

	/System/Library/LaunchDaemons/com.apple.telephonyutilities.callservicesd.plist

	/System/Library/LaunchDaemons/com.apple.CommCenter.plist

While talking about the phone, you may be interested to take a peek at:

	"/System/Library/Carrier Bundles/iPhone/XXX.bundle/"

	"/private/var/mobile/Library/Carrier Bundles/iPhone/XXX.bundle/"

where XXX is your carrier.

REQUIRED FOR "Show my Caller Id" in the phone preferences:

	/System/Library/LaunchDaemons/com.apple.CommCenterMobileHelper.plist

If you disable this, iPhone won't auto-call your carrier to disable caller id on boot, so you will have to do it manually every time you reboot, either directly through the iPhone preferences, or by calling:

	Calling Line ID Restriction (Anzeige ausgehender Call-ID):

	Enable: #31#

	Disable: *31#

	Check: *#31#

	Calling Line Presentation (Anzeige eingehender Call-ID):

	Enable ???: *30#

	Disable ???: #30#

	Check: *#30#

Required FOR WhatsApp:

	/System/Library/LaunchDaemons/com.apple.fairplayd.H2.plist

Plus, as I mentioned above, APSD is required for incoming calls on Whatsapp, unless it's running in the foreground (+ iphone screen must be lighted), so I don't recommend disabling APSD, if you use WhatsApp, although it hammers the Apple servers every time you enable/disable airplane mode, light your screen (wifi returns from sleep mode) etc.

Talking about APSD, note that if you booted your phone while your internet was down, that pathetic service will not check for internet presence, so you miss all your incoming whatsapp calls, even if internet comes back up. So when your internet is back up, enter-exit airplane mode to force APSD to actually connect to Apple servers to establish the connection over which incoming call notifications are pushed to you.

com.apple.fairplayd.H2 is for DRM. WhatsApp is VoIp, not a music player. Now why the hell does it require it? Go ask Facebook! Who knows what goes on in their morose heads!

Required FOR Battery display, you disable it, your battery will constantly show 1%:

	/System/Library/LaunchDaemons/com.apple.powerd.plist

This one is interesting - required for photos (com.apple.mobileslideshow) and camera (com.apple.camera):

	/System/Library/LaunchDaemons/com.apple.assetsd.plist

Or is it? Thing is, we don't actually need this crap service. It just pushes the photos from:

	/private/var/mobile/Media/DCIM/.MISC/Incoming/

	find /private/var/mobile/Containers/Data/Application/ \( -iname "Incoming" \)

to

	/private/var/mobile/Media/PhotoData/

or whatever folder the Photos app is using, not 100% sure. Now do we need this? Why would we want to browse our photos on this tiny iPhone screen, if we can copy them to our computers over ssh? No iTunes access is required, you don't need to connect your iPhone to USB to get your photos, you don't even have to install that pathetic iTunes on your clean nice Windows system! If you disable the assetsd daemon, photos will get stuck in the Incoming folders. Wonderful! We'll just create symlinks from

	/private/var/mobile/Containers/Data/Application/com.apple.mobileslideshow_GUID/Media/DCIM/.MISC/Incoming

	/private/var/mobile/Containers/Data/Application/com.apple.camera_GUID/Media/DCIM/.MISC/Incoming

to:

	/private/var/mobile/Media/DCIM/.MISC/Incoming/

(check the "install" script on this repo for code), now all photos we make through the camera get re-directed to the aforementioned folder and get stuck in there. Now zip them on the iPhone, use Filza WebDav server to download the zip file to your PC. Start editing in Corel Photopaint. Another service killed!

The "Incoming" folders in /private/var/mobile/Containers/Data/Application/ may not be present from the very start, you may need to run Photos + Camera apps at least once before.

While at it, how do you actually find out the preferences Guid for a specific app? Here's something that can help:

ALL apps:

	find /private/var/mobile/Containers/Data/Application/ -iname .com.apple.mobile_container_manager.metadata.plist -type f -exec plutil -show {} \;

SPECIFIC app A:

	find /private/var/mobile/Containers/Data/Application/ -iname .com.apple.mobile_container_manager.metadata.plist -type f -exec bash -c "plutil {} | grep -q 'XXX' && dirname {}" \;

SPECIFIC app B:

	for p in $(find /private/var/mobile/Containers/Data/Application/ -iname .com.apple.mobile_container_manager.metadata.plist -type f); do if grep -qi XXX $p 2>/dev/null; then dirname $p; fi; done

SPECIFIC app C (FAILS for some !!!):

	var1=`lsdtrip app XXX | grep 'Container URL' | sed 's/[^\/]*..//'` && echo $var1

Note that if you delete:

	rm -r /private/var/mobile/Media/PhotoData/*

then Photos and Camera apps will not work properly, since com.apple.assetsd is required to recreate that folder. So if for some reason you want to clean out that folder, do:

	launchctl load -w /System/Library/LaunchDaemons/com.apple.assetsd.plist

	rm -r /private/var/mobile/Media/PhotoData/*

	open com.apple.camera
	sleep 3
	killall Camera

	launchctl unload -w /System/Library/LaunchDaemons/com.apple.assetsd.plist

If you make a selfie on the camera, the image is inverted. lol. I was kind of suprised first, but, well, the mirror effect, of course. In Corel Photopaint, invert image horizontally. Note that images created on iPhone 5s (or is it OS specific?) are broken - they are not displayed correctly in Internet Explorer - they are turned around by 90°, weird, FF displays them correctly. Saving this image in Photopaint fixes this weird bug, so it's an Apple bug, not a Microsoft one. Not surprising - if something is not working properly, rest assured Apple's responsible.

This service is required to use a proxy:

	/System/Library/LaunchDaemons/com.apple.nehelper-embedded.plist

But since most services that run during the boot process are SSL-pinned, you can't use a proxy during the boot anyway. The service is still needed later on for internet access through a proxy in Safari etc.

Required To block phone / SMS contacts (so you can block those nice people, who send you an SMS notifying you you've just won 1.000.000$!):

	/System/Library/LaunchDaemons/com.apple.cmfsyncagent.plist

Of course, nobody on the entire internet knows this. They will tell you this service is required for iCloud. iCloud my ass. Could somebody please make these morons shut up?

REQUIRED FOR WiFi:

	/System/Library/LaunchDaemons/com.apple.wifiFirmwareLoaderLegacy.plist

	/System/Library/LaunchDaemons/com.apple.wifid.plist

	/System/Library/LaunchDaemons/com.apple.SCHelper-embedded.plist

if com.apple.wifid unloaded, wifi will be disabled in settings.

com.apple.SCHelper-embedded is loaded when WiFi settings are opened. if unloaded, all net settings will be empty!

This one is NOT required though on iOS 10.x:

	/System/Library/LaunchDaemons/com.apple.wifiFirmwareLoader.plist

While talking about Wifi, iOS 10.x exhibits an extremely weird behaviour - when you switch off the screen (dark), wifi will enter sleep mode. It's still on, though, you will receive your push notifications, but if you try to use SSH, the reaction will be very sluggish, so you type a letter, and it takes like 10 seconds to pass it to iPhone. This is not the case with iPhone 12.x. Apple obviously doesn't give a damn about SSH or our jailbroken phones. So I was first keeping the display lighted, but this eats up the battery. I searched the entire net trying to find a solution, nothing. Then I accidentally just stumbled upon it:

	https://www.ios-repo-updates.com/search/?s=iNoSleep&repository=all&section=all&price=all

	https://repo.packix.com/package/com.tonykraft.inosleep

This one will keep your phone more alive so you can work over ssh when then screen is dark.

Someone also suggested to rip out Wifi firmware from iOS 12.x and load it into memory on older iOS versions:

	https://github.com/saj0vie/wififirmwareupdater

	/usr/libexec/wifiFirmwareLoaderLegacy -f

	Resolved to Firmware file path: "/usr/share/firmware/wifi/43342a0/cabernet.trx"

	Resolved to NVRAM file path: "/usr/share/firmware/wifi/43342a0/cabernet-m-mt.txt"

	Resolved to Regulatory file path: ""

	Resolved to TxCap file path: ""

Well, this sure as hell doesn't work on iOS 10.x. I got firmware from iOS 12.4.7, but no go:

	/usr/libexec/wifiFirmwareLoaderLegacy -r -F /usr/share/firmware/wifi/43342a0_NEW/cabernet.trx -N /usr/share/firmware/wifi/43342a0_NEW/cabernet-m-mt.txt

Required for Landscape mode (when you turn your iPhone around in programmes like Calculator, Safari, Filza, MobileSMS):

	/System/Library/LaunchDaemons/com.apple.oscard.plist

Required for SMS (I'm talking about real SMS, not that crap iMessage):

	/System/Library/LaunchDaemons/com.apple.imagent.plist

plus:

	/System/Library/PrivateFrameworks/IMDPersistence.framework/XPCServices/IMDPersistenceAgent.xpc/IMDPersistenceAgent

	???

	/System/Library/PrivateFrameworks/IMDMessageServices.framework/XPCServices/IMDMessageServicesAgent.xpc/IMDMessageServicesAgent


Slows down SpringBoard + UserEventAgent + launchd if disabled:

	/System/Library/LaunchDaemons/com.apple.managedconfiguration.profiled.plist

Pretty much kills your system, making it totally unresponsive (morons on the net of course tell you, it's ok to disable. MORONS!):

	/System/Library/LaunchDaemons/com.apple.identityservicesd.plist

this service is responsible displaying this stupid popup:

	Your carrier may charge for SMS messages used to activate iMessage / Facetime

Imagine, my mobile carrier (Telefónica Germany) has a contract with Apple, so activation is free, nevertheless, I still get this popup. iOS has access to the carrier bundles and the SIM card, can't it figure out if a specific carrier charges? Lazy Apple.

Required to get system info:

	/System/Library/LaunchDaemons/com.apple.mobilegestalt.xpc.plist

	http://newosxbook.com/articles/guesstalt.html

REQUIRED FOR touchscreen! touchscreen will NOT RESPOND!!!:

	/System/Library/LaunchDaemons/com.apple.mtmergeprops.plist

So you basically just killed your phone, since re-enabling the service would require ssh, ssh requires jailbreaking, jailbreaking requires you to launch safari, and you can't launch Safari, since the touch screen does not respond to your caresses.

The Chinese state disabling this service will kill your phone. Not tested. Dare to test?

	/System/Library/LaunchDaemons/com.apple.tccd.plist

Morons on the net will tell you this service is safe to disable:

	/System/Library/LaunchDaemons/com.apple.assertiond.plist

Just try it. It will restart the Springboard, but will never succeed

Required to access Contacts:

	/System/Library/LaunchDaemons/com.apple.accountsd.plist

REQUIRED FOR Webkit exploit !!! required for Safari itself !!!:

	/System/Library/LaunchDaemons/com.apple.pluginkit.pkd.plist

You disable this service and reboot, congratulations, you killed the jailbreak and Safari permanently. This service is needed for Javascript to be parsed correctly by the Webkit engine, so say bye bye to the Webkit jailbreak and to javascript in general, most JS functions will exhibit an extremely weird behaviour.

The Chinese tell us the system would not boot if you disable this:

	/System/Library/LaunchDaemons/com.apple.mobile.lockdown.plist

I didn't believe them, damn communists, what do they know... Turns out, the communists were right. This made me embrace communism. Down with capitalism! Down with Apple! Long live the Proletariat!

Required for Filza WebDav:

	/Library/LaunchDaemons/*filza* services

Disable this one if you feel suicidal:

	/System/Library/LaunchDaemons/com.apple.mobileactivationd.plist

Responsible for iPhone activation thru albert.apple.com (which can be done thru iTunes, but it's responsible for other core stuff as well!).

WILL NOT BOOT !!!:

	/System/Library/LaunchDaemons/com.apple.notifyd.plist

btw, I love this service, it was the first time (out of like 20) that I killed my phone. I was thinking, judging by its name, that it has to do with notifications, and in fact, it does, but NOT those notifications mentioned in iOS Preferences, rather, from my understanding, it's the darwin equivalent of the Windows service:

	HKLM\SYSTEM\CurrentControlSet\Services\CoreMessagingRegistrar

If I had known this I wouldn't even have considered disabling it. CoreMessagingRegistrar iss the most important Windows service, some might object, stating lsass is more important, but pals, you kill lsass, you at least get a warning the system will reboot, but killing the notification service will result in an immediate freeze and the system will reboot in picoseconds! It's actually a wonder the system is still capable of rebooting! I once tried to install CoreMessagingRegistrar on WinPE, that system is the only one that doesn't require it, yet even that system rebooted immediately when CoreMessagingRegistrar crashed. So, retrospectively, even intentionally I couldn't have selected a worse target.

Service required for, surprise! text input (it's loaded, when the keyboard appears on screen):

	/System/Library/LaunchDaemons/com.apple.TextInput.kbd.plist

No idea, but I think, these are required, I'm pretty certain about securityd + mediaserverd:

	/System/Library/LaunchDaemons/com.apple.memory-maintenance.plist

	/System/Library/LaunchDaemons/com.apple.FileProvider.plist

	/System/Library/LaunchDaemons/com.apple.containermanagerd.plist

	/System/Library/LaunchDaemons/com.apple.lsd.plist

	/System/Library/LaunchDaemons/com.apple.mediaserverd.plist

	/System/Library/LaunchDaemons/com.apple.distnoted.xpc.daemon.plist

	/System/Library/LaunchDaemons/com.apple.FileCoordination.plist

	/System/Library/LaunchDaemons/com.apple.securityd.plist

	/System/Library/LaunchDaemons/com.apple.configd.plist

	/System/Library/LaunchDaemons/com.apple.UserEventAgent-System.plist

	/System/Library/LaunchDaemons/com.apple.CoreAuthentication.daemon.plist

	/System/Library/LaunchDaemons/com.apple.logd.plist

This one I already mentioned above:

	/System/Library/LaunchDaemons/com.apple.cfprefsd.xpc.daemon.plist

The infamous AMFI-shmamfi, as Levin calls it. The best friend of all jailbreakers:

	/System/Library/LaunchDaemons/com.apple.MobileFileIntegrity.plist

Safari cache + local storage (I think?):

	/System/Library/LaunchDaemons/com.apple.nsurlstoraged.plist

unless you prefer not to use cache? some pages are dependent on local storage and may not display properly:

	https://developer.mozilla.org/en-US/docs/Web/API/Window/localStorage

Required for DNS resolve, as I already stated above:

	/System/Library/LaunchDaemons/com.apple.mDNSResponder.plist

This one is useless, however:

	/System/Library/LaunchDaemons/com.apple.mDNSResponderHelper.plist

when disabled, "Change Voicemail Password" option in Settings -> Phone is NOT present. What's a Voicemail? Crap:

	/System/Library/LaunchDaemons/com.apple.voicemail.vmd.plist

This service displays logos when apps are opened. Absolutely useless crap:

	/System/Library/LaunchDaemons/com.apple.splashboardd.plist

iBooks related? No idea:

	/System/Library/LaunchDaemons/com.apple.storebookkeeperd.plist

iMessage related? NOT required for real SMS:

	/System/Library/LaunchDaemons/com.apple.imtransferagent.plist

any services called "com.apple.nano..." as well as anything at:

	/System/Library/NanoLaunchDaemons

is useless pathetic Apple Watch crap. Kill the services, then take a hammer and kill the Apple Watch as well.

This is not a complete list. If you find out anything else, you can add a topic:

	https://github.com/WRFan/jailbreak10.3.3/issues

Now let me mention some useful commands. The utilities themselves are in the cloud package (above).

Get some system and hardware info:

	cfversion

	ecidecid

	gssc

	uiduid

	uname -a

Btw, uname will display something like:

	Darwin XXX 16.7.0 Darwin Kernel Version 16.7.0: Thu Jun 15 18:33:35 PDT 2017; root:xnu-3789.70.16~4/MarijuanARM64_S5L8960X iPhone6,2 arm64 N53AP Darwin

if you use doubleH3lix. "MarijuanA"? lol. Here:

	https://github.com/Siguza/doubleH3lix/blob/master/doubleH3lix/jailbreak.mm

Meridian uses a different check and doesn't set this, so if you jailbreak with Meridian, doubleH3lix will not realize you are already jailbroken.

Talking about conflicts between jailbreaks, I should probably mention the mobile substrate - substitute controversy. doubleH3lix and checkra1n use substrate, Meridian and unc0ver substitute:

	https://github.com/PsychoTea/MeridianJB/blob/master/Meridian/Meridian/jailbreak.m

	/usr/lib/tweaks

This causes problems when switching between jailbreaks. There are many other differences, different checks, different folders, different programmes are launched after the jailbreak... So be careful if you intend to switch.

get ECID in decimal:

	var1=`ecidecid` && var2=`expr length $var1` && var1="`expr substr $var1 3 $var2`" && echo $((16#$var1))

Get and set kernel vars, ~ linux:

	sysctl -a

	https://manpages.debian.org/unstable/procps/sysctl.8.en.html

	getconf XXX

btw, I am pointing to linux man pages, but that doesn't mean a darwin command will work in EXACTLY the same way as a linux command, even if they share the same name! So be careful, lest you format your harddisk by following debian man pages, lol. Here are some man pages (search on google):

	"XXX" site:manpagez.com

	"XXX" site:ss64.com

	"XXX" site:manpages.debian.org

The first two are for darwin.

Displays installed apps and plugins:

	lsdtrip apps

	getapplist

	getapplist -id

	lsdtrip publicurls

	lsdtrip privateurls

	lsdtrip plugins

	lsdtrip app XXX

where XXX is the app you want to query, like "net.whatsapp.WhatsApp".

Get the directory of an app:

	getappdir org.tihmstar.doubleH3lix

	/Applications/doubleH3lix.app

Convert plist to xml and back to binary:

	plutil -convert xml1 XXX

	plutil -convert binary1 XXX

Display binary plist without converting it:

	jlutil -x PLIST

	plutil PLIST

This may be useful, because if you convert Info.plist files in the WhatsApp folder to XML, it will refuse to run when unjailbroken. Weird.

Also check "jlutil" when working with plists. Besides:

	plistutil -i XXX -o XXX

	plconvert IN_PLIST OUT_PLIST

Check if there's a service corresponding to a binary:

	grep -lir XXX /System/Library/LaunchDaemons

Get process info:

	ps aux | grep -i 'XXX*'

Process programmes:

	taskinfo

	htop

	procexp

	top

Get cpu/memory info (wait, where are "free" and "vmstat" commands? damn iOS!):

	top -l 1 | grep PhysMem

	top -l 1 | grep "CPU usage"

Get resources loaded by a specific process:

	lsof -c Name

	lsof -p PID

	lsof ProcessLibrary

Btw, there's actually an lsof for linux too. Huge thanks to procurs.us for releasing this file for iOS (and not adding that damn _chkstk_darwin symbol to it, lol). There hasn't been a working lsof for iOS for ages.

Find a file on harddisk:

	find / -iname "*XXX*"

Find a string inside files:

	grep -lir 'XXX' /

Don't know how well grep does at finding strings in binary files. Might want to look for an alternative if searching in binaries.

Get harddisk space used overall:

	df -h

iPhone Preferences displays bullshit values, ignore them.

Display harddisk space used by a folder:

	du -hs

Find out how much space subfolders use:

	find -maxdepth 1 -exec du -sh "{}" \; | sort -h

Useful to find useless crap that occupies harddisk space. Like Siri. Hey, you know, I actually killed her! So that's two girls - first Cydia, then Siri... next is Alexa.

Unpack dpkg install instructions:

	dpkg -e XXX /tmp/1/

Extract *.deb archive:

	ar x XXX.deb

Display dpkg installed packages ( /private/var/lib/dpkg/status ):

	dpkg-query -l

Wait, I am stuck! How do I get out of a bash programme that stays in memory? sigterm:

	 STRL + Z

Works on Mac too. That's on a German keyboard. Not German? Look yourself then.

Purge a package:

	dpkg --purge XXX

Actually, for dpkg, look yourself here:

	https://manpages.debian.org/unstable/dpkg/dpkg.1.en.html

Sort files by date:

	ls -lat

Display current path:

	pwd

Display file path of a particular programme:

	which XXX

Note that which will resolve symlinks and display the path of the REAL file.

Mysterious programme:

	what

I have no idea, what "what" does. What? How do you search for "what" on google?

	touch bla

	what bla

	stdout: bla

Look, it said "bla"! Coool! Err, does that mean something?

Copy symlink itself:

	cp -R What WhereTo

Copy directory:

	cp -av What WhereTo

Note that BSD programmes like "cp" have a different syntax than GNU ones, which leads to all kinds of misunderstandings. This will throw on the BSD version, but not the GNU version:

	cp -avr What WhereTo

Rename: "ren", right? Hey, you are not on Windows. There is no rename command on linux/darwin. What? How do I rename then? By moving:

	mv oldName newName

rename by moving? Retarded! That's it, I am going back to Windows!

re-login (preserve jailbreak):

	sbreload

	ldrestart

sbreload is quicker. Check:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/jailbreak/payload

I actually disabled "ldrestart" in the binary and moved the "sbreload" to:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/private/etc/rc.d/zzz

Why restart at all? Because mobile substrate plugins must be injected into the Springboard, so you need to restart it. Wait, why not re-start springboard directly?:

	launchctl unload /System/Library/LaunchDaemons/com.apple.SpringBoard.plist && launchctl load /System/Library/LaunchDaemons/com.apple.SpringBoard.plist

This often causes problems with mobile substrate, may crash. What is "sbreload" actually doing?

	https://github.com/coolstar/uikittools-theos/blob/master/sbreload/sbreload.c

Same, but don't use!!!:

	launchctl reboot userspace

This will work once, if you try again, you get a cryptic error:

	Failed to reboot system: 141: Reentrancy avoided

Seems to work on iOS 12.x though?

Note that there are crap versions of launchctl lacking entitlements. Those will throw on this command:

	Requestor lacks required entitlement

Reboot completely:

	reboot

	shutdown -r now

	launchctl reboot

	launchctl reboot system

halt system (never tested):

	shutdown -h now

Delete everything in a folder including subdirectories:

	rm -r /tmp/*

Get numeric chmod (anybody understand those cryptic letters "ls" displays?):

	stat -c '%a %n' /XXX

Note that on Mac you have a binary of the same name, but the command params are different!:

	stat -f %A /

display network adapters (== ipconfig):

	ifconfig

This displays... something. Yeah! some cool info:

	scutil --dns

	scutil --nwi

	scutil --proxy

No, seriously,

	scutil --proxy

displays, surprise! proxy info. Like there was some naive guy on official Apple forums asking how to add a proxy bypass to the network settings. Haha, Apple doesn't even allow you to bypass your proxy for intranet connections! Stupid Apple! It's possible, when jailbroken:

	/private/var/preferences/SystemConfiguration/preferences.plist

check my "install" script in this repo's root. so you add an exception (reboot), then

	scutil --proxy

	`
	ExceptionsList : <array>
	{
		0 : *.local
		1 : 169.254/16
		2 : 192.168/16
	}

	`

192.168/16 is the one added. The syntax is actually the same across all systems (so can be also be used for iptables, e.g.). so:

	16 -> 192.168.*.*

	8 -> 192.*.*.*

netstat (netstat params differ on linux and darwin !):

	netstat -tan | grep LISTEN

mDNSResponder:

	netstat -an | grep .5353

	lsof -i :5353

SSH:

	netstat -an | grep .22

APSD (push notifications):

	netstat -an | grep .5223

Filza WebDAV (if on port 80):

	netstat -an | grep .80

Apple ports:

	https://support.apple.com/en-gb/HT202944

Get route table (again, syntax differs from linux!):

	route -n get default

ping (why are there two commands on linux/darwin? Windows gets it done with one command!):

	ping

	ping6

default routers (SLAAC):

	ndp -nr

Neighbors:

	ndp -an

On linux, there's "ip" for this. Isn't there "ip" for Darwin? That would be useful.

ARP (look, all three systems use the same command for once!):

	arp -a

System uptime:

	uptime

	sysctl kern.boottime

Check firmware vars (do you really want to mess with this?):

	nvram -p

This is primarily needed for "noapnonce" change, which is unnecessary for our downgrade (check errorlogshsh.txt after downgrade). If you wanna mess with this crap, compile libirecovery on linux:

	https://github.com/libimobiledevice/libirecovery

and mess with the iPhone from there. Works in a virtual machine too. For Workstation:

	net start hcmon

	net start VMUSBArbService

Install "usbmuxd" on linux to recognize the device, which is the equivalent of:

	https://support.apple.com/en-us/HT204095

Also Install libimobiledevice-utils (ideviceenterrecovery).

Now let's play:

	ideviceenterrecovery UDID

"UDID", damn it, not "UUID"! Nobody gets it right!

	UDID -> Universal Device ID, device specific

	UUID -> app specific

How do I get UDID?

on iOS:

	uiduid

on linux:

	lsusb -s :`lsusb | grep iPhone | cut -d ' ' -f 4 | sed 's/://'` -v | grep iSerial | awk '{print $3}'

	lsusb -s `lsusb | grep "Apple" | cut -d ' ' -f 2`:`lsusb | grep "Apple" | cut -d ' ' -f 4 | sed 's/://'` -v | grep iSerial | awk '{print $3}'

All right. How do I get app-specific UUID?

	find /private/var/containers/Shared/SystemGroup/ -iname "com.apple.lsdidentifiers.plist" -type f -exec jlutil -x {} \;

When in recovery (or DFU), get APNonce:

	irecovery -q -v

Reboot:

	irecovery --shell -v

	setenv auto-boot true

	saveenv

	reboot

There's an iOS version of irecovery. This... is strange. How do you execute irecovery on the device itself, when it's in DFU mode? Me not understand.

Change APNonce (also an option in Meridian):

	irecovery -c "setenv com.apple.System.boot-nonce 0x1111111111111111"

On iOS:

	nvram com.apple.System.boot-nonce=0x1111111111111111

Really, don't bother with this shit. You'll probably just kill your system, and, as I said, our downgrade doesn't require this, since we don't require shsh blobs. The downgrade script just gets the current ApNonce, no matter what it is:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/downgrade/device.py

Renew springboard cache:

	uicache mobile

Get bash vars:

	env

Get something from bash... yeah! something cool:

	set

How do I get my iBoot version?

	gssc | grep FirmwareVersion

How do I use "launchctl"?

Small list of services:

	launchctl list

Large list of services:

	launchctl print system

Entire plist cache:

	launchctl print-cache > /tmp/1

Print disabled services:

	launchctl print-disabled

Print launch reason:

	launchctl blame system/XXX

where XXX is the service identifier:

	launchctl blame system/com.apple.mobileactivationd

	ipc (mach)

"mach". right. Ask Levin, he knows.

Supposed to restart a service (xpc too):

	launchctl kickstart system/XXX

XXX -> service name (no ".plist" !). Didn't work for me

Remove service from memory (xpc too). Non-persistent between reboots:

	launchctl remove XXX

	launchctl bootout system/XXX

XXX -> service name (no ".plist" !):

	launchctl remove com.apple.flickr.xpc

Damn flickr.

Loading and unloading services - Check syntax at:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/services

Edit text files:

	nano XXX

nano is cool, especially on mac, where I constantly run into permission problems with textedit, which runs in a different context (non-root).

Kill all processes with Name:

	killall ProcessName

Force-kill:

	killall -9 ProcessName

	killall -SIGSTOP ProcessName

Note that /usr/libexec/substrated will just restart itself. If you want to stop it,

	unlink /private/etc/rc.d/substrate && reboot

Kill pid:

	kill -9 pid

How to get pid? Well, you know, I kindof *appropriated* a "pidof" script from:

	https://github.com/brandonplank/rootlessJB4

hihi.

Btw, why is everybody killing service executables instead of restarting services themselves? Seems a more proper way to restart the service.

Get wifi APs in the neighbourhood:

	wifiutil scan

	airscan -s -v

Check my version of "airscan". It has different colours than the original github repo. The original coder uses purple. Pal, putty background is black by default. Purple on black?! You got to be kidding! I set green.

Disable Wifi:

	wifiutil disable-wifi

	ifconfig en0 down

Enable: Wifi: ... No, I am not going to fall for that! You can't issue any commands when wifi is disabled, because ssh requires wifi! Yeah, but I can open bash through filza and then type on the iPhone! Please? Tell me how to re-enable Wifi? I am not telling you! 'cause I am NOT going to type ANYTHING on that stupid tiny iPhone keyboard! And I won't let you do that either! For your own sake! Nobody touches that keyboard! iOS keyboard is a no-no!

How to restart ssh? Well, in theory:

	killall -9 sshd && /usr/libexec/sshd-keygen-wrapper

but ssh will probably die right before the second part is executed.

How do I clear the ssh screen?

	cls

No, wait, that's Windows, lol. Use "clear". On some guest systems this may fail to work properly, if you scroll with the mouse up, you can still see the cleared text. In that case use:

	clear && echo -en "\e[3J"

People will also suggest using:

	reset

	tput clear

Those people are morons.

Get system messages:

	dmesg

ubuntu provides more info than iOS with this command. also check:

	/private/var/logs/lockdownd.log

	/private/var/installd/Library/Logs/MobileInstallation/mobile_installation.log.0

Get battery data:

	batterydata

	bstat

batterydata actually states when run on one of my iPhones:

	BatteryHealthCondition : Permanent Battery Failure

Suprisingly, it's the very phone that got a new battery a couple of days ago. Seems Apple doesn't pass proper info for batteries manufactured by other companies. Good way to kill competition. Apple is nice to customers and competitors alike!

Some additional questions:

Where's my SMS database?

	/private/var/mobile/Library/SMS/sms.db

Does Safari clear blobs from cache when I clear Safari cache through the Preferences GUI?

	No

Why not?

	because Apple is stupid

How do I REALLY clear Safari cache?

	var1=`lsdtrip app com.apple.mobilesafari | grep 'Container URL' | sed 's/[^\/]*..//'` && rm -rv $var1/Library/Caches/*

wait, I think I made a mistake in the script! Hmm, I think, it will format the hard disk. Bah, who cares...

Where are the crash anylytics stored?

	/private/var/mobile/Library/Logs/CrashReporter/

I disabled the service responsible for this (now, which one was it again?). No more anylytics for you! for Apple either... err, I mean, "neither".

Where are the Appstore and provisioned apps residing?

	/private/var/containers/Bundle/Application/

How do I disable that damn "No SIM Card Installed" popup?

	There's a link to a no-sim package in the "install" script on this repo.

Yeah, but after boot the system is not jailbroken yet, so the popup is still displayed?

	So? Apple is annoying. So it will annoy you with this message FOREVER. Because annoying you is Apple's purpose. That's what Aristotle said. μῆλον φύσις εἰμί βαρύς. Wait... I think I got a grammar mistake in there. Quick, someone correct me!

Some plists contain data encoded as base64. How do I decode it?

	https://github.com/WRFan/jailbreak10.3.3/blob/main/base64/atob.html

How do I compile stuff?

	Install compiler stuff:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/install

	Compile instructions:

	https://github.com/WRFan/jailbreak10.3.3/blob/main/Packs/Work_IOS.txt

How to check entitlements?

	ldid -e XXX

	jtool2 --ent XXX

	jtool --ent -vv -arch arm64 XXX

How to add entitlements?

	ldid -SXXX.plist XXX

	jtool --sign --inplace --ent entitlements.plist XXX

There's no space between "-S" and the plist name in the ldid command!

How do I check file architecture?

	otool -hv -arch arm64 XXX

	vtool -arch arm64 -show XXX

How do I check dependent libraries?

	jtool -L -vv -arch arm64 XXX

	otool -L XXX

	dyldinfo -dylibs XXX

How do I dump symbols?

	dumpbin /symbols "XXX.lib"

No, wait, that's the MSVC command. What system are we on? Right, iOS:

	jtool -arch arm64 -S XXX

	dyldinfo -export XXX

	nm XXX

How do I kill the "update available" badge on icons? Well, you shouldn't have given the iPhone access to Apple servers in the first place! You get what you deserve! Well, let's get rid of it:

	plutil -key kBadgedForSoftwareUpdateKey -false /private/var/mobile/Library/Preferences/com.apple.Preferences.plist

	plutil -key kBadgedForSoftwareUpdateJumpOnceKey -false /private/var/mobile/Library/Preferences/com.apple.Preferences.plist

How do I swipe away an app from the task bar? First, task bar is called "multitasking" on iOS. Second, nobody knows how to do that. But I suggest taking a look at:

	/private/var/mobile/Library/Preferences/com.apple.springboard.plist

	SBRecentDisplayItemRoles

	SBRecentDisplayItems

Is there an easter egg on iOS?

	*3001#12345#*

When my iPhone rings, it only rings like thrice, then turns itself off!

	https://discussions.apple.com/thread/7603663

How do I disable the temporary ipv6 address?

	sysctl -w net.inet6.ip6.use_tempaddr=0

How do I check which package a particular file belongs to?

	grep -lir XXX /private/var/lib/dpkg/info/

How do I set bash colors on iOS?

	https://github.com/WRFan/jailbreak10.3.3/blob/main/private/etc/profile

So much for this. Wait... I notice you still got a question on your mind. Pose it.

All right, so... the more I read about Apple the more despicable this company appears to me. Its OSes suck, its products suck, hell, even the javascript they use on their web pages sucks. This company is inept, intrusive, and generally pathetic. So... I was thinking, should I switch to Samsung?

You are kidding right? Samsung? I got something better for you. Something that is REALLY cool! The all new all wonderful Surface Duo! By Microsoft! The coolest company on earth! Well, at least compared to other giants.

	https://www.theverge.com/21428763/microsoft-surface-duo-review

1400 bucks! That's totally cheap! You get two iPhones in one! You get Outlook! There's a Microsoft logo on it! Everybody is going to envy you! Everybody possesses an iPhone, but you are SPECIAL! A real CHARACTER! An INDIVIDUAL! This device will change your life and redefine you... for the better, I hope.