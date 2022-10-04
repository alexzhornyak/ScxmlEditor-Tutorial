object SimpleReplaceDialog: TSimpleReplaceDialog
  Left = 0
  Top = 0
  AutoSize = True
  BorderStyle = bsDialog
  Caption = 'SimpleReplaceDialog'
  ClientHeight = 153
  ClientWidth = 621
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poMainFormCenter
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 621
    Height = 33
    Align = alTop
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 0
    object Label2: TLabel
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 37
      Height = 25
      Align = alLeft
      Alignment = taCenter
      Caption = 'Search:'
      ExplicitHeight = 13
    end
    object ComboSearch: TComboBox
      AlignWithMargins = True
      Left = 47
      Top = 4
      Width = 570
      Height = 21
      Align = alClient
      TabOrder = 0
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 33
    Width = 621
    Height = 33
    Align = alTop
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 1
    object Label1: TLabel
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 42
      Height = 25
      Align = alLeft
      Alignment = taCenter
      Caption = 'Replace:'
      ExplicitHeight = 13
    end
    object ComboReplace: TComboBox
      AlignWithMargins = True
      Left = 52
      Top = 4
      Width = 565
      Height = 21
      Align = alClient
      TabOrder = 0
    end
  end
  object CheckIgnoreCase: TCheckBox
    AlignWithMargins = True
    Left = 3
    Top = 69
    Width = 615
    Height = 17
    Align = alTop
    Caption = 'Ignore case'
    Checked = True
    State = cbChecked
    TabOrder = 2
  end
  object CheckReplaceAll: TCheckBox
    AlignWithMargins = True
    Left = 3
    Top = 92
    Width = 615
    Height = 17
    Align = alTop
    Caption = 'Replace all'
    Checked = True
    State = cbChecked
    TabOrder = 3
  end
  object Panel3: TPanel
    Left = 0
    Top = 112
    Width = 621
    Height = 41
    Align = alTop
    Caption = 'Panel3'
    ShowCaption = False
    TabOrder = 4
    object BtnReplace: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 75
      Height = 33
      Align = alLeft
      Caption = 'Replace'
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
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
end
