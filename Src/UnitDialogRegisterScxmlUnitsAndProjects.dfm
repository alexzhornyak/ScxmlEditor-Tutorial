object DialogRegisterScxmlFiles: TDialogRegisterScxmlFiles
  Left = 0
  Top = 0
  AutoSize = True
  BorderStyle = bsDialog
  Caption = 'Register SCXML unit and project file extensions'
  ClientHeight = 180
  ClientWidth = 411
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    AlignWithMargins = True
    Left = 10
    Top = 71
    Width = 398
    Height = 13
    Margins.Left = 10
    Align = alTop
    Caption = 'Select Types'
    ExplicitWidth = 61
  end
  object RadioSelectUser: TRadioGroup
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 405
    Height = 62
    Align = alTop
    Caption = 'Select User'
    ItemIndex = 1
    Items.Strings = (
      'Current User'
      'All Users')
    TabOrder = 0
    ExplicitWidth = 521
  end
  object CheckRegisterScxmlUnit: TCheckBox
    AlignWithMargins = True
    Left = 10
    Top = 90
    Width = 398
    Height = 17
    Margins.Left = 10
    Align = alTop
    Caption = 'SCXML Unit Files (*.scxml)'
    Checked = True
    State = cbChecked
    TabOrder = 1
    ExplicitLeft = 224
    ExplicitTop = 176
    ExplicitWidth = 97
  end
  object CheckRegisterScxmlProject: TCheckBox
    AlignWithMargins = True
    Left = 10
    Top = 113
    Width = 398
    Height = 17
    Margins.Left = 10
    Align = alTop
    Caption = 'SCXML Project Files (*.sproj)'
    Checked = True
    State = cbChecked
    TabOrder = 2
    ExplicitLeft = 13
    ExplicitTop = 98
    ExplicitWidth = 514
  end
  object Panel1: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 136
    Width = 405
    Height = 41
    Align = alTop
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 3
    ExplicitLeft = -2
    ExplicitWidth = 521
    object BtnOK: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 75
      Height = 33
      Align = alLeft
      Caption = 'OK'
      ModalResult = 1
      TabOrder = 0
    end
    object BtnCancel: TButton
      AlignWithMargins = True
      Left = 85
      Top = 4
      Width = 75
      Height = 33
      Align = alLeft
      Caption = 'CANCEL'
      ModalResult = 2
      TabOrder = 1
    end
  end
end
