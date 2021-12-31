Dim WShell
Set WShell = CreateObject("WScript.Shell")
WShell.Run "GDrpc\GDrpc.exe", 0
Set WShell = Nothing