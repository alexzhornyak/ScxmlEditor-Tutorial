object FormSimpleHelp: TFormSimpleHelp
  Left = 0
  Top = 0
  Caption = 'Help'
  ClientHeight = 804
  ClientWidth = 702
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 766
    Width = 702
    Height = 38
    Align = alBottom
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 0
    object BtnOk: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 125
      Height = 30
      Align = alLeft
      Caption = 'OK'
      TabOrder = 0
      OnClick = BtnOkClick
    end
    object BtnConvertToDocPanel: TButton
      AlignWithMargins = True
      Left = 568
      Top = 4
      Width = 130
      Height = 30
      Align = alRight
      Caption = 'Convert to doc panel'
      TabOrder = 1
      OnClick = BtnConvertToDocPanelClick
    end
  end
  object RichHelp: TRichEdit
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 696
    Height = 760
    Align = alClient
    Color = clInfoBk
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 1
  end
end
