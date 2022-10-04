object ReplaceRefactor: TReplaceRefactor
  Left = 0
  Top = 0
  Caption = 'ReplaceRefactor'
  ClientHeight = 464
  ClientWidth = 1069
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object ListViewReplace: TListView
    Left = 0
    Top = 0
    Width = 1069
    Height = 423
    Align = alClient
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    Checkboxes = True
    Columns = <
      item
        Caption = 'Path'
        Width = 200
      end
      item
        Caption = 'ID'
        Width = 100
      end
      item
        Caption = 'Property'
        Width = 75
      end
      item
        Caption = 'Old pattern'
        Width = 100
      end
      item
        Caption = 'New pattern'
        Width = 100
      end
      item
        Caption = 'Old text'
        Width = 200
      end
      item
        Caption = 'New text'
        Width = 200
      end>
    MultiSelect = True
    TabOrder = 0
    ViewStyle = vsReport
    ExplicitLeft = 1
    ExplicitTop = 35
    ExplicitWidth = 267
    ExplicitHeight = 319
  end
  object Panel1: TPanel
    Left = 0
    Top = 423
    Width = 1069
    Height = 41
    Align = alBottom
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 1
    ExplicitLeft = 280
    ExplicitTop = 232
    ExplicitWidth = 185
    object Button1: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 75
      Height = 33
      Align = alLeft
      Caption = 'APPLY'
      ModalResult = 1
      TabOrder = 0
    end
    object Button2: TButton
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
