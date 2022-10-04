object FormReplaceDlg: TFormReplaceDlg
  Left = 0
  Top = 0
  AutoSize = True
  BorderStyle = bsDialog
  Caption = 'Replace dialog'
  ClientHeight = 185
  ClientWidth = 537
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 537
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
      Width = 486
      Height = 21
      Align = alClient
      TabOrder = 0
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 33
    Width = 537
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
      Width = 481
      Height = 21
      Align = alClient
      TabOrder = 0
    end
  end
  object CheckIgnoreCase: TCheckBox
    AlignWithMargins = True
    Left = 3
    Top = 101
    Width = 531
    Height = 17
    Align = alTop
    Caption = 'Ignore case'
    Checked = True
    State = cbChecked
    TabOrder = 2
    OnClick = OnChangeValue
    ExplicitTop = 95
  end
  object CheckReplaceAll: TCheckBox
    AlignWithMargins = True
    Left = 3
    Top = 124
    Width = 531
    Height = 17
    Align = alTop
    Caption = 'Replace all'
    Checked = True
    State = cbChecked
    TabOrder = 3
    OnClick = OnChangeValue
    ExplicitTop = 118
  end
  object Panel3: TPanel
    Left = 0
    Top = 144
    Width = 537
    Height = 41
    Align = alTop
    Caption = 'Panel3'
    ShowCaption = False
    TabOrder = 4
    ExplicitTop = 138
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
      OnClick = BtnReplaceClick
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
      OnClick = BtnCancelClick
    end
  end
  object Panel4: TPanel
    Left = 0
    Top = 66
    Width = 537
    Height = 32
    Align = alTop
    Caption = 'Panel4'
    ShowCaption = False
    TabOrder = 5
    ExplicitTop = 64
    object CheckUseRegExp: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 156
      Height = 24
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
    object CheckRegexLiteral: TCheckBox
      AlignWithMargins = True
      Left = 166
      Top = 4
      Width = 367
      Height = 24
      Align = alClient
      Caption = 'Literal replace'
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      TabOrder = 1
      OnClick = OnChangeValue
      ExplicitLeft = 135
      ExplicitTop = 6
      ExplicitWidth = 85
    end
  end
end
