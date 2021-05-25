object Main: TMain
  Left = 15
  Top = 374
  Width = 526
  Height = 383
  Caption = 'VoiceChat Server'
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object PrevImg: TImage
    Left = 0
    Top = 96
    Width = 512
    Height = 256
  end
  object StatusLst: TListBox
    Left = 0
    Top = 0
    Width = 513
    Height = 97
    ItemHeight = 12
    TabOrder = 0
  end
  object UserListBtn: TButton
    Left = 432
    Top = 64
    Width = 75
    Height = 25
    Caption = 'UserList'
    TabOrder = 1
    Visible = False
    OnClick = UserListBtnClick
  end
  object ClearBtn: TButton
    Left = 416
    Top = 312
    Width = 75
    Height = 25
    Caption = 'ClearIMG'
    TabOrder = 2
    OnClick = ClearBtnClick
  end
  object LogSaveBtn: TButton
    Left = 416
    Top = 280
    Width = 75
    Height = 25
    Caption = 'LogSave'
    TabOrder = 3
    OnClick = LogSaveBtnClick
  end
  object UDPS: TIdUDPServer
    OnStatus = UDPSStatus
    BroadcastEnabled = True
    Bindings = <>
    DefaultPort = 9011
    OnUDPRead = UDPSUDPRead
    Left = 8
    Top = 8
  end
  object ProcTimer: TTimer
    Enabled = False
    Interval = 1
    OnTimer = ProcTimerTimer
    Left = 40
    Top = 8
  end
  object PreviewTimer: TTimer
    Enabled = False
    OnTimer = PreviewTimerTimer
    Left = 136
    Top = 8
  end
  object Cons_ProcIntervalSecTimer: TTimer
    Enabled = False
    OnTimer = Cons_ProcIntervalSecTimerTimer
    Left = 176
    Top = 8
  end
  object SendTimer: TTimer
    Enabled = False
    Interval = 1
    OnTimer = SendTimerTimer
    Left = 224
    Top = 8
  end
  object RestartTimer: TTimer
    OnTimer = RestartTimerTimer
    Left = 264
    Top = 8
  end
end
