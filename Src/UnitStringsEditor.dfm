object StringEditorForm: TStringEditorForm
  Left = 0
  Top = 0
  Caption = 'String editor'
  ClientHeight = 266
  ClientWidth = 392
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object sef_Memo: TMemo
    Left = 0
    Top = 0
    Width = 392
    Height = 225
    Align = alClient
    Lines.Strings = (
      '')
    ScrollBars = ssBoth
    TabOrder = 0
    WordWrap = False
  end
  object sef_Panel: TPanel
    Left = 0
    Top = 225
    Width = 392
    Height = 41
    Align = alBottom
    BevelOuter = bvNone
    Caption = 'sef_Panel'
    ShowCaption = False
    TabOrder = 1
    object sef_BtnCncl: TButton
      Left = 104
      Top = 0
      Width = 104
      Height = 41
      Align = alLeft
      Caption = 'CANCEL'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ModalResult = 2
      ParentFont = False
      TabOrder = 0
    end
    object sef_BtnOk: TButton
      Left = 0
      Top = 0
      Width = 104
      Height = 41
      Align = alLeft
      Caption = 'OK'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ModalResult = 1
      ParentFont = False
      TabOrder = 1
    end
  end
end
