object Main: TMain
  Left = -567
  Top = 512
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsDialog
  Caption = 'Main'
  ClientHeight = 120
  ClientWidth = 344
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  Position = poDefaultPosOnly
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object ServerNameLbl: TLabel
    Left = 8
    Top = 16
    Width = 115
    Height = 12
    Caption = 'Server name|'#12469#12540#12496#21517
  end
  object ServerPortLbl: TLabel
    Left = 8
    Top = 40
    Width = 120
    Height = 12
    Caption = 'Port number|'#12509#12540#12488#30058#21495
  end
  object LatencyLbl: TLabel
    Left = 8
    Top = 64
    Width = 138
    Height = 12
    Caption = 'Net latency|'#12493#12483#12488#36933#24310#26178#38291
  end
  object ApplyBtn: TBitBtn
    Left = 168
    Top = 88
    Width = 81
    Height = 25
    Caption = 'Apply|'#36969#29992
    ModalResult = 1
    TabOrder = 4
    OnClick = ApplyBtnClick
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    NumGlyphs = 2
  end
  object CancelBtn: TBitBtn
    Left = 256
    Top = 88
    Width = 81
    Height = 25
    Caption = 'Cancel|'#12461#12515#12531#12475#12523
    TabOrder = 5
    OnClick = CancelBtnClick
    Kind = bkCancel
  end
  object ServerNameEdt: TEdit
    Left = 88
    Top = 13
    Width = 249
    Height = 20
    TabOrder = 0
    Text = 'ServerNameEdt'
  end
  object ServerPortEdt: TEdit
    Left = 88
    Top = 37
    Width = 249
    Height = 20
    TabOrder = 1
    Text = 'ServerPortEdt'
  end
  object LatencyLst: TComboBox
    Left = 88
    Top = 61
    Width = 161
    Height = 20
    Style = csDropDownList
    ItemHeight = 12
    TabOrder = 2
  end
  object AutoDetectBtn: TButton
    Left = 256
    Top = 61
    Width = 81
    Height = 20
    Caption = 'Auto detect|'#33258#21205#35373#23450
    Default = True
    TabOrder = 3
    OnClick = AutoDetectBtnClick
  end
  object PingPrgBar: TProgressBar
    Left = 8
    Top = 92
    Width = 153
    Height = 15
    TabOrder = 6
  end
  object StartupTimer: TTimer
    Enabled = False
    Interval = 1
    OnTimer = StartupTimerTimer
    Left = 216
    Top = 8
  end
  object IdIcmpClient1: TIdIcmpClient
    ReceiveTimeout = 1000
    Left = 248
    Top = 8
  end
  object NDSROMOpenDlg: TOpenDialog
    DefaultExt = 'nds'
    Filter = 'NDSROM Files (*.nds)|*.nds'
    Left = 280
    Top = 8
  end
end
