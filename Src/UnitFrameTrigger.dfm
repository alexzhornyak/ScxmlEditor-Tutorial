object FrameTrigger: TFrameTrigger
  Left = 0
  Top = 0
  Width = 423
  Height = 374
  TabOrder = 0
  object LabelTrack: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 88
    Width = 26
    Height = 13
    Align = alTop
    Caption = 'Track'
  end
  object LabelParams: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 295
    Width = 35
    Height = 13
    Align = alTop
    Caption = 'Params'
  end
  object LabelEdit: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 152
    Width = 18
    Height = 13
    Margins.Bottom = 0
    Align = alTop
    Caption = 'Edit'
  end
  object LabelCombo: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 45
    Width = 33
    Height = 13
    Margins.Bottom = 0
    Align = alTop
    Caption = 'Combo'
  end
  object Button1: TButton
    Left = 0
    Top = 0
    Width = 423
    Height = 25
    Align = alTop
    Caption = 'Button1'
    TabOrder = 0
    OnClick = Button1Click
    OnMouseDown = Button1MouseDown
    OnMouseUp = Button1MouseUp
  end
  object CheckBox1: TCheckBox
    Left = 0
    Top = 25
    Width = 423
    Height = 17
    Align = alTop
    Caption = 'CheckBox1'
    TabOrder = 1
    OnClick = CheckBox1Click
  end
  object TrackBar1: TTrackBar
    Left = 0
    Top = 104
    Width = 423
    Height = 45
    Align = alTop
    TabOrder = 2
    OnChange = TrackBar1Change
    ExplicitTop = 107
  end
  object ScrollBoxParams: TScrollBox
    Left = 0
    Top = 311
    Width = 423
    Height = 50
    Align = alTop
    TabOrder = 3
    ExplicitTop = 263
  end
  object EditBtn1: TElButtonEdit
    AlignWithMargins = True
    Left = 3
    Top = 168
    Width = 417
    Height = 21
    OnChange = EditBtn1Change
    UseCustomScrollBars = False
    ButtonUsePng = False
    ButtonWidth = 22
    OnButtonClick = EditBtn1ButtonClick
    AltButtonUsePng = False
    TabOrder = 4
    Align = alTop
    OnKeyPress = EditBtnKeyPress
    ExplicitTop = 155
  end
  object ComboBox1: TComboBox
    AlignWithMargins = True
    Left = 3
    Top = 61
    Width = 417
    Height = 21
    Align = alTop
    Style = csDropDownList
    TabOrder = 5
    OnChange = ComboBox1Change
  end
  object PanelMemo: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 195
    Width = 417
    Height = 25
    Margins.Bottom = 0
    Align = alTop
    BevelOuter = bvNone
    Caption = 'PanelMemo'
    ShowCaption = False
    TabOrder = 6
    ExplicitTop = 232
    object LabelMemo: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 3
      Width = 28
      Height = 13
      Align = alClient
      Caption = 'Memo'
    end
    object BtnMemo: TButton
      Left = 393
      Top = 0
      Width = 24
      Height = 25
      Align = alRight
      TabOrder = 0
      OnClick = BtnMemoClick
      ExplicitLeft = 392
      ExplicitTop = 1
      ExplicitHeight = 23
    end
  end
  object Memo1: TMemo
    AlignWithMargins = True
    Left = 3
    Top = 221
    Width = 417
    Height = 68
    Margins.Top = 1
    Align = alTop
    ScrollBars = ssVertical
    TabOrder = 7
    OnChange = Memo1Change
    OnKeyPress = Memo1KeyPress
  end
end
