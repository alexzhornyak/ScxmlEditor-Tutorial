inherited DialogFilterProperty: TDialogFilterProperty
  Caption = 'DialogFilterProperty'
  ClientHeight = 180
  ExplicitWidth = 543
  ExplicitHeight = 218
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel [1]
    AlignWithMargins = True
    Left = 3
    Top = 49
    Width = 521
    Height = 13
    Align = alTop
    Caption = 'Filter condition'
    ExplicitWidth = 70
  end
  object Label3: TLabel [2]
    AlignWithMargins = True
    Left = 3
    Top = 95
    Width = 521
    Height = 13
    Align = alTop
    Caption = 'Text for wildcard compare'
    ExplicitWidth = 125
  end
  inherited Panel1: TPanel
    Top = 141
    ExplicitTop = 141
    object CheckCaseSensitive: TCheckBox
      AlignWithMargins = True
      Left = 166
      Top = 4
      Width = 97
      Height = 28
      Align = alLeft
      Caption = 'Case sensitive'
      TabOrder = 3
      ExplicitLeft = 216
      ExplicitTop = 8
      ExplicitHeight = 17
    end
  end
  object ComboFilterCondition: TComboBox
    AlignWithMargins = True
    Left = 3
    Top = 68
    Width = 521
    Height = 21
    Align = alTop
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 2
    Text = 'Equal'
    Items.Strings = (
      'Equal'
      'Not Equal')
  end
  object ComboCompare: TComboBox
    AlignWithMargins = True
    Left = 3
    Top = 114
    Width = 521
    Height = 21
    Align = alTop
    TabOrder = 3
  end
end
