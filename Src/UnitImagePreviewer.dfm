object ImagePreviewer: TImagePreviewer
  Left = 0
  Top = 0
  Caption = 'ImagePreviewer'
  ClientHeight = 546
  ClientWidth = 811
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
  object Image1: TImage
    Left = 0
    Top = 30
    Width = 656
    Height = 516
    Align = alClient
    ExplicitLeft = 88
    ExplicitTop = 104
    ExplicitWidth = 105
    ExplicitHeight = 105
  end
  object Panel1: TPanel
    Left = 656
    Top = 30
    Width = 155
    Height = 516
    Align = alRight
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 0
    object Label1: TLabel
      AlignWithMargins = True
      Left = 4
      Top = 129
      Width = 112
      Height = 13
      Margins.Top = 20
      Align = alTop
      Caption = 'Select Image Types'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label2: TLabel
      AlignWithMargins = True
      Left = 4
      Top = 351
      Width = 47
      Height = 13
      Margins.Top = 20
      Align = alTop
      Caption = 'Settings'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label4: TLabel
      AlignWithMargins = True
      Left = 4
      Top = 171
      Width = 86
      Height = 13
      Align = alTop
      Caption = 'Compression level'
    end
    object CheckBox1: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 66
      Width = 147
      Height = 17
      Action = ActionStretch
      Align = alTop
      TabOrder = 0
    end
    object CheckBox2: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 89
      Width = 147
      Height = 17
      Action = ActionKeepAspectRatio
      Align = alTop
      State = cbChecked
      TabOrder = 1
    end
    object Button2: TButton
      AlignWithMargins = True
      Left = 4
      Top = 35
      Width = 147
      Height = 25
      Action = ActionCopyToClipboard
      Align = alTop
      Images = ImageList1
      TabOrder = 2
    end
    object CheckPNG: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 148
      Width = 147
      Height = 17
      Align = alTop
      Caption = 'PNG'
      Checked = True
      State = cbChecked
      TabOrder = 3
    end
    object CheckJPG: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 234
      Width = 147
      Height = 17
      Margins.Top = 20
      Align = alTop
      Caption = 'JPG'
      TabOrder = 4
      OnClick = HandlerUpdateCaption
      ExplicitTop = 235
    end
    object CheckBMP: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 257
      Width = 147
      Height = 17
      Align = alTop
      Caption = 'BMP'
      TabOrder = 5
      OnClick = HandlerUpdateCaption
      ExplicitTop = 258
    end
    object Button1: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 147
      Height = 25
      Action = ActionSave
      Align = alTop
      Images = ImageList1
      ModalResult = 1
      TabOrder = 6
    end
    object CheckSVG: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 280
      Width = 147
      Height = 17
      Align = alTop
      Caption = 'SVG'
      TabOrder = 7
      OnClick = HandlerUpdateCaption
      ExplicitTop = 281
    end
    object Button3: TButton
      AlignWithMargins = True
      Left = 4
      Top = 303
      Width = 147
      Height = 25
      Action = ActionCopySVGToClipboard
      Align = alTop
      Images = ImageList1
      TabOrder = 8
      ExplicitTop = 304
    end
    object CheckProtectOverwrite: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 393
      Width = 147
      Height = 17
      Align = alTop
      Caption = 'Protect Overwrite'
      TabOrder = 9
      OnClick = HandlerUpdateCaption
      ExplicitTop = 394
    end
    object CheckAutoGenerateFileName: TCheckBox
      AlignWithMargins = True
      Left = 4
      Top = 370
      Width = 147
      Height = 17
      Align = alTop
      Caption = 'Auto Generate FileName'
      Checked = True
      State = cbChecked
      TabOrder = 10
      OnClick = HandlerUpdateCaption
      ExplicitTop = 371
    end
    object SpinPNGCompressionLevel: TElSpinEdit
      AlignWithMargins = True
      Left = 4
      Top = 190
      Width = 147
      Height = 21
      Cursor = crIBeam
      Value = 7
      MaxValue = 10
      UseCustomScrollBars = False
      Align = alTop
      TabOrder = 11
      ExplicitLeft = 6
      ExplicitTop = 202
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 811
    Height = 30
    Align = alTop
    Caption = 'Panel2'
    ShowCaption = False
    TabOrder = 1
    object Label3: TLabel
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 44
      Height = 13
      Align = alLeft
      Caption = 'Directory'
      Layout = tlCenter
    end
    object EditFile: TLabeledEdit
      Left = 495
      Top = 4
      Width = 245
      Height = 21
      EditLabel.Width = 43
      EditLabel.Height = 13
      EditLabel.Caption = 'FileName'
      LabelPosition = lpLeft
      TabOrder = 0
      OnChange = HandlerUpdateCaption
    end
    object EditDirectory: TElFolderNameEdit
      AlignWithMargins = True
      Left = 54
      Top = 4
      Width = 339
      Height = 21
      Cursor = crIBeam
      Options = [bfoDontGoBelowDomain, bfoShowPath, bfoNewDialogStyle, bfoAllowEdit]
      RootFolder = sfoDesktopExpanded
      DialogTitle = 'Directories Explorer'
      Prompt = 'Select Image Directory'
      UseCustomScrollBars = False
      OnChange = HandlerUpdateCaption
      ButtonGlyph.Data = {
        CE000000424DCE0000000000000076000000280000000D0000000B0000000100
        0400000000005800000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00D00000000000
        00003F888888888800003FBFBFBFBFB800003FFBFBFBFBF800003FBFBFBFBFB8
        00003FFBFBFBFBF800003FBFBFBFBFB800003FFFFFFFFF880000388888333333
        D000D3FF830DDDDDD000DD3330DDDDDDD000}
      AltButtonUsePng = False
      TabOrder = 1
      Align = alLeft
      DockOrientation = doNoOrient
      ExplicitTop = 3
    end
  end
  object ActionList1: TActionList
    Images = ImageList1
    Left = 368
    Top = 280
    object SaveAs: TFileSaveAs
      Category = 'File'
      Caption = 'Save &As...'
      Hint = 'Save As|Saves the active file with a new name'
      ImageIndex = 2
    end
    object ActionStretch: TAction
      Caption = 'Stretch'
      OnExecute = ActionStretchExecute
      OnUpdate = ActionStretchUpdate
    end
    object ActionKeepAspectRatio: TAction
      Caption = 'Keep aspect ratio'
      Checked = True
      OnExecute = ActionKeepAspectRatioExecute
      OnUpdate = ActionKeepAspectRatioUpdate
    end
    object ActionSave: TAction
      Caption = 'Save'
      ImageIndex = 0
      OnExecute = ActionSaveExecute
    end
    object ActionCopyToClipboard: TAction
      Caption = 'Copy to clipboard'
      ImageIndex = 4
      OnExecute = ActionCopyToClipboardExecute
    end
    object ActionCopySVGToClipboard: TAction
      Caption = 'Copy SVG to clipboard'
      Hint = 'Copy SVG to clipboard'
      ImageIndex = 4
      OnExecute = ActionCopySVGToClipboardExecute
    end
  end
  object ImageList1: TImageList
    Left = 368
    Top = 344
    Bitmap = {
      494C010105000800200010001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000002000000001002000000000000020
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000E0D8CC00795D0700795D0700795D
      0700795D0700795D0700795D0700795D0700795D0700795D0700795D0700795D
      0700795D0700795D0700795D0700E0D8CC000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700CAE40100D1EF0000D1EF
      0000D1EF0000D1EF0000D1EF0000D0ED0000D1EE0000D1EF0000D1EF0000D1EF
      0000D1EF0000D1EF0000CAE40100795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700D1EF0000D1EF0000D1EF
      0000D1EF0000B6C00500876B17008A681800896818008C741700C1D40100D1EF
      0000D1EF0000D1EF0000D1EF0000795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700D1EF0000D1EF0000D1EF
      0000ABAD0D00966E1400E4A70000EAAC0000EAAC0000DCA0010088651600BAC9
      0300D1EF0000D1EF0000D1EF0000795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700D1EF0000D1EF0000C8E1
      010088651700E9AB0000E0A30100A47E1F00B1851500E6A80000E0A401008466
      1600D1EF0000D1EF0000D1EF0000795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700D1EF0000D1EF0000AFB7
      0300B3810A00EAAC0000806B35004BB1BE00539FA1009A7A2700EAAC00009974
      1800C4D90100D1EF0000D1EF0000795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700D1EF0000D1EF0000A5A4
      0C00C1890200EAAC00007067410041DBFE0042D7F8007D683500EAAC0000A77B
      1200B8C60200D1EF0000D1EF0000795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700D1EF0000D1EF0000B8C5
      0200A67A1100EAAC0000A07F290069785F006C6D4D00BB8C1300EAAC00008D6A
      1700CBE60000D1EF0000D1EF0000795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700D1EF0000D1EF0000D0ED
      000085681600DCA00100EAAC0000DDA10100E3A60000EAAC0000CA9205009280
      1500D1EF0000D1EF0000D1EF0000795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700D1EF0000D1EF0000D1EF
      0000C1D5010083641600BA860800DB9F0100D69B0100AD7E0E00886F1700CBE5
      0000D1EF0000D1EF0000D1EF0000795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000795D0700CEEB0000D1EF0000D1EF
      0000D1EF0000CEE90000A7A70C009380180096871800AEB40900D1EE0000D1EF
      0000D1EF0000D1EF0000CEEB0000795D07000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000D7CDBA00795D0700795D0700795D
      0700795D0700D1EF0000D1EF0000D1EF0000D1EF0000D1EF0000CCE60000795D
      0700795D0700795D0700795D0700D7CDBA000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000795D0700795D0700D1EF0000D1EF0000D1EF0000CFEB0000795D0700795D
      0700000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FBFAF800795D0700795D0700795D0700795D0700795D0700795D07000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000BA6833C5C38458FFD38B68FFE18F70FFDC8D6CFFDA8B
      6DFFD78A6EFFCD8B6CFFAB6D44FFA65F2EFF0000000000000000000000000000
      00000000000000000000616161007D7D7D00888888008E8E8E008B8B8B008A8A
      8A008A8A8A008888880067676700585858000000000000000000993300009933
      0000993300009933000099330000993300009933000099330000993300009933
      0000993300009933000099330000000000000000000000000000999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999009999990099999900000000000000000000000000000000000000
      00000000000000000000C68355FFEFCEBAFFDDFFFFFF87EEC7FFA2F4D7FFA2F6
      D7FF8CEEC7FFE0FFFFFFDDA285FFAB6A3EFF0000000000000000000000000000
      000000000000000000007C7C7C00CBCBCB00FBFBFB00D6D6D600E2E2E200E3E3
      E300D7D7D700FBFBFB009F9F9F00636363000000000099330000CC660000CC66
      000099330000E5E5E500CC66000099330000E5E5E500E5E5E500E5E5E5009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5E5009999
      9900CCCCCC00CCCCCC009999990000000000D69E72C4D3996EF4D19668FFCE92
      63FFCB8E5EFFC98A5BFFC37F51FFEFB69AFFEAF3E8FF51BF84FF6FC998FF71C9
      99FF54BF84FFE4F4E9FFDD9C7BFFAA693AFF96969600929292008E8E8E008A8A
      8A00868686008282820078787800B3B3B300EEEEEE00A1A1A100B0B0B000B0B0
      B000A1A1A100EEEEEE0099999900626262000000000099330000CC660000CC66
      000099330000E5E5E500CC66000099330000E5E5E500E5E5E500E5E5E5009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5E5009999
      9900CCCCCC00CCCCCC009999990000000000D7A175FFF8F2EDFFF7F0EAFFF6ED
      E6FFF4EAE2FFF3E7DEFFC48154FFEAB697FFF3F3EAFFEDF1E6FFEFF1E6FFEFF0
      E6FFEDF1E5FFF3F5EDFFD59C79FFB07044FF99999900F1F1F100EEEEEE00EBEB
      EB00E8E8E800E5E5E5007A7A7A00B2B2B200F0F0F000EDEDED00EDEDED00ECEC
      EC00ECECEC00F2F2F20097979700696969000000000099330000CC660000CC66
      000099330000E5E5E500CC66000099330000E5E5E500E5E5E500E5E5E5009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5E5009999
      9900CCCCCC00CCCCCC009999990000000000D9A47AFFF9F3EEFFEBD2BEFFFFFF
      FFFFEBD3BFFFFFFFFFFFC98B61FFE6B592FFE2A781FFE1A781FFDEA37DFFDCA1
      7BFFDB9F79FFD99E77FFD49A73FFBB7E57FF9D9D9D00F2F2F200CECECE000000
      0000CFCFCF000000000085858500AFAFAF00A2A2A200A1A1A1009E9E9E009C9C
      9C009A9A9A009898980094949400797979000000000099330000CC660000CC66
      000099330000E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E5009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC0099999900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E5009999
      9900CCCCCC00CCCCCC009999990000000000DDA87EFFF9F3EFFFEBD0BAFFEBD0
      BBFFEBD0BBFFEBD0BBFFCA8D65FFEAB899FFDDA57EFFDDA680FFDBA37CFFD9A0
      7AFFD9A079FFD89F78FFD89E78FFBF845DFFA1A1A100F2F2F200CCCCCC00CCCC
      CC00CCCCCC00CCCCCC0087878700B4B4B4009F9F9F00A0A0A0009D9D9D009A9A
      9A009A9A9A0099999900989898007E7E7E000000000099330000CC660000CC66
      0000CC660000993300009933000099330000993300009933000099330000CC66
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC00999999009999990099999900999999009999990099999900CCCC
      CC00CCCCCC00CCCCCC009999990000000000DFAA82FFF9F3EFFFEACEB7FFFFFF
      FFFFEBD0BBFFFFFFFFFFC8885DFFEFBFA1FFFDFCFAFFFEFCFBFFFEFDFDFFFEFD
      FCFFFDFBFAFFFDFCFBFFDDA885FFC17F53FFA3A3A300F2F2F200CACACA000000
      0000CCCCCC000000000082828200BBBBBB00FBFBFB00FBFBFB00FDFDFD00FCFC
      FC00FAFAFA00FBFBFB00A3A3A300797979000000000099330000CC660000CC66
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC009999990000000000E1AE87FFFAF4F0FFEACBB2FFEACC
      B3FFEACCB3FFEACCB3FFC7865BFFEFC09EFFFFFFFFFFCC936EFFFFFFFFFFFFFF
      FFFFFFFBF7FFFFF8F1FFE4AF8CFFC78A61FFA7A7A700F3F3F300C6C6C600C7C7
      C700C7C7C700C7C7C70080808000BABABA00000000008E8E8E00000000000000
      0000FAFAFA00F6F6F600AAAAAA00848484000000000099330000CC660000CC66
      0000993300009933000099330000993300009933000099330000993300009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC00999999009999990099999900999999009999990099999900999999009999
      9900CCCCCC00CCCCCC009999990000000000E3B18CFFFAF6F1FFEAC9AEFFFFFF
      FFFFEAC9B0FFFFFFFFFFCC8D65FFF3CDB0FFFFFFFFFFE3C7B3FFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFEABFA1FFC98960FFABABAB00F4F4F400C4C4C4000000
      0000C5C5C5000000000087878700C8C8C80000000000C4C4C400000000000000
      00000000000000000000BABABA00838383000000000099330000CC6600009933
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099330000CC66000099330000000000000000000099999900CCCCCC009999
      9900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099999900CCCCCC009999990000000000E5B48FFFFAF6F2FFE9C6AAFFE9C6
      ACFFEAC7ACFFE9C7ADFFD4976EFFD49E7BFFD09871FFD6A482FFCD8E68FFCD90
      69FFD09A75FFD19973FFC88B62FFAD5A2036AEAEAE00F5F5F500C1C1C100C2C2
      C200C2C2C200C2C2C2009191910099999900929292009F9F9F00898989008B8B
      8B00949494009393930085858500515151000000000099330000CC6600009933
      0000FFFFFF00993300009933000099330000993300009933000099330000FFFF
      FF0099330000CC66000099330000000000000000000099999900CCCCCC009999
      9900FFFFFF00999999009999990099999900999999009999990099999900FFFF
      FF0099999900CCCCCC009999990000000000E7B794FFFBF7F4FFE9C3A6FFFFFF
      FFFFE8C4A9FFFFFFFFFFFFFFFFFFFFFFFFFFE8C7ACFFFFFFFFFFFFFFFFFFFFFF
      FFFFF7F1EBFFCB8F5FFF0000000000000000B1B1B100F6F6F600BEBEBE000000
      0000BFBFBF00000000000000000000000000C2C2C20000000000000000000000
      0000EFEFEF008787870000000000000000000000000099330000CC6600009933
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099330000CC66000099330000000000000000000099999900CCCCCC009999
      9900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099999900CCCCCC009999990000000000E9BA98FFFBF7F4FFE9C3A6FFE9C3
      A6FFE9C3A6FFE9C3A6FFE9C3A6FFE9C3A6FFE9C3A6FFE9C3A6FFE9C3A6FFE9C3
      A6FFFBF7F4FFCE9364FF0000000000000000B4B4B400F6F6F600BEBEBE00BEBE
      BE00BEBEBE00BEBEBE00BEBEBE00BEBEBE00BEBEBE00BEBEBE00BEBEBE00BEBE
      BE00F6F6F6008B8B8B0000000000000000000000000099330000E5E5E5009933
      0000FFFFFF00993300009933000099330000993300009933000099330000FFFF
      FF00993300009933000099330000000000000000000099999900E5E5E5009999
      9900FFFFFF00999999009999990099999900999999009999990099999900FFFF
      FF0099999900999999009999990000000000EBBD9BFFFBF7F4FFFFFFFFFFFFFF
      FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
      FFFFFBF7F4FFD1976AFF0000000000000000B7B7B700F6F6F600000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000F6F6F6008F8F8F0000000000000000000000000099330000CC6600009933
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099330000CC66000099330000000000000000000099999900CCCCCC009999
      9900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099999900CCCCCC009999990000000000ECBF9EFFFBF7F4FF9CD5A5FF98D3
      A1FF94D09DFF90CE98FF8BCB93FF87C98EFF82C689FF7EC384FF7AC180FF76BE
      7CFFFBF7F4FFD49B6FFF0000000000000000BABABA00F6F6F600C0C0C000BDBD
      BD00BABABA00B6B6B600B3B3B300B0B0B000ACACAC00A8A8A800A5A5A500A2A2
      A200F6F6F6009494940000000000000000000000000099330000993300009933
      0000993300009933000099330000993300009933000099330000993300009933
      0000993300009933000099330000000000000000000099999900999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      990099999900999999009999990000000000EEC1A1EBFBF7F4FFFBF7F4FFFBF7
      F4FFFBF7F4FFFBF7F4FFFBF7F4FFFBF7F4FFFBF7F4FFFBF7F4FFFBF7F4FFFBF7
      F4FFFBF7F4FFD7A074F80000000000000000BCBCBC00F6F6F600F6F6F600F6F6
      F600F6F6F600F6F6F600F6F6F600F6F6F600F6F6F600F6F6F600F6F6F600F6F6
      F600F6F6F6009898980000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000EFC2A37EEFC1A2E3EDC09FFFEBBE
      9DFFEBBC9AFFE9BA96FFE7B793FFE6B590FFE4B28CFFE2AF88FFE0AC84FFDDA9
      80FFDCA57DFFDAA37ACA0000000000000000BDBDBD00BCBCBC00BBBBBB00B9B9
      B900B6B6B600B4B4B400B1B1B100AFAFAF00ACACAC00A8A8A800A5A5A500A2A2
      A2009F9F9F009C9C9C000000000000000000424D3E000000000000003E000000
      2800000040000000200000000100010000000000000100000000000000000000
      000000000000000000000000FFFFFF00FFFF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000F00F000000000000
      F01F000000000000FFFF000000000000FFFFFFFFFC00FC00C001C001FC00FC00
      8001800100000000800180010000000080018001000014008001800100000000
      800180010000140080018001000000B080018001000014BC8001800100000000
      800180010003177380018001000300038001800100033FF38001800100030003
      8001800100030003FFFFFFFF0003000300000000000000000000000000000000
      000000000000}
  end
end
