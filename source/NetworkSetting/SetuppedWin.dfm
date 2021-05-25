object Setupped: TSetupped
  Left = -509
  Top = 392
  BorderStyle = bsDialog
  Caption = 'Setupped'
  ClientHeight = 80
  ClientWidth = 328
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 12
  object Msg1Lbl: TLabel
    Left = 8
    Top = 8
    Width = 279
    Height = 12
    Caption = 'The setup was completed.|'#12475#12483#12488#12450#12483#12503#12364#23436#20102#12375#12414#12375#12383#12290
  end
  object Msg2Lbl: TLabel
    Left = 8
    Top = 24
    Width = 589
    Height = 12
    Caption = 
      'Please refer to "Readme_EXFS.txt" in "files_EXFS/" folder.|"file' +
      's_EXFS/"'#12398'"readme_EXFS.txt"'#12434#21442#29031#12375#12390#12367#12384#12373#12356#12290
  end
  object OpenGlobalINIChk: TCheckBox
    Left = 8
    Top = 53
    Width = 241
    Height = 17
    Caption = 
      'After closed, "shell/global.ini" is opened.|'#32066#20102#24460#12395'"shell/global.in' +
      'i"'#12434#38283#12367
    Checked = True
    State = cbChecked
    TabOrder = 1
  end
  object BitBtn1: TBitBtn
    Left = 256
    Top = 48
    Width = 67
    Height = 25
    TabOrder = 0
    Kind = bkOK
  end
end
