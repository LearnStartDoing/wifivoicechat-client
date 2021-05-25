object SelML: TSelML
  Left = -371
  Top = 423
  BorderStyle = bsDialog
  Caption = 'Select Language'
  ClientHeight = 175
  ClientWidth = 311
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object Label1: TLabel
    Left = 8
    Top = 24
    Width = 170
    Height = 12
    Caption = #20351#29992#12377#12427#35328#35486#12434#36984#25246#12375#12390#12367#12384#12373#12356#12290
  end
  object Label2: TLabel
    Left = 8
    Top = 8
    Width = 190
    Height = 12
    Caption = 'Please select the language for setup.'
  end
  object Label3: TLabel
    Left = 28
    Top = 124
    Width = 135
    Height = 12
    Caption = #35328#35486#35373#23450#12434'%s'#12395#20445#23384#12377#12427#12290
    OnClick = Label3Click
    OnDblClick = Label3Click
  end
  object BitBtn1: TBitBtn
    Left = 128
    Top = 144
    Width = 83
    Height = 25
    TabOrder = 3
    OnClick = BitBtn1Click
    Kind = bkOK
  end
  object BitBtn2: TBitBtn
    Left = 224
    Top = 144
    Width = 83
    Height = 25
    TabOrder = 4
    Kind = bkCancel
  end
  object ENRadioBtn: TRadioButton
    Left = 16
    Top = 48
    Width = 129
    Height = 17
    Caption = 'English / '#33521#35486
    Checked = True
    TabOrder = 0
    TabStop = True
  end
  object JPRadioBtn: TRadioButton
    Left = 16
    Top = 72
    Width = 129
    Height = 17
    Caption = 'Japanese / '#26085#26412#35486
    TabOrder = 1
  end
  object SaveChk: TCheckBox
    Left = 8
    Top = 104
    Width = 297
    Height = 17
    Caption = 'The language setting is saved to %s.'
    TabOrder = 2
  end
end
