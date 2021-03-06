#region --- Au3Recorder generated code Start (v3.3.9.5 KeyboardLayout=00000804)  ---
#include <WindowsConstants.au3>
#include <GuiTreeView.au3>
#include <GuiScrollBars.au3>
#include <Array.au3>
#include <WinAPIShPath.au3>

#region --- Internal functions Au3Recorder Start ---
Func _Au3RecordSetup()
Opt('WinWaitDelay',100)
Opt('WinDetectHiddenText',1)
Opt('MouseCoordMode',0)
EndFunc

Func _WinWaitActivate($title,$text,$timeout=0)
	WinWait($title,$text,$timeout)
	If Not WinActive($title,$text) Then WinActivate($title,$text)
	WinWaitActive($title,$text,$timeout)
 EndFunc

Func _WaitCtrlEnable($ctrlID)
   Sleep(200)
   While (1 <> ControlCommand ($WIN_TITLE, "", $ctrlID, "IsEnabled", "" ))
	  Sleep(50)
	  OutDbgStr("Wait ctrl IsEnabled" & @CRLF)
   WEnd
EndFunc

Func _WaitCtrlVisible($ctrlID)
   While (1 <> ControlCommand ($WIN_TITLE, "", $ctrlID, "IsVisible", "" ))
	  Sleep(50)
	  OutDbgStr("Wait ctrl IsVisible" & @CRLF)
   WEnd
EndFunc

_AU3RecordSetup()
$WIN_TITLE = "华安证券V6"
$ctrlTreeViewID = "SysTreeView321"
$ctrlBuyBtnID = "[CLASS:Button; INSTANCE:23]"
$ctrlSaleBtnID = "[CLASS:Button; INSTANCE:41]"
$ctrlRefreshBtnID = "[CLASS:Button; INSTANCE:57]"
$ctrlJYBtnID = "AfxWnd4212"

#CS  While ControlGetHandle ( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" ) = 0
### ;~  While WinGetHandle("发现了新版本") = 0
###   	Sleep(50)
###  WEnd
###  ConsoleWrite(ControlGetHandle ( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" ))
###  ;ControlClick("[TITLE:华安证券; CLASS:#32770]", "", "关闭" )
###  ControlClick( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" )
###
###  Exit;
 #CE

Local $aCmdLine = _WinAPI_CommandLineToArgv($CmdLineRaw)
;ConsoleWrite($aCmdLine[0]&$aCmdLine[1]& $aCmdLine[2]& @CRLF)
;Exit
If ($aCmdLine[0] ==1) Then;一个参数
	  _RunTDX1($aCmdLine[1])
EndIf

Exit
;#cs
 ;ConsoleWrite("1")
Func _RunTDX($pathTDX)
  ShellExecute ( "F:\new_tdx\TdxW.exe" )
  _WinWaitActivate("通达信金融终端V","")
  MouseClick("left",433,231,1)
  Sleep(100)
  MouseClick("left",519,263,1)
  _WinWaitActivate("通达信信息","")
  WinClose ( "通达信信息","" )
  _WinWaitActivate("通达信金融终端V","")

  Send("3")
  _WinWaitActivate("Tdx","")
  Send("99006{ENTER}")
	  ConsoleWrite(111)
   Local $sText = WinGetTitle("通达信金融终端V")
	  ConsoleWrite($sText)
   While StringInStr ( $sText, "创业板指" ) <= 0
	  $sText = WinGetTitle("通达信金融终端V")
	  ConsoleWrite($sText)
   WEnd
	  ConsoleWrite(2222)
Sleep(1000)
  Send("M")
  _WinWaitActivate("Tdx","")
  Send("5{ENTER}")
  Sleep(1000)
  Send("{CTRLDOWN}m{CTRLUP}")
  Sleep(1500)
  Send("{DOWN}")
  Send("{LWINDOWN}{SHIFTDOWN}{LEFT}{SHIFTUP}{LWINUP}")
  Send("{LWINDOWN}{SHIFTDOWN}{LEFT}{SHIFTUP}{LWINUP}")
  WinSetTitle("通达信金融终端V", "", "TDX金融终端V")
  WinSetTitle("通达信信息", "", "TDX信息")
  Sleep(50)
EndFunc

Func _RunTDX1($pathTDX)
  ShellExecute ( $pathTDX )
  _WinWaitActivate("通达信金融终端V","")
  MouseClick("left",433,231,1)
  Sleep(100)
  MouseClick("left",519,263,1)
;~   _WinWaitActivate("通达信信息","")
;~   WinClose ( "通达信信息","" )
;~   MouseClick("left",759,10,1)

  Sleep(3000)
  _WinWaitActivate("通达信金融终端V","")
  Sleep(3000)
  WinActivate ("通达信金融终端V","")
  Sleep(100)

  Sleep(1000)
  MouseClick("left",355,299,1); 激活行情主窗口
  Sleep(1000)
  Send("3")
  _WinWaitActivate("Tdx","")
  Send("99006{ENTER}")
   Local $sText = WinGetTitle("通达信金融终端V")
   While StringInStr ( $sText, "创业板指" ) <= 0
	  $sText = WinGetTitle("通达信金融终端V")
   WEnd
    ; Display the window title.
;~     MsgBox($MB_SYSTEMMODAL, "", $sText)
;~   Sleep(3000)
  MouseClick("left",521,174,1)
;~   Sleep(1000)
  Send("{CTRLDOWN}x{CTRLUP}")
  Sleep(1500)
  Send("{DOWN}")
;~   Sleep(500)
EndFunc
;#ce

Func _RunTDXHuaan($pathTDX)

If WinActivate ( "华安证券V6" ) = 0 Then
    ShellExecute ( $pathTDX )
   _WinWaitActivate("华安证券V6","")
