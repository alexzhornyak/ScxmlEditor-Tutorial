object ComboStringEditor: TComboStringEditor
  Left = 0
  Top = 0
  Caption = 'ComboStringEditor'
  ClientHeight = 383
  ClientWidth = 527
  Color = clBtnFace
  DefaultMonitor = dmMainForm
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  ShowHint = True
  OnCanResize = FormCanResize
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object sef_Panel: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 339
    Width = 521
    Height = 41
    Align = alBottom
    BevelOuter = bvNone
    Caption = 'sef_Panel'
    ShowCaption = False
    TabOrder = 1
    object sef_BtnCncl: TButton
      AlignWithMargins = True
      Left = 89
      Top = 3
      Width = 80
      Height = 35
      Align = alLeft
      Caption = 'CANCEL'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ModalResult = 2
      ParentFont = False
      TabOrder = 0
    end
    object sef_BtnOk: TButton
      AlignWithMargins = True
      Left = 3
      Top = 3
      Width = 80
      Height = 35
      Align = alLeft
      Caption = 'OK'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ModalResult = 1
      ParentFont = False
      TabOrder = 1
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 527
    Height = 30
    Align = alTop
    BevelOuter = bvNone
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 0
    object Edit1: TEdit
      AlignWithMargins = True
      Left = 3
      Top = 6
      Width = 493
      Height = 21
      Hint = 'Press '#39'Ctrl+Enter'#39' to apply'
      Margins.Top = 6
      Align = alClient
      Constraints.MinWidth = 200
      TabOrder = 0
      OnChange = Edit1Change
      OnKeyDown = Edit1KeyDown
      OnKeyPress = Edit1KeyPress
    end
    object Button1: TButton
      AlignWithMargins = True
      Left = 502
      Top = 6
      Width = 22
      Height = 21
      Margins.Top = 6
      Align = alRight
      Caption = 'x'
      TabOrder = 1
      OnClick = Button1Click
    end
  end
  object ListBox1: TElHTMLListBox
    AlignWithMargins = True
    Left = 3
    Top = 33
    Width = 521
    Height = 300
    Cursor = crDefault
    Hint = 'Press '#39'Enter'#39' or '#39'Dbl Click'#39' to select'
    AllowGrayed = False
    ItemHeight = 16
    ItemIndex = -1
    SelectedFont.Charset = DEFAULT_CHARSET
    SelectedFont.Color = clHighlightText
    SelectedFont.Height = -11
    SelectedFont.Name = 'Tahoma'
    SelectedFont.Style = []
    OnDblClick = ListBox1DblClick
    OnKeyDown = ListBox1KeyDown
    OnKeyPress = ListBox1KeyPress
    Align = alClient
    TabOrder = 2
    UseCustomScrollBars = False
  end
end
