'inputbox "", "", ""
'wscript.sleep(1000)
'wscript.quit

'Args = "FRGSMP.DLL"

'sfk hexfind . -bin /46005200470053004D0050002E0044004C004C/

sPath = wscript.arguments(0)

'inputbox "", "", sPath

Args = InputBox("Wert?")

'------------------------------------------------------------------------------
sString = "/"

for i = 1 to len(Args)
	sString2 = mid(Args, i, 1)

	sString2 = asc(sString2)

	sString2 = hex(sString2)

	if not i = 1 then
		'sString = sString & "00"
	end if

	sString = sString & sString2
next

'inputbox "", "", sString
'wscript.quit

sString = sString & "/"

'------------------------------------------------------------------------------ unicode
sString2 = "/"

for i = 1 to len(Args)
	sString3 = mid(Args, i, 1)

	sString3 = asc(sString3)

	sString3 = hex(sString3)

	if not i = 1 then
		sString2 = sString2 & "00"
	end if

	sString2 = sString2 & sString3
next

'inputbox "", "", sString2
'wscript.quit

sString2 = sString2 & "/"

'------------------------------------------------------------------------------
'sfk hexfind -dir "C:\Copy\Search\Nieren" -bin /46005200470053004D0050002E0044004C004C/

'inputbox "", "", sString

'set activex = CreateObject("shell.application")
'activex.ShellExecute "sfk", "hexfind -dir . -bin " & sString

set activex = wscript.createobject("wscript.shell")
'activex.run "cmd /k sfk hexfind -dir " & chr(34) & sPath & chr(34) & " -nocase -bin " & sString

activex.run "cmd /k " & chr(34) & "sfk hexfind -dir " & chr(34) & sPath & chr(34) & " -nocase -bin " & sString & " & sfk hexfind -dir " & chr(34) & sPath & chr(34) & " -nocase -bin " & sString2 & chr(34)