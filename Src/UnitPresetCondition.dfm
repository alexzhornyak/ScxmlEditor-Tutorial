object PresetCondition: TPresetCondition
  Left = 0
  Top = 0
  Caption = 'Preset Condition'
  ClientHeight = 337
  ClientWidth = 602
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 57
    Width = 596
    Height = 13
    Align = alTop
    Caption = 'Count of cases'
    ExplicitWidth = 72
  end
  object EditScxmlEvent: TEdit
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 596
    Height = 21
    Align = alTop
    TabOrder = 0
    Text = 'ScxmlEvent'
  end
  object Panel1: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 293
    Width = 596
    Height = 41
    Align = alBottom
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 1
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
      OnClick = BtnOKClick
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
  object EditStateName: TEdit
    AlignWithMargins = True
    Left = 3
    Top = 30
    Width = 596
    Height = 21
    Align = alTop
    TabOrder = 2
    Text = 'BaseStateName'
  end
  object EditCasesCount: TEdit
    AlignWithMargins = True
    Left = 3
    Top = 76
    Width = 596
    Height = 21
    Align = alTop
    NumbersOnly = True
    TabOrder = 3
    Text = '2'
  end
  object CheckAutoLayout: TCheckBox
    AlignWithMargins = True
    Left = 3
    Top = 103
    Width = 596
    Height = 17
    Align = alTop
    Caption = 'AutoLayout'
    TabOrder = 4
    ExplicitLeft = 66
    ExplicitTop = 152
    ExplicitWidth = 97
  end
end
