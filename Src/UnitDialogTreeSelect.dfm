object DialogTreeSelect: TDialogTreeSelect
  Left = 0
  Top = 0
  Caption = 'DialogTreeSelect'
  ClientHeight = 337
  ClientWidth = 527
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
  object TreeView1: TTreeView
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 521
    Height = 284
    Align = alClient
    Images = ImageList1
    Indent = 19
    TabOrder = 0
    OnDblClick = TreeView1DblClick
    OnKeyPress = TreeView1KeyPress
  end
  object Panel1: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 293
    Width = 521
    Height = 41
    Align = alBottom
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 1
    object BtnOk: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 75
      Height = 33
      Align = alLeft
      Caption = 'OK'
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
      Caption = 'CANCEL'
      ModalResult = 2
      TabOrder = 1
    end
  end
  object ImageList1: TImageList
    Left = 40
    Top = 24
  end
end