EndIf


Local $hWnd = WinWaitActive("华安证券V6","")

While StringInStr(ControlGetFocus($hWnd),"SafeEdit")= 0
  	Sleep(50)
WEnd

;MsgBox($MB_SYSTEMMODAL, "", $hWnd&"The control that has keyboard focus is: " & ControlGetFocus($hWnd))

;Sleep(500)
Send("28603{ENTER}")

Send("{ENTER}")

  While ControlGetHandle ( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" ) = 0
;~  While WinGetHandle("发现了新版本") = 0
  	Sleep(50)
  WEnd
  ;Sleep(500)
;MsgBox($MB_SYSTEMMODAL, "Title", "This message box will timeout after 10 seconds or select the OK button.", 1)

  While ControlGetHandle ( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" )
	 Sleep(50)
	  ControlClick( "[TITLE:华安证券; CLASS:#32770]", "", "关闭" )
   WEnd
  ;ConsoleWrite(WinGetHandle("发现了新版本"))
   ;ControlEnable("发现了新版本", "关闭", "Button2")

;~    WinSetState(WinGetHandle("发现了新版本"), "", @SW_HIDE)
;~    ConsoleWrite(WinGetHandle("华安证券V6"))
;~    WinSetState(WinGetHandle("华安证券V6"), "", @SW_ENABLE )

WinSetState("华安证券V6","",@SW_MAXIMIZE)

  Sleep(500)
  ;点击【】
  MouseClick("left",521,174,1)

  Sleep(1000)
  ;点击【信用】
  MouseClick("left",152,754,1)

  ;点击买入、卖出、资金股份1

  _ClickTreeViewItem("买入", $ctrlBuyBtnID, 198, 9)
  _ClickTreeViewItem("卖出", $ctrlSaleBtnID, 198, 29)
  _ClickTreeViewItem("资金股份", $ctrlRefreshBtnID, 198, 290)
  _ClickTreeViewItem("卖出", $ctrlSaleBtnID, 198, 29)
  ;_hideJY()


  Sleep(1000)
  MouseClick("left",355,299,1); 激活行情主窗口
  Sleep(1000)
  Send("1")
  _WinWaitActivate("Tdx","")
  Send("59915{ENTER}")
  Sleep(3000)
  WinActivate ("华安证券V6","")
  MouseClick("left",521,174,1)
  Send("m")
  _WinWaitActivate("Tdx","")
  Send("5{ENTER}")
EndFunc



Func _ClickTreeViewItem($tvText, $ctrlBtnID, $x, $y)
	  ;滚动条移动到top
	  $hTreeView = ControlGetHandle($WIN_TITLE, "", $ctrlTreeViewID )
	  ConsoleWrite($hTreeView & @CRLF)
	  $hItemBuy = _GUICtrlTreeView_FindItem($hTreeView, $tvText)
	  ConsoleWrite($hItemBuy)

	  ControlFocus  ($WIN_TITLE, "", $ctrlTreeViewID)
	  Opt("MouseCoordMode", 2) ;设置鼠标函数的坐标参照,相对当前激活窗口客户区坐标
	  $a=ControlGetPos($WIN_TITLE, "", $ctrlTreeViewID) ;获取指定控件相对其窗口的坐标位置和大小
	  While _GUIScrollBars_GetScrollPos ( $hTreeView, $SB_VERT )>0
		 ConsoleWrite(_GUIScrollBars_GetScrollPos ( $hTreeView, $SB_VERT )& @CRLF)
		 MouseClick("left",$a[0]+$x,$a[1]+$y)
		 ConsoleWrite("点击滚动条"& @CRLF)
		 ;_GUIScrollBars_ScrollWindow( $hTreeView, 0, 10 )
	  WEnd

;点击“TreeViewItem”
;~ 	  ControlFocus  ($WIN_TITLE, "", $ctrlTreeViewID)
;~ 	  Opt("MouseCoordMode", 2) ;设置鼠标函数的坐标参照,相对当前激活窗口客户区坐标
;~ 	  $a=ControlGetPos($WIN_TITLE, "", $ctrlTreeViewID) ;获取指定控件相对其窗口的坐标位置和大小
;~ 	  MouseClick("left",$a[0]+34,$a[1]+9)
	  _GUICtrlTreeView_ClickItem($hTreeView, $hItemBuy)
	  ConsoleWrite("点击"&$tvText& @CRLF)

	_WaitCtrlEnable($ctrlBtnID)
;~  	While(ControlGetFocus($WIN_TITLE, "") <> "Edit1")
;~ 	   Sleep(50)
;~     WEnd
EndFunc

Func _showJY()
   $hWnd = ControlGetHandle ($WIN_TITLE, "", "AfxMDIFrame421" )

   Local $iState = WinGetState($hWnd)
   ConsoleWrite($hWnd& @CRLF)
   ; Check if the Notepad window is minimized and display the appropriate message box.
   While (BitAND($iState, 2) ==0)
	  ControlClick($WIN_TITLE, "", $ctrlJYBtnID)
	  Sleep(500)
	  $iState = WinGetState($hWnd)
   WEnd
EndFunc

Func _hideJY()
   $hWnd = ControlGetHandle ($WIN_TITLE, "", "AfxMDIFrame421" )

   Local $iState = WinGetState($hWnd)
   ConsoleWrite($hWnd& @CRLF)
   ; Check if the Notepad window is minimized and display the appropriate message box.
   While (BitAND($iState, 2) <> 0)
	  ControlClick($WIN_TITLE, "", $ctrlJYBtnID)
	  Sleep(500)
	  $iState = WinGetState($hWnd)
   WEnd
EndFunc


