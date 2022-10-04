object DialogSelectShape: TDialogSelectShape
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Select shape'
  ClientHeight = 119
  ClientWidth = 176
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poOwnerFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Button1: TButton
    Left = 8
    Top = 82
    Width = 75
    Height = 25
    Caption = 'OK'
    ModalResult = 1
    TabOrder = 1
  end
  object Button2: TButton
    Left = 89
    Top = 82
    Width = 75
    Height = 25
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object RadioGroup1: TRadioGroup
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 170
    Height = 71
    Align = alTop
    ItemIndex = 0
    Items.Strings = (
      'From Shape'
      'To Shape')
    TabOrder = 0
    ExplicitLeft = 8
    ExplicitTop = 2
    ExplicitWidth = 156
  end
end
