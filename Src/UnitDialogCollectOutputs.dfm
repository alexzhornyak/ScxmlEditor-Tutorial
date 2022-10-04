object DialogCollectOutputs: TDialogCollectOutputs
  Left = 0
  Top = 0
  Caption = 'Outputs'
  ClientHeight = 615
  ClientWidth = 910
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
  object ListViewOutputs: TListView
    Left = 0
    Top = 0
    Width = 910
    Height = 574
    Align = alClient
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    Checkboxes = True
    Columns = <
      item
        Caption = 'ScxmlName [New]'
        Width = 300
      end
      item
        Caption = 'ScxmlName [Old]'
        Width = 300
      end
      item
        Caption = 'Description'
        Width = 300
      end>
    MultiSelect = True
    PopupMenu = PopupMenu1
    TabOrder = 0
    ViewStyle = vsReport
  end
  object Panel1: TPanel
    Left = 0
    Top = 574
    Width = 910
    Height = 41
    Align = alBottom
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 1
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
      OnClick = Button1Click
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
    object CheckWeakBinding: TCheckBox
      AlignWithMargins = True
      Left = 269
      Top = 4
      Width = 97
      Height = 33
      Align = alLeft
      Caption = 'As WeakBinding'
      TabOrder = 2
    end
    object CheckComment: TCheckBox
      AlignWithMargins = True
      Left = 166
      Top = 4
      Width = 97
      Height = 33
      Align = alLeft
      Caption = 'As Comment'
      TabOrder = 3
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 272
    Top = 184
    object Clone1: TMenuItem
      Caption = 'Clone'
      OnClick = Clone1Click
    end
  end
end
