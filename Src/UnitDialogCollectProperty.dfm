object DialogCopyProperty: TDialogCopyProperty
  Left = 0
  Top = 0
  AutoSize = True
  Caption = 'EDITOR'
  ClientHeight = 88
  ClientWidth = 527
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 521
    Height = 13
    Align = alTop
    Caption = 'Property path (for example: XMLText.Text)'
    ExplicitWidth = 209
  end
  object ComboProperty: TComboBox
    AlignWithMargins = True
    Left = 3
    Top = 22
    Width = 521
    Height = 21
    Align = alTop
    TabOrder = 0
  end
  object Panel1: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 49
    Width = 521
    Height = 36
    Align = alTop
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 1
    object BtnOk: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 75
      Height = 28
      Align = alLeft
      Caption = 'OK'
      ModalResult = 1
      TabOrder = 0
      OnClick = BtnOkClick
    end
    object BtnCancel: TButton
      AlignWithMargins = True
      Left = 85
      Top = 4
      Width = 75
      Height = 28
      Align = alLeft
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
    object BtnEdit: TButton
      AlignWithMargins = True
      Left = 442
      Top = 4
      Width = 75
      Height = 28
      Align = alRight
      Caption = 'Edit'
      TabOrder = 2
      OnClick = BtnEditClick
    end
  end
end
