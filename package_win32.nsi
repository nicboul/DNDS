
# /!\ You need to edit that section
!define MINGW_PATH /usr/lib/gcc/i686-w64-mingw32/4.6/
!define PTHREAD_PATH /usr/i686-w64-mingw32/lib/
!define OPENSSL_PATH /opt/mingw32/mingw32/bin
!define QT_PATH /media/nib/Windows7_OS/Qt/4.8.4/bin
#

SetCompressor /FINAL /SOLID lzma

;-------------------
; Include Modern UI
	!include "MUI2.nsh"

; --------
; General
	Name "DNDS Client"
	OutFile "dnds-client_x86.exe"
	InstallDir $PROGRAMFILES\dnds-client

	; Ask admin privileges
	RequestExecutionLevel admin
	ShowInstDetails show
	ShowUninstDetails show

;-------
; Pages
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY

	; Start Menu Folder Page Configuration
	Var StartMenuFolder
	!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
	!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\dnc"
	!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
	!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

	!insertmacro MUI_PAGE_INSTFILES
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES

;-----------
; Languages
	!insertmacro MUI_LANGUAGE "English"

;-------------------
; Installer section
	Section "dnds client" dncExe
		setOutPath $INSTDIR

		File dnc/src/dnc.exe
		File dnc/dnc.conf
		File libdnds/src/libdnds.dll
		File udt4/src/libudt.dll
		File libconfig-1.4.9/lib/.libs/libconfig-9.dll
		File tapcfg-1.0.0/build/tapcfg.dll
		File ${MINGW_PATH}/libgcc_s_sjlj-1.dll
		File ${MINGW_PATH}/libstdc++-6.dll
		File ${PTHREAD_PATH}/pthreadGC2.dll
		File ${OPENSSL_PATH}/libeay32.dll
		File ${OPENSSL_PATH}/ssleay32.dll
		File ${QT_PATH}/libgcc_s_dw2-1.dll
		File ${QT_PATH}/mingwm10.dll
		File ${QT_PATH}/QtCore4.dll
		File ${QT_PATH}/QtGui4.dll

		; Create uninstaller
		WriteUninstaller "$INSTDIR\Uninstall.exe"

		!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
			CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
			CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\dnc.lnk" "$INSTDIR\dnc.exe"
			CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
		!insertmacro MUI_STARTMENU_WRITE_END
	sectionEnd

	Section "TAP Virtual Ethernet Adapter" SecTAP
		SetOverwrite on
		setOutPath "$TEMP\tapdriver"

		File tap-driver-32_64/32-bit/*
		DetailPrint "TAP INSTALL (May need confirmation)"
		nsExec::ExecToLog '"$TEMP\tapdriver\addtap.bat" /S /SELECT_UTILITIES=1'
		RMDir "$TEMP\tapdriver"
	sectionEnd

;---------------------
; Uninstaller section
	Section "Uninstall"
		Delete "$INSTDIR\*"
		RMDir "$INSTDIR"

		!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder

		Delete "$SMPROGRAMS\$StartMenuFolder\dnc.lnk"
		Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
		RMDir "$SMPROGRAMS\$StartMenuFolder"

		DeleteRegKey /ifempty HKCU "Software\dnc"
	SectionEnd