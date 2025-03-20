object FormScxmlGui: TFormScxmlGui
  Left = 0
  Top = 0
  Caption = 'ScxmlEditor'
  ClientHeight = 1002
  ClientWidth = 1095
  Color = 16116191
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 0
    Top = 35
    Width = 1095
    Height = 3
    Cursor = crVSplit
    Align = alTop
    Color = 16116191
    ParentColor = False
    ExplicitLeft = -60
    ExplicitWidth = 1085
  end
  object ControlBar1: TControlBar
    AlignWithMargins = True
    Left = 3
    Top = 6
    Width = 1089
    Height = 26
    Margins.Top = 6
    Align = alTop
    AutoDock = False
    AutoDrag = False
    BevelInner = bvNone
    BevelOuter = bvNone
    BevelKind = bkNone
    Color = 16116191
    Constraints.MinHeight = 26
    DockSite = False
    GradientEndColor = 14926246
    GradientStartColor = 13604455
    ParentBackground = False
    ParentColor = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnPaint = ControlBar1Paint
    object LabelDbgMsgCount: TLabel
      Left = 817
      Top = 2
      Width = 78
      Height = 22
      AutoSize = False
      Caption = 'Msg queue: 0'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clRed
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      Layout = tlCenter
      Visible = False
    end
    object ReportToolbar: TToolBar
      Left = 11
      Top = 2
      Width = 793
      Height = 22
      Align = alNone
      Caption = 'ReportToolbar'
      Images = ActionsImages
      TabOrder = 0
      Wrapable = False
      object ToolButton1: TToolButton
        Left = 0
        Top = 0
        Action = actNewProj
        ParentShowHint = False
        ShowHint = True
      end
      object ToolButton13: TToolButton
        Left = 23
        Top = 0
        Action = actNewUnit
      end
      object ToolButton14: TToolButton
        Left = 46
        Top = 0
        Width = 8
        Caption = 'ToolButton14'
        ImageIndex = 26
        Style = tbsSeparator
      end
      object ToolButton2: TToolButton
        Left = 54
        Top = 0
        Action = actOpenProject
        ParentShowHint = False
        ShowHint = True
      end
      object ToolButton8: TToolButton
        Left = 77
        Top = 0
        Action = actOpenUnit
      end
      object ToolButton7: TToolButton
        Left = 100
        Top = 0
        Action = actCloseAll
      end
      object ToolButton4: TToolButton
        Left = 123
        Top = 0
        Action = actAddUnit
      end
      object ToolButton5: TToolButton
        Left = 146
        Top = 0
        Action = actRemoveUnit
      end
      object ToolButton3: TToolButton
        Left = 169
        Top = 0
        Action = actSave
        ParentShowHint = False
        ShowHint = True
      end
      object ToolButton6: TToolButton
        Left = 192
        Top = 0
        Action = actSaveAll
      end
      object ToolButton9: TToolButton
        Left = 215
        Top = 0
        Action = actRun
      end
      object ToolButton15: TToolButton
        Left = 238
        Top = 0
        Width = 8
        Caption = 'ToolButton15'
        ImageIndex = 26
        Style = tbsSeparator
      end
      object ToolButton10: TToolButton
        Left = 246
        Top = 0
        Action = actStop
      end
      object ToolButton11: TToolButton
        Left = 269
        Top = 0
        Action = actPause
      end
      object ToolButton12: TToolButton
        Left = 292
        Top = 0
        Action = actNextStep
      end
      object ToolButton16: TToolButton
        Left = 315
        Top = 0
        Action = actExternTesting
        Style = tbsCheck
      end
      object ToolButton17: TToolButton
        Left = 338
        Top = 0
        Action = actSetPanelForeground
        Caption = 'Set unit foreground when debug message is received'
      end
      object ToolButton18: TToolButton
        Left = 361
        Top = 0
        Action = actShapeScrollToView
      end
      object ToolButton26: TToolButton
        Left = 384
        Top = 0
        Action = actAutoOpenUnit
      end
      object ToolButton19: TToolButton
        Left = 407
        Top = 0
        Action = actClearEnteredStates
      end
      object ToolButton34: TToolButton
        Left = 430
        Top = 0
        Action = actTestCoverage
      end
      object ToolButton22: TToolButton
        Left = 453
        Top = 0
        Width = 8
        Caption = 'ToolButton22'
        ImageIndex = 62
        Style = tbsSeparator
      end
      object ToolButton20: TToolButton
        Left = 461
        Top = 0
        Action = actDockPanelScxmlViewVisible
        Style = tbsCheck
      end
      object ToolButton21: TToolButton
        Left = 484
        Top = 0
        Action = actTranslateChangesToScxmlView
      end
      object ToolButton24: TToolButton
        Left = 507
        Top = 0
        Width = 8
        Caption = 'ToolButton24'
        ImageIndex = 68
        Style = tbsSeparator
      end
      object ToolButton29: TToolButton
        Left = 515
        Top = 0
        Action = actXMLTransitionView
        Style = tbsCheck
      end
      object ToolButton30: TToolButton
        Left = 538
        Top = 0
        Width = 8
        Caption = 'ToolButton30'
        ImageIndex = 93
        Style = tbsSeparator
      end
      object ToolButton23: TToolButton
        Left = 546
        Top = 0
        Action = actClearClipboardConnections
      end
      object ToolButton25: TToolButton
        Left = 569
        Top = 0
        Action = actClearLog
      end
      object ToolButton27: TToolButton
        Left = 592
        Top = 0
        Width = 8
        Caption = 'ToolButton27'
        ImageIndex = 72
        Style = tbsSeparator
      end
      object ToolButton28: TToolButton
        Left = 600
        Top = 0
        Action = actLockDocking
      end
      object ToolButton31: TToolButton
        Left = 623
        Top = 0
        Width = 9
        Caption = 'ToolButton31'
        ImageIndex = 92
        Style = tbsSeparator
      end
      object ToolButton32: TToolButton
        Left = 632
        Top = 0
        Action = actAutoLayoutOrthoSplines
        Grouped = True
        Style = tbsCheck
      end
      object ToolButton33: TToolButton
        Left = 655
        Top = 0
        Action = actAutoLayoutPolySplines
        Grouped = True
        Style = tbsCheck
      end
      object LabelIDEInsight: TLabel
        Left = 678
        Top = 0
        Width = 85
        Height = 22
        Hint = 'Live search through IDE (Ctrl+.)'
        Alignment = taRightJustify
        AutoSize = False
        Caption = 'IDE Insight ... '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        Transparent = True
        Layout = tlCenter
        OnClick = actIDEInsightExecute
        OnMouseEnter = LabelIDEInsightMouseEnter
        OnMouseLeave = LabelIDEInsightMouseLeave
      end
      object ToolButton35: TToolButton
        Left = 763
        Top = 0
        Action = actIDEInsight
      end
    end
  end
  object LMDDockSite1: TLMDDockSite
    Left = 0
    Top = 38
    Width = 1095
    Height = 964
    Align = alClient
    TabOrder = 1
    OnUnDock = LMDDockSite1UnDock
    Layout = {
      EFBBBF3C3F786D6C2076657273696F6E3D22312E302220656E636F64696E673D
      227574662D38223F3E0D0A3C736974656C6973743E0D0A093C73697465206964
      3D2253454C465F53495445223E0D0A09093C7A6F6E65206B696E643D22484F52
      5A223E0D0A0909093C7A6F6E65206B696E643D2256455254222073697A653D22
      37363222206473697A653D22302E363938343431373936353136393537223E0D
      0A090909093C7A6F6E65206B696E643D22544142532220616374697665746162
      3D2230222073697A653D2237303922206473697A653D22302E37333835343136
      36363636363636372220697373706163653D2231223E0D0A09090909093C7A6F
      6E65206B696E643D2250414E454C222069643D22446F636B50616E656C467261
      6D65526F6F74222076697369626C653D2231222F3E0D0A09090909093C7A6F6E
      65206B696E643D2250414E454C222069643D22446F636B50616E656C5363786D
      6C56696577222076697369626C653D2231222F3E0D0A09090909093C7A6F6E65
      206B696E643D2250414E454C222069643D22446F636B50616E656C4769664D61
      6B6572222076697369626C653D2231222F3E0D0A090909093C2F7A6F6E653E0D
      0A090909093C7A6F6E65206B696E643D2250414E454C222069643D22446F636B
      50616E656C4C6F67222076697369626C653D2231222073697A653D2232353122
      206473697A653D22302E323631343538333333333333333333222F3E0D0A0909
      093C2F7A6F6E653E0D0A0909093C7A6F6E65206B696E643D2256455254222073
      697A653D2233323922206473697A653D22302E33303135353832303334383330
      3433223E0D0A090909093C7A6F6E65206B696E643D2254414253222061637469
      76657461623D2230222073697A653D2233313822206473697A653D22302E3333
      323733333831323934393634223E0D0A09090909093C7A6F6E65206B696E643D
      2250414E454C222069643D22446F636B50616E656C50726F6A65637454726565
      222076697369626C653D2231222F3E0D0A09090909093C7A6F6E65206B696E64
      3D2250414E454C222069643D22446F636B50616E656C43616C6C537461636B22
      2076697369626C653D2231222F3E0D0A090909093C2F7A6F6E653E0D0A090909
      093C7A6F6E65206B696E643D2250414E454C222069643D22446F636B50616E65
      6C5472616E736974696F6E584D4C222076697369626C653D2231222073697A65
      3D2231343222206473697A653D22302E31343833383132393439363430323922
      2F3E0D0A090909093C7A6F6E65206B696E643D22544142532220616374697665
      7461623D2230222073697A653D2234393622206473697A653D22302E35313838
      3834383932303836333331223E0D0A09090909093C7A6F6E65206B696E643D22
      50414E454C222069643D22446F636B50616E656C50726F70496E737022207669
      7369626C653D2231222F3E0D0A09090909093C7A6F6E65206B696E643D225041
      4E454C222069643D22446F636B50616E656C5472696767657273222076697369
      626C653D2231222F3E0D0A09090909093C7A6F6E65206B696E643D2250414E45
      4C222069643D22446F636B50616E656C42696E64696E6773222076697369626C
      653D2231222F3E0D0A09090909093C7A6F6E65206B696E643D2250414E454C22
      2069643D22446F636B50616E656C536561726368222076697369626C653D2231
      222F3E0D0A090909093C2F7A6F6E653E0D0A0909093C2F7A6F6E653E0D0A0909
      3C2F7A6F6E653E0D0A093C2F736974653E0D0A3C2F736974656C6973743E}
    object DockPanelBindings: TLMDDockPanel
      Left = 766
      Top = 468
      Width = 329
      Height = 472
      ParentBackground = False
      TabOrder = 10
      Caption = 'Bindings'
      object Panel2: TPanel
        Left = 1
        Top = 18
        Width = 327
        Height = 453
        Align = alClient
        BevelOuter = bvNone
        Caption = 'Panel2'
        TabOrder = 0
        object PanelBindingsRoot: TPanel
          AlignWithMargins = True
          Left = 3
          Top = 3
          Width = 321
          Height = 30
          Align = alTop
          BevelOuter = bvNone
          Caption = 'PanelBindingsRoot'
          ShowCaption = False
          TabOrder = 0
          object BtnRefreshBindings: TButton
            AlignWithMargins = True
            Left = 288
            Top = 3
            Width = 30
            Height = 24
            Action = actUpdateBindings
            Align = alRight
            Constraints.MaxHeight = 24
            ImageAlignment = iaCenter
            Images = ActionsImages
            TabOrder = 0
          end
          object EditBindings: TEdit
            AlignWithMargins = True
            Left = 3
            Top = 3
            Width = 279
            Height = 21
            Align = alClient
            Constraints.MaxHeight = 21
            TabOrder = 1
            OnChange = EditBindingsChange
          end
        end
        object Panel1: TPanel
          AlignWithMargins = True
          Left = 3
          Top = 39
          Width = 321
          Height = 411
          Align = alClient
          BevelInner = bvLowered
          Caption = 'Panel1'
          TabOrder = 1
          object TreeViewBindings: TTree
            AlignWithMargins = True
            Left = 8
            Top = 8
            Width = 305
            Height = 395
            Margins.Left = 6
            Margins.Top = 6
            Margins.Right = 6
            Margins.Bottom = 6
            AllowDelete = False
            AllowPanning = pmNone
            Page.Height = 1058
            Page.Width = 771
            Page.UsePrinter = False
            TextEditor.Enabled = False
            Align = alClient
            BevelInner = bvNone
            BevelOuter = bvNone
            PopupMenu = PopupMenuBindings
            TabOrder = 0
            OnDblClick = TreeViewBindingsDblClick
          end
        end
      end
    end
    object DockPanelCallStack: TLMDDockPanel
      Left = 766
      Top = 0
      Width = 329
      Height = 294
      HeaderMenu = PopupMenuStack
      ParentBackground = False
      TabOrder = 9
      Caption = 'Call stack'
      object PanelCallStackRoot: TPanel
        AlignWithMargins = True
        Left = 6
        Top = 18
        Width = 321
        Height = 321
        BevelOuter = bvNone
        Caption = 'PanelCallStackRoot'
        ShowCaption = False
        TabOrder = 0
        object ListViewCallStack: TListView
          Left = 0
          Top = 22
          Width = 321
          Height = 299
          Align = alClient
          BevelInner = bvNone
          BevelOuter = bvNone
          BorderStyle = bsNone
          Columns = <
            item
              Caption = 'Time'
              MinWidth = 10
              Width = 80
            end
            item
              Caption = 'Operation'
              MinWidth = 20
              Width = 100
            end
            item
              Caption = 'Msg'
              MinWidth = 20
            end
            item
              Caption = 'Interpreter'
              MinWidth = 20
              Width = 100
            end
            item
              Caption = 'ID'
              MinWidth = 20
            end>
          PopupMenu = PopupMenuStack
          TabOrder = 0
          ViewStyle = vsReport
          OnAdvancedCustomDrawItem = ListViewCallStackAdvancedCustomDrawItem
          OnAdvancedCustomDrawSubItem = ListViewCallStackAdvancedCustomDrawSubItem
        end
        object CheckBox1: TCheckBox
          Left = 0
          Top = 0
          Width = 321
          Height = 22
          Action = actShowCallStack
          Align = alTop
          TabOrder = 1
        end
      end
    end
    object DockPanelFrameRoot: TLMDDockPanel
      Left = 0
      Top = 0
      Width = 762
      Height = 685
      ParentBackground = False
      TabOrder = 6
      Caption = 'Welcome page'
      OnEnter = DockPanelFrameRootEnter
      object HTMLRecent: TTree
        AlignWithMargins = True
        Left = 26
        Top = 33
        Width = 710
        Height = 636
        Margins.Left = 25
        Margins.Top = 15
        Margins.Right = 25
        Margins.Bottom = 15
        AllowDelete = False
        AllowPanning = pmNone
        Page.Height = 1058
        Page.Width = 771
        Page.UsePrinter = False
        ReadOnly = True
        Selected.Border.Visible = False
        Selected.Color = clWindow
        Selected.ColorUnFocused = clWindow
        Selected.TextColor = clWindowText
        Shadow.Visible = False
        TextEditor.Enabled = False
        VertScrollBar.Max = 857
        VertScrollBar.Min = -4
        VertScrollBar.PageSize = 858
        Zoom.Allow = False
        Zoom.Pen.Mode = pmNotXor
        OnClickShape = HTMLRecentClickShape
        OnMouseEnterShape = HTMLRecentMouseEnterShape
        OnMouseLeaveShape = HTMLRecentMouseLeaveShape
        Align = alClient
        BevelInner = bvNone
        BevelOuter = bvNone
        TabOrder = 0
      end
    end
    object DockPanelGifMaker: TLMDDockPanel
      Left = 0
      Top = 0
      Width = 762
      Height = 685
      ParentBackground = False
      TabOrder = 13
      Caption = 'GIF Maker'
    end
    object DockPanelLog: TLMDDockPanel
      Left = 0
      Top = 713
      Width = 762
      Height = 251
      HeaderMenu = PopupMenuLog
      ParentBackground = False
      TabOrder = 4
      Caption = 'Log'
      object TabControlLog: TTabControl
        AlignWithMargins = True
        Left = 4
        Top = 21
        Width = 754
        Height = 226
        TabOrder = 0
        Tabs.Strings = (
          'Debug'
          'CMD output')
        TabIndex = 0
        OnChange = TabControlLogChange
        object EditViewLog: TLMDEditView
          Left = 4
          Top = 24
          Width = 746
          Height = 198
          Align = alClient
          TabOrder = 0
          Document = EditDocDebug
          TabWidth = 4
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Courier New'
          Font.Style = []
          Gutter.LineNumberingInterval = 1
          Gutter.Bars = <
            item
              Kind = gbLineNumberBar
            end
            item
              Kind = gbFoldBar
            end>
          CustomKeyBindings = <>
          CompletionSettings.ShowCompletionOnTimeout = False
          GutterBarsEmpty = False
        end
      end
    end
    object DockPanelProjectTree: TLMDDockPanel
      Left = 766
      Top = 0
      Width = 329
      Height = 294
      ParentBackground = False
      TabOrder = 7
      Caption = 'Project tree'
      object ProjectManagerView1: TLMDProjectManagerView
        Left = 1
        Top = 18
        Width = 327
        Height = 327
        ProjectManager = ProjectManager1
        BevelOuter = bvNone
        BorderStyle = bsNone
        HideSelection = True
        Indent = 19
        ParentShowHint = False
        PopupMenu = PopupProjView
        ShowHint = True
        TabOrder = 0
        OnDblClick = ProjectManagerView1DblClick
        OnMouseMove = ProjectManagerView1MouseMove
      end
    end
    object DockPanelPropInsp: TLMDDockPanel
      Left = 766
      Top = 468
      Width = 329
      Height = 472
      ParentBackground = False
      TabOrder = 5
      Caption = 'Property inspector'
      object PanelPropInspRoot: TPanel
        Left = 1
        Top = 18
        Width = 327
        Height = 453
        Align = alClient
        BevelOuter = bvNone
        Caption = 'PanelPropInspRoot'
        ShowCaption = False
        TabOrder = 0
        object Splitter2: TSplitter
          Left = 0
          Top = 350
          Width = 327
          Height = 3
          Cursor = crVSplit
          Align = alBottom
          ExplicitLeft = -7
          ExplicitTop = 299
        end
        object PanelPropInspPath: TPanel
          Left = 0
          Top = 0
          Width = 327
          Height = 29
          Align = alTop
          BevelOuter = bvNone
          Caption = 'PanelPropInspPath'
          TabOrder = 0
          object EditPropInspPath: TEdit
            AlignWithMargins = True
            Left = 3
            Top = 3
            Width = 242
            Height = 22
            Hint = 'Use wildcard for mask'#13'For example: Margins.*'
            Margins.Right = 0
            Align = alClient
            Constraints.MaxHeight = 22
            ParentShowHint = False
            ShowHint = True
            TabOrder = 0
            OnChange = EditPropInspPathChange
            ExplicitHeight = 21
          end
          object BtnClosePanelPropPath: TButton
            AlignWithMargins = True
            Left = 297
            Top = 3
            Width = 27
            Height = 23
            Align = alRight
            Caption = 'X'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'MS Sans Serif'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 1
            OnClick = actPanelPropPathExecute
          end
          object CheckFilterProp: TCheckBox
            AlignWithMargins = True
            Left = 248
            Top = 3
            Width = 43
            Height = 23
            Align = alRight
            Caption = 'Filter'
            TabOrder = 2
            OnClick = CheckFilterPropClick
          end
        end
        object PropSettingsInspector: TLMDPropertyInspector
          Left = 0
          Top = 29
          Width = 327
          Height = 321
          Selection = DesignObjects1
          PropKinds = [pkProperties, pkReadOnly]
          SortType = stCustom
          GridVisible = False
          GutterVisible = True
          CursorVisible = True
          SelectionStyle = ssFlat
          FlatSelectionBackgroundColor = 15066597
          FlatSelectionColor = clWindowText
          CategoryBackgroundColor = 15066597
          CategoryFont.Charset = DEFAULT_CHARSET
          CategoryFont.Color = clMaroon
          CategoryFont.Height = -11
          CategoryFont.Name = 'Tahoma'
          CategoryFont.Style = [fsBold]
          Splitter = 40
          ExpandedCategories = True
          Align = alClient
          Color = clWindow
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 1
          OnFilterProp = PropSettingsInspectorFilterProp
          OnChange = PropSettingsInspectorChange
          OnGetCaptionColor = PropSettingsInspectorGetCaptionColor
          OnGetEditorClass = PropSettingsInspectorGetEditorClass
          OnCompare = PropSettingsInspectorCompare
          OnClick = PropSettingsInspectorClick
        end
        object HTMLPropInfo: TElHTMLListBox
          Left = 0
          Top = 353
          Width = 327
          Height = 100
          Cursor = crDefault
          WordWrap = True
          AllowGrayed = False
          ItemHeight = 16
          ItemIndex = -1
          SelectedFont.Charset = DEFAULT_CHARSET
          SelectedFont.Color = clHighlightText
          SelectedFont.Height = -11
          SelectedFont.Name = 'Tahoma'
          SelectedFont.Style = []
          Align = alBottom
          Constraints.MinHeight = 25
          TabOrder = 2
          UseCustomScrollBars = False
        end
      end
    end
    object DockPanelScxmlView: TLMDDockPanel
      Left = 0
      Top = 0
      Width = 762
      Height = 685
      ParentBackground = False
      TabOrder = 11
      Caption = 'SCXML view'
    end
    object DockPanelSearch: TLMDDockPanel
      Left = 766
      Top = 468
      Width = 329
      Height = 472
      ParentBackground = False
      TabOrder = 12
      Caption = 'Search'
    end
    object DockPanelTransitionXML: TLMDDockPanel
      Left = 766
      Top = 322
      Width = 329
      Height = 142
      ParentBackground = False
      TabOrder = 14
      Caption = 'Transition XML'
      OnEnter = DockPanelTransitionXMLEnter
      OnExit = DockPanelTransitionXMLExit
    end
    object DockPanelTriggers: TLMDDockPanel
      Left = 766
      Top = 468
      Width = 329
      Height = 472
      ParentBackground = False
      TabOrder = 8
      Caption = 'Triggers'
      object ScrollBoxTriggers: TScrollBox
        Left = 1
        Top = 18
        Width = 327
        Height = 534
        BevelInner = bvNone
        BevelOuter = bvNone
        BorderStyle = bsNone
        TabOrder = 0
      end
    end
  end
  object MainMenu1: TMainMenu
    Images = ActionsImages
    Left = 488
    Top = 72
    object File1: TMenuItem
      Caption = 'File'
      OnClick = File1Click
      object Newproject1: TMenuItem
        Action = actNewProj
      end
      object NewUnit1: TMenuItem
        Action = actNewUnit
      end
      object Open3: TMenuItem
        Action = actOpenProject
      end
      object Openunit1: TMenuItem
        Action = actOpenUnit
      end
      object Build2: TMenuItem
        Action = actBuildProject
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object AddUnit1: TMenuItem
        Action = actAddUnit
      end
      object AddExistingUnit1: TMenuItem
        Action = actAddExisting
      end
      object Removeunit1: TMenuItem
        Action = actRemoveUnit
      end
      object Close2: TMenuItem
        Action = actClose
      end
      object ActionCloseProject1: TMenuItem
        Action = actCloseAll
      end
      object Revert1: TMenuItem
        Action = actRevert
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object SaveProjectAs1: TMenuItem
        Action = actSaveProjectAs
      end
      object Save1: TMenuItem
        Action = actSave
      end
      object SaveAs1: TMenuItem
        Action = actSaveAs
      end
      object Open2: TMenuItem
        Action = actSaveAll
      end
      object ForceSaveAllOpened: TMenuItem
        Action = actForceSaveAllOpened
      end
      object ForceSaveAll: TMenuItem
        Action = actForceSaveAll
      end
      object N8: TMenuItem
        Caption = '-'
      end
      object Saveflat1: TMenuItem
        Action = actSaveFlat
      end
      object Importstatesconfiguration1: TMenuItem
        Action = actImportConfiguration
      end
      object Export1: TMenuItem
        Caption = 'Export'
        object Exporttorawscxml1: TMenuItem
          Action = actExportToRawScxml
        end
        object ExporttoHPP1: TMenuItem
          Action = actExportToHPP
        end
        object ExporttoPas1: TMenuItem
          Action = actExportToDFM
        end
        object ExportasANSIC1: TMenuItem
          Action = actExportAnsiC
        end
        object Exportusingqscxmlc1: TMenuItem
          Action = actExportQt
        end
        object ExportasgraphDOT1: TMenuItem
          Action = actExportGraphDot
        end
        object ExportasgraphDOTPng1: TMenuItem
          Action = actExportGraphDotPlusPng
        end
        object ExportasSVG1: TMenuItem
          Action = actExportSVG
        end
        object N10: TMenuItem
          Caption = '-'
        end
        object ExportasQtCreatorScxml1: TMenuItem
          Action = actExportAsQtCreatorScxml
        end
        object ExportasVSCodeScxml1: TMenuItem
          Action = actExportAsVSCodeScxml
        end
        object ExportasScxmlGui1: TMenuItem
          Action = actExportAsScxmlGui
        end
        object ExportasSimpleVisualScxml1: TMenuItem
          Action = actExportAsSimpleScxml
        end
      end
      object MenuRecentLine: TMenuItem
        Caption = '-'
      end
    end
    object MenuEdit: TMenuItem
      Caption = 'Edit'
      object Search1: TMenuItem
        Action = actSearch
      end
      object SearchAll1: TMenuItem
        Action = actSearchAll
      end
      object Clearclipboardconnections1: TMenuItem
        Action = actClearClipboardConnections
      end
      object Clearlog2: TMenuItem
        Action = actClearLog
      end
      object N21: TMenuItem
        Caption = '-'
      end
      object IDEInsight1: TMenuItem
        Action = actIDEInsight
      end
      object N23: TMenuItem
        Caption = '-'
      end
      object OrthographicSplinesAutoLayout1: TMenuItem
        Action = actAutoLayoutOrthoSplines
        OnAdvancedDrawItem = CheckImageAdvancedDrawItem
      end
      object PolylineSplinesAutoLayout1: TMenuItem
        Action = actAutoLayoutPolySplines
        OnAdvancedDrawItem = CheckImageAdvancedDrawItem
      end
    end
    object View: TMenuItem
      Caption = 'View'
      OnClick = ViewClick
      object Defaultlayout1: TMenuItem
        Action = actDefaultLayout
      end
      object Loadlayout1: TMenuItem
        Action = actLoadLayout
      end
      object Savelayoutas1: TMenuItem
        Action = actSaveLayout
      end
      object LockUI1: TMenuItem
        Action = actLockDocking
        OnAdvancedDrawItem = CheckImageAdvancedDrawItem
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object Scxmlviewshowhide1: TMenuItem
        Action = actDockPanelScxmlViewVisible
        OnAdvancedDrawItem = CheckImageAdvancedDrawItem
      end
      object MenuZoomHintsEnabled: TMenuItem
        Action = actEnableZoomHints
        Caption = 'Zoom Hints'
        OnAdvancedDrawItem = CheckImageAdvancedDrawItem
      end
      object Propertypathpanel2: TMenuItem
        Action = actPanelPropPath
      end
      object N18: TMenuItem
        Caption = '-'
      end
      object MenuDockpanels: TMenuItem
        Caption = 'Dock panels'
      end
    end
    object MenuTools: TMenuItem
      Caption = 'Tools'
      object PropjectOptions1: TMenuItem
        Action = actOptions
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object Applicationsettings1: TMenuItem
        Action = actAppSettings
      end
      object N11: TMenuItem
        Caption = '-'
      end
      object MenuPresets: TMenuItem
        Caption = 'Presets'
        ImageIndex = 49
        object WizardCondition1: TMenuItem
          Action = actPresetsCondition
        end
        object OnOff1: TMenuItem
          Action = actPresetsOnOff
        end
      end
      object N13: TMenuItem
        Caption = '-'
      end
      object GetMousePos1: TMenuItem
        Action = actGetScreenCursorCoordinates
      end
      object estregularexpression1: TMenuItem
        Action = actRegExpDialog
      end
      object RegisterSCXMLprojectandunitfileextensions1: TMenuItem
        Action = actRegisterScxmlFiles
        Caption = 'Register SCXML file extensions'
        ImageIndex = 110
      end
      object N20: TMenuItem
        Caption = '-'
      end
      object exteditor1: TMenuItem
        Action = actShowTextEditor
      end
    end
    object MenuRun: TMenuItem
      Caption = 'Run'
      OnClick = MenuRunClick
      object Build1: TMenuItem
        Action = actBuildProject
      end
      object N9: TMenuItem
        Caption = '-'
      end
      object Run1: TMenuItem
        Action = actRun
      end
      object actPause1: TMenuItem
        Action = actPause
        AutoCheck = True
      end
      object Stop1: TMenuItem
        Action = actStop
      end
      object Nextstep1: TMenuItem
        Action = actNextStep
      end
      object SyncInvoked1: TMenuItem
        Action = actSyncInvoked
      end
      object estCoverage1: TMenuItem
        Action = actTestCoverage
        AutoCheck = True
      end
      object MenuSpacerBeforeTester: TMenuItem
        Caption = '-'
      end
      object MenuSpacerAfterTester: TMenuItem
        Caption = '-'
      end
      object Startlisteningfromexternaltester1: TMenuItem
        Action = actExternTesting
        OnAdvancedDrawItem = CheckImageAdvancedDrawItem
      end
      object N12: TMenuItem
        Caption = '-'
      end
      object Scrollcharttoviewshape1: TMenuItem
        Action = actShapeScrollToView
        OnAdvancedDrawItem = CheckImageAdvancedDrawItem
      end
      object Setpanelforeground1: TMenuItem
        Action = actSetPanelForeground
        OnAdvancedDrawItem = CheckImageAdvancedDrawItem
      end
      object Clearallstackitems2: TMenuItem
        Action = actStackClear
      end
      object Clearenteredstates1: TMenuItem
        Action = actClearEnteredStates
      end
      object Clearallbreakpoints1: TMenuItem
        Action = actClearAllBreakpoints
      end
    end
    object Help1: TMenuItem
      Caption = 'Help'
      object Hotkeys1: TMenuItem
        Caption = 'Hotkeys'
        ImageIndex = 72
        OnClick = Hotkeys1Click
      end
      object MenuLuaviaHelp: TMenuItem
        Caption = 'Quick help (datamodel  '#39'luavia'#39')'
        ImageIndex = 54
        OnClick = MenuLuaviaHelpClick
      end
      object Documentation1: TMenuItem
        Caption = 'Documentation'
        ImageIndex = 119
        OnClick = Documentation1Click
      end
    end
  end
  object LMDDockManager1: TLMDDockManager
    Style.SiteBk = 16116191
    Style.ZoneSplitterBk = 16116191
    Style.AutoHideNormalTabBk = 16116191
    Style.AutoHideNormalTabFrame = 15719113
    Style.AutoHideNormalTabFont = 14066810
    Style.AutoHideHotTabBk = 16116191
    Style.AutoHideHotTabFrame = 15719371
    Style.AutoHideHotTabFont = 14793885
    Style.ZoneInactiveTabFrame = 14066810
    Style.ZoneInactiveTabFont = 8605991
    Style.ZoneHotTabBk = 16116191
    Style.ZoneHotTabFrame = 14066810
    Style.ZoneHotTabFont = 14066810
    Style.ZoneActiveTabFrame = 14066810
    Style.PanelBk = clWhite
    Style.PanelBorder = 14066810
    Style.PnlSplitterBk = 14066810
    Style.InactivePnlHeaderBk = 15719113
    Style.InactivePnlHeaderFont = clBlack
    Style.ActivePnlHeaderBk = 14066810
    Style.ActivePnlHeaderBkGrd = 14066810
    Style.ActivePnlHeaderFont = clBlack
    Style.InactivePnlHotBtnBk = 14066810
    Style.InactivePnlHotBtnFrame = 14066810
    Style.ActivePnlHotBtnBk = 14066810
    Style.ActivePnlHotBtnFrame = 14066810
    Style.ActivePnlDownBtnBk = 16116191
    Style.ActivePnlDownBtnFrame = 14066810
    Style.Painter = 'VS2005'
    PersistFloatSize = True
    StayFloatFormsOnTop = False
    BeforeRead = LMDDockManager1BeforeRead
    OnWriteAppInfo = LMDDockManager1WriteAppInfo
    Left = 64
    Top = 248
  end
  object ProjectManager1: TLMDProjectManager
    OnSaveQuery = ProjectManager1SaveQuery
    OnSaveAs = ProjectManager1SaveAs
    OnOpen = ProjectManager1Open
    OnFilePathChanged = ProjectManager1FilePathChanged
    Left = 64
    Top = 184
  end
  object PopupProjView: TPopupMenu
    Images = ActionsImages
    OnPopup = PopupProjViewPopup
    Left = 856
    Top = 192
    object MenuProjUnitEnabled: TMenuItem
      Caption = 'Enabled'
      OnClick = MenuProjUnitEnabledClick
    end
    object MenuProjRoot4Build: TMenuItem
      Caption = 'Root For Run and Build'
      OnClick = MenuProjRoot4BuildClick
    end
    object MenuProjBindingsDisabled: TMenuItem
      Caption = 'Bindings Disabled'
      OnClick = MenuProjBindingsDisabledClick
    end
    object MenuProjVirtual: TMenuItem
      Caption = 'Virtual'
      OnClick = MenuProjVirtualClick
    end
    object N15: TMenuItem
      Caption = '-'
    end
    object MenuProjViewAddUnit: TMenuItem
      Caption = 'Add Unit'
      Hint = 'Add Unit'
      ImageIndex = 0
      OnClick = MenuProjViewAddUnitClick
    end
    object MenuProjAddExisting: TMenuItem
      Caption = 'Add Existing Unit'
      Hint = 'Add Existing Unit'
      ImageIndex = 1
      OnClick = MenuProjAddExistingClick
    end
    object MenuProjAddExistingUnitWithAllNestedUnits: TMenuItem
      Caption = 'Add Existing Unit With All Nested Units'
      ImageIndex = 81
      OnClick = MenuProjAddExistingUnitWithAllNestedUnitsClick
    end
    object MenuProjAddAllInvokeSrc: TMenuItem
      Caption = 'Add Nested Src Units'
      ImageIndex = 79
      OnClick = MenuProjAddAllInvokeSrcClick
    end
    object MenuProjCloneUnit: TMenuItem
      Caption = 'Clone Unit'
      ImageIndex = 80
      OnClick = MenuProjCloneUnitClick
    end
    object MenuProjViewRemove: TMenuItem
      Caption = 'Remove Unit'
      ImageIndex = 13
      OnClick = MenuProjViewRemoveClick
    end
    object N17: TMenuItem
      Caption = '-'
    end
    object MenuProjEditInherited: TMenuItem
      Caption = 'Inheritance'
      ImageIndex = 88
    end
    object MenuProjViewReload: TMenuItem
      Caption = 'Reload'
      ImageIndex = 97
      OnClick = MenuProjViewReloadClick
    end
    object MenuProjInheritanceMismatches: TMenuItem
      Caption = 'Mismatches'
      ImageIndex = 98
      OnClick = MenuProjInheritanceMismatchesClick
    end
    object MenuProjOpenAllInherited: TMenuItem
      Caption = 'Open All Inherited'
      ImageIndex = 104
      OnClick = MenuProjOpenAllInheritedClick
    end
    object MenuProjForceSaveAllInherited: TMenuItem
      Caption = 'Force Save All Inherited'
      ImageIndex = 103
      OnClick = MenuProjForceSaveAllInheritedClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object MenuProjViewOpen: TMenuItem
      Caption = 'Open Unit'
      ImageIndex = 116
      OnClick = MenuProjViewOpenClick
    end
    object MenuProjViewClose: TMenuItem
      Caption = 'Close Unit'
      ImageIndex = 12
      OnClick = MenuProjViewCloseClick
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object MenuProjViewSave: TMenuItem
      Caption = 'Save Unit'
      ImageIndex = 62
      OnClick = MenuProjViewSaveClick
    end
    object MenuProjViewSaveAs: TMenuItem
      Caption = 'Save Unit As...'
      ImageIndex = 7
      OnClick = MenuProjViewSaveAsClick
    end
    object MenuProjForceSaveUnit: TMenuItem
      Caption = 'Force Save Unit'
      ImageIndex = 86
      OnClick = MenuProjForceSaveUnitClick
    end
    object MenuProjEditPostSave: TMenuItem
      Caption = 'Edit Post Save'
      ImageIndex = 82
      OnClick = MenuProjEditPostSaveClick
    end
    object MenuProjRename: TMenuItem
      Caption = 'Rename'
      ShortCut = 113
      OnClick = MenuProjRenameClick
    end
    object N7: TMenuItem
      Caption = '-'
    end
    object MenuProjPathToClipboard: TMenuItem
      Caption = 'Copy path to clipboard'
      OnClick = MenuProjPathToClipboardClick
    end
    object MenuProjFileNameToClipboard: TMenuItem
      Caption = 'Copy filename to clipboard'
      OnClick = MenuProjFileNameToClipboardClick
    end
    object MenuProjUnitScxmlName: TMenuItem
      Caption = 'ScxmlName'
      OnClick = MenuProjUnitScxmlNameClick
    end
    object MenuProjOpenShell: TMenuItem
      Caption = 'Open Shell'
      OnClick = MenuProjOpenShellClick
    end
    object N16: TMenuItem
      Caption = '-'
    end
    object MenuProjEditOrder: TMenuItem
      Caption = 'Edit project order'
      ImageIndex = 84
      OnClick = MenuProjEditOrderClick
    end
    object MenuProjMoveUp: TMenuItem
      Caption = 'Move Unit Up'
      ImageIndex = 99
      ShortCut = 16422
      OnClick = MenuProjMoveUpClick
    end
    object MenuProjMoveDown: TMenuItem
      Caption = 'Move Unit Down'
      ImageIndex = 101
      ShortCut = 16424
      OnClick = MenuProjMoveDownClick
    end
    object N19: TMenuItem
      Caption = '-'
    end
    object MenuProjOptions: TMenuItem
      Action = actOptions
    end
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'xml'
    Filter = 'Scxml project files (*.sproj)|*.sproj|All files (*.*)|*.*'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
    Left = 320
    Top = 128
  end
  object ActionList: TActionList
    Images = ActionsImages
    Left = 64
    Top = 72
    object actNewProj: TAction
      Category = 'File'
      Caption = 'New Project'
      Hint = 'New Project'
      ImageIndex = 3
      ShortCut = 16462
      OnExecute = actNewProjExecute
      OnUpdate = actIsTesterWorkingUpdate
    end
    object actSearchAll: TAction
      Category = 'Edit'
      Caption = 'Search All'
      ImageIndex = 89
      ShortCut = 24646
      OnExecute = actSearchAllExecute
      OnUpdate = actSearchUpdate
    end
    object actAddUnit: TAction
      Category = 'File'
      Caption = 'Add Unit'
      Hint = 'Add Unit'
      ImageIndex = 0
      OnExecute = actAddUnitExecute
      OnUpdate = actAddUnitUpdate
    end
    object actSave: TAction
      Category = 'File'
      Caption = 'Save Unit'
      Hint = 'Save Unit'
      ImageIndex = 62
      ShortCut = 16467
      OnExecute = actSaveExecute
      OnUpdate = actSaveUpdate
    end
    object actSaveAs: TAction
      Category = 'File'
      Caption = 'Save Unit As...'
      Hint = 'Save Unit As'
      ImageIndex = 7
      OnExecute = actSaveAsExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actSaveProjectAs: TAction
      Category = 'File'
      Caption = 'Save Project As...'
      Hint = 'Save Project As'
      ImageIndex = 9
      OnExecute = actSaveProjectAsExecute
      OnUpdate = actProjectIsOpenedUpdate
    end
    object actSaveAll: TAction
      Category = 'File'
      Caption = 'Save All'
      Hint = 'Save all (project and units)'
      ImageIndex = 57
      ShortCut = 24659
      OnExecute = actSaveAllExecute
      OnUpdate = actSaveAllUpdate
    end
    object actClose: TAction
      Category = 'File'
      Caption = 'Close'
      Hint = 'Close unit'
      ImageIndex = 17
      ShortCut = 16471
      OnExecute = actCloseExecute
      OnUpdate = actCloseUpdate
    end
    object actCloseAll: TAction
      Category = 'File'
      Caption = 'Close All'
      Hint = 'Close all (project and units)'
      ImageIndex = 12
      ShortCut = 24663
      OnExecute = actCloseAllExecute
      OnUpdate = actCloseAllUpdate
    end
    object actOpenProject: TAction
      Category = 'File'
      Caption = 'Open Project'
      Hint = 'Open Project'
      ImageIndex = 114
      ShortCut = 16463
      OnExecute = actOpenProjectExecute
      OnUpdate = actIsTesterWorkingUpdate
    end
    object actOptions: TAction
      Category = 'Tools'
      Caption = 'Project Options'
      ImageIndex = 50
      OnExecute = actOptionsExecute
      OnUpdate = actOptionsUpdate
    end
    object actRun: TAction
      Category = 'Testing'
      Caption = 'Run'
      Enabled = False
      Hint = 
        'Runs Unit for testing'#13'if a flag '#39'Root For Run and Build'#39' is set'#13 +
        'otherwise runs the current active unit'
      ImageIndex = 10
      ShortCut = 120
      OnExecute = actRunExecute
      OnUpdate = actGetActivePanelAndTestingUpdate
    end
    object actStop: TAction
      Category = 'Testing'
      Caption = 'Stop'
      Enabled = False
      ImageIndex = 21
      OnExecute = actStopExecute
      OnUpdate = actStopUpdate
    end
    object actRemoveUnit: TAction
      Category = 'File'
      Caption = 'Remove Unit'
      Hint = 'Remove Unit'
      ImageIndex = 13
      OnExecute = actRemoveUnitExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actAddExisting: TAction
      Category = 'File'
      Caption = 'Add Existing Unit'
      Hint = 'Add Existing Unit'
      ImageIndex = 1
      OnExecute = actAddExistingExecute
      OnUpdate = actAddUnitUpdate
    end
    object actDefaultLayout: TAction
      Category = 'View'
      Caption = 'Default layout'
      Hint = 'Default layout'
      ImageIndex = 64
      OnExecute = actDefaultLayoutExecute
      OnUpdate = UpdateProjectsClosed
    end
    object actOpenUnit: TAction
      Category = 'File'
      Caption = 'Open Unit'
      Hint = 'Open unit'
      ImageIndex = 116
      ShortCut = 24655
      OnExecute = actOpenUnitExecute
      OnUpdate = actIsTesterWorkingUpdate
    end
    object actPause: TAction
      Category = 'Testing'
      AutoCheck = True
      Caption = 'Pause'
      Enabled = False
      Hint = 
        'Press '#39'Pause'#39' for tracing and debugging'#13'If '#39'Pause'#39' is pressed th' +
        'en StateMachine will be stopped on every '#39'AfterEnter'#39' action'
      ImageIndex = 23
      ShortCut = 121
      OnExecute = actPauseExecute
      OnUpdate = actPauseUpdate
    end
    object actNextStep: TAction
      Category = 'Testing'
      Caption = 'Next step'
      Enabled = False
      Hint = 'Next step'
      ImageIndex = 25
      ShortCut = 122
      OnExecute = actNextStepExecute
      OnUpdate = actNextStepUpdate
    end
    object actAppSettings: TAction
      Category = 'Tools'
      Caption = 'Application settings'
      ImageIndex = 53
      OnExecute = actAppSettingsExecute
    end
    object actNewUnit: TAction
      Category = 'File'
      Caption = 'New Unit'
      Hint = 'New Unit'
      ImageIndex = 27
      ShortCut = 24654
      OnExecute = actNewUnitExecute
      OnUpdate = actIsTesterWorkingUpdate
    end
    object actExportToRawScxml: TAction
      Category = 'File'
      Caption = 'Export as raw scxml'
      Hint = 'Export as raw scxml'
      OnExecute = actExportToRawScxmlExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actExportToHPP: TAction
      Category = 'File'
      Caption = 'Export as HPP'
      Hint = 'Export as HPP'
      OnExecute = actExportToHPPExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actShowCallStack: TAction
      Category = 'Testing'
      AutoCheck = True
      Caption = 'Show call stack'
      OnExecute = actShowCallStackExecute
      OnUpdate = actShowCallStackUpdate
    end
    object actSearch: TAction
      Category = 'Edit'
      Caption = 'Search'
      ImageIndex = 29
      ShortCut = 16454
      OnExecute = actSearchExecute
      OnUpdate = actSearchUpdate
    end
    object actSaveFlat: TAction
      Category = 'File'
      Caption = 'Save flat'
      Hint = 'Save flat'
      ImageIndex = 6
      OnExecute = actSaveFlatExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actExportToDFM: TAction
      Category = 'File'
      Caption = 'Export as DFM'
      Hint = 'Export as DFM'
      OnExecute = actExportToDFMExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actBuildProject: TAction
      Category = 'File'
      Caption = 'Build'
      ImageIndex = 51
      ShortCut = 16450
      OnExecute = actBuildProjectExecute
      OnUpdate = actProjectIsOpenedUpdate
    end
    object actExternTesting: TAction
      Category = 'Testing'
      Caption = 'Start listening from external application'
      Hint = 'Start listening from external application'
      ImageIndex = 37
      OnExecute = actExternTestingExecute
      OnUpdate = actExternTestingUpdate
    end
    object actStackClear: TAction
      Category = 'Testing'
      Caption = 'Clear all stack items'
      ImageIndex = 33
      OnExecute = actStackClearExecute
    end
    object actSetPanelForeground: TAction
      Category = 'Testing'
      Caption = 'Set panel foreground'
      Hint = 'Set unit foreground in testing mode'
      ImageIndex = 76
      OnExecute = actSetPanelForegroundExecute
      OnUpdate = actSetPanelForegroundUpdate
    end
    object actShapeScrollToView: TAction
      Category = 'Testing'
      Caption = 'Scroll chart to view shape'
      Hint = 'Scroll chart to view shape in testing mode'
      ImageIndex = 77
      OnExecute = actShapeScrollToViewExecute
      OnUpdate = actShapeScrollToViewUpdate
    end
    object actClearEnteredStates: TAction
      Category = 'Testing'
      Caption = 'Clear entered states'
      Hint = 'Clear all entered states of active editor panel'
      ImageIndex = 43
      OnExecute = actClearEnteredStatesExecute
      OnUpdate = actGetActivePanelUpdate
    end
    object actExportAnsiC: TFileSaveAs
      Category = 'File'
      Caption = 'Export as ANSI C'
      Dialog.DefaultExt = 'c'
      Dialog.Filter = 'ANSI C files (*.c)|*.c|All files (*.*)|*.*'
      Dialog.Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
      Hint = 'Transform SCXML to ANSI C file'
      BeforeExecute = actExportDefExtBeforeExecute
      OnAccept = actExportAnsiCAccept
      OnUpdate = actGetActiveEditorUpdate
    end
    object actForceSaveAll: TAction
      Category = 'File'
      Caption = 'Force Save All'
      Hint = 
        'Force Save All'#13'Saves all files and project even if it seems to h' +
        'ave no changes'
      ImageIndex = 86
      OnExecute = actForceSaveAllExecute
      OnUpdate = actForceSaveAllUpdate
    end
    object actPresetsCondition: TAction
      Category = 'Presets'
      Caption = 'Condition'
      ImageIndex = 48
      OnExecute = actPresetsConditionExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actPresetsOnOff: TAction
      Category = 'Presets'
      Caption = 'On-Off'
      ImageIndex = 56
      OnExecute = actPresetsOnOffExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actForceSaveAllOpened: TAction
      Category = 'File'
      Caption = 'Force Save All Opened'
      Hint = 
        'Force Save All Opened'#13'Saves all opened files and project even if' +
        ' it seems to have no changes'
      ImageIndex = 105
      OnExecute = actForceSaveAllOpenedExecute
      OnUpdate = actForceSaveAllOpenedUpdate
    end
    object actDockPanelScxmlViewVisible: TAction
      Category = 'View'
      Caption = 'SCXML View - show/hide'
      Hint = 'SCXML view - show/hide'
      ImageIndex = 59
      OnExecute = actDockPanelScxmlViewVisibleExecute
      OnUpdate = actDockPanelScxmlViewVisibleUpdate
    end
    object actTranslateChangesToScxmlView: TAction
      Category = 'View'
      Caption = 'Translate all changes to scxml view'
      Hint = 'Translate all changes to scxml view'#13'PERFORMANCE WARNING!!!'
      ImageIndex = 61
      OnExecute = actTranslateChangesToScxmlViewExecute
      OnUpdate = actTranslateChangesToScxmlViewUpdate
    end
    object actLoadLayout: TFileOpen
      Category = 'View'
      Caption = 'Load Layout'
      Dialog.DefaultExt = 'xml'
      Dialog.Filter = 'Layout files (*.xml)|*.xml|All files (*.*)|*.*'
      Dialog.Options = [ofHideReadOnly, ofFileMustExist, ofEnableSizing]
      Hint = 'Load Layout'
      ImageIndex = 15
      BeforeExecute = actLayoutBeforeExecute
      OnAccept = actLoadLayoutAccept
      OnUpdate = UpdateProjectsClosed
    end
    object actSaveLayout: TFileSaveAs
      Category = 'View'
      Caption = 'Save Layout As'
      Dialog.DefaultExt = 'xml'
      Dialog.Filter = 'Layout files (*.xml)|*.xml|All files (*.*)|*.*'
      Dialog.Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
      Hint = 'Save Layout As'
      ImageIndex = 62
      BeforeExecute = actLayoutBeforeExecute
      OnAccept = actSaveLayoutAccept
      OnUpdate = UpdateProjectsClosed
    end
    object actClearAllBreakpoints: TAction
      Category = 'Testing'
      Caption = 'Clear all breakpoints'
      Hint = 'Clear all breakpoints'
      ImageIndex = 65
      OnExecute = actClearAllBreakpointsExecute
      OnUpdate = actGetActivePanelUpdate
    end
    object actUpdateBindings: TAction
      Caption = 'Update bindings'
      Hint = 'Update all bindings'
      ImageIndex = 66
      OnExecute = actUpdateBindingsExecute
    end
    object actClearClipboardConnections: TAction
      Category = 'Edit'
      Caption = 'Clear clipboard connections'
      Hint = 'Clear clipboard connections'
      ImageIndex = 67
      OnExecute = actClearClipboardConnectionsExecute
      OnUpdate = actClearClipboardConnectionsUpdate
    end
    object actShowTextEditor: TAction
      Category = 'Tools'
      Caption = 'Text editor'
      Hint = 'Open Text Editor'
      ImageIndex = 69
      OnExecute = actShowTextEditorExecute
    end
    object actClearLog: TAction
      Category = 'Edit'
      Caption = 'Clear log'
      Hint = 'Clear log'
      ImageIndex = 71
      OnExecute = actClearLogExecute
    end
    object actPanelPropPath: TAction
      Category = 'View'
      Caption = 'Property path panel'
      OnExecute = actPanelPropPathExecute
      OnUpdate = actPanelPropPathUpdate
    end
    object actExportQt: TFileSaveAs
      Category = 'File'
      Caption = 'Export using '#39'qscxmlc'#39
      Dialog.DefaultExt = 'cpp'
      Dialog.Filter = 'CPP files (*.cpp)|*.cpp|All files (*.*)|*.*'
      Dialog.Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
      Hint = 'Compile SCXML by '#39'qscxmlc'#39
      BeforeExecute = actExportDefExtBeforeExecute
      OnAccept = actExportQtAccept
      OnUpdate = actGetActiveEditorUpdate
    end
    object actEnableZoomHints: TAction
      Category = 'View'
      Hint = 'Zoom hints are disabled'
      ImageIndex = 74
      ShortCut = 24648
      OnExecute = actEnableZoomHintsExecute
      OnUpdate = actEnableZoomHintsUpdate
    end
    object actAutoOpenUnit: TAction
      Category = 'Testing'
      Caption = 'Auto open unit'
      Hint = 'Automatically opens closed unit in testing mode'
      ImageIndex = 75
      OnExecute = actAutoOpenUnitExecute
      OnUpdate = actAutoOpenUnitUpdate
    end
    object actImportConfiguration: TFileOpen
      Category = 'File'
      Caption = 'Import states configuration'
      Dialog.DefaultExt = 'dump'
      Dialog.Filter = 'Dump files (*.dump)|*.dump|All files (*.*)|*.*'
      Dialog.Options = [ofHideReadOnly, ofFileMustExist, ofEnableSizing]
      Hint = 'Open|Opens active states configuration'
      ImageIndex = 78
      OnAccept = actImportConfigurationAccept
      OnUpdate = actGetActiveEditorUpdate
    end
    object actLockDocking: TAction
      Category = 'View'
      Caption = 'Lock UI'
      Hint = 'Locks UI for undocking'
      ImageIndex = 91
      OnExecute = actLockDockingExecute
      OnUpdate = actLockDockingUpdate
    end
    object actGetScreenCursorCoordinates: TAction
      Category = 'Tools'
      Caption = 'Get Mouse Position'
      ImageIndex = 93
      ShortCut = 24693
      OnExecute = actGetScreenCursorCoordinatesExecute
    end
    object actRegExpDialog: TAction
      Category = 'Tools'
      Caption = 'Test regular expression'
      ImageIndex = 95
      OnExecute = actRegExpDialogExecute
    end
    object actXMLTransitionView: TAction
      Category = 'View'
      Caption = 'Transition XML - show/hide'
      Hint = 'Transition XML - show/hide'
      ImageIndex = 96
      OnExecute = actXMLTransitionViewExecute
      OnUpdate = actXMLTransitionViewUpdate
    end
    object actIDEInsight: TAction
      Category = 'Edit'
      Caption = 'IDE Insight ...'
      Hint = 'Live search through IDE'
      ImageIndex = 106
      ShortCut = 16574
      OnExecute = actIDEInsightExecute
    end
    object actAutoLayoutOrthoSplines: TAction
      Category = 'Edit'
      Caption = 'Orthographic Splines (Auto Layout)'
      GroupIndex = 20
      Hint = 'Orthographic Splines (Auto Layout)'
      ImageIndex = 108
      OnExecute = actAutoLayoutOrthoSplinesExecute
      OnUpdate = actAutoLayoutOrthoSplinesUpdate
    end
    object actAutoLayoutPolySplines: TAction
      Category = 'Edit'
      Caption = 'Polyline Splines (Auto Layout)'
      GroupIndex = 20
      Hint = 'Polyline Splines (Auto Layout)'
      ImageIndex = 109
      OnExecute = actAutoLayoutPolySplinesExecute
      OnUpdate = actAutoLayoutPolySplinesUpdate
    end
    object actExportGraphDot: TFileSaveAs
      Category = 'File'
      Caption = 'Export as graph DOT'
      Dialog.DefaultExt = 'gv'
      Dialog.Filter = 
        'DOT files (*.gv)|*.gv|DOT files (*.dot)|*.dot|All files (*.*)|*.' +
        '*'
      Dialog.Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
      Hint = 'Export as graph DOT'
      BeforeExecute = actExportDefExtBeforeExecute
      OnAccept = actExportGraphDotAccept
      OnUpdate = actGetActiveEditorUpdate
    end
    object actExportGraphDotPlusPng: TFileSaveAs
      Category = 'File'
      Caption = 'Export as graph DOT + Png'
      Dialog.DefaultExt = 'gv'
      Dialog.Filter = 
        'DOT files (*.gv)|*.gv|DOT files (*.dot)|*.dot|All files (*.*)|*.' +
        '*'
      Dialog.Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
      Hint = 'Export as graph DOT + Png'
      BeforeExecute = actExportDefExtBeforeExecute
      OnAccept = actExportGraphDotPlusPngAccept
      OnUpdate = actGetActiveEditorUpdate
    end
    object actRegisterScxmlFiles: TAction
      Category = 'Tools'
      Caption = 'Register SCXML project and unit file extensions'
      OnExecute = actRegisterScxmlFilesExecute
    end
    object actExportSVG: TFileSaveAs
      Category = 'File'
      Caption = 'Export as SVG'
      Dialog.DefaultExt = 'svg'
      Dialog.Filter = 'Scalable Vector Graphics (*.svg)|*.svg|All files (*.*)|*.*'
      Dialog.Options = [ofOverwritePrompt, ofHideReadOnly, ofEnableSizing]
      Hint = 'Export as SVG'
      BeforeExecute = actExportDefExtBeforeExecute
      OnAccept = actExportSVGAccept
      OnUpdate = actGetActiveEditorUpdate
    end
    object actSyncInvoked: TAction
      Category = 'Testing'
      Caption = 'Sync Invoked'
      OnExecute = actSyncInvokedExecute
      OnUpdate = actSyncInvokedUpdate
    end
    object actTestCoverage: TAction
      Category = 'Testing'
      AutoCheck = True
      Caption = 'Test Coverage'
      Hint = 
        'When its on, the highlighted states and transitions are not clea' +
        'red during a run'
      ImageIndex = 111
      OnExecute = actTestCoverageExecute
    end
    object actRevert: TAction
      Category = 'File'
      Caption = 'Revert'
      ImageIndex = 112
      ShortCut = 41047
      OnExecute = actRevertExecute
      OnUpdate = actRevertUpdate
    end
    object actExportAsQtCreatorScxml: TAction
      Category = 'File'
      Caption = 'Export as Qt Creator Scxml'
      Hint = 'Save file with Qt Creator visual metainformation'
      OnExecute = actExportAsQtCreatorScxmlExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actExportAsScxmlGui: TAction
      Category = 'File'
      Caption = 'Export as ScxmlGui'
      Hint = 'Save file with ScxmlGui visual metainformation'
      OnExecute = actExportAsScxmlGuiExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actExportAsVSCodeScxml: TAction
      Category = 'File'
      Caption = 'Export as VSCode Scxml'
      OnExecute = actExportAsVSCodeScxmlExecute
      OnUpdate = actGetActiveEditorUpdate
    end
    object actExportAsSimpleScxml: TAction
      Category = 'File'
      Caption = 'Export as Simple Visual Scxml'
      OnExecute = actExportAsSimpleScxmlExecute
      OnUpdate = actGetActiveEditorUpdate
    end
  end
  object ActionsImages: TImageList
    Left = 129
    Top = 72
    Bitmap = {
      494C01017900E002CC0710001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      000000000000360000002800000040000000F0010000010020000000000000F0
      010000000000000000000000000000000000000000000000000000000000CCCC
      CC00999999009999990099999900999999009999990099999900999999009999
      9900CCCCCC00E2EFF10000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000CCCCCC009999
      9900ECC6D900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      990099999900B2B2B200CCCCCC00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000CCCCCC0099999900CCCC
      CC00FFFFFF00CCCCCC00CCCCCC00CCCCCC00CCCCCC00E2EFF100FFFFFF00CCCC
      CC009999990099999900B2B2B200E2EFF1000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900CCCCCC00FFFF
      FF00CCCCCC009999990099999900CCCCCC009999990099999900CCCCCC00FFFF
      FF00CCCCCC009999990099999900CCCCCC000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000CCCCCC0099999900FFFFFF00CCCC
      CC009999990099999900CCCCCC00FFFFFF00999999009999990099999900CCCC
      CC00FFFFFF00CCCCCC0099999900999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900FFFFFF00CCCCCC009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900CCCCCC00CCCCCC0099999900999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900FFFFFF00999999009999
      9900999999009999990099999900CCCCCC009999990099999900999999009999
      9900CCCCCC00FFFFFF00CCCCCC00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900CCCCCC00999999009999
      9900999999009999990099999900FFFFFF00CCCCCC0099999900999999009999
      990099999900E2EFF100CCCCCC00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900FFCCCC00999999009999
      9900999999009999990099999900CCCCCC00FFFFFF0099999900999999009999
      990099999900E2EFF100CCCCCC00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900CCCCCC00999999009999
      990099999900999999009999990099999900CCCCCC00FFFFFF00999999009999
      990099999900FFFFFF00CCCCCC00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900FFFFFF00999999009999
      990099999900CCCCCC00CCCCCC009999990099999900FFFFFF00999999009999
      990099999900FFFFFF00CCCCCC0099A8AC000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900E2EFF100CCCCCC009999
      990099999900FFFFFF00FFFFFF00CCCCCC00FFFFFF00FFFFFF00999999009999
      9900FFFFFF00CCCCCC0099999900CCCCCC000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900FFFFFF00CCCC
      CC0099999900CCCCCC00CCCCCC00E2EFF100CCCCCC009999990099999900FFCC
      CC00FFFFFF00CCCCCC0099999900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000099999900FFFF
      FF00FFFFFF00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00FFFFFF00FFFF
      FF00CCCCCC0099999900E2EFF100000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009999
      990099999900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00999999009999
      990099999900E2EFF10000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CCCCCC009999990099999900999999009999990099999900CCCCCC00CCCC
      CC00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000248CBF00248CBF00248CBF00248CBF00248CBF00248CBF00248C
      BF00248CBF00248CBF004197BF00000000000000000000000000000000000000
      0000000000007777770077777700777777007777770077777700777777007777
      7700777777007777770085858500000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000E5E5
      E500C0C0C000666666004C4C4C004C4C4C004C4C4C004C4C4C004C4C4C008080
      8000CCCCCC00E2EFF10000000000000000000000000000000000D4A96E00D4AA
      6E00D4A96E0067C1EC005DB9EA00A9FAFF00B3FFFF00B0FFFF00B0FFFF00B0FF
      FF00B5FFFF0093E8FF00248CBF0088BDD80000000000000000009A9A9A009A9A
      9A009A9A9A00AEAEAE00A8A8A800DCDCDC00E1E1E100DFDFDF00DFDFDF00DFDF
      DF00E1E1E100CFCFCF0077777700B3B3B3000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000E5E5E500B2B2
      B200ECC6D900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500C0C0C0004C4C
      4C00646F7100B2B2B200E5E5E500000000000000000000000000D4A96E00FFFF
      FF00FFFFFF0067C1EC0067C1EC0093E7F900B3FFFF00ADFDFF00ADFEFF00ADFE
      FF00B1FFFF0092E7FF0078D3E4003F90BD0000000000000000009A9A9A00FFFF
      FF00FFFFFF00AEAEAE00AEAEAE00CDCDCD00E1E1E100DEDEDE00DEDEDE00DEDE
      DE00E0E0E000CFCFCF00B5B5B500828282000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000E5E5E500C0C0C000E5E5
      E500FFFFFF00E5E5E500CC999900CC999900CCCCCC00E2EFF100FFFFFF00E5E5
      E5008080800066666600B2B2B200E2EFF1000000000000000000D4A96E00FFFF
      FF00EAEAEA0067C1EC0067C1EC0078CFD800B1FDFC00B7FFFF00B6FFFF00B6FF
      FF00BAFFFF0098EAFF00ADFAFF004CAAC60000000000000000009A9A9A00FFFF
      FF00EAEAEA00AEAEAE00AEAEAE00B0B0B000DEDEDE00E2E2E200E2E2E200E2E2
      E200E3E3E300D2D2D200DDDDDD00909090000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000C0C0C0000000
      00000080800000000000000000000000000000000000CCCCCC00E5E5E500FFFF
      FF00CC999900CC660000CC663300CC999900CC663300CC663300CC999900FFFF
      FF00E5E5E500646F7100646F7100CCCCCC000000000000000000D4AA6E00FFFF
      FF00FFFFFF0067C1EC00A5F7FF0067C1EC009DDAF000ADE4F400A8E3F400A9E3
      F400AAE3F4009EE9F800ADE7F70079C6E30000000000000000009A9A9A00FFFF
      FF00FFFFFF00AEAEAE00D9D9D900AEAEAE00CACACA00D4D4D400D2D2D200D3D3
      D300D3D3D300D1D1D100D6D6D600B3B3B3000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000C0C0C0000000
      000000808000000000000000000000000000E5E5E500E5E5E500FFFFFF00CC66
      6600CC330000CC660000CC999900FFFFFF00CC996600CC330000CC330000CC99
      9900FFFFFF00E5E5E5004C4C4C00999999000000000000000000D4A96E00FFFF
      FF00EAEAEA0067C1EC00C5FFFF009FF5FF0067C1EC0067C1EC0092D4EE0090D3
      ED0098D4EE0044A1D1007DB6D600ABD4E80000000000000000009A9A9A00FFFF
      FF00EAEAEA00AEAEAE00E8E8E800D7D7D700AEAEAE00AEAEAE00C4C4C400C3C3
      C300C6C6C6008F8F8F00ACACAC00000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000000000000000
      000000808000000000000000000000000000E5E5E500FFFFFF00CC999900CC33
      0000CC663300CC663300CC663300CC996600CC663300CC663300CC663300CC33
      0000CCCCCC00E5E5E50099999900666666000000000000000000D4AA6E00FFFF
      FF00FFFFFF00C5DBE5008ED4EE00C8FAFD00CCFEFF00ADEBF70067C1EC0067C1
      EC0067C1EC00A4CADB00000000000000000000000000000000009A9A9A00FFFF
      FF00FFFFFF00D6D6D600C2C2C200E7E7E700EAEAEA00D7D7D700AEAEAE00AEAE
      AE00AEAEAE00C2C2C20000000000000000000000000000000000000000000000
      0000008080000080800000808000008080000080800000808000008080000080
      800000808000000000000000000000000000E5E5E500FFFFFF00CC663300CC66
      3300CC663300CC663300CC996600E5E5E500CC663300CC663300CC663300CC66
      3300CC666600FFFFFF00CCCCCC004C4C4C000000000000000000D4A96E00FFFF
      FF00EBEBEB00FFFFFF00C0D7E10067C1EC0067C1EC0067C1EC00D7E4D500EAEA
      EA00FFFFFF00D4A96E00000000000000000000000000000000009A9A9A00FFFF
      FF00EBEBEB00FFFFFF00D2D2D200AEAEAE00AEAEAE00AEAEAE00DBDBDB00EAEA
      EA00FFFFFF009A9A9A0000000000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000000000000080
      800000808000000000000000000000000000E5E5E500E5E5E500CC663300CC66
      3300CC663300CC663300CC996600FFFFFF00FF999900CC330000CC663300CC66
      3300CC663300E2EFF100E5E5E5004C4C4C000000000000000000D4A96E00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00D4A96E00000000000000000000000000000000009A9A9A00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF009A9A9A0000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      000000808000000000000000000000000000E5E5E500FFCCCC00CC663300CC66
      3300CC663300CC663300CC663300CCCC9900FFFFFF00CC996600CC663300CC66
      3300CC663300E2EFF100E5E5E5004C4C4C000000000000000000D4AA6E00FFFF
      FF00EAEAEA00EAEAEA00EAEAEA00EAEAEA00EAEAEA00EAEAEA00EBEBEB00EAEA
      EA00FFFFFF00D4A96D00000000000000000000000000000000009A9A9A00FFFF
      FF00EAEAEA00EAEAEA00EAEAEA00EAEAEA00EAEAEA00EAEAEA00EBEBEB00EAEA
      EA00FFFFFF009A9A9A0000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      000000808000000000000000000000000000E5E5E500E5E5E500CC663300CC66
      3300CC663300CC663300CC663300CC330000CCCCCC00FFFFFF00CC663300CC66
      3300CC663300FFFFFF00E5E5E500666666000000000000000000D4A96D00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00D4AA6E00000000000000000000000000000000009A9A9A00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF009A9A9A0000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      000000000000000000000000000000000000E5E5E500FFFFFF00FF996600CC66
      3300CC996600E5E5E500CC666600CC330000CC996600FFFFFF00CC996600CC66
      0000CC996600FFFFFF00CCCCCC0099A8AC000000000000000000D4AA6E00FFFF
      FF00EAEAEA00EAEAEA00EAEAEA00EAEAEA00EAEAEA00FFFFFF00D4A96E00D4A9
      6D00D4A96E00D4A96E00000000000000000000000000000000009A9A9A00FFFF
      FF00EAEAEA00EAEAEA00EAEAEA00EAEAEA00EAEAEA00FFFFFF009A9A9A009A9A
      9A009A9A9A009A9A9A0000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000C0C0C000000000000000000000000000E5E5E500E2EFF100FFFFCC00FF99
      3300CC996600FFFFFF00FFFFFF00FFCC9900FFFFFF00FFFFFF00CC663300FF66
      3300FFFFFF00E5E5E50099999900E5E5E5000000000000000000D4AA6E00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00D4A96E00EAEA
      EA00D4A96E0000000000000000000000000000000000000000009A9A9A00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009A9A9A00EAEA
      EA009A9A9A000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000E5E5E500FFFFFF00F2EA
      BF00FF996600FF999900E5E5E500E2EFF100E5E5E500FF996600FF996600FFCC
      CC00FFFFFF00F2EABF00C0C0C000000000000000000000000000D4A96E00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00D4A96D00D4A9
      6E000000000000000000000000000000000000000000000000009A9A9A00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009A9A9A009A9A
      9A00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000E5E5E500E5E5E500FFFF
      FF00FFFFFF00FFFFCC00FFCC9900FFCC9900FFCC9900FFCC9900FFFFFF00FFFF
      FF00E5E5E500C0C0C000E2EFF100000000000000000000000000D4A96E00D4A9
      6E00D4A96E00D4A96E00D4A96E00D4A96E00D4A96E00D4A96E00D4A96D000000
      00000000000000000000000000000000000000000000000000009A9A9A009A9A
      9A009A9A9A009A9A9A009A9A9A009A9A9A009A9A9A009A9A9A009A9A9A000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000E5E5E500FFCC
      CC00E5E5E500FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00E5E5E500FFCC
      CC00CCCCCC00E2EFF10000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000E5E5E500FFCCCC00FFCCCC00FFCCCC00F2EABF00FFCCCC00E5E5E500E5E5
      E500000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000248CBF00248CBF00248CBF00248CBF00248CBF00248CBF00248C
      BF00248CBF00248CBF004197BF00000000000000000000000000000000000000
      0000000000007777770077777700777777007777770077777700777777007777
      7700777777007777770085858500000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000E1ACA400DA968B00D994
      8B00D8938B0067C1EC005DB9EA00A9FAFF00B3FFFF00B0FFFF00B0FFFF00B0FF
      FF00B5FFFF0093E8FF00248CBF0088BDD80000000000B3B3B3009F9F9F009E9E
      9E009E9E9E00AEAEAE00A8A8A800DCDCDC00E1E1E100DFDFDF00DFDFDF00DFDF
      DF00E1E1E100CFCFCF0077777700B3B3B3000000000000000000000000000000
      0000CC9966009933000099330000CC9966000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000B2B2B2009999990099999900B2B2B2000000000000000000000000000000
      00000000000000000000000000000000000000000000D65C2B00F0B08600F0B1
      8C00EFAF8A0067C1EC0067C1EC0093E7F900B3FFFF00ADFDFF00ADFEFF00ADFE
      FF00B1FFFF0092E7FF0078D3E4003F90BD000000000061616100ACACAC00AFAF
      AF00ADADAD00AEAEAE00AEAEAE00CDCDCD00E1E1E100DEDEDE00DEDEDE00DEDE
      DE00E0E0E000CFCFCF00B5B5B500828282000000000000000000000000009933
      000099330000CC660000CC660000993300000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009999
      990099999900CCCCCC00CCCCCC00999999000000000000000000000000000000
      00000000000000000000000000000000000000000000E4744B00FEFBF700FFFF
      FF00FFFFFF0067C1EC0067C1EC0078CFD800B1FDFC00B7FFFF00B6FFFF00B6FF
      FF00BAFFFF0098EAFF00ADFAFF004CAAC600000000007A7A7A00FAFAFA00FFFF
      FF00FFFFFF00AEAEAE00AEAEAE00B0B0B000DEDEDE00E2E2E200E2E2E200E2E2
      E200E3E3E300D2D2D200DDDDDD0090909000000000000000000099330000CC66
      0000CC6600009933000099330000CC9966000000000000000000000000000000
      000000000000000000000000000000000000000000000000000099999900CCCC
      CC00CCCCCC009999990099999900B2B2B2000000000000000000000000000000
      00000000000000000000000000000000000000000000E4744B00FCF4EC00FFFF
      FF00FFFFFF0067C1EC00A5F7FF0067C1EC009DDAF000ADE4F400A8E3F400A9E3
      F400AAE3F4009EE9F800ADE7F70079C6E300000000007A7A7A00F2F2F200FFFF
      FF00FFFFFF00AEAEAE00D9D9D900AEAEAE00CACACA00D4D4D400D2D2D200D3D3
      D300D3D3D300D1D1D100D6D6D600B3B3B30000000000CC99660099330000CC66
      000099330000CC99660000000000000000009933000099330000993300009933
      00009933000099330000993300000000000000000000B2B2B20099999900CCCC
      CC0099999900B2B2B20000000000000000009999990099999900999999009999
      99009999990099999900999999000000000000000000E4744B00FCF5ED00FFFF
      FF00FFFFFF0067C1EC00C5FFFF009FF5FF0067C1EC0067C1EC0092D4EE0090D3
      ED0098D4EE0044A1D1007DB6D600ABD4E800000000007A7A7A00F3F3F300FFFF
      FF00FFFFFF00AEAEAE00E8E8E800D7D7D700AEAEAE00AEAEAE00C4C4C400C3C3
      C300C6C6C6008F8F8F00ACACAC00000000000000000099330000CC6600009933
      0000CC99660000000000000000000000000099330000CC660000CC660000CC66
      0000CC660000CC66000099330000000000000000000099999900CCCCCC009999
      9900B2B2B20000000000000000000000000099999900CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00999999000000000000000000E4744B00FCF4ED00FFFF
      FF00FFFFFF00C4DBE4008ED4EE00C8FAFD00CCFEFF00ADEBF70067C1EC0067C1
      EC0067C1EC00A4CADB00E4744B0000000000000000007A7A7A00F3F3F300FFFF
      FF00FFFFFF00D5D5D500C2C2C200E7E7E700EAEAEA00D7D7D700AEAEAE00AEAE
      AE00AEAEAE00C2C2C2007A7A7A00000000000000000099330000CC6600009933
      0000000000000000000000000000000000000000000099330000CC660000CC66
      0000CC660000CC66000099330000000000000000000099999900CCCCCC009999
      9900000000000000000000000000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00999999000000000000000000E4744B00FCF5EE00FFFF
      FF00FFFFFF00FFFFFF00C0D7E10067C1EC0067C1EC0067C1EC00D7E4D500FFFF
      FF00FFFFFF00FDF7F100E4744B0000000000000000007A7A7A00F4F4F400FFFF
      FF00FFFFFF00FFFFFF00D2D2D200AEAEAE00AEAEAE00AEAEAE00DBDBDB00FFFF
      FF00FFFFFF00F6F6F6007A7A7A00000000000000000099330000CC6600009933
      0000CC99660000000000000000000000000000000000CC99660099330000CC66
      0000CC660000CC66000099330000000000000000000099999900CCCCCC009999
      9900B2B2B20000000000000000000000000000000000B2B2B20099999900CCCC
      CC00CCCCCC00CCCCCC00999999000000000000000000E4744B00FDF5EF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FDF7F100E4744B0000000000000000007A7A7A00F4F4F400FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00F6F6F6007A7A7A000000000000000000CC99660099330000CC66
      000099330000CC99660000000000CC9966009933000099330000CC6600009933
      0000CC660000CC660000993300000000000000000000B2B2B20099999900CCCC
      CC0099999900B2B2B20000000000B2B2B2009999990099999900CCCCCC009999
      9900CCCCCC00CCCCCC00999999000000000000000000E4744B00FDF7F100FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FDF7F100E4744B0000000000000000007A7A7A00F6F6F600FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00F6F6F6007A7A7A0000000000000000000000000099330000CC66
      0000CC660000993300009933000099330000CC660000CC66000099330000CC99
      660099330000CC6600009933000000000000000000000000000099999900CCCC
      CC00CCCCCC00999999009999990099999900CCCCCC00CCCCCC0099999900B2B2
      B20099999900CCCCCC00999999000000000000000000E4744B00FDF7F100F8FA
      FE00F8F9FD00F8F9FD00F7F9FD00F8FAFE00F8FAFE00F7FAFE00F8FAFF00F8FA
      FF00F8FEFF00FDF7F100E4744B0000000000000000007A7A7A00F6F6F600FBFB
      FB00FAFAFA00FAFAFA00FAFAFA00FBFBFB00FBFBFB00FAFAFA00FBFBFB00FBFB
      FB00FCFCFC00F6F6F6007A7A7A00000000000000000000000000000000009933
      000099330000CC660000CC660000CC6600009933000099330000000000000000
      0000000000009933000099330000000000000000000000000000000000009999
      990099999900CCCCCC00CCCCCC00CCCCCC009999990099999900000000000000
      00000000000099999900999999000000000000000000E4744B00E4744B00E474
      4B00E4744B00E4744B00E4744B00E4744B00E4744B00E4744B00E4744B00E474
      4B00E4744B00E4744B00E4744B0000000000000000007A7A7A007A7A7A007A7A
      7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A
      7A007A7A7A007A7A7A007A7A7A00000000000000000000000000000000000000
      0000CC996600993300009933000099330000CC99660000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000B2B2B200999999009999990099999900B2B2B20000000000000000000000
      00000000000000000000000000000000000000000000E4744B00FDF7F100FDF7
      F100FDF7F100FDF7F100E4744B00E4744B00E4744B00E4744B00E4744B00E474
      4B00E4744B00E4744B00E4744B0000000000000000007A7A7A00F6F6F600F6F6
      F600F6F6F600F6F6F6007A7A7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A
      7A007A7A7A007A7A7A007A7A7A00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000E4744B00FDF7F100FDF7
      F100FDF7F100FDF7F100E4744B00E4744B00E4744B00E4744B00E4744B00E474
      4B00E4744B00E4744B00E4744B0000000000000000007A7A7A00F6F6F600F6F6
      F600F6F6F600F6F6F6007A7A7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A
      7A007A7A7A007A7A7A007A7A7A00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000E4744B00E4744B00E474
      4B00E4744B00E4744B00E4744B00E4744B00E4744B00E4744B00E4744B00E474
      4B00E4744B00E4744B00E4744B0000000000000000007A7A7A007A7A7A007A7A
      7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A7A007A7A
      7A007A7A7A007A7A7A007A7A7A00000000000000000000000000000000000000
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
      00000000000000000000000000000000000000000000B6B6B600111111001111
      1100B8B8B700000000000000000000000000000000000000000000000000B6B6
      B6001111110011111100B8B8B700000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000B6B6
      B6001111110011111100B8B8B700000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00009999CC0000008000000080000000800000008000000080009999CC000000
      0000000000000000000000000000000000000000000011111100CBCBCB00C9C9
      C900111111000000000000000000000000000000000000000000000000001111
      1100111111001111110011111100000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000001111
      1100CBCBCB00CBCBCB0011111100000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      80000000CC000000FF000000FF000000FF000000FF000000FF000000CC000000
      8000000000000000000000000000000000000000000011111100CDCDCD00CCCC
      CC00111111000000000011111100000000001111110000000000111111001111
      1100111111001111110011111100000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000001111
      1100CBCBCB00CBCBCB0011111100000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000080000000
      FF000000FF0000008000000080000000800000008000000080000000FF000000
      FF000000800000000000000000000000000000000000B4B4B400111111001111
      1100B6B6B600000000000000000000000000000000000000000000000000B4B4
      B4001111110011111100B6B6B600000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000B4B4
      B4001111110011111100B6B6B600000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000080000000FF000000
      FF0000008000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000080000000
      FF000000FF000000800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000277FFF00277F
      FF00277FFF00277FFF00277FFF00277FFF00277FFF00277FFF00277FFF00277F
      FF00277FFF00277FFF0000000000000000009999CC000000CC000000FF000000
      8000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      80000000FF000000CC009999CC00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000F4F4F40000000000000000000000000000000000000000000000
      000000000000000000000000000000000000B4B4B40000000000000000000000
      0000000000000000000000000000000000000000000000000000277FFF00277F
      FF00000000000000000000000000000000000000000000000000000000000000
      0000277FFF00277FFF000000000000000000000080000000FF0000008000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000080000000FF0000008000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000F1F1F10000000000000000000000000000000000000000000000
      0000000000000000000000000000B4B4B40000000000B4B4B400000000000000
      0000000000000000000000000000000000000000000000000000277FFF00277F
      FF00000000000000000000000000000000000000000000000000000000000000
      0000277FFF00277FFF000000000000000000000080000000FF0000008000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000080000000FF0000008000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000011111100F1F1F10000000000000000000000000000000000000000000000
      00000000000000000000B4B4B40000000000B4B4B40000000000000000000000
      0000000000000000000000000000000000000000000000000000277FFF00277F
      FF00277FFF00277FFF00277FFF00277FFF00277FFF00277FFF00277FFF00277F
      FF00277FFF00277FFF000000000000000000000080000000FF0000008000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000080000000FF0000008000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000011111100F1F1F10000000000000000000000000000000000000000000000
      000000000000B4B4B40000000000B4B4B4000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000080000000FF0000008000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000080000000FF0000008000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000011111100F1F1F10000000000000000000000000000000000000000000000
      0000B4B4B40000000000B4B4B400000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000080000000FF0000008000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000080000000FF0000008000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000011111100F1F1F1000000000000000000000000000000000000000000B4B4
      B40000000000B4B4B40000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000009999CC000000CC000000FF000000
      8000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      80000000FF000000CC009999CC00000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000011111100DBDB
      DB0011111100B7B7B70011111100000000000000000000000000B4B4B4000000
      0000B4B4B4000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000080000000FF000000
      FF0000008000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000080000000
      FF000000FF000000800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000ABABAB001111
      11001111110011111100C4C4C40000000000000000000000000000000000B4B4
      B400000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000080000000
      FF000000FF0000008000000080000000800000008000000080000000FF000000
      FF00000080000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000B1B1
      B10011111100ADADAD0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      80000000CC000000FF000000FF000000FF000000FF000000FF000000CC000000
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FCFCFC000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00009999CC0000008000000080000000800000008000000080009999CC000000
      000000000000000000000000000000000000000000003399CC00006699000066
      9900006699000066990000669900006699000066990000669900006699000066
      990066CCCC00000000000000000000000000000000000000000000000000515F
      3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A00515F3A00515F3A00515F3A00515F3A00000000000000000000000000CC99
      6600CC996600CC996600CC996600CC996600CC996600CC996600CC996600CC99
      6600CC996600CC996600CC996600CC9966000000000000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999009999990099999900999999003399CC003399CC0099FFFF0066CC
      FF0066CCFF0066CCFF0066CCFF0066CCFF0066CCFF0066CCFF0066CCFF003399
      CC00006699000000000000000000000000000000000000000000515F3A006C7F
      4D006C7F4D00515F3A00E6EBDF006C7F4D00515F3A00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A00000099000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000999999003399CC003399CC0066CCFF0099FF
      FF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0066CC
      FF00006699003399CC0000000000000000000000000000000000515F3A006C7F
      4D006C7F4D00515F3A00E6EBDF006C7F4D00515F3A00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A000000CC000000990000000000CC99
      6600FFFFFF009933000099330000993300009933000099330000993300009933
      00009933000099330000FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000999999003399CC003399CC0066CCFF0099FF
      FF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0066CC
      FF0066CCCC000066990000000000000000000000000000000000515F3A006C7F
      4D006C7F4D00515F3A00E6EBDF006C7F4D00515F3A00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A000000FF000000CC0000009900CC99
      66000000000000000000CC996600CC996600CC996600CC996600CC996600CC99
      6600CC99660099330000FFFFFF00CC9966000000000000000000000000009999
      99000000000000000000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC000000000000000000999999003399CC0066CCFF003399CC0099FF
      FF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0066CC
      FF0099FFFF00006699003399CC00000000000000000000000000515F3A006C7F
      4D006C7F4D00515F3A00E6EBDF00E6EBDF00E6EBDF00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A000000FF000000CC00000000000000
      0000FFFFFF008080800000000000993300009933000099330000993300009933
      00009933000099330000FFFFFF00CC9966009999990000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000999999003399CC0066CCFF0066CCCC0066CC
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0066CC
      FF0099FFFF0066CCCC0000669900000000000000000000000000515F3A006C7F
      4D006C7F4D006C7F4D00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A006C7F4D006C7F4D006C7F4D00515F3A000000FF0000000000000000000000
      000080808000FFFFFF008080800000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC996600B2B2B20099999900000000009999
      9900000000009999990099999900999999009999990099999900999999009999
      9900999999009999990000000000999999003399CC0099FFFF0066CCFF003399
      CC00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF0099FF
      FF00CCFFFF00CCFFFF000066990000000000000000005CADD60010658600166B
      8A00166B8A00166B8A00166B8A00166B8A00166B8A00166B8A00166B8A00166B
      8A0067BDB3006C7F4D006C7F4D00515F3A00000000000000000000000000CC99
      66000000000080808000FFFFFF000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC996600CCCCCC00B2B2B200999999009999
      99000000000099999900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC009999990000000000999999003399CC0099FFFF0099FFFF00CB7C
      2A00CB7C2A00CB7C2A00CB7C2A003399CC003399CC003399CC003399CC003399
      CC003399CC003399CC0066CCFF00000000005CADD6005CADD6008BDFD80067BD
      DB0067BDDB0062B6D80062B6D80062B6D80062B6D80062B6D80062B6D800398D
      AF00106586006C7F4D006C7F4D00515F3A00000000000000000000000000CC99
      6600FFFFFF000000000080808000FFFFFF000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00CC996600CCCCCC00B2B2B200000000009999
      9900000000009999990099999900999999009999990099999900999999009999
      9900999999009999990000000000999999003399CC00CB7C2A00CB7C2A00FEBF
      8300FFFFFF00FFFFFF00FEBF8300CB7C2A00CB7C2A00CCFFFF00CCFFFF000066
      9900000000000000000000000000000000005CADD6005CADD60062B6D80090E5
      DB008BDFD800ADFFFF00ADFFFF00ADFFFF00ADFFFF00ADFFFF00ADFFFF0085D6
      FF003385AD00398DAF006C7F4D00515F3A00000000000000000000000000CC99
      6600FFFFFF00FFFFFF00000000008080800000000000FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF00FFFFFF00CC996600CCCCCC0000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CB7C2A00FE9A3700FEA54D00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFA54D00FE9A3700CB7C2A003399CC000000
      0000000000000000000000000000000000005CADD6005CADD60062B6D80090E5
      DB008BDFD800ADFFFF008BDFD8008BDFD8008BDFD8008BDFD8008BDFD80062B6
      D80085D6D600106586006C7F4D00515F3A00000000000000000000000000CC99
      6600FFFFFF00FFFFFF00CCCCCC0000000000CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC0000000000FFFFFF00CC9966000000000000000000000000009999
      99000000000000000000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00000000000000000099999900CB7C2A00FFFFFF00FFFFFF00FF9F
      4000FF9F4000FF9E3F00FF9F4000FFFFFF00FFFFFF00CB7C2A00000000000000
      0000000000000000000000000000000000005CADD60085D6FF00398DAF0090E5
      DB008BDFD800ADFFFF00ADFFFF00ADFFFF00ADFFFF00ADFFFF00ADFFFF0085D6
      FF00ADFFFF00106586003E94B300515F3A00000000000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0000000000FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CB7C2A00FFFFFF00FFFFFF00FFFF
      FF00F29C3700F29C3700FFFFFF00FFFFFF00FFFFFF00CB7C2A00000000000000
      0000000000000000000000000000000000005CADD60085D6FF0062B6AF0080D2
      D0008BDFD800ADFFFF008BDFD8008BDFD8008BDFD8008BDFD8008BDFD80062B6
      D800ADFFFF0062B6AF0010658600515F3A00000000000000000000000000CC99
      6600FFFFFF00FFFFFF00CCCCCC0000000000CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC0000000000FFFFFF00CC9966000000000000000000000000009999
      99000000000000000000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00000000000000000099999900CB7C2A00FFFFFF00FFFFFF00FFFF
      FF00F29C3700F29C3700FFFFFF00FFFFFF00FFFFFF00CB7C2A00000000000000
      0000000000000000000000000000000000005CADD600ADFFFF0062B6D8003E94
      B300B3DFD800D6FFFF00D6FFFF00D6FFFF00D6FFFF00D6FFFF00D6FFFF00ADFF
      FF00D6FFFF00B3DFD800166B8A00515F3A00000000000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CB7C2A00FFFFFF00FFFFFF00FFFF
      FF00F29C3700F29C3700FFFFFF00FFFFFF00FFFFFF00CB7C2A00000000000000
      0000000000000000000000000000000000005CADD600ADFFFF008BDFD8008BDF
      D800398DAF00398DAF00398DAF00398DAF00398DAF00398DAF00398DAF00398D
      AF00398DAF00398DAF0062B6D800515F3A00000000000000000000000000CC99
      6600FFFFFF00FFFFFF00CCCCCC00CCCCCC00CCCCCC0000000000000000000000
      0000CCCCCC00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      99000000000000000000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC0000000000000000009999990000000000CB7C2A00CB7C2A00FFFF
      FF00F29C3700F29C3700FFFFFF00CB7C2A00CB7C2A0000000000000000000000
      0000000000000000000000000000000000005CADD600D6FFFF00D6FFFF00D6FF
      FF00D6FFFF00D6FFFF00D6FFFF00D6FFFF00D6FFFF00D6FFFF00D6FFFF003385
      AD0000000000000000000000000000000000000000000000000000000000CC99
      6600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00CC9966000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000999999000000000000000000CB7C2A00CB7C
      2A00CB7C2A00CB7C2A00CB7C2A00CB7C2A000000000000000000000000000000
      0000000000000000000000000000000000005CADD6005CADD6005CADD6005CAD
      D6005CADD6005CADD6005CADD6005CADD6005CADD6005CADD6005CADD6000000
      000000000000000000000000000000000000000000000000000000000000CC99
      6600CC996600CC996600CC996600CC996600CC996600CC996600CC996600CC99
      6600CC996600CC996600CC996600CC9966000000000000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999009999990099999900999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000515F
      3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A00515F3A00515F3A00515F3A00515F3A000000000000000000000000000000
      0000000000009999990099999900999999009999990099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000993300000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000999999000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000515F
      3A006C7F4D00515F3A00E6EBDF006C7F4D00515F3A00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000099330000CC6600009933000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000099999900CCCCCC009999990000000000000000000000
      000000000000000000000000000000000000000000000000000000000000515F
      3A006C7F4D00515F3A00E6EBDF006C7F4D00515F3A00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      000000000000000000000000000000000000000000000000000000000000515F
      3A006C7F4D00515F3A00E6EBDF006C7F4D00515F3A00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000099330000CC660000CC660000CC660000CC660000CC660000993300000000
      0000000000000000000000000000000000000000000000000000000000000000
      000099999900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00999999000000
      000000000000000000000000000000000000000000000000000000000000515F
      3A006C7F4D00515F3A00E6EBDF00E6EBDF00E6EBDF00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC6600009933
      0000000000000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      990000000000000000000000000000000000000000000000000000000000515F
      3A006C7F4D006C7F4D00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A006C7F4D006C7F4D006C7F4D00515F3A000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      000000000000000000000000000000000000000000000000000099330000CC66
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      000099330000000000000000000000000000000000000000000099999900CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC0099999900000000000000000000000000000000000000000000000000515F
      3A006C7F4D006C7F4D006C7F4D006C7F4D006C7F4D006C7F4D006C7F4D006C7F
      4D006C7F4D006C7F4D006C7F4D00515F3A000000000000000000999999009999
      99009999990099999900CCCCCC00CCCCCC00CCCCCC0099999900999999009999
      990099999900000000000000000000000000000000000000000099330000CC66
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      000099330000000000000000000000000000000000000000000099999900CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC0099999900000000000000000000000000000000000000000000000000CB7C
      2A00CB7C2A00CB7C2A00CB7C2A00515F3A00515F3A00515F3A00515F3A00515F
      3A00515F3A006C7F4D006C7F4D00515F3A00000000000000000099999900CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000993300009933
      00009933000099330000CC660000CC660000CC66000099330000993300009933
      0000993300000000000000000000000000000000000000000000999999009999
      99009999990099999900CCCCCC00CCCCCC00CCCCCC0099999900999999009999
      99009999990000000000000000000000000000000000CB7C2A00CB7C2A00FEBF
      8300FFFFFF00FFFFFF00FEBF8300CB7C2A00CB7C2A00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00515F3A006C7F4D00515F3A00000000000000000099999900CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      000000000000000000000000000000000000CB7C2A00FE9A3700FEA54D00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFA54D00FE9A3700CB7C2A00515F3A00515F
      3A00FFFFFF00515F3A006C7F4D00515F3A000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      9900000000000000000000000000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      000000000000000000000000000000000000CB7C2A00FFFFFF00FFFFFF00FF9F
      4000FF9F4000FF9E3F00FF9F4000FFFFFF00FFFFFF00CB7C2A00FFFFFF00FFFF
      FF00FFFFFF00515F3A006C7F4D00515F3A000000000000000000000000000000
      000099999900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      000000000000000000000000000000000000CB7C2A00FFFFFF00FFFFFF00FFFF
      FF00F29C3700F29C3700FFFFFF00FFFFFF00FFFFFF00CB7C2A00515F3A00515F
      3A00FFFFFF00515F3A006C7F4D00515F3A000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      000000000000000000000000000000000000CB7C2A00FFFFFF00FFFFFF00FFFF
      FF00F29C3700F29C3700FFFFFF00FFFFFF00FFFFFF00CB7C2A00FFFFFF00FFFF
      FF00FFFFFF00515F3A006C7F4D00515F3A000000000000000000000000000000
      0000000000000000000099999900CCCCCC009999990000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC00CCCCCC00CCCCCC0099999900000000000000
      000000000000000000000000000000000000CB7C2A00FFFFFF00FFFFFF00FFFF
      FF00F29C3700F29C3700FFFFFF00FFFFFF00FFFFFF00CB7C2A00515F3A00515F
      3A00515F3A00515F3A00515F3A00515F3A000000000000000000000000000000
      0000000000000000000000000000999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009933000099330000993300009933000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009999990099999900999999009999990099999900000000000000
      00000000000000000000000000000000000000000000CB7C2A00CB7C2A00FFFF
      FF00F29C3700F29C3700FFFFFF00CB7C2A00CB7C2A0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000CB7C2A00CB7C
      2A00CB7C2A00CB7C2A00CB7C2A00CB7C2A000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099330000993300000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000008B49000E9054000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000993300000000FF009933
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009933000099330000993300009933000099330000000000000000
      000000000000000000000000000000000000000000000000000000000000CC66
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000CC6600000000000000000000000000000000000000000000000000000000
      00000000000000874300A1E2D500239A60000000000000000000000000000000
      000000000000000000000000000000000000CC996600CC996600CC996600CC99
      6600CC996600CC996600CC996600CC996600CC996600993300000000FF000000
      FF00993300000000000000000000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000087430093DCC9001ACAAD0000B68E000096580000975B00008B4A000089
      450046A17600000000000000000000000000CC996600FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00993300000000FF000000
      FF000000FF009933000000000000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000008A
      480081DBC20014CEA90000C49900000000000000000056DDC40056DEC50056DC
      C40044C19A00008B4A000000000000000000CC996600FFFFFF00E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500993300000000FF000000
      FF000000FF000000FF0099330000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000008A
      48006FD7B800000000000000000000FFFF0000FFFF00000000000000000000D1
      A0001DD8AE002FCCA300018A490000000000CC996600FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00993300000000FF000000
      FF000000FF000000FF000000FF00993300000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      000000000000000000000000000000000000000000000000000000B88D00014E
      2D00014E2D00014E2D00014E2D00014E2D00014E2D00014E2D00014E2D00014E
      2D00014E2D0000B88D0000000000000000000000000000000000000000000000
      0000008744000000000000FFFF0000FFFF0000FFFF0000FFFF000000000000A5
      680000C48A0004DDA80016BA830055A78200CC996600FFFFFF00E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500993300000000FF000000
      FF000000FF000000FF0099330000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000014E2D004CDF
      C1004CDFC1004CDFC1004CDFC1004CDFC1004CDFC1004CDFC1004CDFC1004CDF
      C1004CDFC100014E2D0000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF000000
      00000E8F510000B5780000D89F00008B4B00CC996600FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00993300000000FF000000
      FF000000FF009933000000000000000000000000000000000000993300009933
      00009933000099330000CC660000CC660000CC66000099330000993300009933
      0000993300000000000000000000000000000000000000000000014E2D004CDF
      C1004CDFC1004CDFC100014E2D0000B1760000B17600014E2D004CDFC1004CDF
      C1004CDFC100014E2D00000000000000000000000000008D4D00008B4B0066AC
      8C000000000000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF000000
      00000000000056B38800008B4B00008D4D00CC996600FFFFFF00E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500993300000000FF000000
      FF0099330000000000000000000000000000000000000000000099330000CC66
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000014E2D004CDF
      C1004CDFC100014E2D0000714C0000BF8A0000BF8A0000714C00014E2D004CDF
      C1004CDFC100014E2D00000000000000000000000000008B4B0000D89F0000B5
      78001A9158000000000000FFFF0000FFFF0000FFFF0000FFFF00000000000088
      460000000000000000000000000000000000CC996600FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00993300000000FF009933
      000000000000000000000000000000000000000000000000000099330000CC66
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000014E2D004CDF
      C100014E2D0000754F0000C2920000D1A00000CE990000D1950000976400014E
      2D004CDFC100014E2D000000000000000000000000006CBC97001ABA860007DB
      A80000BE8500000000000000000000FFFF0000FFFF00000000000000000063D3
      AF0000874400000000000000000000000000CC996600FFFFFF00E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E5009933000099330000CC99
      6600000000000000000000000000000000000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC6600009933
      0000000000000000000000000000000000000000000000000000014E2D004CDF
      C100014E2D0000764F0000BF920000CE9F0000C99B0000D1A10000996F00014E
      2D004CDFC100014E2D0000000000000000000000000000000000008A490035D0
      AA0020D7B1000DD3A7000DD4A70000000000000000000ED1A40000CA9A0013D3
      A90073D7BB00008A48000000000000000000CC996600FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC99
      6600000000000000000000000000000000000000000000000000000000000000
      000099330000CC660000CC660000CC660000CC660000CC660000993300000000
      0000000000000000000000000000000000000000000000000000014E2D004CDF
      C1004CDFC100014E2D0000724C0000B88D0000CDA20000936A00014E2D004CDF
      C1004CDFC100014E2D000000000000000000000000000000000000000000018B
      490048C29C005CDCC6005BDEC8005ADEC7005BDDC6005CDBC30000C3990015CC
      AA0085DBC300008A48000000000000000000CC996600FFFFFF00E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500FFFFFF00CC99
      6600000000000000000000000000000000000000000000000000000000000000
      00000000000099330000CC660000CC660000CC66000099330000000000000000
      0000000000000000000000000000000000000000000000000000014E2D004CDF
      C1004CDFC1004CDFC100014E2D0000A67E0000C99F00014E2D004CDFC1004CDF
      C1004CDFC100014E2D0000000000000000000000000000000000000000000000
      000056B186000089460000884400008744000089470000B58E001BC8AE0098DE
      CB0000874200000000000000000000000000CC996600FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000000000000000000099330000CC6600009933000000000000000000000000
      0000000000000000000000000000000000000000000000000000014E2D004CDF
      C1004CDFC1004CDFC1004CDFC1004CDFC1004CDFC1004CDFC1004CDFC1004CDF
      C1004CDFC100014E2D0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000239B6100A1E2D5000087
      430000000000000000000000000000000000CC996600FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000993300000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000B88D00014E
      2D00014E2D00014E2D00014E2D00014E2D00014E2D00014E2D00014E2D00014E
      2D00014E2D0000B88D0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000012945800008B49000000
      000000000000000000000000000000000000CC996600CC996600CC996600CC99
      6600CC996600CC996600CC996600CC996600CC996600CC996600CC996600CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000CC996600CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000999999009999
      9900000000000000000000000000000000000000000000000000000000009696
      9600000000000000000000000000000000000000000000000000000000000000
      0000969696000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009999
      9900999999009999990000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000CC996600FFFFFF00FFFF
      FF00CC9966000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900000000000000
      0000999999000000000000000000000000000000000000000000000000009696
      9600969696009696960096969600969696009696960096969600969696009696
      9600969696000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000099999900CCCC
      CC00CCCCCC000000000099999900000000000000000000000000000000000000
      00000000000000000000000000000000000000000000CC996600FFFFFF00FFFF
      FF00CC9966000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900000000000000
      0000999999000000000000000000000000000000000000000000000000009696
      9600969696009696960096969600969696009696960096969600969696009696
      9600969696000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900CCCCCC009999
      9900000000000000000099999900000000000000000000000000000000000000
      0000CC996600000000000000000000000000CC996600FFFFFF00FFFFFF00CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000999999000000000000000000000000009999990000000000000000009999
      990000000000000000000000000000000000A9490D00A9490D00A9490D00A949
      0D00A9490D00A9490D00A9490D00A9490D00A9490D00A9490D00A9490D00A949
      0D00A9490D00A9490D00A9490D00A9490D000000000000000000000000000000
      00000000000000000000000000000000000099999900CCCCCC00999999000000
      0000CCCCCC000000000099999900000000000000000000000000000000000000
      0000CC996600CC9966000000000000000000CC996600FFFFFF00FFFFFF00CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000999999009999990000000000000000009999990000000000000000009999
      990000000000000000000000000000000000FDD3B900FDD3B900FDD3B900FDD3
      B900FDD3B900FDD3B900FDD3B900FDD3B900FDD3B900FDD3B900FDD3B900FDD3
      B900FDD3B900FDD3B900FDD3B900FDD3B9000000000000000000000000000000
      000000000000000000009999990099999900CCCCCC009999990000000000CCCC
      CC00000000009999990000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00CC996600CC996600FFFFFF00FFFFFF00CC9966000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000999999000000000099999900999999000000000000000000999999000000
      000000000000000000000000000000000000FDD3B900471E0500FDD3B900FDD3
      B900471E0500FDD3B900471E0500471E0500471E0500471E0500FDD3B900FDD3
      B900471E0500471E0500471E0500FDD3B9000000000000000000999999009999
      99009999990099999900CCCCCC00CCCCCC009999990000000000CCCCCC000000
      0000999999000000000000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC9966000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000999999000000000000000000000000000000000000000000999999000000
      000000000000000000000000000000000000FDD3B900471E0500FDD3B900FDD3
      B900471E0500FDD3B900471E0500FDD3B900FDD3B900FDD3B900FDD3B900471E
      0500FDD3B900FDD3B900FDD3B900471E05000000000099999900000000000000
      00000000000000000000999999009999990000000000CCCCCC00000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC996600CC99
      6600CC996600CC99660000000000000000000000000000000000000000000000
      0000999999000000000000000000000000000000000000000000999999009999
      990099999900999999000000000000000000FDD3B900471E0500FDD3B900471E
      0500FDD3B900FDD3B900471E0500FDD3B900FDD3B900FDD3B900FDD3B900471E
      0500FDD3B900FDD3B900471E0500471E0500999999000000000000000000CCCC
      CC0000000000000000000000000000000000CCCCCC0000000000999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00CC9966000000000000000000000000000000000000000000000000000000
      0000999999000000000000000000000000000000000000000000000000000000
      000099999900000000000000000000000000FDD3B900471E0500471E0500471E
      0500471E0500FDD3B900471E0500471E0500471E0500471E0500FDD3B900471E
      0500FDD3B900FDD3B900FDD3B900FDD3B9009999990000000000CCCCCC000000
      0000CCCCCC000000000000000000000000000000000099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000999999000000000000000000000000000000000000000000000000009999
      990000000000000000000000000000000000FDD3B900471E0500FDD3B900FDD3
      B900471E0500FDD3B900471E0500FDD3B900FDD3B900FDD3B900FDD3B900471E
      0500FDD3B900FDD3B900FDD3B900471E0500999999000000000000000000CCCC
      CC0000000000CCCCCC0000000000000000000000000099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC9966000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000999999000000000000000000000000000000000000000000999999000000
      000000000000000000000000000000000000FDD3B900471E0500471E0500471E
      0500471E0500FDD3B900471E0500471E0500471E0500471E0500FDD3B900FDD3
      B900471E0500471E0500471E0500FDD3B9009999990000000000000000000000
      0000CCCCCC0000000000CCCCCC00000000000000000099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC996600000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000999999000000000000000000000000000000000099999900000000000000
      000000000000000000000000000000000000FDD3B900FDD3B900FDD3B900FDD3
      B900FDD3B900FDD3B900FDD3B900FDD3B900FDD3B900FDD3B900FDD3B900FDD3
      B900FDD3B900FDD3B900FDD3B900FDD3B9009999990000000000999999009999
      990000000000CCCCCC0000000000CCCCCC000000000099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00FFFFFF00FFFFFF00CC99660000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000999999000000000000000000000000009999990000000000000000000000
      000000000000000000000000000000000000A9490D00A9490D00A9490D00A949
      0D00A9490D00A9490D00A9490D00A9490D00A9490D00A9490D00A9490D00A949
      0D00A9490D00A9490D00A9490D00A9490D009999990000000000CCCCCC009999
      99000000000000000000CCCCCC00000000000000000099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00FFFFFF00CC9966000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000999999000000000000000000999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009696
      9600969696009696960096969600969696009696960096969600969696009696
      9600969696000000000000000000000000000000000099999900000000000000
      0000000000000000000000000000000000009999990000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CC996600FFFFFF00CC996600000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000999999000000000099999900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009696
      9600969696009696960096969600969696009696960096969600969696009696
      9600969696000000000000000000000000000000000000000000999999009999
      9900999999009999990099999900999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000CC996600CC99660000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000999999009999990000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009696
      9600000000000000000000000000000000000000000000000000000000000000
      0000969696000000000000000000000000000000000000000000000000000000
      000000000000D1742E00CC7B2A00CE7F2B00CE7F2B00CC7C2900D1742E000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000009933000099330000000000009933
      0000993300000000000099330000993300000000000000000000000000000000
      0000000000000000000000000000000000009999990099999900000000009999
      9900999999000000000099999900999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000C97C
      2700CE7E2A00CE7D2A00DA903300DC923500DC8F3500DA8E3300CE7D2A00CE7E
      2A00C97C27000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000CC6600009933000000000000CC66
      00009933000000000000CC660000993300000000000000000000000000000000
      000000000000000000000000000000000000B2B2B2009999990000000000B2B2
      B2009999990000000000B2B2B200999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000066
      99000066990000669900000000000000000000000000CE7A2A00CE7F2B00CE7E
      2B00FFC34D00F9BB4900FFC34D00E59F3B00E5993B00FDC14C00FDC14C00FFC3
      4D00CE7E2B00CE7F2B00CE7A2A00000000000000000000000000000000000000
      00000000000000000000000000000000FF000000800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000B2B2B2009999990000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000006699003399
      CC003399CC0066CCFF000066990000000000CC7B2900D2832D00FFC34D00FDC1
      4C00FFC34D00FFC34D00FFC34D00E59F3B00E5983B00FDC14C00FDC14C00FDC1
      4C00FDC14C00FFC34D00D2832D00CC7B29000000000000000000000000000000
      000000000000000000000000FF00000099000000990000008000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000B2B2B200999999009999990099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000006699003399CC000066
      990000CCFF0066CCFF000066990000000000CB7C2A00FFC34D00FFC34D00FDC1
      4C00FDC14C00FDC14C00F29C3700F29C3700F29C3700F29C3700FDC14C00FDC1
      4C00FDC14C00FFC34D00FFC34D00CB7C2A000000000000000000000000000000
      000000000000000000000000FF00000099000000800000009900000080000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000B2B2B200999999009999990099999900999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000006699003399CC000066990000CC
      FF003399CC0066CCFF000066990000000000CB7C2A00FFC34D00FDC14C00FDC1
      4C00F29C3700F29C3700FEBF83000000000000000000FEBF8300F29C3700F29C
      3700FAAF4600FFC34D00FFC34D00CB7C2A000000000000000000000000000000
      000000000000000000000000FF00000099000000990000008000000099000000
      8000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000B2B2B200999999009999990099999900999999009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000669900006699003399CC000066990000CCFF003399
      CC0066CCFF00006699000000000000000000CB7C2A00FFC34D00FFC34D00F29C
      3700FE9A3700FEA54D0000000000000000000000000000000000FFA54D00FE9A
      3700F2993700FFC34D00FFC34D00CB7C2A000000000000000000000000000000
      000000000000FFFFFF00CC9966000000990000009900000099000000FF000000
      9900000080000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF0099999900999999009999990099999900B2B2B2009999
      9900999999000000000000000000000000000000000000000000006699000066
      990000669900006699003399CC003399CC000066990000CCFF003399CC0066CC
      FF0000669900000000000000000000000000CB7C2A00FFC34D00FFC34D00F29C
      37000000000000000000FF9F4000FF9F4000FF9E3F00FF9F4000000000000000
      0000F2983700FFC34D00FFC34D00CB7C2A000000000000000000000000000000
      000000000000FFFFFF00FFCC9900FFCC9900000099000000FF000000FF000000
      FF00000099000000800000000000000000000000000000000000000000000000
      000000000000FFFFFF00CCCCCC00CCCCCC0099999900B2B2B200B2B2B200B2B2
      B20099999900999999000000000000000000000000000066990000CCFF0000CC
      FF0000CCFF0000CCFF00006699000066990000CCFF003399CC0066CCFF000066
      990000000000000000000000000000000000CB7C2A00FFC34D00FFC34D00F29C
      3700000000000000000000000000F29C3700F29C370000000000000000000000
      0000F2983700FFC34D00FFC34D00CB7C2A000000000000000000000000000000
      0000FFFFFF00FFCC9900FFCC9900FFFFFF00FFFFFF000000FF003399FF003399
      FF003399FF000000990000000000000000000000000000000000000000000000
      0000FFFFFF00CCCCCC00CCCCCC00FFFFFF00FFFFFF00B2B2B200CCCCCC00CCCC
      CC00CCCCCC009999990000000000000000000066990000CCFF0000CCFF003399
      CC0000CCFF0000CCFF0000CCFF0000CCFF003399CC0066CCFF00006699000000
      000000000000000000000000000000000000CB7C2A00FFC34D00FFC34D00F29C
      3700000000000000000000000000F29C3700F29C370000000000000000000000
      0000F2973700FFC34D00FFC34D00CB7C2A00000000000000000000000000FFFF
      FF00FFCC9900FFCC9900FFFFFF00FFFFFF00FFFFFF00FFFFFF00000099000000
      990000009900000000000000000000000000000000000000000000000000FFFF
      FF00CCCCCC00CCCCCC00FFFFFF00FFFFFF00FFFFFF00FFFFFF00999999009999
      9900999999000000000000000000000000000066990000CCFF003399CC0000CC
      FF003399CC0000CCFF0000CCFF0000CCFF0066CCFF0000669900000000000000
      000000000000000000000000000000000000CB7C2A00FFC34D00FFC34D00F29C
      3700000000000000000000000000F29C3700F29C370000000000000000000000
      0000F2963600FFC34D00FFC34D00CB7C2A000000000000000000FFFFFF00FFCC
      9900FFCC9900FFFFFF00FFFFFF00FFFFFF00CC996600CC996600000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF00CCCC
      CC00CCCCCC00FFFFFF00FFFFFF00FFFFFF009999990099999900000000000000
      0000000000000000000000000000000000000066990000CCFF0000CCFF003399
      CC0000CCFF003399CC0000CCFF0000CCFF0066CCFF0000669900000000000000
      000000000000000000000000000000000000CB7C2A00FFC34D00FDC14C00FDC1
      4C00F29C3700F29C370000000000F29C3700F29C370000000000F29C3700F29C
      3700FFC34D00FFC34D00FFC34D00CB7C2A0000000000FFFFFF00FFCC9900FFCC
      9900FFFFFF00FFFFFF00FFFFFF00CC9966000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00CCCCCC00CCCC
      CC00FFFFFF00FFFFFF00FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000066990000CCFF0000CCFF0000CC
      FF003399CC0000CCFF003399CC0000CCFF0066CCFF0000669900000000000000
      000000000000000000000000000000000000CC7B2800CC7B2900FFC34D00FDC1
      4C00FDC14C00F29C3700F29C3700F29C3700F29C3700F29C3700F29C3700FFC3
      4D00FDC14C00FFC34D00CC7B2900CC7B2800FFFFFF00FFCC9900FFCC9900FFFF
      FF00FFFFFF00FFFFFF00CC9966000000FF000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00CCCCCC00CCCCCC00FFFF
      FF00FFFFFF00FFFFFF0099999900B2B2B2000000000000000000000000000000
      0000000000000000000000000000000000000066990000CCFF00006699000066
      990000CCFF003399CC0000CCFF003399CC0066CCFF0000669900000000000000
      00000000000000000000000000000000000000000000CE7A2A00CE822B00CE7F
      2A00FFC34D00FFC34D00FFC34D00FFC34D00FFC34D00FFC34D00FFC34D00FFC3
      4D00CE7F2A00CE822B00CE7A2A0000000000CC996600FFCC9900FFFFFF00FFFF
      FF00FFFFFF00CC9966000000FF00000000000000000000000000000000000000
      00000000000000000000000000000000000099999900CCCCCC00FFFFFF00FFFF
      FF00FFFFFF0099999900B2B2B200000000000000000000000000000000000000
      0000000000000000000000000000000000000066990099FFFF003399CC000066
      990000CCFF0000CCFF003399CC0000CCFF0099FFFF0000669900000000000000
      000000000000000000000000000000000000000000000000000000000000C97C
      2700CE7E2A00CE7E2A00FFC34D00F2C84400F2C84400FFC34D00CE7E2A00CE7E
      2A00C97C270000000000000000000000000000000000CC996600FFFFFF00FFFF
      FF00CC9966000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900FFFFFF00FFFF
      FF00999999000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000066990099FFFF0066CC
      FF0066CCFF0066CCFF0066CCFF0099FFFF000066990000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000D1742E00CC7B2A00CE812B00CE812B00CC7C2900D1742E000000
      0000000000000000000000000000000000000000000000000000CC996600CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000999999009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000006699000066
      9900006699000066990000669900006699000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FF330000FF330000FF330000FF330000FF330000FF33
      0000FF330000FF330000FF330000FF3300000000000000000000000000000000
      0000000000000000000099999900999999009999990099999900999999009999
      990099999900999999009999990099999900000000000000000000000000515F
      3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A00515F3A00515F3A00515F3A00515F3A000000000000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999009999990099999900999999000000000000000000000000000000
      00000000000000000000FF330000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FF3300000000000000000000000000000000
      0000000000000000000099999900000000000000000000000000000000000000
      0000000000000000000000000000999999000000000000000000515F3A006C7F
      4D006C7F4D00515F3A00E6EBDF006C7F4D00515F3A00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A00000000000000000099999900CCCC
      CC00CCCCCC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5
      E50099999900CCCCCC00CCCCCC00999999000000000000000000000000000000
      00000000000000000000FF330000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FF3300000000000000000000000000000000
      0000000000000000000099999900000000000000000000000000000000000000
      00000000000000000000000000009999990000000000515F3A00515F3A006C7F
      4D006C7F4D00515F3A00E6EBDF006C7F4D00515F3A00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A00000000009999990099999900CCCC
      CC00CCCCCC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5
      E50099999900CCCCCC00CCCCCC00999999000000000000000000000000000000
      00000000000000000000FF330000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FF3300000000000000000000000000000000
      0000000000000000000099999900000000000000000000000000000000000000
      000000000000000000000000000099999900515F3A006C7F4D00515F3A006C7F
      4D006C7F4D00515F3A00E6EBDF006C7F4D00515F3A00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A0099999900CCCCCC0099999900CCCC
      CC00CCCCCC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5
      E50099999900CCCCCC00CCCCCC0099999900000000000000000000000000FF33
      0000FF330000FF330000FF330000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FF3300000000000000000000000000009999
      9900999999009999990099999900000000000000000000000000000000000000
      000000000000000000000000000099999900515F3A006C7F4D00515F3A006C7F
      4D006C7F4D00515F3A00E6EBDF00E6EBDF00E6EBDF00E6EBDF00E6EBDF00E6EB
      DF00515F3A006C7F4D006C7F4D00515F3A0099999900CCCCCC0099999900CCCC
      CC00CCCCCC0099999900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E50099999900CCCCCC00CCCCCC0099999900000000000000000000000000FF33
      0000FFFFFF00FFFFFF00FF330000FF330000FF330000FF330000FF330000FF33
      0000FF330000FF330000FF330000FF3300000000000000000000000000009999
      9900000000000000000099999900999999009999990099999900999999009999
      990099999900999999009999990099999900515F3A006C7F4D00515F3A006C7F
      4D006C7F4D006C7F4D00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A006C7F4D006C7F4D006C7F4D00515F3A0099999900CCCCCC0099999900CCCC
      CC00CCCCCC00CCCCCC0099999900999999009999990099999900999999009999
      9900CCCCCC00CCCCCC00CCCCCC0099999900000000000000000000000000FF33
      0000FFFFFF00FFFFFF00FF330000FF330000FF330000FF330000FF330000FF33
      0000FFCC9900FF3300006666FF00FF3300000000000000000000000000009999
      9900000000000000000099999900999999009999990099999900999999009999
      9900CCCCCC0099999900CCCCCC0099999900515F3A006C7F4D00515F3A006C7F
      4D006C7F4D006C7F4D006C7F4D006C7F4D006C7F4D006C7F4D006C7F4D006C7F
      4D006C7F4D006C7F4D006C7F4D00515F3A0099999900CCCCCC0099999900CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC0099999900000000000000000000000000FF33
      0000FFFFFF00FFFFFF00FFCC9900FF330000FF330000FF330000FF330000FF33
      0000FF330000FF330000FF330000FFCC99000000000000000000000000009999
      99000000000000000000CCCCCC00999999009999990099999900999999009999
      9900999999009999990099999900CCCCCC00515F3A006C7F4D00515F3A006C7F
      4D006C7F4D00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A00515F3A006C7F4D006C7F4D00515F3A0099999900CCCCCC0099999900CCCC
      CC00CCCCCC009999990099999900999999009999990099999900999999009999
      990099999900CCCCCC00CCCCCC0099999900FF330000FF330000FF330000FF33
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FF3300000000000000000000000000009999990099999900999999009999
      9900000000000000000000000000000000000000000000000000000000000000
      000099999900000000000000000000000000515F3A006C7F4D00515F3A006C7F
      4D00515F3A00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00515F3A006C7F4D00515F3A0099999900CCCCCC0099999900CCCC
      CC0099999900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0099999900CCCCCC0099999900FF330000FFFFFF00FFFFFF00FF33
      0000FF330000FF330000FF330000FF330000FF330000FF330000FF330000FF33
      0000FF3300000000000000000000000000009999990000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      990099999900000000000000000000000000515F3A006C7F4D00515F3A006C7F
      4D00515F3A00FFFFFF00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A00FFFFFF00515F3A006C7F4D00515F3A0099999900CCCCCC0099999900CCCC
      CC0099999900FFFFFF0099999900999999009999990099999900999999009999
      9900FFFFFF0099999900CCCCCC0099999900FF330000FFFFFF00FFFFFF00FF33
      0000FF330000FF330000FF330000FF330000FF330000FFCC9900FF3300006666
      FF00FF3300000000000000000000000000009999990000000000000000009999
      99009999990099999900999999009999990099999900CCCCCC0099999900CCCC
      CC0099999900000000000000000000000000515F3A006C7F4D00515F3A006C7F
      4D00515F3A00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00515F3A006C7F4D00515F3A0099999900CCCCCC0099999900CCCC
      CC0099999900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0099999900CCCCCC0099999900FF330000FFFFFF00FFFFFF00FFCC
      9900FF330000FF330000FF330000FF330000FF330000FF330000FF330000FF33
      0000FFCC9900000000000000000000000000999999000000000000000000CCCC
      CC00999999009999990099999900999999009999990099999900999999009999
      9900CCCCCC00000000000000000000000000515F3A006C7F4D00515F3A00E6EB
      DF00515F3A00FFFFFF00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A00FFFFFF00515F3A00515F3A00515F3A0099999900CCCCCC0099999900E5E5
      E50099999900FFFFFF0099999900999999009999990099999900999999009999
      9900FFFFFF00999999009999990099999900FF330000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FF330000000000000000
      0000000000000000000000000000000000009999990000000000000000000000
      0000000000000000000000000000000000000000000099999900000000000000
      000000000000000000000000000000000000515F3A006C7F4D00515F3A006C7F
      4D00515F3A00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00515F3A006C7F4D00515F3A0099999900CCCCCC0099999900CCCC
      CC0099999900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0099999900CCCCCC0099999900FF330000FF330000FF330000FF33
      0000FF330000FF330000FF330000FF330000FF330000FF330000000000000000
      0000000000000000000000000000000000009999990099999900999999009999
      9900999999009999990099999900999999009999990099999900000000000000
      000000000000000000000000000000000000515F3A00E6EBDF00515F3A00515F
      3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A00515F3A00515F3A00515F3A00515F3A0099999900E5E5E500999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      990099999900999999009999990099999900FF330000FF330000FF330000FF33
      0000FF330000FF330000FFCC9900FF3300006666FF00FF330000000000000000
      0000000000000000000000000000000000009999990099999900999999009999
      99009999990099999900CCCCCC0099999900CCCCCC0099999900000000000000
      000000000000000000000000000000000000515F3A006C7F4D00515F3A00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00515F
      3A006C7F4D00515F3A00000000000000000099999900CCCCCC0099999900FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009999
      9900CCCCCC00999999000000000000000000FFCC9900FF330000FF330000FF33
      0000FF330000FF330000FF330000FF330000FF330000FFCC9900000000000000
      000000000000000000000000000000000000CCCCCC0099999900999999009999
      99009999990099999900999999009999990099999900CCCCCC00000000000000
      000000000000000000000000000000000000515F3A00515F3A00515F3A00515F
      3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F3A00515F
      3A00515F3A00515F3A0000000000000000009999990099999900999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      990099999900999999000000000000000000000000001F1F1F00242424001F1F
      1F001F1F1F001F1F1F002D2D2D00D2D2D200000000002F2F2F00282828002A2A
      2A002A2A2A002A2A2A001F1F1F00000000000000000000000000000000000000
      0000000000000000000000000000808080008080800080808000808080008080
      8000808080008080800080808000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000001F1F1F009E9E9E000000
      000000000000000000002E2E2E0000000000000000001F1F1F001F1F1F001F1F
      1F001F1F1F001F1F1F001F1F1F00000000000000000000000000000000000000
      000000000000000000000000000080808000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00808080003333330033333300999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000006666660066666600999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000031313100787878000000
      000000000000000000003838380000000000000000001F1F1F001F1F1F001F1F
      1F001F1F1F001F1F1F003B3B3B00000000000000000000000000000000000000
      000080606000000000000000000080808000FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF0080808000CCCCCC0066666600333333003333
      3300000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000009999990080808000666666006666
      6600000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000008E8E8E00232323000000
      000000000000000000003838380000000000000000001F1F1F001F1F1F001F1F
      1F001F1F1F001F1F1F0090909000000000000000000000000000000000000000
      000080606000806060000000000080808000FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00808080000000000066666600666666006666
      6600333333009999990000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080808000808080008080
      8000666666009999990000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000E2E2E200262626007E7E
      7E0000000000000000003838380000000000000000001F1F1F001F1F1F001F1F
      1F001F1F1F0030303000E3E3E300000000000000000080606000806060008060
      600080606000806060008060600080808000FFFFFF0000000000000000000000
      00000000000000000000FFFFFF008080800000000000CCCCCC00666666006666
      6600666666003333330000999900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900808080008080
      8000808080006666660099999900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000787878002E2E
      2E0000000000000000003838380000000000000000001F1F1F001F1F1F001F1F
      1F001F1F1F007F7F7F0000000000000000000000000080606000000000000000
      000080606000806060000000000080808000FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00808080000000000000000000666666006666
      660099FFFF0099CCCC0099CCCC00009999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000808080008080
      8000E5E5E500CCCCCC00CCCCCC00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000D3D3D3001F1F
      1F0093939300000000003838380000000000000000001F1F1F001F1F1F001F1F
      1F002A2A2A00D4D4D40000000000000000000000000080606000000000000000
      000080606000000000000000000080808000FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF00FFFFFF00808080000000000000000000CCCCCC008080
      8000CCFFFF0099FFFF0099CCCC0099CCCC000099990099CCCC00000000000000
      0000000000000000000000000000000000000000000000000000999999008080
      8000E5E5E500E5E5E500CCCCCC00CCCCCC0099999900CCCCCC00000000000000
      0000000000000000000000000000000000000000000000000000000000006E6E
      6E0036363600000000003939390000000000000000001F1F1F001F1F1F001F1F
      1F00737373000000000000000000000000000000000080606000000000000000
      000000000000000000000000000080808000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF008080800000000000000000000000000066CC
      CC00FFFFFF00CCFFFF0099FFFF0099FFFF0099CCCC0066CCCC00009999000000
      000000000000000000000000000000000000000000000000000000000000CCCC
      CC00FFFFFF00E5E5E500E5E5E500E5E5E500CCCCCC00CCCCCC00999999000000
      000000000000000000000000000000000000000000000000000000000000C8C8
      C8001F1F1F00A7A7A7004343430000000000000000001F1F1F001F1F1F002121
      2100C9C9C9000000000000000000000000008080800080606000806060008060
      6000806060008060600080606000808080008080800080808000808080008080
      80008080800080808000808080008080800000000000000000000000000066CC
      CC00FFFFFF00CCFFFF00CCFFFF0099FFFF0099FFFF0099CCCC0099CCCC000099
      990066CCCC00000000000000000000000000000000000000000000000000CCCC
      CC00FFFFFF00E5E5E500E5E5E500E5E5E500E5E5E500CCCCCC00CCCCCC009999
      9900CCCCCC000000000000000000000000000000000000000000000000000000
      00005B5B5B004C4C4C002D2D2D0000000000000000001F1F1F001F1F1F006363
      63000000000000000000000000000000000080606000F0FBFF0080E0E00080E0
      E00080E0E00080E0E00080E0E000F0FBFF008060600000000000000000000000
      0000000000000000000080808000000000000000000000000000000000000000
      000099CCCC00FFFFFF00CCFFFF00CCFFFF0099FFFF0099FFFF0099CCCC00FFCC
      CC00993300009933000000000000000000000000000000000000000000000000
      0000CCCCCC00FFFFFF00E5E5E500E5E5E500E5E5E500E5E5E500CCCCCC00CCCC
      CC00666666006666660000000000000000000000000000000000000000000000
      0000BEBEBE001F1F1F001F1F1F0000000000000000001F1F1F001F1F1F00BEBE
      BE00000000000000000000000000000000008060600080E0E00080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E0008060600080808000808080008080
      8000808080008080800080808000000000000000000000000000000000000000
      000066CCCC00FFFFFF00CCFFFF00CCFFFF00FFCCCC00CC660000CC6600009933
      0000993300009933000099330000000000000000000000000000000000000000
      0000CCCCCC00FFFFFF00E5E5E500E5E5E500CCCCCC0080808000808080006666
      6600666666006666660066666600000000000000000000000000000000000000
      0000FAFAFA005A5A5A001F1F1F0000000000000000001F1F1F005A5A5A000000
      0000000000000000000000000000000000008060600080E0E00080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E0008060600000000000000000000000
      0000808080000000000000000000000000000000000000000000000000000000
      00000000000099CCCC00FFFFFF00CCFFFF00CC660000CC660000CC660000CC66
      0000993300009933000099330000993300000000000000000000000000000000
      000000000000CCCCCC00FFFFFF00E5E5E5008080800080808000808080008080
      8000666666006666660066666600666666000000000000000000000000000000
      000000000000B0B0B0001F1F1F0000000000000000001F1F1F00B0B0B0000000
      0000000000000000000000000000000000008060600000FFFF0080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E0008060600080808000808080008080
      8000808080000000000000000000000000000000000000000000000000000000
      00000000000066CCCC00FFFFFF00FFCCCC00CC660000CC660000CC660000CC66
      0000CC6600009933000099330000993300000000000000000000000000000000
      000000000000CCCCCC00FFFFFF00CCCCCC008080800080808000808080008080
      8000808080006666660066666600666666000000000000000000000000000000
      000000000000F8F8F8003D3D3D0000000000000000003A3A3A00000000000000
      0000000000000000000000000000000000008060600080606000806060008060
      600040E0E00080E0E00080E0E000F0FBFF008060600000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FF990000FF990000CC660000CC660000CC660000CC66
      0000CC660000CC66000099330000993300000000000000000000000000000000
      0000000000000000000099999900999999008080800080808000808080008080
      8000808080008080800066666600666666000000000000000000000000000000
      000000000000000000009191910000000000000000008F8F8F00000000000000
      0000000000000000000000000000000000008060600000FFFF000000000000E0
      E000806060008060600080606000806060008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000CC660000FF990000FF990000CC660000CC660000CC66
      0000CC660000CC660000CC660000993300000000000000000000000000000000
      0000000000000000000066666600999999009999990080808000808080008080
      8000808080008080800080808000666666000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000F0FBFF0080606000806060008060
      6000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000CC660000FF990000FF990000CC660000CC66
      0000CC660000CC660000CC660000CC6600000000000000000000000000000000
      0000000000000000000000000000666666009999990099999900808080008080
      8000808080008080800080808000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800080808000808080008080800080808000808080008080
      8000808080008080800080808000808080000000000000000000000000000000
      0000000000000000000000000000000000009D6138009D6138009D6138009D61
      38009D6138009D6138009D61380000000000CFAC8E00BC936C00BC936C00BC93
      6C00BC936C00BC936C00BC936C00BC936C00BC936C00BC936C00BC936C00C59D
      7D00C59D7D00C59D7D00C59D7D00CFAC8E00000000000000000000000000CFCF
      CF00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CFCFCF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000808080000000000000000000000000000000
      0000000000000000000000000000000000009D613800BE996E00BE996E00BE99
      6E00BE996E00BE996E009D61380000000000C59D7D00FBF3E800FBF3E800FBF3
      E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800C59D
      7D00FBF3E800FBF3E800FBF3E800C59D7D00000000000000000000000000BB89
      2600B67E0E00B47B0900B47A0700B47A0800B47B0800B47A0800B47A0800B47B
      0800B47A0800B47B0900B67E0E00BB8825000000000000000000000000000000
      0000808080008080800080808000808080008080800080808000808080008080
      8000808080008080800000000000808080000000000000000000000000000000
      0000000000000000000000000000000000009D613800BE996E00BE996E00BE99
      6E00BE996E00BE996E009D61380000000000CFAC8E00FBF3E800DCC0A300DCC0
      A300E1CDA800DCC0A300E2CFA900FAECD400FAECD400FAECD400FBF3E800CFAC
      8E00FBF3E8009D613800FBF3E800CFAC8E000000000000000000CCCCCC00B67E
      0E00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000B67E0E000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800000000000808080000000000000000000000000003399
      CC00006699000066990000669900006699000066990000669900006699000066
      9900BE996E00BE996E009D61380000000000CFAC8E00FBF3E800E2CFA900E2CF
      A900E2CFA900E2CFA900E2CFA900FAECD400FAECD400FAECD400FBF3E800CFAC
      8E00FBF3E800FBF3E800FBF3E800CFAC8E000000000000000000B8831700B379
      040000000000000000000000000000000000D7BFA4000000000000000000D7BF
      A400000000000000000000000000B47B09000000000000000000000000008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000000000008080800000000000808080000000000000000000000000003399
      CC00FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E8000066
      9900BE996E00BE996E009D61380000000000D0AC8F00FBF3E800BC936C00CFAC
      8E00CFAC8E00BC936C00F6E8CA00FBF3E800DCC0A300DCC0A300FBF3E800CFAC
      8E00D0AC8F00D0AC8F00D0AC8F00CFAC8E00CCCCCC00CCCCCC00B57E0E000000
      0000AC6D0000DAC4AC00D8C0A400D8BFA200D9C1A300D8BFA100D8BFA100D9C1
      A300D7BE9F00D5BC9C0000000000B47B08000000000000000000000000008080
      8000000000000000000000000000000000000000000000000000000000008080
      8000000000008080800000000000808080000000000000000000000000003399
      CC00FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E8000066
      9900BE996E00BE996E009D61380000000000DCC0A300FBF3E800FBF3E800FBF3
      E800FBF3E800FBF3E800FBF3E800FBF3E800F6E8CA00F6E8CA00FBF3E800CFAC
      8E00DCC0A300DCC0A300DCC0A300CFAC8E00B47C0B00B37A0900AF7400000000
      000000000000AC6D00000000000000000000D7BC9E00FFFFFC00FFFFFC00D7BC
      9D00FFFFFB00FFFFF70000000000B47B08000000000000000000000000008080
      8000000000000000000000000000000000000000000000000000000000008080
      8000000000008080800000000000808080000000000000000000000000003399
      CC00FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E8000066
      99009D6138009D6138009D61380000000000DCC0A300FFFFFF0003599D000961
      A5001A77BB002587CB001976BA002587CB001570B400055CA000FFFFFF00CFAC
      8E00DCC0A300DCC0A300DCC0A300CFAC8E00FFFAE500FFFAE400FFF7DE00FFFA
      EB00FFFAEB0000000000AD6E0000FFFFFE00D7BC9A00FFFFF400FFFFF400D7BB
      9800FFFFF300FFFDEF0000000000B47B08000000000000000000000000008080
      8000000000000000000000000000000000000000000000000000000000008080
      8000000000008080800000000000808080009D6138009D6138009D6138003399
      CC00FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E8000066
      990000000000000000000000000000000000DCC0A300FFFFFF0003599D00C2D8
      E90089B9DB0090C3E500B6CFE0002587CB001570B400055CA000FFFFFF00CFAC
      8E00DCC0A300DCC0A300DCC0A300CFAC8E00000000000000000000000000FFF5
      E000FFF6E00000000000AD6F0000D9BD9D00D9BC9900D7B99500D7B99500D8BB
      9700D6B89300D4B58F0000000000B47B08000000000000000000000000008080
      8000000000000000000000000000000000000000000000000000000000008080
      8000000000008080800000000000808080009D613800BE996E00BE996E003399
      CC00FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E8000066
      990000000000000000000000000000000000DCC0A300FFFFFF0002579B000961
      A500126DB1001876BA001976BA00136EB2000B63A70003599D00FFFFFF00CFAC
      8E00DCC0A300DCC0A300DCC0A300CFAC8E00B47A0800B37A0600AF730000FFF8
      E10000000000AA6B0000FFFFF000FFFEEC00D7B99100FFFCE800FFFCE800D7B8
      9000FFFBE600FFF8E20000000000B47B08000000000000000000000000008080
      8000000000000000000000000000000000000000000000000000000000008080
      8000000000008080800000000000808080009D613800BE996E00BE996E003399
      CC00FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E8000066
      990000000000000000000000000000000000DCC0A300FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CFAC
      8E00DCC0A300DCC0A300DCC0A300CFAC8E000000000000000000B57D0D000000
      0000AC6D0000FFFCE700FFF9E200FFF9E100D7B58C00FFF9E100FFF9E100D7B5
      8C00FFF8DF00FFF5DA0000000000B47B08000000000000000000000000008080
      8000000000000000000000000000000000000000000000000000000000008080
      8000000000008080800000000000808080009D613800BE996E00BE996E003399
      CC00FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E8000066
      990000000000000000000000000000000000DCC0A300FBF3E800DCC0A300E1CD
      A800E2CFA900E1CDA800F6E8CA00FBF3E800DCC0A300E1CDA800FBF3E800D0AC
      8F00D0AC8F00D0AC8F00D0AC8F00D0AC8F000000000000000000B8831700B378
      030000000000DEB48200E0B58100E0B68300DEB78700DFB68400DFB68400DEB7
      8700DFB58200DDB27D0000000000B57B09000000000000000000000000008080
      8000000000000000000000000000000000000000000000000000000000008080
      8000000000008080800000000000000000009D613800BE996E00BE996E003399
      CC003399CC003399CC003399CC003399CC003399CC003399CC003399CC003399
      CC0000000000000000000000000000000000E1CDA800FBF3E800F6E8CA00F6E8
      CA00F6E8CA00F6E8CA00F6E8CA00FBF3E800F6E8CA00F6E8CA00FBF3E800DCC0
      A300FFFFFF00FFFFFF00FFFFFF00DCC0A300000000000000000000000000B57B
      08000000000044C4FF0049C5FF0047C6FF00E3B57E0046C7FF0046C7FF00E3B5
      7E0046C6FF0044C4FF0000000000B57C09000000000000000000000000008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000000000000000000000000000000000009D613800BE996E00BE996E00BE99
      6E00BE996E00BE996E009D613800000000000000000000000000000000000000
      000000000000000000000000000000000000E1CDA800FBF3E800BC936C00DCC0
      A300CFAC8E00CFAC8E00BC936C00FBF3E800DCC0A300E2CFA900FBF3E800DCC0
      A300FFFFFF00C59D7D00FFFFFF00DCC0A300000000000000000000000000B67E
      0E00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000B67E0E000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000009D6138009D6138009D6138009D61
      38009D6138009D6138009D613800000000000000000000000000000000000000
      000000000000000000000000000000000000E2CFA900FFFFFF00FBF3E800FBF3
      E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FBF3E800FFFFFF00E1CD
      A800FFFFFF00FFFFFF00FFFFFF00E1CDA800000000000000000000000000CFAA
      6000B67E0E00B67C0900B67B0700B67B0800B57B0800B67B0800B67B0800B57B
      0800B67B0800B67C0900B67E0E00BD8C27000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000F6E8CA00E2CFA900E2CFA900E2CF
      A900E2CFA900E2CFA900E2CFA900E2CFA900E2CFA900E2CFA900E2CFA900E2CF
      A900E2CFA900E2CFA900E2CFA900F6E8CA000000000000000000000000000000
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
      00009D6138009D6138009D6138009D6138009D6138009D6138009D6138000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000003399CC00666699009999
      9900E5E5E5000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000003399CC00666699009999
      9900E5E5E5000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009D61
      3800BC946B00F7E8C800F7E8C800F7E8C800F7E8C800F7E8C800F7E8C8009D61
      3800000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000066CCFF003399CC006666
      990099999900E5E5E50000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000066CCFF003399CC006666
      990099999900E5E5E50000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000009D613800E1CD
      A800C59D7D00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2
      DC009D6138000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000CCCCFF0066CCFF003399
      CC0066669900CCCCCC00FFCCCC00CC999900CC999900CC999900CCCC9900E5E5
      E5000000000000000000000000000000000000000000CCCCFF0066CCFF00262C
      E90066669900CCCCCC00FFCCCC00CC999900CC999900CC999900CCCC9900E5E5
      E500000000004019EF000000000000000000000000009D613800E2CFA900DCC0
      A300CFAC8E00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2
      DC009D61380000000000000000000000000000000000DFDFDF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000CCCCFF001B36
      440000000000000000007A7A5C00F2EABF00FFFFCC00817D6600000000000000
      000000000000BB00BB0000000000000000000000000000000000CCCCFF002221
      CE00221ADE00000000007A7A5C00F2EABF00FFFFCC00817D6600000000000000
      0000221ADE004017E40000000000000000009D613800BC936C00C59C7C00D0AC
      8F00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2
      DC009D6138000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000E5E5
      E500523D3D00886D5200FFFFCC00FFFFCC00FFFFCC00FFFFFF00444444002222
      2200B1858500E5E5E5000000000000000000000000000000000000000000E5E5
      E5002C22CC002B21E300FFFFCC00FFFFCC00FFFFCC00FFFFFF0044444400241C
      E0003E30DA00E5E5E50000000000000000009D613800F6E8CA00FFF2DC00FFF2
      DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2
      DC009D6138000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFCC
      CC00BEBE8F0044443600F2EABF00FFFFCC00FFFFCC00CCCCCC00000000008888
      8800F2EABF00CCCC99000000000000000000000000000000000000000000FFCC
      CC00BEBE8F002A24CB003129EA00FFFFCC00FFFFCC00CCCCCC00221ADE003731
      DB00F2EABF00CCCC990000000000000000009D613800F6E8CA00FFF2DC00FFF2
      DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2
      DC009D6138000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CCCC
      9900FFCC9900514E40000000000000000000000000000000000000000000DDDD
      DD00F2EABF00CC9999000000000000000000000000000000000000000000CCCC
      9900FFCC9900514E40001D17C100221ADE0000000000221ADE001D17C100DDDD
      DD00F2EABF00CC99990000000000000000009D613800F6E8CA00FFF2DC00FFF2
      DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2
      DC009D6138000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CC99
      9900F2EABF00A19C7F00716D5900F2EABF00FFFFCC003333290033332900FFFF
      CC00FFFFCC00CC9999000000000000000000000000000000000000000000CC99
      9900F2EABF00A19C7F00716D59004B43E500251DED002721C90033332900FFFF
      CC00FFFFCC00CC99990000000000000000009D613800F6E8CA00FFF2DC00FFF2
      DC00FFF2DC00FDEED900FFF2DC00BE996E00BE996E00BE996E00BE996E00BE99
      6E00BE996E00BE996E0000000000000000000000000000000000000000000000
      000000000000E1E1E10000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CCCC
      9900F2EABF00FFFFCC00201F1900F2EABF00C2BB99000000000088886D00FFFF
      CC00F2EABF00CC9999000000000000000000000000000000000000000000CCCC
      9900F2EABF00FFFFCC00201F19003129EA002922EB00221ADE0088886D00FFFF
      CC00F2EABF00CC99990000000000000000009D613800F6E8CA00FFF2DC00FFF2
      DC00FFF2DC00FFF2DC00BE996E00FFF2DC00FFF2DC00FFF2DC00FFF2DC000066
      9900FFF2DC00FFF2DC00BE996E00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFCC
      CC00CCCC9900FFFFFF0077777700A19C7F00817D660000000000D2CBA600FFFF
      CC00CCCC9900CCCC99000000000000000000000000000000000000000000FFCC
      CC00CCCC9900FFFFFF002921E5003B34D900817D66001D17C1002F27E800FFFF
      CC00CCCC9900CCCC990000000000000000009D613800F6E8CA00FFF2DC00FFF2
      DC00FFF2DC00FFF2DC00BE996E00FFF2DC000066990000669900006699000066
      990000669900FFF2DC00BE996E00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000E5E5
      E500CC999900ECC6D900CCCCCC0055554400302F2600302F2600F2EABF00FFCC
      9900CC999900E5E5E5000000000000000000000000000000000000000000E5E5
      E500CC9999003126EC00433DE70055554400302F2600302F26004B43E5003227
      E800CC999900E5E5E50000000000000000009D613800F6E8CA00FFF2DC00FFF2
      DC00FFF2DC00FFF2DC00BE996E00FFF2DC00FFF2DC00FDEFD900FDEED9000066
      9900FFF2DC00FAECD400BE996E00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFCCCC00CC999900FFCCCC00302F260000000000817D6600CCCC9900CC99
      9900FFCCCC000000000000000000000000000000000000000000000000000000
      00003227EB004334DE00FFCCCC00302F260000000000817D6600CCCC99004334
      DE003227EB000000000000000000000000009D613800F6E8CA00FFF2DC00FFF2
      DC00FFF2DC00FFF2DC00FFF2DC00BE996E00BE996E00BE996E00BE996E00BE99
      6E00BE996E00BE996E0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000E5E5E500CCCC99006D52520000000000A37A7A00CC999900E5E5
      E500000000000000000000000000000000000000000000000000000000005B15
      F2004D17F100E5E5E500CCCC99006D52520000000000A37A7A00CC999900E5E5
      E5004D17F1005B15F20000000000000000009D613800F6E8CA00FFF2DC00FFF2
      DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FFF2DC00FBF3
      E8009D6138000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000009D613800FFF4E100FFF4
      E100FFF4E100FFF4E100FFF4E100FFF4E100FFF4E100FFF4E100FFF4E3009D61
      38009D6138000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000009D6138009D61
      38009D6138009D6138009D6138009D6138009D6138009D6138009D6138000000
      0000000000000000000000000000000000000000000000000000686868004B4B
      4B004B4B4B004B4B4B004B4B4B004B4B4B004B4B4B004B4B4B004B4B4B004B4B
      4B004B4B4B0068686800000000000000000000000000FF990000CC660000CC66
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC6600000000
      00000000000000000000000000000000000000000000CCCCCC00999999009999
      9900999999009999990099999900999999009999990099999900999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000051515100F3F3
      F300ECECEC00ECECEC00ECECEC00ECECEC00ECECEC00ECECEC00ECECEC00ECEC
      EC00F3F3F300515151000000000000000000FF990000CC660000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC66
      000000000000000000000000000000000000CCCCCC0099999900FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009999
      9900000000000000000000000000000000000000000000000000000000001111
      1100111111001111110011111100111111001111110011111100111111001111
      110011111100111111000000000000000000000000000000000059595900EEEE
      EE00E2E2E200E2E2E200E2E2E200E2E2E200E2E2E2007A7A7A00565656005656
      5600595959003A3A3A005C5C5C0086868600FF990000CC660000E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500CC66
      000000000000000000000000000000000000CCCCCC0099999900E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E5009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000001111110000000000111111000000000011111100000000001111
      11000000000011111100000000000000000000000000000000005C5C5C00EFEF
      EF00E4E4E400E4E4E400E4E4E400E4E4E400E4E4E4005E5E5E009B9B9B009B9B
      9B009B9B9B009B9B9B009B9B9B0064646400FF990000CC660000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC66
      000000000000000000000000000000000000CCCCCC0099999900FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000001111110000000000111111000000000011111100000000001111
      11000000000011111100000000000000000000000000000000005D5D5D00F1F1
      F100E7E7E700E7E7E700E7E7E700E7E7E700E7E7E70085858500646464006464
      64006666660048484800696969008E8E8E00FF990000CC660000E5E5E500E5E5
      E500E5E5E500E5E5E500CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CC66
      000000000000000000000000000000000000CCCCCC0099999900E5E5E500E5E5
      E500E5E5E500E5E5E500CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000001111110000000000111111000000000011111100000000001111
      110000000000111111000000000000000000000000000000000060606000F3F3
      F300EBEBEB00EBEBEB00EBEBEB00EBEBEB00EBEBEB00EBEBEB00EBEBEB00EBEB
      EB00F3F3F300606060000000000000000000FF990000CC660000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500CC66
      000000000000000000000000000000000000CCCCCC0099999900FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E5009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000001111110000000000111111000000000011111100000000001111
      110000000000111111000000000000000000000000000000000061616100F5F5
      F500EEEEEE00EEEEEE00EEEEEE00EEEEEE00EEEEEE00EEEEEE00EEEEEE00EEEE
      EE00F5F5F500616161000000000000000000FF990000CC660000E5E5E500E5E5
      E500E5E5E500CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CC66
      000000000000000000000000000000000000CCCCCC0099999900E5E5E500E5E5
      E500E5E5E500CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000001111110000000000111111000000000011111100000000001111
      110000000000111111000000000000000000000000000000000063636300F7F7
      F700F1F1F100F1F1F100F1F1F100F1F1F100F1F1F100F1F1F100F1F1F100F1F1
      F100F7F7F700636363000000000000000000FF990000CC660000FFFFFF00FFFF
      FF00FFFFFF00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500CC66
      000000000000000000000000000000000000CCCCCC0099999900FFFFFF00FFFF
      FF00FFFFFF00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E5009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000001111110000000000111111000000000011111100000000001111
      110000000000111111000000000000000000000000000000000065656500F9F9
      F900F5F5F500F5F5F500F5F5F500F5F5F500F5F5F500F5F5F500F5F5F500F5F5
      F500F9F9F900656565000000000000000000FF990000CC660000E5E5E500E5E5
      E500CCCCCC00CCCCCC00CC660000CC660000CC660000CC660000CC660000CC66
      0000CC660000CC660000CC660000CC660000CCCCCC0099999900E5E5E500E5E5
      E500CCCCCC00CCCCCC0099999900999999009999990099999900999999009999
      9900999999009999990099999900999999000000000000000000000000000000
      0000000000001111110000000000111111000000000011111100000000001111
      110000000000111111000000000000000000000000000000000067676700FBFB
      FB00F8F8F800F8F8F800F8F8F800F8F8F800F8F8F800F8F8F800F8F8F800F8F8
      F800FBFBFB00676767000000000000000000FF990000CC660000FFFFFF00FFFF
      FF00E5E5E500CC660000FF990000CC660000CC660000CC660000CC660000CC66
      0000CC660000CC660000CC660000CC660000CCCCCC0099999900FFFFFF00FFFF
      FF00E5E5E50099999900CCCCCC00999999009999990099999900999999009999
      9900999999009999990099999900999999000000000000000000000000000000
      0000000000001111110000000000111111000000000011111100000000001111
      110000000000111111000000000000000000000000000000000068686800FDFD
      FD00FBFBFB00FBFBFB00FBFBFB00FBFBFB00FBFBFB00FBFBFB00FBFBFB00FBFB
      FB00FDFDFD00686868000000000000000000FF990000CC660000E5E5E500CCCC
      CC00FF990000FF990000FF990000FF990000FF990000CC660000CC660000CC66
      0000CC660000CC660000CC66000000000000CCCCCC0099999900E5E5E500CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC0099999900999999009999
      9900999999009999990099999900000000000000000000000000020202000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000696969000000
      0000FDFDFD00DCDCDC00DCDCDC00DCDCDC00DCDCDC00DCDCDC00DCDCDC00FDFD
      FD0000000000696969000000000000000000FF990000CC660000FFFFFF00FF99
      0000FF996600FF996600FF990000FF990000FF990000FF990000FF990000CC66
      0000CC660000CC6600000000000000000000CCCCCC0099999900FFFFFF00CCCC
      CC00E5E5E500E5E5E500CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      9900999999009999990000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000006B6B6B000000
      0000A2A2A2008989890098989800A5A5A500AAAAAA009F9F9F0090909000A6A6
      A600000000006B6B6B000000000000000000FF990000CC660000FF990000FF99
      6600FF996600FF996600FF996600FF996600FF990000FF990000FF990000FF99
      0000CC660000000000000000000000000000CCCCCC0099999900CCCCCC00E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000111111001111
      1100111111001111110011111100111111001111110011111100111111001111
      11001111110011111100111111000000000000000000000000006C6C6C009C9C
      9C006F6F6F00C9C9C900D1D1D100DFDFDF00DFDFDF00D1D1D100C9C9C9006F6F
      6F009C9C9C006C6C6C000000000000000000FF990000FF990000FF996600FF99
      6600FF996600FF996600FF996600FF996600FF996600FF996600FF990000CC66
      000000000000000000000000000000000000CCCCCC00CCCCCC00E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500CCCCCC009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000000000000011111100000000000000000000000000111111000000
      0000000000000000000000000000000000000000000000000000919191006D6D
      6D006D6D6D006262620091919100DADADA00DADADA0091919100626262006D6D
      6D006D6D6D0091919100000000000000000000000000FF99000099999900FFFF
      FF0099999900FFFFFF0099999900FFFFFF0099999900FFFFFF0099999900FFFF
      FF000000000000000000000000000000000000000000CCCCCC0099999900FFFF
      FF0099999900FFFFFF0099999900FFFFFF0099999900FFFFFF0099999900FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000011111100111111001111110011111100111111000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000C8C8C800BABABA00BABABA00C8C8C800000000000000
      000000000000000000000000000000000000000000000000000099999900CCCC
      CC0099999900CCCCCC0099999900CCCCCC0099999900CCCCCC00999999000000
      000000000000000000000000000000000000000000000000000099999900CCCC
      CC0099999900CCCCCC0099999900CCCCCC0099999900CCCCCC00999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000065000000FF000000FF0000
      00FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF0000
      00FF000000FF000000FF00000065000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000595C7C003338
      6C0033386C0033386C0033386C0033386C0033386C0033386C0033386C003338
      6C0033386C00595C7C000000000000000000000000FFBAC9C6FF789490FF7692
      8EFF75918EFF73908CFF728F8BFF728F8BFF728F8BFF728F8BFF728F8BFF728F
      8BFF728F8BFF516764FF000000FF000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000F7F7F700ABABAB00F1F1F100000000000000000000000000000000000000
      00000000000000000000008B49000E9054000000000000000000000000000000
      000000000000000000000000000000000000000000000000000033407700F9F9
      E900F3F3E200F3F3E200F3F3E200F3F3E200F3F3E200F3F3E200F3F3E200F3F3
      E200F9F9E900334077000000000000000000000000FFE0E8E8FFAFC6C3FFADC4
      C1FFABC2BFFFA8C1BDFFA5BEBBFFA3BDBAFFA0BAB7FFA0BAB7FFA0BAB7FFA0BA
      B7FFA0BAB7FF728F8BFF000000FF000000000000000000000000000000000000
      0000DFDFF2006060BF0010109F00000099000000990010109F006060BF00D8D8
      EA00979797007D7D7D00A2A2A200000000000000000000000000000000000000
      00000000000000874300A1E2D500239A60000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000334A8500F4F4
      E500E8E8D900E8E8D900E8E8D900E8E8D900E8E8D9005D5DA5002E2E91002E2E
      9100313194000A0F8000333399006666B500000000FFE7EEECFFB3C9C6FFB1C8
      C4FFAFC6C3FFADC4C1FFABC2BFFFA8C1BDFFA5BEBBFFA5BEBBFFA3BDBAFFA0BA
      B7FFA0BAB7FF728F8BFF000000FF00000000000000000000000000000000CFCF
      EC0010109F000B24CC000330F2000032FF000032FF00002FF9000923CC006768
      85007D7D7D0091919100F1F1F100000000000000000000000000000000000000
      00000087430093DCC9001ACAAD0000B68E000096580000975B00008B4A000089
      450046A176000000000000000000000000000000000000000000334F8B00F5F5
      E700EAEADC00EAEADC00EAEADC00EAEADC00EAEADC002F2FA5005E5EF7005E5E
      F7005E5EF7005E5EF7005E5EF7003333AD00000000FFE8EEEDFFB9CDCAFFB6CB
      C8FFB3C9C6FFB1C8C4FFAFC6C3FFADC4C1FFABC2BFFFABC2BFFFA8C1BDFF434D
      4CFFA3BDBAFF728F8BFF000000FF000000000000000000000000DFDFF2001010
      9F00254DEC001148FF000A3FFF000437FF000032FF000A38F400656F95007D7D
      7D006C6C8300D6D6E7000000000000000000000000000000000000000000008A
      480081DBC20014CEA90000C4990057DBC10056DCC30056DDC40056DEC50056DC
      C40044C19A00008B4A000000000000000000000000000000000033518E00F6F6
      EA00ECECE000ECECE000ECECE000ECECE000ECECE0005F5FBE002F2FB3002F2F
      B3003131B5000A11A2003333B9006666CA00000000FFEBF0EFFFBDD1CEFFBACE
      CCFFB9CDCAFFB6CBC8FFB3C9C6FFB1C8C4FFAFC6C3FFAFC6C3FF8A9C9AFF0000
      00FF869A97FF75918EFF000000FF0000000000000000000000006060BF002B40
      CC001F5AFF001851FF001148FF000B3FFF000E3DF400656F95007D7D7D006871
      92000C25CA006060BF000000000000000000000000000000000000000000008A
      48006FD7B80012D5A90000CD9B0000CE9D0000D1A00000D2A10000D1A00000D1
      A0001DD8AE002FCCA300018A490000000000000000000000000033549200F7F7
      ED00EFEFE400EFEFE400EFEFE400EFEFE400EFEFE400EFEFE400EFEFE400EFEF
      E400F7F7ED00335492000000000000000000000000FFECF1F0FFC2D4D2FFC0D2
      D0FFBDD1CEFFBACECCFFB9CDCAFFB6CBC8FFB3C9C6FFB3C9C6FF5A6563FFE5E5
      E5FF586362FF789490FF000000FF00000000000000000000000010109F004570
      F2002662FF001F5AFF001951FF001B4CF4006A7392007D7D7D00687192000C39
      F300002FF90010109F0000000000000000000000000000000000000000000000
      0000008744005FD1AC0011DDAA0000CA9000008B490000874500009C5E0000A5
      680000C48A0004DDA80016BA830055A78200000000000000000033569500F9F9
      F000F2F2E800F2F2E800F2F2E800F2F2E800F2F2E800F2F2E800F2F2E800F2F2
      E800F9F9F000335695000000000000000000000000FFEBF0F0FFC5D6D3FFC4D6
      D3FFC2D4D2FFC0D2D0FFBDD1CEFFBACECCFFB9CDCAFFB9CDCAFF000000FFFFFF
      FFFF000000FF7C9693FF000000FF000000000000000000000000000099004F85
      FF002D6BFF002763FF00295DF3006D7692007D7D7D00697294000E3DF4000032
      FF000032FF000000990000000000000000000000000000000000000000000000
      000000000000008846004ED3A900129155000000000000000000000000000000
      00000E8F510000B5780000D89F00008B4B00000000000000000033599900FAFA
      F300F4F4ED00F4F4ED00F4F4ED00F4F4ED00F4F4ED00F4F4ED00F4F4ED00F4F4
      ED00FAFAF300335999000000000000000000000000FFF6F7F7FFC5D6D3FFC4D6
      D3FFC2D4D2FFC0D2D0FFBDD1CEFFBACECCFF869593FF202423FF13252DFFB0BB
      BDFF09181DFF303A39FF000000FF00000000000000000000000000009900598F
      FF003474FF00356EF300707991007D7D7D006A7595001B4DF4000B40FF000437
      FF000032FF0000009900000000000000000000000000008D4D00008B4B0066AC
      8C0000000000000000005CB58B00008D4C00000000001A955C00008C4A000000
      00000000000056B38800008B4B00008D4D000000000000000000335C9C00FBFB
      F600F7F7F100F7F7F100F7F7F100F7F7F100F7F7F100F7F7F100F7F7F100F7F7
      F100FBFBF600335C9C000000000000000000000000FFDDE7E6FFF3F6F5FFDDE7
      E6FFDDE7E6FFDDE7E6FFDBE5E4FFDDE7E6FF000000FF3390BAFF93D6ECFFA6E0
      F1FF0A87BEFF08709EFF000000FF00000000000000000000000010109F007094
      F200437DF100727C91007D7D7D006D789500285DF4001952FF001249FF000B40
      FF000431F20010109F00000000000000000000000000008B4B0000D89F0000B5
      78001A915800000000000000000000000000000000001191540053D4AA000088
      4600000000000000000000000000000000000000000000000000335D9F00FDFD
      F900FAFAF500FAFAF500FAFAF500FAFAF500FAFAF500FAFAF500FAFAF500FAFA
      F500FDFDF900335D9F000000000000000000000000FFF0F4F3FF99AEABFFADC0
      BDFFAABEBCFF9FB4B1FF5C716EFF000000FF000000FF000000FF3A95BDFF9FCA
      D6FF076690FF000000FF000000840000000000000000000000006060BF005F69
      C400767F8F007D7D7D006F7B9500346DF6002764FF00205BFF001952FF00134A
      FF000B24CC006060BF000000000000000000000000006CBC97001ABA860007DB
      A80000BE850000A06100009C5D0000874400008A490000C9900012DCAA0063D3
      AF000087440000000000000000000000000000000000000000003360A100FEFE
      FB00FCFCF900FCFCF900FCFCF900FCFCF900FCFCF900FCFCF900FCFCF900FCFC
      F900FEFEFB003360A100000000000000000000000039000000FFDEE5E4FFCAD5
      D3FFCAD5D3FF97A7A5FF000000FF0000003900000000333333FF4DB0DDFFAEE4
      F4FF097BADFF000000FF00000000000000000000000000000000D4D4E5006F6F
      81007D7D7D0073809700417EF6003575FF002E6CFF002764FF00215BFF00254D
      EC0010109F00DFDFF20000000000000000000000000000000000008A490035D0
      AA0020D7B1000DD3A7000DD4A7000DD4A8000DD3A7000ED1A40000CA9A0013D3
      A90073D7BB00008A4800000000000000000000000000000000003361A400FFFF
      FE00FEFEFC00DCDCDB00DCDCDB00DCDCDB00DCDCDB00DCDCDB00DCDCDB00FEFE
      FC00FFFFFE003361A40000000000000000000000000000000039000000FF0000
      00FF000000FF000000FF000000390000000000000000000000FF0C94D1FFA7E2
      F3FF0A87BEFF000000FF000000450000000000000000F0F0F0008E8E8E007D7D
      7D00676783005E69C6007094F200598FFF005086FF004571F2002B40CC001010
      9F00CFCFEC00000000000000000000000000000000000000000000000000018B
      490048C29C005CDCC6005BDEC8005ADEC7005BDDC6005CDBC30000C3990015CC
      AA0085DBC300008A4800000000000000000000000000000000003364A6000000
      0000A3A2A2008B8989009A989800A7A5A500ACAAAA00A19F9E0092909000A7A6
      A600000000003364A60000000000000000000000000000000000000000000000
      000000000000000000000000000000000045000000FF097BADFF3A95BDFF8AB7
      C4FF08709EFF065D83FF000000FF0000004500000000959595007D7D7D009999
      9900D9D9EB006060BF0010109F00000099000000990010109F006060BF00DFDF
      F200000000000000000000000000000000000000000000000000000000000000
      000056B186000089460000884400008744000089470000B58E001BC8AE0098DE
      CB000087420000000000000000000000000000000000000000003365A9006295
      D9003568AC00D2C7C700D8CFCF00E4DEDE00E4DEDE00D8CFCF00D2C7C7003568
      AC006295D9003365A90000000000000000000000000000000000000000000000
      0000000000000000000000000000000000FF0C94D1FF4DB0DDFF6ABDE1FFA7E2
      F3FF5AC0E4FF097BADFF076690FF000000FF00000000E5E5E500A2A2A200F7F7
      F700000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000239B6100A1E2D5000087
      4300000000000000000000000000000000000000000000000000668CBF003366
      AA003366AA002F579C007C85AB00DCDADA00DCDADA007C85AB002F579C003366
      AA003366AA00668CBF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000FF000000FF000000FF000000FF1616
      16FF000000FF000000FF000000FF000000FF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000012945800008B49000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000C8C8C800BABABA00BABABA00C8C8C800000000000000
      00000000000000000000000000000000000000000000000000002D2D2D004545
      4500444444004242420045454500424242004040400044444400474747004949
      4900454545002D2D2D0000000000000000000000000099999900222222000606
      060040404000D1D1D10000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000057575700B6B6
      B600CDCDCD00D8D8D800C9C9C900D4D4D400D0D0D000CFCFCF00D9D9D900CDCD
      CD00BABABA005454540000000000000000009E9E9E00080808008A8A8A00ADAD
      AD00404040000F0F0F00C8C8C800000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000993300009933
      0000993300009933000099330000993300009933000099330000993300009933
      0000993300009933000099330000000000000000000000000000999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      990099999900999999009999990000000000000000000000000050505000AEAE
      AE00B9B9B900E4E4E400AEAEAE00E4E4E400DCDCDC00D5D5D500CCCCCC00A2A2
      A200B0B0B0004E4E4E0000000000000000002B2B2B008B8B8B00000000000000
      0000F9F9F9004E4E4E000F0F0F00C8C8C8000000000000000000000000000000
      0000000000000000000000000000000000000000000099330000CC660000CC66
      000099330000E5E5E500CC66000099330000E5E5E500E5E5E500E5E5E5009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5E5009999
      9900CCCCCC00CCCCCC009999990000000000000000000000000056565600B6B6
      B600CDCDCD00D5D5D500CBCBCB00D8D8D800BFBFBF00D9D9D900D0D0D000B1B1
      B100B8B8B80054545400000000000000000011111100B4B4B400000000000000
      000000000000FAFAFA004F4F4F000F0F0F00D3D3D30000000000000000000000
      0000000000000000000000000000000000000000000099330000CC660000CC66
      000099330000E5E5E500CC66000099330000E5E5E500E5E5E500E5E5E5009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5E5009999
      9900CCCCCC00CCCCCC00999999000000000000000000000000002D2D2D005A5A
      5A00565656005353530056565600545454005353530054545400565656005454
      5400585858002D2D2D0000000000000000004343430049494900FBFBFB000000
      0000DFDFDF00B1B1B100FAFAFA00383838004949490000000000000000000000
      0000000000000000000000000000000000000000000099330000CC660000CC66
      000099330000E5E5E500CC66000099330000E5E5E500E5E5E500E5E5E5009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5E5009999
      9900CCCCCC00CCCCCC009999990000000000000000000000000057575700A5A5
      A5009B9B9B009B9B9B009D9D9D009D9D9D009B9B9B009B9B9B009B9B9B009C9C
      9C00A5A5A500545454000000000000000000CFCFCF000D0D0D0054545400FBFB
      FB00AAAAAA000101010093939300999999001313130000000000000000000000
      0000000000000000000000000000000000000000000099330000CC660000CC66
      000099330000E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E5009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC0099999900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E5009999
      9900CCCCCC00CCCCCC00999999000000000000000000000000008B8B8B00F5F5
      F500EBEBEB00F2F2F200E8E8E800E6E6E600EDEDED00F3F3F300EEEEEE00ECEC
      EC00F5F5F50087878700000000000000000000000000C6C6C6000D0D0D005454
      5400FBFBFB008B8B8B0001010100929292008686860000000000FEFEFE000000
      0000000000000000000000000000000000000000000099330000CC660000CC66
      0000CC660000993300009933000099330000993300009933000099330000CC66
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC00999999009999990099999900999999009999990099999900CCCC
      CC00CCCCCC00CCCCCC009999990000000000000000000000000083838300ECEC
      EC00ECECEC00BEBEBE006B6B6B007E7E7E00EEEEEE008F8F8F00D8D8D800E8E8
      E800ECECEC007E7E7E0000000000000000000000000000000000C6C6C6000D0D
      0D003C3C3C009F9F9F008A8A8A00010101009393930071717100020202003C3C
      3C00D0D0D0000000000000000000000000000000000099330000CC660000CC66
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC009999990000000000000000000000000083838300F0F0
      F000B9B9B9004E4E4E00BFBFBF0057575700E5E5E5009D9D9D0057575700D8D8
      D800EEEEEE007E7E7E000000000000000000000000000000000000000000CDCD
      CD003D3D3D000A0A0A00898989008B8B8B000101010091919100A1A1A1003A3A
      3A000F0F0F00CACACA0000000000000000000000000099330000CC660000CC66
      0000993300009933000099330000993300009933000099330000993300009933
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC00999999009999990099999900999999009999990099999900999999009999
      9900CCCCCC00CCCCCC009999990000000000000000000000000084848400F4F4
      F4004E4E4E008E8E8E00F9F9F9006B6B6B00ABABAB00F8F8F8004D4D4D008E8E
      8E00F4F4F4007E7E7E0000000000000000000000000000000000000000000000
      00000000000000000000000000008A8A8A008A8A8A000101010093939300F9F9
      F9004E4E4E000F0F0F00CBCBCB00000000000000000099330000CC6600009933
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099330000CC66000099330000000000000000000099999900CCCCCC009999
      9900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099999900CCCCCC009999990000000000000000000000000083838300EFEF
      EF00D8D8D8004E4E4E00BABABA00B9B9B9006E6E6E008787870070707000E9E9
      E900EFEFEF008080800000000000000000000000000000000000000000000000
      00000000000000000000000000000E0E0E00A4A4A4008B8B8B0001010100B1B1
      B100F9F9F9004E4E4E0010101000D8D8D8000000000099330000CC6600009933
      0000FFFFFF00993300009933000099330000993300009933000099330000FFFF
      FF0099330000CC66000099330000000000000000000099999900CCCCCC009999
      9900FFFFFF00999999009999990099999900999999009999990099999900FFFF
      FF0099999900CCCCCC009999990000000000000000000000000083838300ECEC
      EC00E9E9E900D4D4D400C1C1C100E5E5E5005A5A5A00A9A9A900E3E3E300D8D8
      D800E2E2E2007B7B7B0000000000000000000000000000000000000000000000
      00000000000000000000000000004343430043434300FBFBFB00AAAAAA00DFDF
      DF0000000000F9F9F9003E3E3E00575757000000000099330000CC6600009933
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099330000CC66000099330000000000000000000099999900CCCCCC009999
      9900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099999900CCCCCC009999990000000000000000000000000083838300ECEC
      EC00E3E3E300E6E6E600E7E7E700E5E5E500D5D5D500EAEAEA00606060005353
      5300575757001414140000000000000000000000000000000000000000000000
      0000000000000000000000000000D0D0D0000D0D0D0054545400FBFBFB000000
      000000000000000000009B9B9B00262626000000000099330000E5E5E5009933
      0000FFFFFF00993300009933000099330000993300009933000099330000FFFF
      FF00993300009933000099330000000000000000000099999900E5E5E5009999
      9900FFFFFF00999999009999990099999900999999009999990099999900FFFF
      FF0099999900999999009999990000000000000000000000000083838300ECEC
      EC00E3E3E300E3E3E300E3E3E300E3E3E300E6E6E600E7E7E70058585800BDBD
      BD00414141000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C6C6C6000D0D0D0054545400FBFB
      FB0000000000000000006A6A6A00444444000000000099330000CC6600009933
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099330000CC66000099330000000000000000000099999900CCCCCC009999
      9900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0099999900CCCCCC00999999000000000000000000000000008E8E8E00F8F8
      F800EFEFEF00EFEFEF00EFEFEF00EFEFEF00EFEFEF00F3F3F300606060003333
      3300000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000C6C6C6000D0D0D004343
      4300A5A5A5007676760005050500BBBBBB000000000099330000993300009933
      0000993300009933000099330000993300009933000099330000993300009933
      0000993300009933000099330000000000000000000099999900999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      99009999990099999900999999000000000000000000000000003F3F3F006464
      6400606060006060600060606000606060006060600064646400292929001212
      1200121212001212120000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000D3D3D3005151
      51002323230043434300BABABA00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000C9C9C9004040
      40000A0A0A000000000000000000000000000000000000000000000000000909
      09003E3E3E00C5C5C50000000000000000000000000000000000000000009933
      0000993300009933000099330000993300009933000099330000993300009933
      0000993300009933000099330000993300000000000000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999009999990099999900999999000000000000000000152634001937
      51001A364E0018344E001B374E0018344E0015334D0019364E001C384E00223A
      4E001937510016263300000000000000000000000000CFCFCF00050505000000
      000000000000000000000000000000000000000000000606060060606000A4A4
      A4004A4A4A0004040400C9C9C90000000000000000000000000099330000CC66
      0000CC66000099330000E5E5E500CC66000099330000E5E5E500E5E5E500E5E5
      E50099330000CC660000CC66000099330000000000000000000099999900CCCC
      CC00CCCCCC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5
      E50099999900CCCCCC00CCCCCC00999999000000000000000000083C6F000D78
      FF003995FF0053A8FF00318FFF0048A0FF00419AFF003E99FF0055AAFF003A95
      FF00137DFF00093B6B0000000000000000000000000057575700000000000000
      000000000000000000000000000000000000000000006B6B6B00000000000000
      000000000000363636004F4F4F0000000000000000009933000099330000CC66
      0000CC66000099330000E5E5E500CC66000099330000E5E5E500E5E5E500E5E5
      E50099330000CC660000CC66000099330000000000009999990099999900CCCC
      CC00CCCCCC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5
      E50099999900CCCCCC00CCCCCC00999999000000000000000000073967000C6D
      F3002D81E10087BDEC001E75DE0087BDEC0071B0EA0061A6E7004E9AE5000D67
      DC000F70F3000938630000000000000000000000000033333300000000000000
      00000000000000000000000000000000000000000000B6B6B600000000000000
      000000000000858585002B2B2B000000000099330000CC66000099330000CC66
      0000CC66000099330000E5E5E500CC66000099330000E5E5E500E5E5E500E5E5
      E50099330000CC660000CC6600009933000099999900CCCCCC0099999900CCCC
      CC00CCCCCC0099999900E5E5E500CCCCCC0099999900E5E5E500E5E5E500E5E5
      E50099999900CCCCCC00CCCCCC00999999000000000000000000073B6E000D77
      FF003A94FF004AA0FF003691FF0052A5FF002582F80055A8FF004098FF001171
      F3001079FF00093B6A0000000000000000000000000059595900000000000000
      0000000000000000000000000000000000000000000069696900000000000000
      00000000000034343400515151000000000099330000CC66000099330000CC66
      0000CC66000099330000E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E50099330000CC660000CC6600009933000099999900CCCCCC0099999900CCCC
      CC00CCCCCC0099999900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E50099999900CCCCCC00CCCCCC0099999900000000000000000003213B00083E
      73000A3C6C00063A6C000A3C6C00073A6C00063A6C00083B6C00093B6C00083B
      6C00073E730004203900000000000000000000000000D3D3D300070707000000
      0000000000000000000000000000000000000000000005050500585858009C9C
      9C004242420005050500CDCDCD000000000099330000CC66000099330000CC66
      0000CC660000CC66000099330000993300009933000099330000993300009933
      0000CC660000CC660000CC6600009933000099999900CCCCCC0099999900CCCC
      CC00CCCCCC00CCCCCC0099999900999999009999990099999900999999009999
      9900CCCCCC00CCCCCC00CCCCCC0099999900000000000000000036403E00667F
      7A005F7572005F75720060787400607874005F7572005F7572005F7572006077
      7200667F7A00343E3C0000000000000000000000000000000000CFCFCF004848
      4800121212000707070007070700070707000707070007070700070707001111
      110046464600CBCBCB00000000000000000099330000CC66000099330000CC66
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000CC660000CC660000CC6600009933000099999900CCCCCC0099999900CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC0099999900000000000000000052696900B1F6
      F600A4DEDC00ADEEEC00A0D6D6009CD1D100A6E0E000AFEEEE00A8E3E300A4DE
      DE00B1F6F6005065650000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099330000CC66000099330000CC66
      0000CC6600009933000099330000993300009933000099330000993300009933
      000099330000CC660000CC6600009933000099999900CCCCCC0099999900CCCC
      CC00CCCCCC009999990099999900999999009999990099999900999999009999
      990099999900CCCCCC00CCCCCC009999990000000000000000004E626200A5DE
      DE00A5E0E000749A9A00405050004C5E5F00A8E3E300546C6D008CBABA00A0D5
      D500A5DEDE004C5F5F0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099330000CC66000099330000CC66
      000099330000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0099330000CC6600009933000099999900CCCCCC0099999900CCCC
      CC0099999900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0099999900CCCCCC009999990000000000000000004E626200AAEA
      EA00709494002F3B3B00759C9C00344141009CD0D0005D797900344141008EBB
      BB00A8E3E3004C5F5F0000000000000000000000000000000000CBCBCB004646
      4600111111000707070007070700070707000707070007070700070707001212
      120048484800D0D0D000000000000000000099330000CC66000099330000CC66
      000099330000FFFFFF0099330000993300009933000099330000993300009933
      0000FFFFFF0099330000CC6600009933000099999900CCCCCC0099999900CCCC
      CC0099999900FFFFFF0099999900999999009999990099999900999999009999
      9900FFFFFF0099999900CCCCCC009999990000000000000000004E636300B1F3
      F3002F3B3B00546A6B00BDFFFF004050500066858500B9FFFF002E393900546C
      6C00B0F6F3004C5F5F00000000000000000000000000CDCDCD00050505004242
      42009C9C9C005858580005050500000000000000000000000000000000000000
      00000000000007070700D3D3D3000000000099330000CC66000099330000CC66
      000099330000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0099330000CC6600009933000099999900CCCCCC0099999900CCCC
      CC0099999900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0099999900CCCCCC009999990000000000000000004E626200A9E5
      E5008CBABA002F3A3A007195950070949400425353005065650043535400A1D8
      D800A9E3E5004C60600000000000000000000000000051515100343434000000
      0000000000000000000069696900000000000000000000000000000000000000
      00000000000000000000595959000000000099330000CC66000099330000E5E5
      E50099330000FFFFFF0099330000993300009933000099330000993300009933
      0000FFFFFF0099330000993300009933000099999900CCCCCC0099999900E5E5
      E50099999900FFFFFF0099999900999999009999990099999900999999009999
      9900FFFFFF0099999900999999009999990000000000000000004E626200A5DE
      DE00A1D8D80089B4B400779D9D009CD0D000364444006585850099CCCC008FBB
      BA0099CBC9004A5C5C000000000000000000000000002B2B2B00858585000000
      00000000000000000000B6B6B600000000000000000000000000000000000000
      00000000000000000000333333000000000099330000CC66000099330000CC66
      000099330000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0099330000CC6600009933000099999900CCCCCC0099999900CCCC
      CC0099999900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0099999900CCCCCC009999990000000000000000004E626200A5DE
      DE0099CCCC009DD1D1009ED5D5009CD0D00089B6B600A2DADA00394848002F3E
      4100314144000C0F0F000000000000000000000000004F4F4F00363636000000
      000000000000000000006B6B6B00000000000000000000000000000000000000
      00000000000000000000575757000000000099330000E5E5E500993300009933
      0000993300009933000099330000993300009933000099330000993300009933
      00009933000099330000993300009933000099999900E5E5E500999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      99009999990099999900999999009999990000000000000000004E626200A5E0
      E00099CCCC0099CCCC0099CCCC0099CCCC009DD1D100A0D3D30034434400629D
      A8002332340000000000000000000000000000000000C9C9C900040404004A4A
      4A00A4A4A4006060600006060600000000000000000000000000000000000000
      00000000000005050500CFCFCF000000000099330000CC66000099330000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009933
      0000CC66000099330000000000000000000099999900CCCCCC0099999900FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009999
      9900CCCCCC009999990000000000000000000000000000000000546B6B00B6FF
      FF00A8E5E500A8E5E500A8E5E500A8E5E500A8E5E500B0F0F000384849001C27
      2900000000000000000000000000000000000000000000000000C5C5C5003E3E
      3E00090909000000000000000000000000000000000000000000000000000A0A
      0A0040404000C9C9C90000000000000000009933000099330000993300009933
      0000993300009933000099330000993300009933000099330000993300009933
      0000993300009933000000000000000000009999990099999900999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      99009999990099999900000000000000000000000000000000002D333300444F
      4F00414C4C00414C4C00414C4C00414C4C00414C4C00444F4F00202323001212
      1200121212001212120000000000000000000000000000000000000000000000
      0000000000009999990099999900CCCCCC0099999900CCCCCC00999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000993300006600000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000999999009999990000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC0099999900CCCCCC0099999900999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009933000099330000CC6600009933000066000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009999990099999900CCCCCC009999990099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009999990099999900CCCCCC0099999900CCCCCC00999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000606
      0600010101002A2A2A0000000000000000000000000000000000000000009933
      00009933000099330000E5E5E500E5E5E500CCCCCC0099330000660000000000
      0000000000000000000000000000000000000000000000000000000000009999
      99009999990099999900E5E5E500E5E5E500CCCCCC0099999900999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009999990099999900999999009999990099999900999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000121212000000
      0000000000000F0F0F0000000000000000000000000099330000993300009933
      0000E5E5E500E5E5E500E5E5E500E5E5E500CCCCCC00CCCCCC00993300006600
      0000000000000000000000000000000000000000000099999900999999009999
      9900E5E5E500E5E5E500E5E5E500E5E5E500CCCCCC00CCCCCC00999999009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000000000000099999900FFFFFF00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000010101000101
      0100010101000101010001010100010101000202020000000000202020000000
      0000000000000000000001010100000000009933000099330000E5E5E500E5E5
      E500E5E5E500E5E5E50099330000993300009999990099999900CCCCCC009933
      0000660000000000000000000000000000009999990099999900E5E5E500E5E5
      E500E5E5E500E5E5E50099999900999999009999990099999900CCCCCC009999
      9900999999000000000000000000000000000000000000000000000000000000
      0000000000000000000099999900FFFFFF00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000F0F0F000000
      00000000000012121200000000000000000099330000E5E5E500E5E5E500E5E5
      E500993300009933000099330000993300006600000080808000999999009999
      99009933000066000000000000000000000099999900E5E5E500E5E5E500E5E5
      E500999999009999990099999900999999009999990080808000999999009999
      9900999999009999990000000000000000000000000000000000000000000000
      0000000000000000000099999900FFFFFF00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000002C2C2C00000000000000000099330000E5E5E500993300009933
      000099330000CC660000CC660000CC6600009933000066000000666666009999
      99009999990099330000660000000000000099999900E5E5E500999999009999
      990099999900CCCCCC00CCCCCC00CCCCCC009999990099999900666666009999
      9900999999009999990099999900000000000000000000000000000000000000
      0000000000000000000099999900FFFFFF00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000993300009933000099330000CC66
      0000CC6600009933000099330000CC660000CC66000099330000660000006666
      660099999900808080009933000066000000999999009999990099999900CCCC
      CC00CCCCCC009999990099999900CCCCCC00CCCCCC0099999900999999006666
      6600999999008080800099999900999999000000000000000000000000000000
      0000000000000000000099999900FFFFFF00CCCCCC0099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099330000FF990000CC660000CC66
      0000CC66000000FFFF0033CCFF00993300009933000099330000993300006600
      00006666660099999900993300006600000099999900E5E5E500CCCCCC00CCCC
      CC00CCCCCC00E5E5E500E5E5E500999999009999990099999900999999009999
      9900666666009999990099999900999999000000000000000000000000000000
      0000000000000000000099999900999999009999990099999900000000000000
      0000000000000000000000000000999999000000000000000000000000000000
      0000000000000000000001010100000000000000000001010100000000000000
      0000000000000000000000000000000000000000000099330000FF990000CC66
      0000CC660000CC660000CC66000000FFFF0000FFFF0033CCFF00336699009933
      0000660000006666660099330000000000000000000099999900E5E5E500CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00E5E5E500E5E5E500E5E5E500999999009999
      9900999999006666660099999900000000000000000099999900999999000000
      0000000000009999990099999900999999009999990099999900999999000000
      0000000000000000000099999900999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000099330000FF99
      0000CC660000CC660000CC660000CC660000CC66000033CCFF0000FFFF003366
      990099330000660000009933000000000000000000000000000099999900E5E5
      E500CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00E5E5E500E5E5E5009999
      9900999999009999990099999900000000009999990099999900999999009999
      99009999990099999900CCCCCC00CCCCCC00CCCCCC0099999900999999009999
      99009999990099999900CCCCCC00999999000000000000000000020202000202
      0200020202000101010000000000000000000000000000000000020202000202
      0200020202000202020001010100000000000000000000000000000000009933
      0000FF990000CC660000CC66000000FFFF0000FFFF0000FFFF003399CC003366
      9900993300009933000066000000660000000000000000000000000000009999
      9900E5E5E500CCCCCC00CCCCCC00E5E5E500E5E5E500E5E5E500999999009999
      99009999990099999900999999009999990099999900FFFFFF00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00999999009999
      990099999900CCCCCC0099999900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000099330000FF990000CC660000CC6600003366990033669900660066009933
      0000993300009933000066000000000000000000000000000000000000000000
      000099999900E5E5E500CCCCCC00CCCCCC009999990099999900999999009999
      99009999990099999900999999000000000099999900FFFFFF00CCCCCC00CCCC
      CC00FFFFFF00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC009999990000000000000000000000000000000000000000000000
      0000000000000000000000000000070707000707070000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099330000FF990000CC660000CC660000CC660000993300009933
      0000660000000000000000000000000000000000000000000000000000000000
      00000000000099999900E5E5E500CCCCCC00CCCCCC00CCCCCC00999999009999
      99009999990000000000000000000000000099999900FFFFFF00FFFFFF009999
      990099999900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009999
      9900999999000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000099330000FF990000CC66000099330000660000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000099999900E5E5E500CCCCCC0099999900999999000000
      0000000000000000000000000000000000000000000099999900999999000000
      0000000000009999990099999900999999009999990099999900999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000993300006600000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000999999009999990000000000000000000000
      000000000000000000000000000000000000000000005050500054545400FCFC
      FC00000000000000000000000000000000000000000000000000000000000000
      0000FCFCFC005454540050505000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009933000099330000CC66000099330000CC660000993300000000
      0000000000000000000000000000000000004B4B4B000000000000000000FAFA
      FA00000000000000000000000000000000000000000000000000000000000000
      0000FAFAFA0000000000000000004B4B4B000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099330000CC66000099330000CC66000099330000993300000000
      0000000000000000000000000000000000004F4F4F000000000000000000FAFA
      FA00000000000000000000000000000000000000000000000000000000000000
      0000FAFAFA0000000000000000004F4F4F00C6A18C00C38E6800C08B6600BE88
      6400BB856100B9835F00B47E5C00B27C5A00B17B5800AE795700AD765600AB75
      5400A9735300A9715100C6A18C00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009933000099330000CC66000099330000CC660000993300000000
      000000000000000000000000000000000000FAFAFA0085858500A9A9A9000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000A9A9A90085858500FAFAFA00C8926C0000000000000000000000
      00000000000000000000DCA77B00000000000000000000000000000000000000
      00000000000000000000A972510000000000000000000000000000000000C4C4
      C400777777005D5D5D005C5C5C005C5C5C005C5C5C005C5C5C005D5D5D006B6B
      6B00B0B0B0000000000000000000000000000000000000000000000000000000
      0000000000009999990099330000993300009933000099330000999999000000
      00000000000000000000000000000000000000000000000000003F3F3F000000
      0000000000000000000000000000B4B4B400B4B4B40000000000000000000000
      0000000000003F3F3F000000000000000000CA946E0000000000FF8E2B009595
      95008787870000000000DCA77B0000000000FEFEFC00FEFEFC00FEFEFA00FEFE
      FA00FCFCF90000000000AA735300000000000000000000000000C5C5C5002626
      2600010101000202020002020200020202000202020002020200020202000101
      0100101010009E9E9E0000000000000000000000000000000000000000000000
      0000000000000000000099999900FFFFFF00FFCCCC0066666600000000000000
      0000000000000000000000000000000000000000000000000000525252000000
      000000000000BBBBBB0000000000000000000000000000000000BBBBBB000000
      000000000000525252000000000000000000CC976F0000000000000000000000
      00000000000000000000DCA77B0000000000FDFDFA00FDFDFA00FDFDFA00FCFC
      F700FBFBF60000000000AC7554000000000000000000D0D0D000010101007F7F
      7F00E7E7E700F7F7F700F7F7F700F7F7F700F7F7F700F9F9F90000000000FBFB
      FB00A8A8A800020202009D9D9D00FCFCFC000000000000000000000000000000
      0000000000000000000099999900FFFFFF00FFCCCC0066666600000000000000
      0000000000000000000000000000000000000000000000000000000000007171
      710000000000000000005C5C5C0000000000000000005C5C5C00000000000000
      000071717100000000000000000000000000D19C730000000000FF9E3A009F9F
      9F009999990000000000DCA77B0000000000FDFDF800FBFBF900FBFAF700FBFA
      F600FBF8F40000000000B07A580000000000BBBBBB00252525007F7F7F000000
      00000000000000000000000000000000000000000000F3F3F3005F5F5F004A4A
      4A00E9E9E900C5C5C50000000000A9A9A9000000000000000000000000000000
      0000000000000000000099999900FFFFFF00FFCCCC0066666600000000000000
      0000000000000000000000000000000000000000000080808000000000000000
      00000000000000000000E5E5E5000000000000000000E5E5E500000000000000
      000000000000000000008080800000000000D49E750000000000000000000000
      00000000000000000000DCA77B0000000000FBF9F700FBF9F500FBF8F400FBF7
      F200FBF5F20000000000B27C5A00000000006565650001010100E2E2E2000000
      000000000000000000000000000000000000000000006A6A6A00000000000000
      00002F2F2F0000000000141414005D5D5D000000000000000000000000000000
      0000000000000000000099999900FFFFFF00FFCCCC0066666600000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FAFAFA00272727000000000000000000000000000000000027272700FAFA
      FA0000000000000000000000000000000000D5A0760000000000FFBF6800AFAF
      AF00AAAAAA0000000000DCA77B0000000000FBF8F400FBF7F300FBF5F200FAF3
      EF00F8F2EC0000000000B57E5C00000000006464640001010100E1E1E1000000
      000000000000000000000000000000000000000000006C6C6C00000000000000
      00003232320000000000131313005C5C5C000000000000000000000000000000
      0000000000000000000099999900FFFFFF00FFCCCC0066666600000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000F6F6F6000000000000000000F6F6F600000000000000
      000000000000000000000000000000000000D8A2790000000000000000000000
      00000000000000000000DCA77B0000000000FBF6F100F8F4EE00F7F2EB00F7F0
      EA00F6ECE80000000000B7815E0000000000BCBCBC0026262600808080000000
      00000000000000000000000000000000000000000000F3F3F300626262005050
      5000E7E7E700C4C4C40000000000ABABAB000000000000000000000000000000
      0000000000000000000066666600666666006666660066666600000000000000
      0000000000000000000000000000666666000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000D9A3790000000000FFBF6800BDBD
      BD00B7B7B70000000000DCA77B0000000000F7F3ED00F6EFEA00F5EBE700F3EA
      E400F2E7DE0000000000BA8560000000000000000000D1D1D100000000008080
      8000E6E6E600F5F5F500F5F5F500F5F5F500F5F5F500F7F7F70000000000FBFB
      FB00A8A8A800020202009C9C9C00FDFDFD000000000066666600666666000000
      0000000000006666660099999900999999009999990099999900666666000000
      0000000000000000000066666600666666000000000000000000000000000000
      00000000000000000000000000000E0E0E000E0E0E0000000000000000000000
      000000000000000000000000000000000000DBA47A0000000000000000000000
      00000000000000000000DCA77B00000000000000000000000000000000000000
      00000000000000000000BD876300000000000000000000000000C6C6C6002626
      2600000000000101010001010100010101000101010001010100000000000101
      0100101010009E9E9E0000000000000000009999990099999900999999006666
      66006666660099999900CCCCCC00CCCCCC00CCCCCC0099999900666666006666
      66006666660066666600CCCCCC00666666000000000000000000000000000000
      00000000000000000000000000009F9F9F009F9F9F0000000000000000000000
      000000000000000000000000000000000000DCA77B00DCA77B00DCA77B00DCA7
      7B00DCA77B00DCA77B00DCA77B00DCA77B00DCA77B00DCA77B00DCA77B00DCA7
      7B00DCA77B00DCA77B00C08B660000000000000000000000000000000000D6D6
      D600888888006565650065656500656565006565650065656500656565007B7B
      7B00C3C3C30000000000000000000000000099999900FFFFFF00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00999999009999
      990099999900CCCCCC0066666600000000000000000000000000000000000000
      000000000000000000009F9F9F0000000000000000009F9F9F00000000000000
      000000000000000000000000000000000000DDAD8600E8B99200E8B99200E8B9
      9200E8B99200E8B99200E8B99200E8B99200E8B99200E8B99200E8B99200E8B9
      9200E8B99200E8B99200C1917000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900FFFFFF00CCCCCC00CCCC
      CC00FFFFFF00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00CCCCCC009999990000000000000000000000000000000000000000000000
      00000000000000000000ADADAD000000000000000000ADADAD00000000000000
      000000000000000000000000000000000000DBC3B600DEB49200DCA77B00DCA6
      7A00DAA47A00D8A27900D5A07600D49E7500D29D7300CF9A7200CE997000CB96
      6F00C9946C00C79E8000DBC3B600000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900FFFFFF00FFFFFF009999
      990099999900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF009999
      9900999999000000000000000000000000000000000000000000000000000000
      00000000000000000000F6F6F6000000000000000000F6F6F600000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900999999000000
      0000000000009999990099999900999999009999990099999900999999000000
      00000000000000000000000000000000000000000000A6A6A600888888008888
      8800888888008888880088888800888888008888880088888800888888008888
      880088888800A6A6A60000000000000000000000000000000000000000000000
      000000000000000000004E8B42000F6503001A831E0015721C00166D1C00176B
      1C00186B1C00238223003D7A2200000000000000000000000000000000000000
      00000000000000000000626262002D2D2D00454545003C3C3C003A3A3A003A3A
      3A003A3A3A00494949004B4B4B00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000088888800FEFEFE00FEFE
      FE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFE
      FE00FEFEFE0088888800000000000000000000000000E1ACA400DA968B00D994
      8B00D8938B00D7908B002872160006660D002EB9530038D46B0037D0680036D0
      680036D3690037C862001D4B1A000000000000000000B4B4B4009F9F9F009E9E
      9E009E9E9E009C9C9C003E3E3E002E2E2E006D6D6D00818181007E7E7E007E7E
      7E007F7F7F007A7A7A002E2E2E00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000088888800FEFEFE00E3E3
      E300E7E7E700EEEEEE00B0B0B0005D5D5D005D5D5D00EEEEEE00E3E3E300E3E3
      E300FEFEFE005D5D5D005D5D5D00B3B3B30000000000D65C2B00F0B08600F0B1
      8C00EFAF8A00EFA8850033752B000E7D23004CC2690060DE86005CDA82005CDA
      82005DDC84005AD27B0025592200000000000000000061616100ACACAC00AFAF
      AF00ADADAD00A8A8A8004A4A4A003F3F3F00818181009A9A9A00969696009696
      9600989898009191910039393900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000088888800FEFEFE00C4C4
      C400D5D5D500A1A1A1005D5D5D00B6B6B6005D5D5D00D5D5D500D5D5D500D5D5
      D500FEFEFE005D5D5D00ACACAC005D5D5D0000000000D9693B00FEFBF7000000
      00000000000000000000217F280010741C006BCF7E0087EDA10083E99C0083E9
      9C0084EB9E007DE093002E6B2A0000000000000000006D6D6D00FAFAFA000000
      00000000000000000000484848003A3A3A0097979700B5B5B500B1B1B100B1B1
      B100B2B2B200A9A9A90045454500000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000C0C0C0000000
      0000008080000000000000000000000000000000000088888800FEFEFE00EEEE
      EE00B0B0B0005D5D5D00B3B3B300ADADAD005D5D5D005D5D5D005D5D5D005D5D
      5D00585858006D6D6D00E4E4E4005A5A5A0000000000DA6C3E00FCF4EC000000
      000000000000FDFAFA0027872B000069000087CB8C00AAE8B200A6E6AF00A7E7
      AF00A9E7B000A6E3AE0000580000000000000000000070707000F2F2F2000000
      000000000000FBFBFB004E4E4E002A2A2A00A3A3A300C4C4C400C1C1C100C2C2
      C200C3C3C300C0C0C00023232300000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000C0C0C0000000
      0000008080000000000000000000000000000000000088888800FEFEFE00A1A1
      A1005D5D5D00B3B3B3007F7F7F00ADADAD00D0D0D000D0D0D000D0D0D000D0D0
      D000DCDCDC00DCDCDC00BEBEBE005D5D5D0000000000DB6F4100FCF5ED000000
      000000000000FAF9F9002B8F2E007BDF8F00007D000000830000005D0000286C
      27002A712900456033006B462700000000000000000073737300F3F3F3000000
      000000000000F9F9F90054545400A7A7A7003232320034343400252525004343
      4300464646004949490041414100000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000000000000000
      0000008080000000000000000000000000000000000088888800FEFEFE005D5D
      5D00B4B4B4007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F7F007F7F
      7F009B9B9B00E0E0E0005D5D5D00B8B8B80000000000DD724500FCF4ED000000
      000000000000F8FAF8002A812B009AE1A3009BE3A4009BDBA200306C2C00AAC3
      A600ACC5A800B29E6E00C8625500000000000000000076767600F3F3F3000000
      000000000000F9F9F9004D4D4D00B8B8B800BABABA00B6B6B60046464600B2B2
      B200B4B4B4008F8F8F0071717100000000000000000000000000000000000000
      0000008080000080800000808000008080000080800000808000008080000080
      8000008080000000000000000000000000000000000088888800FEFEFE00D5D5
      D5005D5D5D00E4E4E400CACACA00DBDBDB00EDEDED00EDEDED00EDEDED00EDED
      ED00EAEAEA005D5D5D00B8B8B8000000000000000000DF754800FCF5EE000000
      00000000000000000000BED9BE004F8D4D00558F4D004B834900D7E4D5000000
      000000000000FFD1B700C7625100000000000000000079797900F4F4F4000000
      00000000000000000000C9C9C900676767006969690061616100DBDBDB000000
      000000000000D0D0D00070707000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000000000000080
      8000008080000000000000000000000000000000000088888800FEFEFE00F0F0
      F000EFEFEF005D5D5D00E3E3E300E7E7E7005D5D5D005D5D5D005D5D5D005D5D
      5D005D5D5D0072727200000000000000000000000000E0784C00FDF5EF000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000F6CCB000C863510000000000000000007C7C7C00F4F4F4000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000C9C9C90070707000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000008080000000000000000000000000000000000088888800FEFEFE00BABA
      BA00D5D5D500D5D5D5005D5D5D00E8E8E8005D5D5D00EEEEEE00E3E3E300DEDE
      DE00ECECEC0088888800000000000000000000000000E27B5000FDF7F1000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000F7CEB300C965510000000000000000007F7F7F00F6F6F6000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000CBCBCB0071717100000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000008080000000000000000000000000000000000088888800FEFEFE00E8E8
      E800E7E7E700F0F0F000F0F0F0005D5D5D005D5D5D00E8E8E800BCBCBC00C2C2
      C200D0D0D00088888800000000000000000000000000E37D5200F4EFEC00F8FA
      FE00F8F9FD00F8F9FD00F7F9FD00F8FAFE00F8FAFE00F7FAFE00F8FAFF00F8FA
      FF00F8FEFF00F1CAB300CB685100000000000000000081818100EFEFEF00FBFB
      FB00FAFAFA00FAFAFA00FAFAFA00FBFBFB00FBFBFB00FAFAFA00FBFBFB00FBFB
      FB00FCFCFC00C9C9C90072727200000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000000000000000000000000000000000000000000088888800FEFEFE00BABA
      BA00BABABA00BABABA00D5D5D500D5D5D500F0F0F000CBCBCB009D9D9D009393
      93008888880088888800000000000000000000000000E6805400E2BCA200E2BB
      A400E1B9A000E1B89E00E1B69D00E1B59B00E1B49900E1B39700E1B29500E1B1
      9300E1B39800E69D6E00CE6C5500000000000000000082828200B9B9B900BABA
      BA00B7B7B700B6B6B600B5B5B500B3B3B300B2B2B200B1B1B100B0B0B000AFAF
      AF00B1B1B1009999990077777700000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000C0C0C0000000000000000000000000000000000088888800FEFEFE00EBEB
      EB00EAEAEA00EAEAEA00E9E9E900E8E8E800E8E8E8009D9D9D00EFEFEF00E4E4
      E400E4E4E40088888800000000000000000000000000F1906200FFCF9300FFC2
      8200FFBE7C00FFBA7500FFB66E00FFB16700FFAD6000FFA95900FFA55200FFA0
      4B00FF9D4400FF994000CF6F5800000000000000000091919100C1C1C100B5B5
      B500B1B1B100ACACAC00A8A8A800A3A3A3009F9F9F009A9A9A00969696009191
      91008D8D8D008A8A8A0079797900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000088888800FEFEFE00BABA
      BA00BABABA00BABABA00BABABA00BABABA00C6C6C600AEAEAE00E4E4E400FBFB
      FB0088888800DFDFDF00000000000000000000000000E4744B00F0916300EE89
      5A00EC855500EB815000E97D4B00E77A4700E5764200E4723D00E26E3900E06B
      3400DE673000DC622A00D27F6F0000000000000000007A7A7A00929292008A8A
      8A0086868600838383007F7F7F007B7B7B007777770074747400707070006C6C
      6C00696969006464640089898900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000088888800FEFEFE00FEFE
      FE00FEFEFE00FEFEFE00FEFEFE00FBFBFB00FBFBFB00F6F6F600FBFBFB008888
      8800DFDFDF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000C4C4C400888888008888
      880088888800888888008888880088888800888888008888880088888800DFDF
      DF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000E9E9E900E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500E5E5E500E5E5E500E9E9E9000000000000000000E9E9E900E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500E5E5E500E5E5E500E9E9E9000000000000000000A4A7A600858A8800858A
      8800858A8800858A8800858A8800858A8800858A8800858A8800858A8800858A
      8800858A8800A4A7A60000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A7A7A7008E8E8E008E8E8E008E8E
      8E008E8E8E008E8E8E008E8E8E008E8E8E008E8E8E008E8E8E008E8E8E009292
      9200929292009292920092929200AAAAAA00A7A7A7008E8E8E008E8E8E008E8E
      8E008E8E8E008E8E8E008E8E8E008E8E8E008E8E8E008E8E8E008E8E8E009292
      9200929292009292920092929200AAAAAA0000000000858A8800FEFEFE00FEFE
      FE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFEFE00FEFE
      FE00FEFEFE00858A8800000000000000000000000000CCCCCC0000000000CCCC
      CC0000000000CCCCCC0000000000CCCCCC0000000000CCCCCC0000000000CCCC
      CC0000000000CCCCCC0000000000000000009A9A9A00EBEBEB00E7E7E700E7E7
      E700E7E7E700E7E7E700E7E7E700E7E7E700E7E7E700E7E7E700EBEBEB009F9F
      9F00EBEBEB00E7E7E700EBEBEB009F9F9F009A9A9A00EBEBEB00E7E7E700E7E7
      E700E7E7E700E7E7E700E7E7E700E7E7E700E7E7E700E7E7E700EBEBEB009F9F
      9F00EBEBEB00E7E7E700EBEBEB009F9F9F0000000000858A8800FEFEFE00E3E3
      E300E7E7E700EEEEEE008FC9C200008FA000008FA000EEEEEE00E3E3E300E3E3
      E300FEFEFE00008FA000008FA0008ECFC6000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A9A9A900E9E9E900B4B4B400BDBD
      BD00C5C5C500BDBDBD00CECECE00E1E1E100E1E1E100E1E1E100E9E9E900A8A8
      A800E9E9E90058585800E9E9E900A8A8A800A9A9A900E9E9E900B4B4B400BDBD
      BD00C5C5C500BDBDBD00CECECE00E1E1E100E1E1E100E1E1E100E9E9E900A8A8
      A800E9E9E90058585800E9E9E900A8A8A80000000000858A8800FEFEFE00C1C7
      C500D3D7D5007FBBB300008FA00051EDFF00008FA000D3D7D500D3D7D500D3D7
      D500FEFEFE00008FA00039EAFF00008FA00000000000CCCCCC00000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      990000000000CCCCCC000000000000000000AEAEAE00EAEAEA00D0D0D000D0D0
      D000D0D0D000D0D0D000D0D0D000E3E3E300E3E3E300E3E3E300EAEAEA00AAAA
      AA00EDEDED00EAEAEA00EDEDED00AAAAAA00AEAEAE00EAEAEA00D0D0D000D0D0
      D000D0D0D000D0D0D000D0D0D000E3E3E300E3E3E300E3E3E300EAEAEA00AAAA
      AA00EDEDED00EAEAEA00EDEDED00AAAAAA0000000000858A8800FEFEFE00EEEE
      EE008FC9C200008FA0004BECFF003DEAFF00008FA000008FA000008FA000008F
      A000008F950000A8BC00BFF8FF00008B9C000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      990000000000000000000000000000000000B2B2B200EDEDED008E8E8E00A5A5
      A500A5A5A5008E8E8E00D3D3D300E6E6E600C2C2C200B9B9B900EDEDED00AAAA
      AA00B2B2B200B2B2B200B2B2B200AAAAAA00B2B2B200EDEDED008E8E8E00A5A5
      A500A5A5A5008E8E8E00D3D3D300E6E6E600C2C2C200B9B9B900EDEDED00AAAA
      AA00B2B2B200B2B2B200B2B2B200AAAAAA0000000000858A8800FEFEFE007FBB
      B300008FA0004BECFF0000C5DC003DEAFF0090F2FF0090F2FF0090F2FF0090F2
      FF00ADF5FF00AFF5FD0065EFFF00008FA00000000000CCCCCC00000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      990000000000CCCCCC000000000000000000B5B5B500EFEFEF00E9E9E900E9E9
      E900E9E9E900E9E9E900E9E9E900E9E9E900D5D5D500D5D5D500EFEFEF00AAAA
      AA00B6B6B600B6B6B600B6B6B600AAAAAA00B5B5B500EFEFEF00E9E9E900E9E9
      E900E9E9E900E9E9E900E9E9E900E9E9E900D5D5D500D5D5D500EFEFEF00AAAA
      AA00B6B6B600B6B6B600B6B6B600AAAAAA0000000000858A8800FEFEFE00008F
      A0004DECFF0000C5DC0000C5DC0000C5DC0000C5DC0000C5DC0000C5DC0000C5
      DC0011E6FF00B5F7FF00008FA00095D2CA000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      990000000000000000000000000000000000B9B9B900F1F1F100BFBFBF00C7C7
      C700D0D0D000C7C7C700D8D8D800ECECEC00BFBFBF00C7C7C700F1F1F100AAAA
      AA00B9B9B900B9B9B900B9B9B900AAAAAA00B9B9B900F1F1F100BFBFBF00C7C7
      C700D0D0D000C7C7C700D8D8D800ECECEC00BFBFBF00C7C7C700F1F1F100AAAA
      AA00B9B9B900B9B9B900B9B9B900AAAAAA0000000000858A8800FEFEFE00D3D7
      D500008FA000BFF8FF0081F2FF00A9F6FF00D5FBFF00D5FBFF00D5FBFF00D5FB
      FF00CDFAFF00008FA00095D2CA000000000000000000CCCCCC00000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      990000000000CCCCCC000000000000000000BCBCBC00F3F3F300DBDBDB00DBDB
      DB00DBDBDB00DBDBDB00DBDBDB00EFEFEF00DBDBDB00DBDBDB00F3F3F300AAAA
      AA00BCBCBC00BCBCBC00BCBCBC00AAAAAA00BCBCBC00F3F3F300DBDBDB00DBDB
      DB00DBDBDB00DBDBDB00DBDBDB00EFEFEF00DBDBDB00DBDBDB00F3F3F300AAAA
      AA00BCBCBC00BCBCBC00BCBCBC00AAAAAA0000000000858A8800FEFEFE00F0F0
      F000EFEFEF00008FA000BDF8FF00C5F9FF00008FA000008FA000008FA000008F
      A000008FA0004E8D850000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      990000000000000000000000000000000000BFBFBF00F6F6F60091919100C4C4
      C400AAAAAA00AAAAAA0091919100F2F2F200BBBBBB00CCCCCC00F6F6F600AAAA
      AA00BFBFBF00BFBFBF00BFBFBF00AAAAAA00BFBFBF00F6F6F60091919100C4C4
      C400AAAAAA00AAAAAA0091919100F2F2F200BBBBBB00CCCCCC00F6F6F600AAAA
      AA00BFBFBF00BFBFBF00BFBFBF00AAAAAA0000000000858A8800FEFEFE00B6BD
      BA00D3D7D500D3D7D500008FA000C9F9FF00008FA000EEEEEE00E2E4E300DEDF
      DE00ECECEC00858A8800000000000000000000000000CCCCCC00000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      990000000000CCCCCC000000000000000000C2C2C200F8F8F800F5F5F500F5F5
      F500F5F5F500F5F5F500F5F5F500F5F5F500F5F5F500F5F5F500F8F8F800AAAA
      AA00C2C2C200C2C2C200C2C2C200AAAAAA00C2C2C200F8F8F800F5F5F500F5F5
      F500F5F5F500F5F5F500F5F5F500F5F5F500F5F5F500F5F5F500F8F8F800AAAA
      AA00C2C2C200C2C2C200C2C2C200AAAAAA0000000000858A8800FEFEFE00E8E8
      E800E8E7E700F0F0F000F0F0F000008FA000008FA000E8E9E800BBBDBC00C1C3
      C200D0D0D000858A880000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      990000000000000000000000000000000000C4C4C400FAFAFA0003599D000961
      A5001A77BB002587CB001976BA002587CB001570B400055CA000FAFAFA00B1B1
      B100B1B1B100B1B1B100B1B1B100B1B1B100C4C4C400FAFAFA00525252005959
      59006D6D6D007B7B7B006C6C6C007B7B7B006767670054545400FAFAFA00B1B1
      B100B1B1B100B1B1B100B1B1B100B1B1B10000000000858A8800FEFEFE00B6BD
      BA00B6BDBA00B6BDBA00D3D7D500D3D7D500F0F0F000CACCCB009A9F9D009095
      9300858A8800858A8800000000000000000000000000CCCCCC00000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      990000000000CCCCCC000000000000000000C6C6C600FCFCFC0003599D00C2D8
      E90089B9DB0090C3E500B6CFE0002587CB001570B400055CA000FCFCFC00BDBD
      BD00FDFDFD00FCFCFC00FDFDFD00BDBDBD00C6C6C600FCFCFC0052525200D6D6
      D600B3B3B300BCBCBC00CCCCCC007B7B7B006767670054545400FCFCFC00BDBD
      BD00FDFDFD00FCFCFC00FDFDFD00BDBDBD0000000000858A8800FEFEFE00EBEB
      EB00EAEAEA00EAEAE900E9E9E800E8E8E800E7E8E8009A9F9D00EFEFEF00E4E5
      E400E4E5E400858A880000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C9C9C900FEFEFE0002579B000961
      A500126DB1001876BA001976BA00136EB2000B63A70003599D00FEFEFE00C3C3
      C300FEFEFE0098989800FEFEFE00C3C3C300C9C9C900FEFEFE00505050005959
      5900646464006C6C6C006C6C6C00656565005B5B5B0052525200FEFEFE00C3C3
      C300FEFEFE0098989800FEFEFE00C3C3C30000000000858A8800FEFEFE00B6BD
      BA00B6BDBA00B6BDBA00B6BDBA00B6BDBA00C6C6C700ABB0AE00E4E5E400FBFB
      FB00858A8800DEE0DF00000000000000000000000000CCCCCC0000000000CCCC
      CC0000000000CCCCCC0000000000CCCCCC0000000000CCCCCC0000000000CCCC
      CC0000000000CCCCCC000000000000000000CACACA00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00C8C8
      C800FFFFFF00FFFFFF00FFFFFF00C8C8C800CACACA00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00C8C8
      C800FFFFFF00FFFFFF00FFFFFF00C8C8C80000000000858A8800FEFEFE00FEFE
      FE00FEFEFE00FEFEFE00FEFEFE00FBFBFB00FBFBFB00F6F6F600FBFBFB00858A
      8800DEE0DF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000D9D9D900CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CBCB
      CB00CBCBCB00CBCBCB00CBCBCB00D8D8D800D9D9D900CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CBCB
      CB00CBCBCB00CBCBCB00CBCBCB00D8D8D80000000000C4C4C400858A8800858A
      8800858A8800858A8800858A8800858A8800858A8800858A8800858A8800DEE0
      DF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CECD
      CC00B8B6B500E4E4E300D2D1D100B8B6B500D2D1D100E4E4E300B8B6B500D2D1
      D10000000000000000000000000000000000000000000000000000000000CDCD
      CD00B6B6B600E4E4E400D1D1D100B6B6B600D1D1D100E4E4E400B6B6B600D1D1
      D100000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000009999990099999900999999009999
      9900999999009999990099999900000000000000000000000000000000000000
      0000B4B2B1009D9B9A00E1E0E00087858300E1E0E000A5A3A200B4B2B1000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000B2B2B2009B9B9B00E0E0E00085858500E0E0E000A3A3A300B2B2B2000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC0099999900000000000000000000000000000000000000
      0000EEEDED00A3A19F00A3A19F00A3A19F00A3A19F00A3A19F00E2E2E1000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000EDEDED00A1A1A100A1A1A100A1A1A100A1A1A100A1A1A100E2E2E2000000
      00000000000000000000000000000000000000000000CC99660000000000CC99
      660000000000CC99660000000000CC99660000000000CC99660000000000CC99
      660000000000CC99660000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099999900CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC0099999900000000000000000000000000000000000000
      000000000000CECCCA00DFDEDD00C7C5C300E3E2E100CECCCA00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000CCCCCC00DEDEDE00C5C5C500E2E2E200CCCCCC00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900CCCCCC00CCCCCC0099999900000000000000000000000000000000000000
      000000000000E2E1E000C0BEBC00BCBAB800C4C2C100DDDCDB00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000E1E1E100BEBEBE00BABABA00C2C2C200DCDCDC00000000000000
      00000000000000000000000000000000000000000000CC996600000000009933
      0000993300009933000099330000993300009933000099330000993300009933
      000000000000CC99660000000000000000000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000000000000000000000000
      000000000000FAFAF900ACA9A700ACA9A700ACA9A700F5F4F400000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FAFAFA00A9A9A900A9A9A900A9A9A900F4F4F400000000000000
      0000000000000000000000000000000000000000000000000000000000009933
      0000CC996600CC996600CC996600CC996600CC996600CC996600CC9966009933
      0000000000000000000000000000000000000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000000000000000000000000
      00000000000000000000BAB8B7009B989600B4B1B00000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000B8B8B80098989800B1B1B10000000000000000000000
      00000000000000000000000000000000000000000000CC996600000000009933
      0000CC996600CC996600CC996600CC996600CC996600CC996600CC9966009933
      000000000000CC99660000000000000000000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000009999
      9900999999009999990099999900000000000000000000000000000000000000
      000000000000D7D6F300817ECD008F8C8A008181CC00D7D7F400000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000E2E2E2009E9E9E008C8C8C009F9F9F00E3E3E300000000000000
      0000000000000000000000000000000000000000000000000000000000009933
      0000CC996600CC996600CC996600CC996600CC996600CC996600CC9966009933
      0000000000000000000000000000000000009999990099999900999999009999
      9900000000000000000000000000000000000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000615CD0002B35CA004767CB004659D900444FD600000000003746
      D600000000002A38D10000000000000000000000000000000000000000000000
      0000000000008D8D8D006D6D6D00828282008585850081818100000000007979
      79000000000070707000000000000000000000000000CC996600000000009933
      0000CC996600CC996600CC996600CC996600CC996600CC996600CC9966009933
      000000000000CC996600000000000000000099999900CCCCCC00CCCCCC009999
      9900000000000000000000000000000000000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000002D2AC3002667E30042A1F4006FBEF5002844DC00000000002940
      D90000000000293ED60000000000000000000000000000000000000000000000
      000000000000686868007F7F7F009C9C9C00B4B4B40076767600000000007474
      7400000000007272720000000000000000000000000000000000000000009933
      0000CC996600CC996600CC996600CC996600CC996600CC996600CC9966009933
      00000000000000000000000000000000000099999900CCCCCC00CCCCCC009999
      9900000000000000000000000000000000000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000005353D1003F55DA00719FF300346EF1003456E500E4E8FB003451
      E000F2F3FD002843DC0000000000000000000000000000000000000000000000
      0000000000008585850081818100AEAEAE008B8B8B0081818100EEEEEE007E7E
      7E00F6F6F60075757500000000000000000000000000CC996600000000009933
      0000CC996600CC996600CC996600CC996600CC996600CC996600CC9966009933
      000000000000CC996600000000000000000099999900CCCCCC00CCCCCC009999
      9900000000000000000000000000000000000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000D7D7F4004450D7002845DD003457E600D7DCF90093A5F100778F
      EE00BBC7F600415EE40000000000000000000000000000000000000000000000
      000000000000E3E3E300818181007676760082828200E5E5E500BCBCBC00ACAC
      AC00D5D5D5008888880000000000000000000000000000000000000000009933
      0000CC996600CC996600CC996600CC996600CC996600CC996600CC9966009933
      00000000000000000000000000000000000099999900CCCCCC00CCCCCC009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000E4E8FB0093A5F100335AE900E4E9
      FC004E71ED0092A5F10000000000000000000000000000000000000000000000
      000000000000000000000000000000000000EEEEEE00BCBCBC0084848400EFEF
      EF0095959500BCBCBC00000000000000000000000000CC996600000000009933
      0000993300009933000099330000993300009933000099330000993300009933
      000000000000CC996600000000000000000099999900CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC0099999900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000002A3AD3002841D9002747DF006984ED00E4E9FC007697
      F6004D74F0000000000000000000000000000000000000000000000000000000
      00000000000000000000717171007474740077777700A3A3A300EFEFEF00AFAF
      AF00969696000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000009999990099999900999999009999
      9900999999009999990099999900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000BBC7F6004E71ED004D74
      F000F1F4FE000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000D5D5D500959595009696
      9600F7F7F70000000000000000000000000000000000CC99660000000000CC99
      660000000000CC99660000000000CC99660000000000CC99660000000000CC99
      660000000000CC99660000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000002A38D100293ED6002843DC00415EE400A0B0F3000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000070707000727272007575750088888800C5C5C5000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000009933000099330000993300009933
      0000993300009933000099330000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF008080800000009900000099000000990000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF008080800066666600666666006666660000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099330000CC996600CC996600CC99
      6600CC996600CC99660099330000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000C0C0C0000000
      000000808000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00808080000000CC0000009900000099000000990000009900000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00808080009999990066666600666666006666660066666600000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099330000CC996600CC996600CC99
      6600CC996600CC99660099330000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000C0C0C0000000
      000000808000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF000000CC000000CC000000CC00000099000000990000009900000099000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00999999009999990099999900666666006666660066666600666666000000
      0000000000000000000000000000000000000000000000000000000000003399
      CC00006699000066990000669900006699000066990000669900006699000066
      9900CC996600CC99660099330000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000000000000000
      000000808000000000000000000000000000FFFFFF0000000000FFFFFF000000
      00000000FF000000CC000000CC000000CC000000990000009900000099000099
      990000000000000000000000000000000000FFFFFF0000000000FFFFFF000000
      0000CCCCCC009999990099999900999999006666660066666600666666006666
      6600000000000000000000000000000000000000000000000000000000003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF000066
      9900CC996600CC99660099330000000000000000000000000000000000000000
      0000008080000080800000808000008080000080800000808000008080000080
      8000008080000000000000000000000000000000000000000000000000000000
      00000000FF000000FF000000CC000000CC000000CC00000099000099990000CC
      CC00009999000000000000000000000000000000000000000000000000000000
      0000CCCCCC00CCCCCC0099999900999999009999990066666600666666009999
      9900666666000000000000000000000000000000000000000000000000003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF000066
      9900CC996600CC99660099330000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000000000000080
      8000008080000000000000000000000000000000000000000000000000000000
      0000000000000000FF000000FF000000CC000000CC000099990000CCCC0000CC
      CC0000CCCC000099990000000000000000000000000000000000000000000000
      000000000000CCCCCC00CCCCCC00999999009999990066666600999999009999
      9900999999006666660000000000000000000000000000000000000000003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF000066
      9900993300009933000099330000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000008080000000000000000000000000000000000000000000000000000000
      000000000000000000000000FF000000800000CCCC0000FFFF0000FFFF0000CC
      CC0000CCCC0000CCCC0000999900000000000000000000000000000000000000
      00000000000000000000CCCCCC006666660099999900CCCCCC00CCCCCC009999
      9900999999009999990066666600000000009933000099330000993300003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF000066
      9900000000000000000000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000008080000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000CCCC0000FFFF00FFFFFF00C0C0C00000FF
      FF0000CCCC0000CCCC0000CCCC00009999000000000000000000000000000000
      000000000000000000000000000099999900CCCCCC00FFFFFF00C0C0C000CCCC
      CC009999990099999900999999006666660099330000CC996600CC9966003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF000066
      9900000000000000000000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000CCCC0000FFFF00FFFFFF00C0C0
      C00000FFFF0000CCCC0000999900993300000000000000000000000000000000
      00000000000000000000000000000000000099999900CCCCCC00FFFFFF00C0C0
      C000CCCCCC0099999900666666006666660099330000CC996600CC9966003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF000066
      9900000000000000000000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000C0C0C0000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000CCCC0000FFFF00FFFF
      FF0000FFFF0000999900CC660000993300000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900CCCCCC00FFFF
      FF00CCCCCC0066666600999999006666660099330000CC996600CC9966003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF000066
      9900000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000CCCC0000FF
      FF0000CCCC00CC660000CC660000CC6600000000000000000000000000000000
      000000000000000000000000000000000000000000000000000099999900CCCC
      CC009999990099999900999999009999990099330000CC996600CC9966003399
      CC003399CC003399CC003399CC003399CC003399CC003399CC003399CC003399
      CC00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000000000CC
      CC00FF990000FF990000CC660000CC6600000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00999999009999990099330000CC996600CC996600CC99
      6600CC996600CC99660099330000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000099330000FF990000FF990000CC6600000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000066666600CCCCCC00CCCCCC00999999009933000099330000993300009933
      0000993300009933000099330000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099330000FF990000FF9900000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000066666600CCCCCC00CCCCCC000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999009999990000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009999990000000000000000000000000000000000000000000000
      00000000000000000000000000000000FF000000800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000B2B2B2009999990000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000999999000000
      0000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00000000009999990000000000000000000000000000000000000000000000
      000000000000000000000000FF00000099000000990000008000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000B2B2B200999999009999990099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000C0C0C0000000
      0000008080000000000000000000000000000000000000000000999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009999990000000000000000000000000000000000000000000000
      000000000000000000000000FF00000099000000800000009900000080000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000B2B2B200999999009999990099999900999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000C0C0C0000000
      0000008080000000000000000000000000000000000000000000999999000000
      0000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00000000009999990000000000000000000000000000000000000000000000
      000000000000000000000000FF00000099000000990000008000000099000000
      8000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000B2B2B200999999009999990099999900999999009999
      9900000000000000000000000000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000000000000000
      0000008080000000000000000000000000000000000000000000999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009999990000000000000000000000000000000000000000000000
      000000000000FFFFFF00CC9966000000990000009900000099000000FF000000
      9900000080000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF0099999900999999009999990099999900B2B2B2009999
      9900999999000000000000000000000000000000000000000000000000000000
      0000008080000080800000808000008080000080800000808000008080000080
      8000008080000000000000000000000000000000000000000000999999000000
      0000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00000000009999990000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFCC9900FFCC9900000099000000FF000000FF000000
      FF00000099000000800000000000000000000000000000000000000000000000
      000000000000FFFFFF00CCCCCC00CCCCCC0099999900B2B2B200B2B2B200B2B2
      B200999999009999990000000000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000000000000080
      8000008080000000000000000000000000000000000000000000999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009999990000000000000000000000000000000000000000000000
      0000FFFFFF00FFCC9900FFCC9900FFFFFF00FFFFFF000000FF003399FF003399
      FF003399FF000000990000000000000000000000000000000000000000000000
      0000FFFFFF00CCCCCC00CCCCCC00FFFFFF00FFFFFF00B2B2B200CCCCCC00CCCC
      CC00CCCCCC009999990000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000008080000000000000000000000000000000000000000000999999000000
      0000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC0000000000999999000000000000000000000000000000000000000000FFFF
      FF00FFCC9900FFCC9900FFFFFF00FFFFFF00FFFFFF00FFFFFF00000099000000
      990000009900000000000000000000000000000000000000000000000000FFFF
      FF00CCCCCC00CCCCCC00FFFFFF00FFFFFF00FFFFFF00FFFFFF00999999009999
      9900999999000000000000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000008080000000000000000000000000000000000000000000999999000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000009999990000000000000000000000000000000000FFFFFF00FFCC
      9900FFCC9900FFFFFF00FFFFFF00FFFFFF00CC996600CC996600000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF00CCCC
      CC00CCCCCC00FFFFFF00FFFFFF00FFFFFF009999990099999900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000000000000000000000000000000000000000000000000000999999000000
      0000CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC0000000000999999009999
      99009999990099999900000000000000000000000000FFFFFF00FFCC9900FFCC
      9900FFFFFF00FFFFFF00FFFFFF00CC9966000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00CCCCCC00CCCC
      CC00FFFFFF00FFFFFF00FFFFFF00999999000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000080800000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000C0C0C0000000000000000000000000000000000000000000999999000000
      0000000000000000000000000000000000000000000000000000999999000000
      000099999900000000000000000000000000FFFFFF00FFCC9900FFCC9900FFFF
      FF00FFFFFF00FFFFFF00CC9966000000FF000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00CCCCCC00CCCCCC00FFFF
      FF00FFFFFF00FFFFFF0099999900B2B2B2000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000999999000000
      0000000000000000000000000000000000000000000000000000999999009999
      990000000000000000000000000000000000CC996600FFCC9900FFFFFF00FFFF
      FF00FFFFFF00CC9966000000FF00000000000000000000000000000000000000
      00000000000000000000000000000000000099999900CCCCCC00FFFFFF00FFFF
      FF00FFFFFF0099999900B2B2B200000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000999999009999
      9900999999009999990099999900999999009999990099999900999999000000
      00000000000000000000000000000000000000000000CC996600FFFFFF00FFFF
      FF00CC9966000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099999900FFFFFF00FFFF
      FF00999999000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000CC996600CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000999999009999
      9900000000000000000000000000000000000000000000000000000000000000
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
      0000000000000000000000000000000000000000000000000000CC996600CC99
      6600CC996600CC996600CC996600CC996600CC996600CC996600CC996600CC99
      6600CC996600CC99660000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000CC996600FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00CC99660000000000000000000000000000000000000000009999
      9900999999009999990099999900000000000000000099999900999999009999
      9900999999000000000000000000000000000000000099330000993300000000
      0000000000000000000000000000000000009933000099330000000000000000
      0000000000000000000000000000000000000000000099999900999999000000
      0000000000000000000000000000000000009999990099999900000000000000
      0000000000000000000000000000000000000000000000000000CC996600FFFF
      FF00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500FFFFFF00CC99660000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCC
      CC00999999000000000000000000000000000000000099330000CC6600009933
      00000000000000000000000000000000000099330000CC660000993300000000
      0000000000000000000000000000000000000000000099999900CCCCCC009999
      99000000000000000000000000000000000099999900CCCCCC00999999000000
      0000000000000000000000000000000000000000000000000000CC996600FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00CC99660000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCC
      CC00999999000000000000000000000000000000000099330000CC660000CC66
      00009933000000000000000000000000000099330000CC660000CC6600009933
      0000000000000000000000000000000000000000000099999900CCCCCC00CCCC
      CC009999990000000000000000000000000099999900CCCCCC00CCCCCC009999
      9900000000000000000000000000000000000000000000000000CC996600FFFF
      FF00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500FFFFFF00CC99660000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCC
      CC00999999000000000000000000000000000000000099330000CC660000CC66
      0000CC66000099330000000000000000000099330000CC660000CC660000CC66
      0000993300000000000000000000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000CC996600FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00CC99660000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCC
      CC00999999000000000000000000000000000000000099330000CC660000CC66
      0000CC660000CC660000993300000000000099330000CC660000CC660000CC66
      0000CC6600009933000000000000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00999999000000000099999900CCCCCC00CCCCCC00CCCC
      CC00CCCCCC009999990000000000000000000000000000000000CC996600FFFF
      FF00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500FFFFFF00CC99660000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCC
      CC00999999000000000000000000000000000000000099330000CC660000CC66
      0000CC660000CC660000CC6600009933000099330000CC660000CC660000CC66
      0000CC660000CC66000099330000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00CCCCCC009999990099999900CCCCCC00CCCCCC00CCCC
      CC00CCCCCC00CCCCCC0099999900000000000000000000000000CC996600FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00CC99660000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCC
      CC00999999000000000000000000000000000000000099330000CC660000CC66
      0000CC660000CC660000993300000000000099330000CC660000CC660000CC66
      0000CC6600009933000000000000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC00CCCCCC00999999000000000099999900CCCCCC00CCCCCC00CCCC
      CC00CCCCCC009999990000000000000000000000000000000000CC996600FFFF
      FF00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500FFFFFF00CC99660000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCC
      CC00999999000000000000000000000000000000000099330000CC660000CC66
      0000CC66000099330000000000000000000099330000CC660000CC660000CC66
      0000993300000000000000000000000000000000000099999900CCCCCC00CCCC
      CC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000CC996600FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00CC99660000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000099999900CCCCCC00CCCC
      CC00999999000000000000000000000000000000000099330000CC660000CC66
      00009933000000000000000000000000000099330000CC660000CC6600009933
      0000000000000000000000000000000000000000000099999900CCCCCC00CCCC
      CC009999990000000000000000000000000099999900CCCCCC00CCCCCC009999
      9900000000000000000000000000000000000000000000000000CC996600FFFF
      FF00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500FFFFFF00CC996600CC99
      6600CC996600CC99660000000000000000000000000000000000000000009999
      9900999999009999990099999900000000000000000099999900999999009999
      9900999999000000000000000000000000000000000099330000CC6600009933
      00000000000000000000000000000000000099330000CC660000993300000000
      0000000000000000000000000000000000000000000099999900CCCCCC009999
      99000000000000000000000000000000000099999900CCCCCC00999999000000
      0000000000000000000000000000000000000000000000000000CC996600FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC996600E5E5
      E500CC9966000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000099330000993300000000
      0000000000000000000000000000000000009933000099330000000000000000
      0000000000000000000000000000000000000000000099999900999999000000
      0000000000000000000000000000000000009999990099999900000000000000
      0000000000000000000000000000000000000000000000000000CC996600FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00CC996600CC99
      6600000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000CC996600CC99
      6600CC996600CC996600CC996600CC996600CC996600CC996600CC9966000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000080808000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800080808000808080008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000080808000FFFFFF0080808000808080008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000FFFFFF00FFFFFF0080808000FFFFFF00FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009933
      0000993300009933000099330000993300009933000099330000993300009933
      0000993300000000000000000000000000000000000000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999000000000000000000000000000000000000000000000000009933
      0000993300009933000099330000000000000000000099330000993300009933
      0000993300000000000000000000000000000000000000000000808080008080
      8000FFFFFF008080800080808000FFFFFF008080800080808000000000000000
      0000000000000000000000000000000000000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000000000009933
      0000CC660000CC66000099330000000000000000000099330000CC660000CC66
      0000993300000000000000000000000000000000000080808000808080008080
      8000808080008080800080808000808080008080800080808000000000000000
      0000000000000000000000000000999999000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000000000009933
      0000CC660000CC66000099330000000000000000000099330000CC660000CC66
      00009933000000000000000000000000000080808000FFFFFF00FFFFFF008080
      8000FFFFFF008080800080808000C0C0C0008080800080808000808080000000
      0000000000000000000099999900CCCCCC000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000000000009933
      0000CC660000CC66000099330000000000000000000099330000CC660000CC66
      00009933000000000000000000000000000080808000FFFFFF00808080008080
      8000FFFFFF0080808000C0C0C000FFFFFF008080800080808000FFFFFF008080
      80000000000099999900CCCCCC00FFFFFF000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000000000009933
      0000CC660000CC66000099330000000000000000000099330000CC660000CC66
      0000993300000000000000000000000000008080800080808000808080008080
      8000808080008080800080808000FFFFFF0080808000FFFFFF00FFFFFF00FFFF
      FF0099999900CCCCCC00FFFFFF00000000000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000000000009933
      0000CC660000CC66000099330000000000000000000099330000CC660000CC66
      0000993300000000000000000000000000000000000080808000808080008080
      8000808080008080800080808000808080008080800080808000FFFFFF009999
      9900CCCCCC00FFFFFF0000000000000000000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000000000009933
      0000CC660000CC66000099330000000000000000000099330000CC660000CC66
      0000993300000000000000000000000000000000000080808000C0C0C000C0C0
      C0008080800080808000FFFFFF0080808000808080008080800099999900CCCC
      CC00FFFFFF000000000000000000000000000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000000000009933
      0000CC660000CC66000099330000000000000000000099330000CC660000CC66
      0000993300000000000000000000000000000000000080808000C0C0C000FFFF
      FF0080808000FFFFFF00FFFFFF00FFFFFF008080800099999900CCCCCC00FFFF
      FF00000000000000000000000000000000000000000000000000000000009933
      0000CC660000CC660000CC660000CC660000CC660000CC660000CC660000CC66
      0000993300000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC00999999000000000000000000000000000000000000000000000000009933
      0000CC660000CC66000099330000000000000000000099330000CC660000CC66
      0000993300000000000000000000000000000000000000000000808080008080
      80008080800080808000FFFFFF00FFFFFF0099999900CCCCCC00FFFFFF000000
      0000000000000000000000000000000000000000000000000000000000009933
      0000993300009933000099330000993300009933000099330000993300009933
      0000993300000000000000000000000000000000000000000000000000009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999000000000000000000000000000000000000000000000000009933
      0000993300009933000099330000000000000000000099330000993300009933
      0000993300000000000000000000000000000000000000000000000000008080
      800080808000808080008080800099999900CCCCCC00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000808080008080800099999900CCCCCC00FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC00FFFFFF000000000000000000000000000000
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
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800000000000000000008080800000000000000000000000
      0000000000000000000000000000000000000000000099999900999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900CCCCCC000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000080808000FFFFFF0000000000000000008080800000000000000000000000
      0000000000000000000000000000000000009999990099999900E5E5E500CCCC
      CC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC009999
      99009999990000000000000000000000000000000000000000003399CC000066
      9900006699000066990000669900006699000066990000669900006699000066
      9900006699000066990000000000000000000000000000000000999999009999
      9900999999009999990099999900999999009999990099999900999999009999
      9900999999009999990000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF0000000000000000000000
      0000000000000000000000000000000000009999990099999900CCCCCC00E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500CCCC
      CC0099999900999999000000000000000000000000003399CC0066CCFF003399
      CC0099FFFF0066CCFF0066CCFF0066CCFF0066CCFF0066CCFF0066CCFF0066CC
      FF003399CC0099FFFF0000669900000000000000000099999900CCCCCC009999
      9900E5E5E500CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCCCC00CCCC
      CC0099999900E5E5E50099999900000000000000000000000000000000000000
      0000FFFFFF000000000000000000FFFFFF008080800080808000000000000000
      0000000000000000000000000000000000009999990099999900CCCCCC00E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500CCCC
      CC00CCCCCC00999999000000000000000000000000003399CC0066CCFF003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FF
      FF0066CCFF0099FFFF0000669900000000000000000099999900CCCCCC009999
      9900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500CCCCCC00E5E5E50099999900000000000000000080808000000000000000
      0000000000000000000000000000808080000000000000000000000000000000
      00000000000000000000000000006666990099999900CCCCCC0099999900E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500CCCC
      CC00E5E5E500999999009999990000000000000000003399CC0066CCFF003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FF
      FF0066CCFF0099FFFF0000669900000000000000000099999900CCCCCC009999
      9900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500CCCCCC00E5E5E500999999000000000080808000FFFFFF00FFFFFF000000
      0000FFFFFF000000000000000000C0C0C0000000000000000000000000000000
      00000000000000000000666699003399CC0099999900CCCCCC00CCCCCC00CCCC
      CC00E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500CCCC
      CC00E5E5E500CCCCCC009999990000000000000000003399CC0066CCFF003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FF
      FF0066CCFF0099FFFF0000669900000000000000000099999900CCCCCC009999
      9900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500CCCCCC00E5E5E500999999000000000080808000FFFFFF00000000000000
      0000FFFFFF0080808000C0C0C000FFFFFF008080800000000000FFFFFF008080
      800000000000666699003399CC0066CCFF0099999900E5E5E500CCCCCC009999
      9900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500E5E5E500E5E5E5009999990000000000000000003399CC0066CCFF003399
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FF
      FF0066CCFF0099FFFF0000669900000000000000000099999900CCCCCC009999
      9900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500CCCCCC00E5E5E50099999900000000008080800000000000000000000000
      0000808080000000000000000000FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00666699003399CC0066CCFF000000000099999900E5E5E500E5E5E500CCCC
      CC00999999009999990099999900999999009999990099999900999999009999
      99009999990099999900CCCCCC0000000000000000003399CC0066CCFF003399
      CC00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFF
      FF0099FFFF00CCFFFF0000669900000000000000000099999900CCCCCC009999
      9900E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500E5E5E500E5E5E50099999900000000000000000000000000000000008080
      8000000000000000000000000000000000000000000000000000FFFFFF006666
      99003399CC0066CCFF00000000000000000099999900E5E5E500E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E5009999
      990000000000000000000000000000000000000000003399CC0099FFFF0099FF
      FF003399CC003399CC003399CC003399CC003399CC003399CC003399CC003399
      CC003399CC003399CC003399CC00000000000000000099999900E5E5E500E5E5
      E500999999009999990099999900999999009999990099999900999999009999
      9900999999009999990099999900000000000000000000000000C0C0C000C0C0
      C0000000000000000000FFFFFF00000000000000000000000000666699003399
      CC0066CCFF000000000000000000000000000000000099999900E5E5E500E5E5
      E500E5E5E500E5E5E50099999900999999009999990099999900999999000000
      000000000000000000000000000000000000000000003399CC00CCFFFF0099FF
      FF0099FFFF0099FFFF0099FFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFF
      FF00006699000000000000000000000000000000000099999900E5E5E500E5E5
      E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5E500E5E5
      E500999999000000000000000000000000000000000080808000C0C0C000FFFF
      FF0000000000FFFFFF00FFFFFF00FFFFFF0000000000666699003399CC0066CC
      FF00000000000000000000000000000000000000000000000000999999009999
      9900999999009999990000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000003399CC00CCFF
      FF00CCFFFF00CCFFFF00CCFFFF003399CC003399CC003399CC003399CC003399
      CC0000000000000000000000000000000000000000000000000099999900E5E5
      E500E5E5E500E5E5E500E5E5E500999999009999990099999900999999009999
      9900000000000000000000000000000000000000000000000000808080008080
      80000000000000000000FFFFFF00FFFFFF00666699003399CC0066CCFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000003399
      CC003399CC003399CC003399CC00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000009999
      9900999999009999990099999900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000666699003399CC0066CCFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000666699003399CC0066CCFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000666699003399CC0066CCFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000E2EFF100000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000E2EF
      F100E5E5E500CCCCCC00E5E5E500E2EFF1000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      9900000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000FF00000000000000000000000000000000009999
      9900000000000000000000000000000000000000000000000000000000000000
      00000000000000000000CCCCCC00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000E2EFF100E5E5E500B2B2
      B200CC9999009966660099666600B2B2B200CCCCCC00E5E5E500E2EFF1000000
      00000000000000000000000000000000000000000000000000003333CC000000
      FF00000099000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000099999900CCCC
      CC00999999000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000003399CC00006699000066
      9900006699000066990000669900006699000066990000669900006699000066
      990066CCCC00000000000000000000000000E5E5E500CC99990099666600CC99
      9900CC999900FFFFFF00996666009999990099999900B2B2B200E5E5E5000000
      00000000000000000000000000000000000000000000000000003333CC003399
      FF000000FF000000990000000000000000000000000000000000000000000000
      0000000000000000FF000000000000000000000000000000000099999900E5E5
      E500CCCCCC009999990000000000000000000000000000000000000000000000
      000000000000CCCCCC0000000000000000003399CC003399CC0099FFFF0066CC
      FF0066CCFF0066CCFF0066CCFF0066CCFF0066CCFF0066CCFF0066CCFF003399
      CC000066990000000000000000000000000099666600CC999900FFCC9900FFCC
      9900FFCCCC00FFFFFF0099666600336699003366990033669900E2EFF1000000
      0000000000000000000000000000000000000000000000000000000000003333
      CC000066FF000000CC0000000000000000000000000000000000000000000000
      00000000FF000000000000000000000000000000000000000000000000009999
      9900E5E5E5009999990000000000000000000000000000000000000000000000
      0000CCCCCC000000000000000000000000003399CC003399CC0066CCFF0099FF
      FF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0066CC
      FF00006699003399CC00000000000000000099666600FFCC9900FFCC9900FFCC
      9900FFCCCC00FFFFFF009966660066CCCC0066CCCC000099CC00FFFFFF00FFCC
      CC00000000000000000000000000000000000000000000000000000000000000
      00000000CC000000FF0000009900000000000000000000000000000000000000
      FF00000099000000000000000000000000000000000000000000000000000000
      000099999900CCCCCC009999990000000000000000000000000000000000CCCC
      CC00999999000000000000000000000000003399CC003399CC0066CCFF0099FF
      FF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0066CC
      FF0066CCCC0000669900000000000000000099666600FFCC9900FFCC9900FFCC
      9900FFCCCC00FFFFFF009966660066CCCC0066CCFF003399CC00FFCCCC00CC66
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000CC000000FF000000990000000000000000000000FF000000
      9900000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC00999999000000000000000000CCCCCC009999
      9900000000000000000000000000000000003399CC0066CCFF003399CC0099FF
      FF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0066CC
      FF0099FFFF00006699003399CC000000000099666600FFCC9900CC999900CC99
      6600FFCCCC00FFFFFF009966660099CCCC0099CCFF00B2B2B200FF660000CC66
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000CC000000FF00000099000000FF00000099000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000099999900CCCCCC0099999900CCCCCC00999999000000
      0000000000000000000000000000000000003399CC0066CCFF0066CCCC0066CC
      CC0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0099FFFF0066CC
      FF0099FFFF0066CCCC00006699000000000099666600FFCC990099666600FFFF
      FF00FFCCCC00FFFFFF009966660099CCCC00C0C0C000CC660000CC660000CC66
      0000CC660000CC660000CC660000000000000000000000000000000000000000
      00000000000000000000000000000000CC000000FF0000009900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000099999900CCCCCC0099999900000000000000
      0000000000000000000000000000000000003399CC0099FFFF0066CCFF003399
      CC00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF0099FF
      FF00CCFFFF00CCFFFF00006699000000000099666600FFCC9900CC9999009966
      6600FFCCCC00FFFFFF009966660000000000CC660000CC660000CC660000CC66
      0000CC660000CC660000CC660000000000000000000000000000000000000000
      000000000000000000000000CC000000FF00000099000000CC00000099000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000099999900CCCCCC009999990099999900999999000000
      0000000000000000000000000000000000003399CC0099FFFF0099FFFF0066CC
      FF003399CC003399CC003399CC003399CC003399CC003399CC003399CC003399
      CC003399CC003399CC0066CCFF000000000099666600FFCC9900FFCC9900FFCC
      9900FFCCCC00FFFFFF009966660000000000CC999900CC660000CC660000CC66
      0000CC660000CC660000CC660000000000000000000000000000000000000000
      0000000000000000CC000000FF000000990000000000000000000000CC000000
      9900000000000000000000000000000000000000000000000000000000000000
      00000000000099999900CCCCCC00999999000000000000000000999999009999
      9900000000000000000000000000000000003399CC00CCFFFF0099FFFF0099FF
      FF0099FFFF0099FFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF00CCFFFF000066
      99000000000000000000000000000000000099666600FFCC9900FFCC9900FFCC
      9900FFCCCC00FFFFFF0099666600CCCCCC00E2EFF100CC999900FF660000CC66
      0000000000000000000000000000000000000000000000000000000000000000
      CC000000FF000000FF0000009900000000000000000000000000000000000000
      CC00000099000000000000000000000000000000000000000000000000009999
      9900CCCCCC00CCCCCC0099999900000000000000000000000000000000009999
      990099999900000000000000000000000000000000003399CC00CCFFFF00CCFF
      FF00CCFFFF00CCFFFF003399CC003399CC003399CC003399CC003399CC000000
      00000000000000000000000000000000000099666600FFCC9900FFCC9900FFCC
      9900FFCCCC00FFFFFF009966660099CCCC000000000099CCCC00FFCC9900CC66
      00000000000000000000000000000000000000000000000000000000CC003399
      FF000000FF000000990000000000000000000000000000000000000000000000
      00000000CC00000099000000000000000000000000000000000099999900E5E5
      E500CCCCCC009999990000000000000000000000000000000000000000000000
      00009999990099999900000000000000000000000000000000003399CC003399
      CC003399CC003399CC0000000000000000000000000000000000000000000000
      00000000000000000000000000000000000099666600CC999900FFCC9900FFCC
      9900FFCCCC00FFFFFF0099666600CCCCCC00000000003399CC0000000000FFCC
      9900000000000000000000000000000000000000000000000000666699000000
      CC00666699000000000000000000000000000000000000000000000000000000
      000000000000000000000000CC00000000000000000000000000CCCCCC009999
      9900CCCCCC000000000000000000000000000000000000000000000000000000
      0000000000000000000099999900000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000C0C0C000CC996600CC99
      9900CCCC9900FFFFFF00996666000099CC000099CC000099CC00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000CCCC
      CC00CC9999009966660099666600000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000FFFFFF000000000000000000000000004040
      4000404040004040400040404000404040004040400040404000404040004040
      4000404040004040400040404000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00C0C0C000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00C0C0C000FFFFFF00FFFFFF00000000000000000000000000C0C0
      C000808080008080800080808000808080008080800080808000808080008080
      8000808080008080800080808000404040000000000000000000000000000000
      0000000000000000000040404000404040000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      8000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C00080808000404040000000000000000000000000000000
      0000000000000080000000E0000000C020004040400000000000000000000000
      00000000000000000000000000000000000000000000000000000000FF000000
      0000000080000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF0000000000F0CA
      A600F0CAA600F0CAA600F0CAA600F0CAA600F0CAA600F0CAA600F0CAA600F0CA
      A60000000000FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C00080808000404040000000000000000000000000000000
      0000000000000080000000E0000000E0000000C0200040404000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF00000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF0000000000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A0
      800000000000FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000C0C0C0000000FF000000FF00C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C00080808000404040000000000000000000000000000000
      0000000000000080000000E0000000E0000000E0000000C02000404040000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000FF000000000000000000000000000000000000000000808080000000
      000000000000000000000000000000000000FFFFFF00FFFFFF0000000000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A0
      800000000000FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000C0C0C000000000000000000000000000000000000000
      0000000000000080000000E0000000FF000000E0000000E0000000C020004040
      4000000000000000000000000000000000000000000000000000000000000000
      0000000000000000FF0000000000000000000000000000000000000000008080
      800000000000000000000000000000000000FFFFFF00FFFFFF0000000000C0A0
      8000808080008080800080808000808080008080800080808000808080008080
      800000000000FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000080000000E0200000E0000000FF000000E0000000E0000000C0
      2000404040000000000000000000000000000000000080808000000000000000
      00000000000000000000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      00000000000000000000000000000000000000000000FFFFFF00C0C0C000C0A0
      800080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E000A4A0
      A000C0C0C000FFFFFF00C0C0C000000000000000000040404000404040004040
      4000404040004040400040404000404040000000000000000000000000000000
      0000000000004040400000000000000000000000000000000000000000000000
      0000000000000080000040E0000000E0200000E0000000FF000000E0000000E0
      000000C020004040400000000000000000000000000000000000000000008080
      800000FFFF000000000000000000C0C0C00000000000C0C0C000000000000000
      000000FFFF00000000000000000000000000FFFFFF00FFFFFF000000000080E0
      E00080E0E000A4A0A000A4A0A000A4A0A000A4A0A000A4A0A00080E0E00080E0
      E00000000000FFFFFF00FFFFFF00FFFFFF0040404000F0FBFF0080E0E00080E0
      E00080E0E00080E0E00080E0E000F0FBFF004040400000000000000000000000
      0000404040004040400040404000000000000000000000000000000000000000
      0000000000000080000040E0A00000FF000000E0200000E0000000FF000000E0
      000000E000000080000000000000000000000000000000000000000000000000
      00008080800000000000C0C0C00000000000C0C0C00000000000C0C0C0000000
      000080808000000000000000000000000000FFFFFF00FFFFFF000000000080E0
      E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0
      E00000000000FFFFFF00FFFFFF00FFFFFF004040400080E0E00080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E0004040400000000000000000004040
      4000404040004040400040404000404040000000000000000000000000000000
      0000000000000080000040E0A00000FF000000FF000000E0200000E0000040E0
      A00000800000000000000000000000000000000000000000000000FFFF000000
      000000FFFF008080800000000000C0C0C00000000000C0C0C000000000008080
      800000FFFF00808080000000000000000000FFFFFF00FFFFFF0000000000FFFF
      FF0080E0E000C0804000C0804000A4A0A000A4A0A000A4A0A00080E0E00080E0
      E00000000000FFFFFF00FFFFFF00FFFFFF004040400080E0E00080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E0004040400000000000000000000000
      0000000000004040400000000000000000000000000000000000000000000000
      0000000000000080000080E0200000FF000000FF000000FF000000E020000080
      0000000000000000000000000000000000000000000000000000000000008080
      80000000000000FFFF00808080000000000000000000000000008080800000FF
      FF008080800000FFFF000000000000000000FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0
      E00000000000FFFFFF00FFFFFF00FFFFFF004040400000FFFF0080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E0004040400000000000000000000000
      0000000000004040400000000000000000000000000000000000000000000000
      0000000000000080000080E0200000FF000000FF000080E02000008000000000
      000000000000000000000000000000000000000000000000000000FFFF000000
      0000000000008080800000FFFF008080800000FFFF008080800000FFFF008080
      800000FFFF008080800000FFFF0000000000FFFFFF00FFFFFF00FFFFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF004040400040404000404040004040
      400000C0C00080E0E00080E0E000F0FBFF004040400000000000000000000000
      0000000000004040400000000000000000000000000000000000000000000000
      00000000000000800000C0DCC00000FF000080E0200000800000000000000000
      00000000000000000000000000000000000000000000000000000000000000FF
      FF00000000000000000000000000000000008080800000FFFF008080800000FF
      FF008080800000FFFF008080800000000000FFFFFF00C0C0C000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00C0C0C000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00C0C0C000FFFFFF00FFFFFF004040400000FFFF000000000000C0
      C000404040004040400040404000404040000000000040404000404040004040
      4000404040008080800000000000000000000000000000000000000000000000
      00000000000000800000C0DCC000C0DCC0000080000000000000000000000000
      000000000000000000000000000000000000000000000000000000FFFF000000
      000000FFFF000000000000FFFF00000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000FFFFFF000000000040404000404040004040
      4000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000800000C0DCC000008000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000000000FF
      FF000000000000FFFF000000000000FFFF000000000000FFFF000000000000FF
      FF0000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000800000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000808080000000000000000000000000000000000080808000808080008080
      8000808080008080800080808000808080008080800080808000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000080602000000000000000
      0000000000000000000000000000000000000000000000000000A4A0A0008080
      8000808080004060600040606000404040004040400040404000404040004040
      400040404000A4A0A000000000000000000000000000A4A0A000808080008080
      800040606000404040004040400040404000404040004040400040404000A4A0
      A000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080808000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      00000000000000000000000000000000000080600000C0A02000806020000000
      00000000000000000000000000000000000000000000A4A0A00080400000C0C0
      C000F0FBFF00C0C0C000C0C0C000C0C0C000F0FBFF00F0FBFF00F0FBFF00A4A0
      A0004040400040404000A4A0A00000000000A4A0A00040606000C0C0C000F0FB
      FF00C0C0C000C0C0C000F0FBFF00F0FBFF00F0FBFF00A4A0A000404040004040
      4000A4A0A00000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000080808000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00A4A0A00080808000404020004040
      20004040400040404000C0C0C00080600000C0A02000C0A08000C0A020008060
      2000808080008080800040404000A4A0A000804000008040000080400000C0DC
      C000F0FBFF00F0FBFF00F0FBFF00F0FBFF00F0FBFF00F0FBFF00F0FBFF00C0DC
      C000C08040008040000040404000404040004040400040606000C0DCC000F0FB
      FF00F0FBFF00F0FBFF00F0FBFF00F0FBFF00F0FBFF00C0DCC000808080004020
      20004040400000000000000000000000000000000000FFFFFF00808080008080
      8000808080008080800080808000FFFFFF000000000080808000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00A4A0A00080E0E00080E0E00080E0
      E00040E0E00040C0E00080600000C0A08000C0A08000C0A08000C0A08000C0A0
      20008060200040E0E00040C0E0004040400080400000C080400080400000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0804000C0804000C0804000C080
      4000C08040008040000080600000404040004040400040606000C0A08000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000808080004040
      400040404000A4A0A000A4A0A0000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000080808000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00A4A0A00080E0E00080E0E00080E0
      E00080E0E00080600000C0A080000000000000000000C0A08000C0A08000C0A0
      8000C0A020008060200040E0E0004040400080400000C0A0800080400000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0804000C0804000C080
      4000C080400080400000C0804000404040004040400040606000C0A08000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000808080004040
      4000404040008080800040404000A4A0A00000000000FFFFFF00808080008080
      8000808080008080800080808000FFFFFF000000000080808000000000000000
      000000000000000000000000000000000000A4A0A00080E0E00080E0E00080E0
      E000C0C080008060200080602000806020000000000000000000C0A080008060
      20008060200080602000C0C080008080800080400000C0A0800080400000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0804000C080
      4000C080400080400000C0804000404040004060600040606000C0A080008080
      8000808080008080800080808000808080008080800080808000808080004060
      60004040400080808000402020004040400000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000080808000FFFFFF000000
      0000FFFFFF0000000000FFFFFF0000000000A4A0A00080E0E00080E0E00080E0
      E00080E0E00080E0E00080E0E000808040000000000000000000F0CAA6008080
      4000C0C0C00080E0E00040E0E0004040400080400000C0A0800080400000C040
      2000804000008040000080400000804000008040000080400000804000008040
      0000C080400080400000C0804000404040004060600040606000C0A0800080E0
      E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E000A4A0A0004060
      60004040400080808000406060004040400000000000FFFFFF00808080008080
      8000808080008080800080808000FFFFFF00000000008080800000000000FFFF
      FF0000000000FFFFFF000000000000000000A4A0A000F0FBFF0080E0E00080E0
      E00080E0E00080E0E00080E0E000C08040000000000000000000C0A08000F0CA
      A60080E0E00040E0E000C0DCC0004040400080400000C0A0800080400000A4A0
      A00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0
      E0008040000080400000C080400040404000806060004060600080E0E00080E0
      E000A4A0A000A4A0A000A4A0A000A4A0A000A4A0A00080E0E00080E0E0008080
      80004060600080808000406060004040400000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000080808000FFFFFF000000
      0000FFFFFF0000000000FFFFFF0000000000A4A0A00080E0E00080E0E00080E0
      E00080E0E00080E0E000C0A04000C0A08000F0CAA60000000000C080400080E0
      E00080E0E00080E0E00040E0E0004060600080400000C0A080008040000080E0
      E000C0C0C000A4A0A000A4A0A000A4A0A000A4A0A000A4A0A000A4A0A000C0C0
      C00080E0E00080400000C08040004040400080808000A4A0A00080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E000A4A0
      A0004060600080808000406060004040400000000000FFFFFF00808080008080
      8000808080008080800080808000FFFFFF00808080008080800000000000FFFF
      FF0000000000FFFFFF000000000000000000A4A0A000F0FBFF0080E0E000F0FB
      FF0080E0E00080E0E000C0A04000F0CAA600F0CAA600C0C08000F0CAA60080E0
      E00080E0E00080E0E00080E0E0008060600080400000C0A080008060000080E0
      E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0
      E00080E0E00080600000C08040004040400080808000A4A0A000F0FBFF0080E0
      E000C0804000C0804000A4A0A000A4A0A000A4A0A00080E0E00080E0E000A4A0
      A0008060600080808000406060004040400000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000080808000FFFFFF000000
      0000FFFFFF0000000000FFFFFF0000000000C0C0C00080E0E000F0FBFF0080E0
      E00080E0E000C0804000C0A08000F0CAA600C0C0800080E0E00080E0E00080E0
      E00080E0E00080E0E00080E0E0008080800080400000C0A080008060000080E0
      E000C0C0C000A4A0A000A4A0A000A4A0A000A4A0A000A4A0A000A4A0A000C0C0
      C00080E0E00080600000C08040004040400080808000C0C0C000F0FBFF00F0FB
      FF0080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E000C0C0
      C000808080008080800080606000404040000000000000000000000000000000
      00000000000000000000000000000000000080808000FFFFFF0000000000FFFF
      FF0000000000FFFFFF000000000000000000C0C0C000F0FBFF0080E0E000F0FB
      FF00C0C06000C0C06000C0C06000C0C06000F0CAA600C0C0C000C0C0C000C0DC
      C00040C0E00040C0E00040C0E0008080800080400000C0A08000A4A0A00080E0
      E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0
      E00080E0E000A4A0A000C080400040606000A4A0A00080808000406060004060
      6000406060004060600040606000406060004060600040606000806060008080
      8000A4A0A000808080008080800040606000FFFFFF00FFFFFF00FFFFFF008080
      8000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0DCC000C0DCC00040E0E00040E0E00040E0E00040E0E00040E0
      E00040E0E00040E0E00040C0E000C0C0C00080400000C0A08000A4A0A000F0FB
      FF00C0C0C000C0804000C0804000A4A0A000A4A0A000A4A0A000A4A0A000C0C0
      C00080E0E000A4A0A000C0A080008060600000000000A4A0A000808080008080
      800080808000808080008080800080808000808080008080800080808000A4A0
      A000C0DCC000808080008080800080606000FFFFFF00FFFFFF00FFFFFF008080
      8000800000008000000080000000800000008000000080000000800000008000
      00008000000080000000800000000000000000000000C0DCC00000FFFF0000FF
      FF0000FFFF0000FFFF0040C0E000F0FBFF008080800080808000808080008080
      800080808000808080008080800000000000A4A0A0008040000080600000F0FB
      FF00F0FBFF0080E0E00080E0E00080E0E00080E0E00080E0E00080E0E00080E0
      E00080E0E0008060000080400000A4A0A000000000000000000000000000C0C0
      C000C0C0C000C0DCC000C0DCC000C0DCC000C0DCC000C0DCC000C0DCC000C0DC
      C000C0DCC000C0DCC000C0C0C00080808000FFFFFF00FFFFFF00FFFFFF008080
      8000FFFFFF008000000080000000800000008000000080000000800000008000
      0000FFFFFF0080000000FFFFFF000000000000000000A4A0A00040E0E00080E0
      E00080E0E00080E0E00000FFFF00C0C0C0000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000A4A0A0008040
      0000804000008040000080400000804000008040000080400000804000008040
      000080400000A4A0A0000000000000000000000000000000000000000000A4A0
      A00080808000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C00080808000A4A0A000FFFFFF00FFFFFF00FFFFFF008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000808080008080800080808000808080000000000000000000A4A0A0008080
      80008080800080808000A4A0A000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000A4A0A0008080800080808000808080008080800080808000808080008080
      80008080800080808000A4A0A000000000000000000000000000000000000000
      0000806060000000000000000000000000000000000000000000C0A080000000
      0000C0A080000000000000000000000000000000000000000000000000000000
      0000806060000000000000000000000000000000000000000000C0A080000000
      0000C0A080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000080606000806060000000000000000000F0CAA600C0A08000C0A080000000
      0000C0A08000C0A08000F0CAA600000000000000000000000000000000000000
      000080606000806060000000000000000000F0CAA600C0A08000C0A080000000
      0000C0A08000C0A08000F0CAA600000000008080800040606000406060004060
      6000406060004060600040606000406060004060600040606000406060004060
      600040606000406060004040400080808000000000000000000080808000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C00000000000000000000000
      0000000000000000000000000000000000000000000080606000806060008060
      600080606000806060008060600000000000C0A08000F0CAA600C0A080000000
      0000C0A08000F0CAA600C0A08000000000000000000080606000806060008060
      600080606000806060008060600000000000C0A08000F0CAA600C0A080000000
      0000C0A08000F0CAA600C0A080000000000040404000C0A08000C0804000C080
      4000C0804000C0804000C0804000C0804000C0804000C0804000C0804000C080
      4000C0804000C0804000C0804000404040000000000000000000808080000000
      000080000000800000008000000080000000C0C0C00000000000000000000000
      0000000000000000000000000000000000000000000080606000000000000000
      0000806060008060600000000000C0A08000C0A08000C0A08000F0CAA6000000
      0000F0CAA600C0A08000C0A08000C0A080000000000080606000000000000000
      0000806060008060600000000000C0A08000C0A08000C0A08000F0CAA6000000
      0000F0CAA600C0A08000C0A08000C0A080008060000000000000C0A08000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A0
      8000C0A08000C0A08000C0804000404040008080800000FFFF00808080000000
      000000000000000000000000000000000000C0C0C00000000000000000000000
      0000000000000000000000000000000000000000000080606000000000000000
      0000806060000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080606000000000000000
      0000806060000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000008060000000000000C0A08000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A0
      8000C0A08000C0A08000C0804000404040008080800000000000808080000000
      000080000000800000008000000080000000C0C0C000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000000000000000000080606000000000000000
      0000000000000000000000000000C0A08000C0A08000C0A08000F0CAA6000000
      0000F0CAA600C0A08000C0A08000C0A080000000000080606000000000000000
      0000000000000000000000000000C0A08000C0A08000C0A08000F0CAA6000000
      0000F0CAA600C0A08000C0A08000C0A080008060000000000000C0A08000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A0
      8000C0A08000C0A08000C0804000404040008080800000FFFF00808080000000
      000000000000000000000000000000000000C0C0C00000000000C0C0C0000000
      0000C0C0C00000000000C0C0C000000000000000000000000000000000000000
      000000000000000000000000000000000000C0A08000F0CAA600C0A080000000
      0000C0A08000F0CAA600C0A08000000000000000000000000000000000000000
      000000000000000000000000000000000000C0A08000F0CAA600C0A080000000
      0000C0A08000F0CAA600C0A08000000000008060000000000000C0A08000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A0
      8000C0A08000C0A08000C0804000404040008080800000000000808080000000
      000080000000800000008000000080000000C0C0C000C0C0C00000000000C0C0
      C00000000000C0C0C000C0C0C000000000008060600080606000806060008060
      600080606000806060008060600080606000F0CAA600C0A08000C0A080000000
      0000C0A08000C0A08000F0CAA600000000000000000000000000000000000000
      000000000000000000000000000000000000F0CAA600C0A08000C0A080000000
      0000C0A08000C0A08000F0CAA600000000008060000000000000C0A08000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A0
      8000C0A08000C0A08000C0804000404040008080800000FFFF00808080000000
      000000000000000000000000000000000000C0C0C00000000000C0C0C0000000
      0000C0C0C00000000000C0C0C000000000008060600000000000000000000000
      0000000000000000000000000000806060000000000000000000C0A080000000
      0000C0A080000000000000000000000000008080800080606000806060008060
      6000806060008060600080606000806060008080800000000000C0A080000000
      0000C0A0800000000000000000000000000080600000F0FBFF00C0A08000C0A0
      8000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A08000C0A0
      8000C0A08000C0A08000C0804000404040008080800000000000808080008080
      80008080800080808000808080008080800080808000C0C0C00000000000C0C0
      C00000000000C0C0C000C0C0C000000000008060600000000000806060008060
      6000806060008060600000000000806060000000000000000000000000000000
      00000000000000000000000000000000000080606000F0FBFF0080E0E00080E0
      E00080E0E00080E0E00080E0E000F0FBFF008060600000000000000000000000
      00000000000000000000000000000000000080600000F0FBFF00F0FBFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000C0A08000404040008080800000FFFF000000000000FF
      FF000000000000FFFF000000000080808000C0C0C00000000000C0C0C0000000
      0000C0C0C00000000000C0C0C000000000008060600000000000000000000000
      0000000000000000000000000000806060000000000000000000000000000000
      0000000000000000000000000000000000008060600080E0E00080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E0008060600000000000000000000000
      00000000000000000000000000000000000080600000C0800000C0800000C080
      0000C0800000C0800000C0800000C0800000C0800000C0800000C0800000C080
      0000C0800000C0800000C0800000404040008080800080808000808080008080
      80008080800080808000808080008080800000000000C0C0C00000000000C0C0
      C00000000000C0C0C000C0C0C000000000008060600000000000806060008060
      6000806060008060600000000000806060000000000000000000000000000000
      0000000000000000000000000000000000008060600080E0E00080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E0008060600000000000000000000000
      00000000000000000000000000000000000080600000C0A00000F0FBFF00C080
      000080A0000080A00000C0A0000080A0000080A0000080A00000C0800000F0FB
      FF00C0800000F0FBFF00C0800000404040000000000000000000808080000000
      0000C0C0C00000000000C0C0C00000000000C0C0C00000000000C0C0C0000000
      0000C0C0C00000000000C0C0C000000000008060600000000000000000000000
      0000000000000000000000000000806060000000000000000000000000000000
      0000000000000000000000000000000000008060600000FFFF0080E0E00080E0
      E00080E0E00080E0E00080E0E00080E0E0008060600000000000000000000000
      00000000000000000000000000000000000080600000C0A00000C0A00000C0A0
      0000C0A00000C0A00000C0A00000C0A00000C0A00000C0A00000C0A00000C0A0
      0000C0A00000C0A00000C0A00000404040000000000000000000808080008000
      0000800000008000000080000000800000008000000080000000800000008000
      00008000000080000000C0C0C000000000008060600000000000806060000000
      0000806060008060600080606000806060000000000000000000000000000000
      0000000000000000000000000000000000008060600080606000806060008060
      600040E0E00080E0E00080E0E000F0FBFF008060600000000000000000000000
      000000000000000000000000000000000000C080000080600000806000008060
      0000806000008060000080600000806000008060000080600000806000008060
      0000806000008060000040404000808080000000000000000000808080008000
      0000800000008000000080000000800000008000000080000000800000008000
      00008000000080000000C0C0C000000000008060600000000000000000000000
      0000806060000000000080606000000000000000000000000000000000000000
      0000000000000000000000000000000000008060600000FFFF000000000000E0
      E000806060008060600080606000806060008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000808080008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000808080008080800080808000000000008060600080606000806060008060
      6000806060008060600000000000000000000000000000000000000000000000
      000000000000000000000000000000000000F0FBFF0080606000806060008060
      6000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000F00100000100010000000000800F00000000000000000000
      000000000000000000000000FFFFFF00E003000000000000C001000000000000
      8000000000000000800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000008001000000000000C001000000000000
      E003000000000000F00F000000000000F801F801FF7EE003C000C000BFFFC001
      C000C000F0038000C000C000E0038000C000C000E0030000C000C001E0030000
      C003C003E0030000C003C00320030000C003C003E0020000C003C003E0030000
      C003C003E0030000C003C003E0030000C007C007E0038001C00FC00FFFFF8001
      C01FC01FBF7DC003FFFFFFFF7F7EF00FFFFFFFFFF801F801FFFFFFFF80008000
      F0FFF0FF80008000E0FFE0FF80008000C0FFC0FF800080008301830180008001
      87018701800180018F818F818001800187818781800180018201820180018001
      C001C00180018001E039E03980018001F07FF07F80018001FFFFFFFF80018001
      FFFFFFFF80018001FFFFFFFFFFFFFFFFFFFFFFFFC003FFFF87E1FFE1DFFBF01F
      87E1FFE1DFFBE00F8541FFE1DFFBC00787E1FFE180018003FFFFFFFF80010001
      FFFBFF3F80010001FFFBFE3F80010001FFF3FC7F80010001FFF3F8FF80010001
      FFF3F1FFDFFB0001FFF3A3FFDFFB0001FFC187FFDFC38003FFC18FFFDFC7C007
      FFE387FFDFCFE00FFFF7FFFFC01FF01F8007E000E000E0000007C0006000EFFE
      0003C0002000EFFE0003C0000000EC060001C00020006FFE0001C00060002802
      00018000E000080200010000E0002802000F0000E0006FFE001F0000E000EC06
      003F0000E000EFFE003F0000E000EC06003F0000E000EFFE003F0000E000EC06
      807F000FE000EFFEC0FF001FE000E000FFFFFFFFFFFFE000F83FFEFFFEFFE000
      F83FFC7FFC7FE000F83FF83FF83FE000F83FF01FF01FE000F83FE00FE00FE000
      F83FC007C007E000C007C007C007E000C007C007C0078000C007F83FF83F0000
      E00FF83FF83F0000F01FF83FF83F0000F83FF83FF83F0000FC7FF83FF83F0000
      FEFFF83FF83F807FFFFFFFFFFFFFC0FFDFFBFFFFFF9FFFFF9FF9FCFFFF8FF83F
      0000F8FF0007F83F9FF9F0070003F83FDFFBE0030001F83FFFFFE0010000F83F
      C003F0000001F83FC003F0000003C007C00380080007C007C003800F000FC007
      C0038007000FE00FC003C003000FF01FC003E003000FF83FC003F007000FFC7F
      C003FF8F000FFEFFC003FF9F000FFFFFFFFFFFCFFFCFEFF7FFE3FF87FFB7E007
      FFC5FF87FFB7E007FF8DF70FF76F0000FF15F30FF36F0000FC2BF01FF4DF0000
      C057F01FF7DF0000BCAFF003F7C300006F5FF007F7F7000057BFF00FF7EF0000
      6BBFF01FF7DF000075BFF03FF7BF00004ABFF07FF77F00004DBFF0FFF6FFE007
      BF7FF1FFF5FFE007C0FFF3FFF3FFEFF7F81FFF24FF24FFFFE007FF24FF24FFE3
      8001FE7FFE7FFFC10000FC3FFC3FFF810000FC1FFC1FFF010180FC0FFC0FFC03
      03C0F807F807C0070C30F803F803800F0E70F003F003001F0E70E007E007003F
      0E70C03FC03F003F024080FF80FF003F000000FF00FF003F800101FF01FF003F
      E00787FF87FF807FF81FCFFFCFFFC0FFFC00FC00E000E000FC00FDFEC000C000
      FC00FDFE80008000FC00FDFE00000000E000E1FE00000000E000EC0000000000
      E000EC0000000000E000EC000000000000070FF7000000000007600700000000
      00076007000000000007600700000000003F7FBF00000000003F003F00000000
      003F003F00030003003F003F000300038081FE00FFFFFFFF9D81FE001FFF1FFF
      9D81F6000FFF0FFF9D81F20083FF83FF8D81800081FF81FFCD83B200C0FFC0FF
      C583B600C03FC03FE587BE00E01FE01FE1870000E007E007F18F007DF003F003
      F18F0001F001F001F19F0077F800F800F99F0007F800F800F9BF007FFC00FC00
      FDBF207FFC00FC00FFFF0FFFFE00FE00FFFFFFFFFFFFF800FF010000E000FFFE
      FF010000E000F002FF010000CFFEFFFAE0010000CF6EE00AE00100001002EFEA
      E00100001B02EEEAE00100000402EEEA000F0000E402E82A000F00000802EEEA
      000F0000D002EEEA000F0000C802EFEB000F0000E802E00F01FF0000EFFEFFFF
      01FF0000E000FFFFFFFF0000FFFFFFFFFFFFFFFFFFFFF01FFFFF87FF87FFE00F
      FFFF83FF83FFC0070000800F800B80073FFEC003C0030007500AE003E0030007
      7FFEE003E00300074A52E003E00300075FFAE003E003000351AAE003E0030001
      7FFEE003E00300010000E003E0030001FFFFF007F0070003FFFFF80FE0030007
      FFFFFFFFFFFF8007FFFFFFFFFFFFC01FC003801F801FFFFFC003000F000FE003
      C000000F000FEAABC000000F000FEAABC000000F000FEAABC003000F000FEAAB
      C003000F000FEAABC003000F000FEAABC00300000000EAABC00300000000EAAB
      C00300010001C001D00B00030003DFFDD00B00070007C001C003000F000FFDDF
      C003800F800FFC1FFC3FC01FC01FFFFF0001FFFFFFFFC0030001FFF1FCFFC003
      0001F001F8FFC0000001E001F007C0000001C003E003C0000001C003E001C003
      0001C003F000C0030001C003F8F0C0030001C0038C98C0030001C003878FC003
      0001C0038007C0030083C003C003C00381818007E003D00BFE00800FF007C003
      FE008FFFFF8FC003FE00FFFFFF9FFC3FC00383FFFFFFFFFFC00301FFC001C001
      C00330FF80018001C003387F80018001C003107F80018001C003007F80018001
      C003805F80018001C003C00780018001C003E00380018001C003FE0180018001
      C003FE0080018001C003FE0880018001C003FE1C80018001C003FF0C80018001
      C003FF8080018001C003FFC1FFFFFFFFC003E000E000C0038001C000C000C003
      803980008000C003803900000000C003803900000000C003800100000000C003
      C00300000000C003FFFF00000000C003FFFF00000000C003C00300000000C003
      800100000000C0039C0100000000C0039C0100000000C0039C0100000000C003
      800100030003C003C00300030003C003F81FFFFFFE7FFE7FF81FFFFFF83FF83F
      F81FFFC3E01FE01FF81FFFDB800F800FFC3F801900070007FC3FFFDB00030003
      FC3FFFC300010001FC3FFFFF00000000FC3FFFFF00000000FC3EFC3F80018001
      981CFDBFC001C00100008181E000E0000001FDBFF001F0010003FC3FF807F807
      0007FFFFFC1FFC1F981FFFFFFE7FFE7F8FF1FFFFFFFFF81F0FF0FFFFFFFFF81F
      0FF00001FFFFF81F1FF87DFDE007F81F9E794505C003FC3F98197D058020FC3F
      818145051F80FC3F81817D051F84FC3FF00F45051F84FC3FFC3F7D051F80FC3E
      FE7F45058020981CFE7F7DFDC0030000FE7F0001E0070001FC3F0001FFFF0003
      FC3F0001FFFF0007FC3FFFFFFFFF981F8003FC01FC01FF7E800380018001BFFF
      800080018001F00380009C019C01E003800098019801E003800098019801E003
      800098019801E00380019C199C19200380039FF99FF9E00280039FF99FF9E003
      800380018001E003800380018001E003800380018001E003800380018001FFFF
      8007FFFFFFFFBF7D800FFFFFFFFF7F7EFFFF800180018003FFFF000000008003
      AAAB000000008000FFFF000000008000A00B000000008000E00F000000008000
      A00B000000008000E00F000000008001A00B000000008003E00F000000008003
      A00B000000008003E00F000000008003A00B000000008003FFFF000000008003
      AAAB000000008007FFFF00000000800FFFFFE00FE00FFFFFFF01F01FF01FFFFF
      FF01F01FF01FAAABFF01F83FF83FFFFFE001F83FF83FA00BEFE1F83FF83FE00F
      EFE1FC7FFC7FA00BEFE1F83FF83FE00F0FEFF82BF82BA00B0FEFF82BF82BE00F
      0FEFF803F803A00B0FEFF803F803E00F000FFF03FF03A00B01FFFC07FC07FFFF
      01FFFF87FF87AAABFFFFFC1FFC1FFFFFFF7EA7FFA7FFFFFFBFFF01FF01FFFF01
      F003007F007FFF01E003003F003FFF01E003001F001FE001E003500F500FE001
      E003F007F007E0012003F803F803E001E002FC01FC01000FE003FE00FE00000F
      E003FF00FF00000FE003FF80FF80000FE003FFC0FFC0000FFFFFFFE0FFE001FF
      BF7DFFF0FFF001FF7F7EFFF8FFF8FFFFFFFFFFFFFFFFFF7EC003FFFFFFFFBFFF
      DFFBFE7FFE7FF003D00BFC3FFC3FE003DFFBFC1FFC1FE003D00BFC0FFC0FE003
      DFFBF807F807E003D00BF803F8032003DFFBF003F003E002D00BE007E007E003
      DFFBC03FC03FE003D04380FF80FFE003DFD700FF00FFE003DFCF01FF01FFFFFF
      C01F87FF87FFBF7DFFFFCFFFCFFF7F7EFFFFFFFFFFFFFFFFFFFFFFFFFFFFC003
      FFFFFFFFFFFFC003E1879F3F9F3FC003E1878F1F8F1FC003E187870F870FC003
      E18783078307C003E18781038103C003E18780018001C003E18781038103C003
      E18783078307C003E187870F870FC003E1878F1F8F1FC007FFFF9F3F9F3FC00F
      FFFFFFFFFFFFC01FFFFFFFFFFFFFFFFFFCFFFFFFFFFFFFFFF87FFFFFFFFFFFFF
      F07FFFFFFFFFFFFFE07FE007E007E187C03FE007E007E187803EE007E007E187
      001CE007E007E1870008E007E007E1870001E007E007E1878003E007E007E187
      8007E007E007E187800FE007E007E187C01FE007E007E187E03FFFFFFFFFFFFF
      F07FFFFFFFFFFFFFF8FFFFFFFFFFFFFFFFFFFFFFFFFFFCFFFFFFFFFFFFFFF87F
      8007FFFFFFFFF07F0007C003C003E07F000380018001C03F000380018001803E
      000180018001001C000180018001000800018001800100010001800180018003
      000F800180018007801F80078007800FC3FFC00FC00FC01FFFFFE1FFE1FFE03F
      FFFFFFFFFFFFF07FFFFFFFFFFFFFF8FFFDFFFFFFFFFFFFFFE0FFEFFDEFFDFFFF
      801FC7FFC7FF8007001FC3FBC3FB0007001FE3F7E3F70003000FF1E7F1E70003
      000FF8CFF8CF0001000FFC1FFC1F00010001FE3FFE3F00010101FC1FFC1F0001
      0101F8CFF8CF000F000FE1E7E1E7801F008FC3F3C3F3C3FF00AFC7FDC7FDFFFF
      803FFFFFFFFFFFFFE1FFFFFFFFFFFFFF8002E001FFFFCFFF0000E000FCFFA7FF
      1FF0F000F87F93FF2008F000F83FC9FF2008F000F81FE41F2008FFFDF80FF1CF
      2008FFFFF8078007800180FBF80382A320080071F803A14320080060F8078081
      2008007BF80FA0012008007BF81F90001FF0007BF83FA80000002083F87F9501
      81028FFFF8FFAAA70000FFFFFDFF80078000FFBFC003800FFF80FF1F80010007
      808000000000000780800000000000018080018000000000808000C000000000
      809400C00000000080AA00C0000000008094004000000000802A000000000000
      8094000000000000FF2A0000000000000000000000008000000080010000E000
      000080FFC003E0000000C1FFFFFFF001F7D7F7D7FFFFC03FF311F3110000C03F
      811181110000103FB210B21040001F00B7FFB7FF40005000BE10BE1040001F54
      FF11FF11400050280011FF1140001F547ED700570000402842FF007F1FFC2A54
      7EFF007F000000A842FF007F0000D5547EFF007F0000C00050FF007F0000C000
      75FF207FFFFFC00003FF0FFFFFFFFFFF00000000000000000000000000000000
      000000000000}
  end
  object OpenDialog: TOpenDialog
    Filter = 'Scxml Projects (*.sproj)|*.sproj|All files (*.*)|*.*'
    FilterIndex = 0
    Options = [ofHideReadOnly, ofFileMustExist, ofEnableSizing]
    Left = 321
    Top = 80
  end
  object DesignObjects1: TLMDDesignObjects
    Left = 64
    Top = 312
  end
  object UDPServer1: TIdUDPServer
    Bindings = <
      item
        IP = '0.0.0.0'
        Port = 11005
      end>
    DefaultPort = 11005
    ThreadedEvent = True
    OnUDPRead = UDPServer1UDPRead
    Left = 224
    Top = 184
  end
  object EditDocDebug: TLMDEditDocument
    Lines.Strings = (
      '')
    SyntaxSchemes = <
      item
        Name = 'SyntaxLog'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39'>'
          ''
          '    <RegexBlock innerScheme='#39'Debug'#39' end_token0='#39'debug'#39' >'
          '        <Start> ^DEBUG  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Info'#39' end_token0='#39'info'#39' >'
          '        <Start> ^INFO  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Warning'#39' end_token0='#39'warning'#39' >'
          '        <Start> ^WARN(ING)*  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Error'#39' end_token0='#39'error'#39' >'
          '        <Start> ^ERROR  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>    '
          ''
          '  <Scheme name='#39'Debug'#39' defaultToken='#39'debug'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Info'#39' defaultToken='#39'info'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Warning'#39' defaultToken='#39'warning'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Error'#39' defaultToken='#39'error'#39'>'
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39
          '          keywordsIgnoreCase='#39'true'#39' >'
          '  </Scheme>'
          '  '
          '</SchemeList>')
      end>
    ColorSchemes = <
      item
        Name = 'ColorLog'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          '<Attributes>'
          #9'<Token name="info" textColor="Blue"/>'
          #9'<Token name="debug" textColor="Green"/>'
          #9'<Token name="warning" textColor="Navy" fontStyle="bold"/>'
          #9'<Token name="error" textColor="Red" fontStyle="bold"/>'
          '</Attributes>')
      end>
    ActiveSyntaxScheme = 'SyntaxLog'
    ActiveColorScheme = 'ColorLog'
    Left = 64
    Top = 376
  end
  object EditDocTester: TLMDEditDocument
    Lines.Strings = (
      '')
    SyntaxSchemes = <
      item
        Name = 'SyntaxTester'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39'>'
          ''
          '    <RegexBlock innerScheme='#39'Fatal'#39' end_token0='#39'fatal'#39' >'
          '        <Start> ^\[*(Fatal|FATAL)\]*  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Info'#39' end_token0='#39'info'#39' >'
          '        <Start> ^\[*(Info|INFO|I\d+)\]*  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Warning'#39' end_token0='#39'warning'#39' >'
          
            '        <Start> ^\[*(Warning|WARNING|WARN|W\d+)\]* </Start>     ' +
            '   '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Error'#39' end_token0='#39'error'#39' >'
          
            '        <Start> ^\[*(Error|ERROR|E\d+|Assertion)\]*  </Start>   ' +
            '     '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>    '
          ''
          '  <Scheme name='#39'Fatal'#39' defaultToken='#39'fatal'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Info'#39' defaultToken='#39'info'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Warning'#39' defaultToken='#39'warning'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Error'#39' defaultToken='#39'error'#39'>'
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39
          '          keywordsIgnoreCase='#39'true'#39' >'
          '  </Scheme>'
          '  '
          '</SchemeList>')
      end>
    ColorSchemes = <
      item
        Name = 'ColorTester'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          '<Attributes>'
          #9'<Token name="info" textColor="Green"/>'
          #9'<Token name="warning" textColor="Navy" fontStyle="bold"/>'
          #9'<Token name="error" textColor="Red" fontStyle="bold"/>'
          #9'<Token name="fatal" textColor="Purple" fontStyle="bold"/>'
          '</Attributes>')
      end>
    ActiveSyntaxScheme = 'SyntaxTester'
    ActiveColorScheme = 'ColorTester'
    Left = 64
    Top = 440
  end
  object ElDropTarget1: TElDropTarget
    OnTargetDrag = ElDropTarget1TargetDrag
    OnTargetDrop = ElDropTarget1TargetDrop
    Target = LMDDockSite1
    Left = 632
    Top = 136
  end
  object ApplicationEvents1: TApplicationEvents
    OnActivate = ApplicationEvents1Activate
    OnException = ApplicationEvents1Exception
    OnIdle = ApplicationEvents1Idle
    OnMinimize = ApplicationEvents1Minimize
    OnRestore = ApplicationEvents1Restore
    Left = 632
    Top = 200
  end
  object PopupMenuStack: TPopupMenu
    OnPopup = PopupMenuStackPopup
    Left = 856
    Top = 120
    object Showcallstack1: TMenuItem
      Action = actShowCallStack
      AutoCheck = True
    end
    object N22: TMenuItem
      Caption = '-'
    end
    object Clearallstackitems1: TMenuItem
      Action = actStackClear
    end
    object MenuStackCopyMsg: TMenuItem
      Caption = 'Copy msg'
      OnClick = MenuStackCopyMsgClick
    end
    object MenuStackCopyScxmlName: TMenuItem
      Caption = 'Copy scxml'
      OnClick = MenuProjUnitScxmlNameClick
    end
    object MenuStackCopyID: TMenuItem
      Caption = 'Copy id'
      OnClick = MenuStackCopyIDClick
    end
    object MenuStackFilterMsg: TMenuItem
      Caption = 'Filter message'
      OnClick = MenuStackFilterMsgClick
    end
  end
  object PopupMenuBindings: TPopupMenu
    Images = ActionsImages
    OnPopup = PopupMenuBindingsPopup
    Left = 904
    Top = 776
    object SelectWithoutScrollChart: TMenuItem
      Caption = 'Select without scrolling chart'
      OnClick = SelectWithoutScrollChartClick
    end
    object N14: TMenuItem
      Caption = '-'
    end
    object CopyScxmlNametoClipboard1: TMenuItem
      Caption = 'Copy [ScxmlName] to Clipboard'
      OnClick = CopyNametoClipboard1Click
    end
    object CopyAviaNametoClipboard1: TMenuItem
      Caption = 'Copy [AviaName] to Clipboard'
      OnClick = CopyNametoClipboard1Click
    end
  end
  object ThreadTesterComponent: TIdThreadComponent
    Active = False
    Loop = False
    Priority = tpNormal
    StopMode = smTerminate
    OnRun = ThreadTesterComponentRun
    Left = 224
    Top = 248
  end
  object PopupMenuLog: TPopupMenu
    Images = ActionsImages
    Left = 344
    Top = 960
    object Clearlog1: TMenuItem
      Action = actClearLog
    end
  end
  object TimerControlBar: TTimer
    Enabled = False
    Left = 56
    Top = 552
  end
end
