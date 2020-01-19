;
; NiL isn't Liero
;
; InnoSetup file for win32
;
; Written by Phil Howlett
; 05-Sept-2004
;
; Changes:
; 07-Sept-2004 by Christoph Brill
;  * german localization
;  * added URL
;  * fixed Nil -> NiL
;  * added bot
;  * added data and docs as components

[Setup]
AppName=NiL isn't Liero
AppVerName=NiL isn't Liero 0.01
AppVersion=0.01
DefaultDirName={pf}\NiL
DefaultGroupName=NiL
LicenseFile="..\..\COPYING"
OutputBaseFilename=nil-setup
Compression=zip/9
ShowLanguageDialog=yes
AppPublisherURL=http://nil.sourceforge.net

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"

[Type]
Name: "full"; Description: "Full Installation"
Name: "compact"; Description: "Compact Instalation"
Name: "custom"; Description: "Custom Installation"; Flags: iscustom

[Components]
Languages: en; Name: "main"; Description: "Main Files"; Types: full compact custom; Flags: fixed
Languages: de; Name: "main"; Description: "Programmdateien"; Types: full compact custom; Flags: fixed
Languages: en; Name: "data"; Description: "Game Data"; Types: full compact custom; Flags: fixed
Languages: de; Name: "data"; Description: "Spieldaten"; Types: full compact custom; Flags: fixed
Languages: en; Name: "docs"; Description: "Documentation"; Types: full custom;
Languages: de; Name: "docs"; Description: "Dokumentation"; Types: full custom;

[Dirs]
; Setup directory structure
Name: "{app}\bin"
Name: "{app}\data"; Components: data
Name: "{app}\docs"; Components: docs

[Files]
Source: "..\nil.exe"; DestDir: "{app}\bin"; Components: main; Flags: overwritereadonly
Source: "..\..\data\*.*"; Excludes: "CVS\*"; DestDir: "{app}\data"; Components: data; Flags: recursesubdirs
Source: "..\..\docs\*.*"; Excludes: "CVS\*"; DestDir: "{app}\docs"; Components: docs; Flags: recursesubdirs
; library files aren't in CVS - go compile your own. :)
Source: "lib\*.dll"; DestDir: "{app}\bin"; Components: main; Flags: recursesubdirs
; sample nil.cfg file
Source: "nil.cfg"; DestDir: "{app}\bin"; Components: main; Flags: overwritereadonly
Source: "nil.ico"; DestDir: "{app}\bin"; Components: main

[Icons]
Name: "{group}\NiL Server"; Filename: "{app}\bin\nil.exe"; WorkingDir: "{app}\bin"; IconFilename: "{app}\bin\nil.ico"; Parameters: "mode=server"
Name: "{group}\NiL Client"; Filename: "{app}\bin\nil.exe"; WorkingDir: "{app}\bin"; IconFilename: "{app}\bin\nil.ico"; Parameters: "mode=client"
Name: "{group}\NiL Bot"; Filename: "{app}\bin\nil.exe"; WorkingDir: "{app}\bin"; IconFilename: "{app}\bin\nil.ico"; Parameters: "mode=bot"
Languages: en; Name: "{group}\Edit nil.cfg"; Filename: "{sys}\notepad.exe"; WorkingDir: "{app}\bin"; Parameters: "nil.cfg"
Languages: de; Name: "{group}\nil.cfg bearbeiten"; Filename: "{sys}\notepad.exe"; WorkingDir: "{app}\bin"; Parameters: "nil.cfg"
Languages: en; Name: "{group}\Uninstall NiL"; Filename: "{uninstallexe}"
Languages: de; Name: "{group}\NiL desinstallieren"; Filename: "{uninstallexe}"

