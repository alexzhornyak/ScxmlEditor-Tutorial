object DialogDelayedTransition: TDialogDelayedTransition
  Left = 0
  Top = 0
  AutoSize = True
  BorderStyle = bsDialog
  Caption = 'Delayed Transition'
  ClientHeight = 211
  ClientWidth = 537
  Color = clBtnFace
  DefaultMonitor = dmMainForm
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCloseQuery = FormCloseQuery
  OnKeyDown = FormKeyDown
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    AlignWithMargins = True
    Left = 10
    Top = 3
    Width = 517
    Height = 13
    Margins.Left = 10
    Margins.Right = 10
    Align = alTop
    Caption = 'Event'
    ExplicitWidth = 28
  end
  object EditEvent: TEdit
    AlignWithMargins = True
    Left = 6
    Top = 22
    Width = 525
    Height = 21
    Margins.Left = 6
    Margins.Right = 6
    Align = alTop
    TabOrder = 0
    ExplicitLeft = 208
    ExplicitTop = 176
    ExplicitWidth = 121
  end
  object GroupBox1: TGroupBox
    AlignWithMargins = True
    Left = 3
    Top = 49
    Width = 531
    Height = 112
    Align = alTop
    TabOrder = 1
    ExplicitWidth = 521
    object Label2: TLabel
      AlignWithMargins = True
      Left = 5
      Top = 18
      Width = 27
      Height = 13
      Align = alTop
      Caption = 'Delay'
    end
    object Label3: TLabel
      AlignWithMargins = True
      Left = 5
      Top = 64
      Width = 49
      Height = 13
      Align = alTop
      Caption = 'DelayExpr'
    end
    object EditDelay: TEdit
      AlignWithMargins = True
      Left = 5
      Top = 37
      Width = 521
      Height = 21
      Align = alTop
      TabOrder = 0
      OnChange = EditDelayChange
      ExplicitLeft = 8
      ExplicitTop = 145
    end
    object EditDelayExpr: TEdit
      AlignWithMargins = True
      Left = 5
      Top = 83
      Width = 521
      Height = 21
      Align = alTop
      TabOrder = 1
      OnChange = EditDelayExprChange
      ExplicitLeft = 7
    end
  end
  object Panel1: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 167
    Width = 531
    Height = 41
    Align = alTop
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 2
    ExplicitTop = 293
    ExplicitWidth = 521
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
end
