object FormSearch: TFormSearch
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Search'
  ClientHeight = 294
  ClientWidth = 459
  Color = clBtnFace
  DefaultMonitor = dmMainForm
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poMainFormCenter
  ShowHint = True
  OnClose = FormClose
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    AlignWithMargins = True
    Left = 4
    Top = 12
    Width = 30
    Height = 13
    Caption = 'What:'
    Layout = tlCenter
  end
  object LabelInfo: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 253
    Width = 453
    Height = 13
    Align = alBottom
    AutoSize = False
    ExplicitTop = 183
    ExplicitWidth = 452
  end
  object HTMLSearchAddInfo: TElHTMLLabel
    AlignWithMargins = True
    Left = 3
    Top = 272
    Width = 453
    Height = 19
    Align = alBottom
    AutoSize = False
    ExplicitTop = 225
    ExplicitWidth = 463
  end
  object BtnCancel: TButton
    Left = 343
    Top = 63
    Width = 108
    Height = 26
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = BtnCancelClick
  end
  object EditSearch: TEdit
    AlignWithMargins = True
    Left = 33
    Top = 9
    Width = 304
    Height = 21
    Constraints.MaxHeight = 21
    TabOrder = 0
    OnChange = EditSearchChange
    OnKeyDown = EditSearchKeyDown
  end
  object BtnNext: TButton
    AlignWithMargins = True
    Left = 343
    Top = 8
    Width = 107
    Height = 26
    Caption = 'Find'
    Enabled = False
    TabOrder = 1
    OnClick = BtnNextClick
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 36
    Width = 202
    Height = 117
    Caption = 'Match Options'
    TabOrder = 3
    object CheckTrim: TCheckBox
      AlignWithMargins = True
      Left = 5
      Top = 44
      Width = 192
      Height = 17
      Align = alTop
      Caption = 'Trim'
      Checked = True
      State = cbChecked
      TabOrder = 0
      OnClick = OnChangeValue
      ExplicitTop = 18
    end
    object CheckMatchCase: TCheckBox
      AlignWithMargins = True
      Left = 5
      Top = 67
      Width = 192
      Height = 17
      Align = alTop
      Caption = 'Case sensitive'
      TabOrder = 1
      OnClick = OnChangeValue
      ExplicitTop = 41
    end
    object CheckBoxWholeWord: TCheckBox
      AlignWithMargins = True
      Left = 5
      Top = 90
      Width = 192
      Height = 17
      Align = alTop
      Caption = 'Whole word'
      TabOrder = 2
      OnClick = OnChangeValue
      ExplicitTop = 64
    end
    object Panel1: TPanel
      Left = 2
      Top = 15
      Width = 198
      Height = 26
      Align = alTop
      BevelOuter = bvNone
      Caption = 'Panel1'
      ShowCaption = False
      TabOrder = 3
      object CheckUseRegExp: TCheckBox
        AlignWithMargins = True
        Left = 3
        Top = 3
        Width = 102
        Height = 20
        Align = alLeft
        Caption = 'Use RegExp'
        Color = clBtnFace
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentColor = False
        ParentFont = False
        TabOrder = 0
        OnClick = OnChangeValue
      end
      object BtnRegExpValidate: TButton
        AlignWithMargins = True
        Left = 169
        Top = 3
        Width = 26
        Height = 20
        Hint = 'Validate Regular Expression'
        Align = alRight
        Caption = '...'
        TabOrder = 1
        OnClick = BtnRegExpValidateClick
      end
    end
  end
  object GroupBox2: TGroupBox
    Left = 216
    Top = 205
    Width = 265
    Height = 42
    Caption = 'Extra Options'
    TabOrder = 4
    object CheckCloseTabIfNotFound: TCheckBox
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 255
      Height = 17
      Align = alTop
      Caption = 'Close tab if result is not found'
      TabOrder = 0
      Visible = False
      OnClick = OnChangeValue
      ExplicitTop = 41
    end
  end
  object GroupBox3: TGroupBox
    Left = 8
    Top = 159
    Width = 202
    Height = 88
    Caption = 'Search Options'
    TabOrder = 5
    object CheckStateText: TCheckBox
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 192
      Height = 17
      Align = alTop
      Caption = 'State text'
      Checked = True
      State = cbChecked
      TabOrder = 0
      OnClick = OnChangeValue
    end
    object CheckTransitionText: TCheckBox
      AlignWithMargins = True
      Left = 5
      Top = 41
      Width = 192
      Height = 17
      Align = alTop
      Caption = 'Transition text'
      Checked = True
      State = cbChecked
      TabOrder = 1
      OnClick = OnChangeValue
    end
    object CheckAllProperties: TCheckBox
      AlignWithMargins = True
      Left = 5
      Top = 64
      Width = 192
      Height = 17
      Align = alTop
      Caption = 'All properties'
      Checked = True
      State = cbChecked
      TabOrder = 2
      OnClick = OnChangeValue
    end
  end
  object BtnDefault: TButton
    Left = 343
    Top = 135
    Width = 108
    Height = 25
    Caption = 'Default'
    TabOrder = 6
    OnClick = BtnDefaultClick
  end
  object GroupBox5: TGroupBox
    Left = 216
    Top = 41
    Width = 121
    Height = 88
    Caption = 'Document type'
    TabOrder = 7
    object RadioDocTypeOpened: TRadioButton
      AlignWithMargins = True
      Left = 5
      Top = 41
      Width = 111
      Height = 17
      Align = alTop
      Caption = 'All opened doc'
      TabOrder = 0
      OnClick = RadioGroupDocTypeClick
    end
    object RadioDocTypeCurrent: TRadioButton
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 111
      Height = 17
      Align = alTop
      Caption = 'Current doc'
      Checked = True
      TabOrder = 1
      TabStop = True
      OnClick = RadioGroupDocTypeClick
    end
    object RadioDocTypeWholeProject: TRadioButton
      AlignWithMargins = True
      Left = 5
      Top = 64
      Width = 111
      Height = 17
      Align = alTop
      Caption = 'Whole project'
      TabOrder = 2
      OnClick = RadioGroupDocTypeClick
    end
  end
  object GroupBox6: TGroupBox
    Left = 216
    Top = 135
    Width = 121
    Height = 66
    Caption = 'Search Mode'
    TabOrder = 8
    object RadioModeSelectedStates: TRadioButton
      AlignWithMargins = True
      Left = 5
      Top = 41
      Width = 111
      Height = 17
      Align = alTop
      Caption = 'Selected States'
      TabOrder = 0
      OnClick = RadioModeSelectedStatesClick
    end
    object RadioModeNormal: TRadioButton
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 111
      Height = 17
      Align = alTop
      Caption = 'Normal'
      Checked = True
      TabOrder = 1
      TabStop = True
      OnClick = OnChangeValue
      ExplicitLeft = 3
    end
  end
  object CheckShowResultsInSeparateWindow: TCheckBox
    AlignWithMargins = True
    Left = 343
    Top = 36
    Width = 108
    Height = 27
    Hint = 'Find all Matches and display result in separate window'
    Caption = 'Find All Matches'
    TabOrder = 9
    OnClick = CheckShowResultsInSeparateWindowClick
  end
end
