Dim WShell
Set WShell = CreateObject("WScript.Shell")
WShell.Run "GDrpc.exe", 0
Set WShell = Nothing