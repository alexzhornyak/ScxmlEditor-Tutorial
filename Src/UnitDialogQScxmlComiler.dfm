object DialogQScxmlCompiler: TDialogQScxmlCompiler
  Left = 0
  Top = 0
  AutoSize = True
  Caption = #39'qscxmlc'#39'  command-line options'
  ClientHeight = 298
  ClientWidth = 1204
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 39
    Width = 1204
    Height = 39
    Align = alTop
    Caption = 'Panel1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clHighlight
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    ShowCaption = False
    TabOrder = 0
    object Label1: TLabel
      AlignWithMargins = True
      Left = 23
      Top = 4
      Width = 197
      Height = 31
      Align = alLeft
      AutoSize = False
      Caption = '--namespace <namespace>'
      Layout = tlCenter
      OnClick = CheckNamespaceClick
      ExplicitLeft = 4
      ExplicitHeight = 33
    end
    object Label2: TLabel
      AlignWithMargins = True
      Left = 708
      Top = 4
      Width = 411
      Height = 31
      Align = alRight
      AutoSize = False
      Caption = 
        'Put the generated class(es) in the specified namespace. You can ' +
        'use the QSCXMLC_NAMESPACE variable to specify this in your proje' +
        'ct file.'
      Layout = tlCenter
      WordWrap = True
      ExplicitLeft = 792
      ExplicitTop = 1
      ExplicitHeight = 39
    end
    object ComboNamespace: TComboBox
      AlignWithMargins = True
      Left = 226
      Top = 4
      Width = 476
      Height = 21
      Align = alClient
      TabOrder = 0
    end
    object CheckNamespace: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 13
      Height = 31
      Align = alLeft
      TabOrder = 1
      OnClick = CheckNamespaceClick
    end
    object Button1: TButton
      AlignWithMargins = True
      Left = 1125
      Top = 4
      Width = 75
      Height = 31
      Align = alRight
      Caption = 'Edit'
      TabOrder = 2
      OnClick = BtnEditClick
      ExplicitLeft = 1148
      ExplicitTop = 2
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 1204
    Height = 39
    Align = alTop
    Caption = 'Panel1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    ShowCaption = False
    TabOrder = 1
    object Label3: TLabel
      AlignWithMargins = True
      Left = 24
      Top = 4
      Width = 197
      Height = 31
      Margins.Left = 23
      Align = alLeft
      AutoSize = False
      Caption = '-o <base/out/name>'
      Layout = tlCenter
      ExplicitLeft = 4
      ExplicitHeight = 33
    end
    object Label4: TLabel
      AlignWithMargins = True
      Left = 708
      Top = 4
      Width = 411
      Height = 31
      Align = alRight
      AutoSize = False
      Caption = 
        'The base name of the output files. This can include a path. If n' +
        'one is specified, the basename of the input file is used.'
      Layout = tlCenter
      WordWrap = True
      ExplicitLeft = 792
      ExplicitTop = 1
      ExplicitHeight = 39
    end
    object ComboOutName: TComboBox
      AlignWithMargins = True
      Left = 227
      Top = 4
      Width = 475
      Height = 21
      Align = alClient
      TabOrder = 0
    end
    object BtnEdit: TButton
      AlignWithMargins = True
      Left = 1125
      Top = 4
      Width = 75
      Height = 31
      Align = alRight
      Caption = 'Edit'
      TabOrder = 1
      OnClick = BtnEditClick
      ExplicitLeft = 1171
      ExplicitTop = 2
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 156
    Width = 1204
    Height = 50
    Align = alTop
    Caption = 'Panel1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clHighlight
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    ShowCaption = False
    TabOrder = 2
    object Label5: TLabel
      AlignWithMargins = True
      Left = 23
      Top = 4
      Width = 197
      Height = 42
      Align = alLeft
      AutoSize = False
      Caption = '--classname <StateMachineClassName>'
      Layout = tlCenter
      OnClick = CheckNamespaceClick
      ExplicitLeft = 4
      ExplicitHeight = 33
    end
    object Label6: TLabel
      AlignWithMargins = True
      Left = 708
      Top = 4
      Width = 411
      Height = 42
      Align = alRight
      AutoSize = False
      Caption = 
        'The class name of the generated state machine. If none is specif' +
        'ied, the value of the name attribute of the <scxml> tag is taken' +
        '. If that attribute is not specified either, the basename (exclu' +
        'ding path) is taken from the input file name.'
      Layout = tlCenter
      WordWrap = True
      ExplicitLeft = 792
      ExplicitTop = 1
      ExplicitHeight = 39
    end
    object ComboClassName: TComboBox
      AlignWithMargins = True
      Left = 226
      Top = 4
      Width = 476
      Height = 21
      Align = alClient
      TabOrder = 0
    end
    object CheckClassName: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 13
      Height = 42
      Align = alLeft
      TabOrder = 1
      OnClick = CheckNamespaceClick
      ExplicitHeight = 31
    end
    object Button4: TButton
      AlignWithMargins = True
      Left = 1125
      Top = 4
      Width = 75
      Height = 42
      Align = alRight
      Caption = 'Edit'
      TabOrder = 2
      OnClick = BtnEditClick
      ExplicitLeft = 1156
      ExplicitTop = 6
    end
  end
  object Panel4: TPanel
    Left = 0
    Top = 117
    Width = 1204
    Height = 39
    Align = alTop
    Caption = 'Panel1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clHighlight
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    ShowCaption = False
    TabOrder = 3
    object Label7: TLabel
      AlignWithMargins = True
      Left = 23
      Top = 4
      Width = 197
      Height = 31
      Align = alLeft
      AutoSize = False
      Caption = '--impl <cpp/out>'
      Layout = tlCenter
      OnClick = CheckNamespaceClick
      ExplicitLeft = 4
      ExplicitHeight = 33
    end
    object Label8: TLabel
      AlignWithMargins = True
      Left = 708
      Top = 4
      Width = 411
      Height = 31
      Align = alRight
      AutoSize = False
      Caption = 
        'The name of the output header file. If none is specified, .cpp i' +
        's added to the base name.'
      Layout = tlCenter
      WordWrap = True
      ExplicitLeft = 792
      ExplicitTop = 1
      ExplicitHeight = 39
    end
    object ComboImpl: TComboBox
      AlignWithMargins = True
      Left = 226
      Top = 4
      Width = 476
      Height = 21
      Align = alClient
      TabOrder = 0
    end
    object CheckImpl: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 13
      Height = 31
      Align = alLeft
      TabOrder = 1
      OnClick = CheckNamespaceClick
    end
    object Button3: TButton
      AlignWithMargins = True
      Left = 1125
      Top = 4
      Width = 75
      Height = 31
      Align = alRight
      Caption = 'Edit'
      TabOrder = 2
      OnClick = BtnEditClick
      ExplicitLeft = 1148
      ExplicitTop = 2
    end
  end
  object Panel5: TPanel
    Left = 0
    Top = 78
    Width = 1204
    Height = 39
    Align = alTop
    Caption = 'Panel1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clHighlight
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    ShowCaption = False
    TabOrder = 4
    object Label9: TLabel
      AlignWithMargins = True
      Left = 23
      Top = 4
      Width = 197
      Height = 31
      Align = alLeft
      AutoSize = False
      Caption = '--header <header/out>'
      Layout = tlCenter
      OnClick = CheckNamespaceClick
      ExplicitLeft = 4
      ExplicitHeight = 33
    end
    object Label10: TLabel
      AlignWithMargins = True
      Left = 708
      Top = 4
      Width = 411
      Height = 31
      Align = alRight
      AutoSize = False
      Caption = 
        'The name of the output header file. If none is specified, .h is ' +
        'added to the base name.'
      Layout = tlCenter
      WordWrap = True
      ExplicitLeft = 792
      ExplicitTop = 1
      ExplicitHeight = 39
    end
    object ComboHeader: TComboBox
      AlignWithMargins = True
      Left = 226
      Top = 4
      Width = 476
      Height = 21
      Align = alClient
      TabOrder = 0
    end
    object CheckHeader: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 13
      Height = 31
      Align = alLeft
      TabOrder = 1
      OnClick = CheckNamespaceClick
    end
    object Button2: TButton
      AlignWithMargins = True
      Left = 1125
      Top = 4
      Width = 75
      Height = 31
      Align = alRight
      Caption = 'Edit'
      TabOrder = 2
      OnClick = BtnEditClick
      ExplicitLeft = 1148
      ExplicitTop = 2
    end
  end
  object Panel6: TPanel
    Left = 0
    Top = 206
    Width = 1204
    Height = 50
    Align = alTop
    Caption = 'Panel1'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clHighlight
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentBackground = False
    ParentFont = False
    ShowCaption = False
    TabOrder = 5
    ExplicitTop = 164
    object Label11: TLabel
      AlignWithMargins = True
      Left = 23
      Top = 4
      Width = 197
      Height = 42
      Align = alLeft
      AutoSize = False
      Caption = '--statemethods'
      Layout = tlCenter
      OnClick = CheckNamespaceClick
      ExplicitLeft = 4
      ExplicitHeight = 33
    end
    object Label12: TLabel
      AlignWithMargins = True
      Left = 708
      Top = 4
      Width = 411
      Height = 42
      Align = alRight
      AutoSize = False
      Caption = 
        'Generate extra accessor and signal methods for states. This way ' +
        'you can connect to state changes with plain QObject::connect() a' +
        'nd directly call a method to find out if a state is currently ac' +
        'tive.'
      Layout = tlCenter
      WordWrap = True
      ExplicitLeft = 792
      ExplicitTop = 1
      ExplicitHeight = 39
    end
    object CheckStateMethods: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 13
      Height = 42
      Align = alLeft
      TabOrder = 0
      OnClick = CheckNamespaceClick
    end
    object Button5: TButton
      AlignWithMargins = True
      Left = 1125
      Top = 4
      Width = 75
      Height = 42
      Align = alRight
      Caption = 'Edit'
      TabOrder = 1
      OnClick = BtnEditClick
      ExplicitLeft = 1148
      ExplicitTop = 2
    end
  end
  object Panel7: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 259
    Width = 1198
    Height = 36
    Align = alTop
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 6
    ExplicitLeft = 8
    ExplicitTop = 313
    object BtnOk: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 75
      Height = 28
      Align = alLeft
      Caption = 'OK'
      ModalResult = 1
      TabOrder = 0
      OnClick = BtnOkClick
    end
    object BtnCancel: TButton
      AlignWithMargins = True
      Left = 85
      Top = 4
      Width = 75
      Height = 28
      Align = alLeft
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
end
