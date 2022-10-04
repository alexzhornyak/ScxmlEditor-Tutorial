object FormHotKeysHelp: TFormHotKeysHelp
  Left = 0
  Top = 0
  Caption = 'Hotkeys (Keyboard shortcuts) help'
  ClientHeight = 682
  ClientWidth = 702
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
  object Panel1: TPanel
    Left = 0
    Top = 644
    Width = 702
    Height = 38
    Align = alBottom
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 0
    object Button1: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 125
      Height = 30
      Align = alLeft
      Caption = 'OK'
      ModalResult = 1
      TabOrder = 0
    end
  end
  object ElHTMLView1: TElHTMLView
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 696
    Height = 638
    Cursor = crDefault
    CheckFormDisplay = True
    Caption = 
      '<FONT COLOR="#008000"><B>Hotkeys (Keyboard shortcuts)</B></FONT>' +
      '<BR>'#13#10'<BR>'#13#10'<FONT COLOR="#804000"><B>Global:</B></FONT><BR>'#13#10'<B>' +
      'Ctrl+N</B> - New project<BR>'#13#10'<B>Shift+Ctrl+N</B> - New unit<BR>' +
      #13#10'<B>Ctrl+O</B> - Open project<BR>'#13#10'<B>Shift+Ctrl+O</B> - Open u' +
      'nit<BR>'#13#10'<BR>'#13#10'<B>Ctrl+W</B> - Close unit<BR>'#13#10'<B>Shift+Ctrl+W</' +
      'B> - Close all<BR>'#13#10'<BR>'#13#10'<B>Ctrl+S</B> - Save unit<BR>'#13#10'<B>Shif' +
      't+Ctrl+S</B> - Save all<BR>'#13#10'<BR>'#13#10'<B>Ctrl+F</B> - Search<BR>'#13#10'<' +
      'BR>'#13#10'<B>Ctrl+B</B> - Build project<BR>'#13#10'<BR>'#13#10'<B>F9 </B>- Run<BR' +
      '>'#13#10'<B>F10 </B>- Pause debug<BR>'#13#10'<B>F11</B> - Next debug step<BR' +
      '>'#13#10'<BR>'#13#10'<FONT COLOR="#804000"><B>Chart Editor:</B></FONT><BR>'#13#10 +
      '        <B>Ctrl+Mouse Right</B> - Init connecting shapes<BR>'#13#10'<B' +
      'R>'#13#10'<FONT COLOR="#804000"><B>When shape is selected:</B></FONT><' +
      'BR>'#13#10'        <B>Ctrl+Left | Right | Up | Down</B> - Move shape w' +
      'ith children by 1 px<BR>'#13#10'        <B>Ctrl+Shift+Left | Right | U' +
      'p | Down</B> - Move shape with children by 1 grid step<BR>'#13#10'    ' +
      '    <B>Ctrl+Alt+Left | Right | Up | Down</B> - Move shape withou' +
      't children by 1 px<BR>'#13#10'<BR>'#13#10'<FONT COLOR="#804000"><B>When conn' +
      'ection is selected:</B></FONT><BR>'#13#10'        <B>Ctrl+Left | Right' +
      ' | Up | Down</B> - Move connection by 1 px<BR>'#13#10'<BR>'#13#10'<B>Shift+L' +
      'eft | Right | Up | Down</B> - Resize shape by 1 px<BR>'#13#10'<BR>'#13#10'<B' +
      '>Ctrl+C | Ctrl+Ins</B> - Copy shape | connection<BR>'#13#10'<B>Shift+C' +
      'trl+C</B> - Copy brief text shape | connection info<BR>'#13#10'<B>Ctrl' +
      '+V | Shift+Ins</B> - Paste shape | connection<BR>'#13#10'<B>Ctrl+X</B>' +
      ' - Cut shape | connection<BR>'#13#10'<B>Ctrl+K</B> - Clone shape<BR>'#13#10 +
      '<B>Del</B> - Delete shape | connection<BR>'#13#10'<BR>'#13#10'<B>Ctrl+A</B> ' +
      '- Select all<BR>'#13#10'<BR>'#13#10'<B>Ctrl+M</B> - Zoom 100% selection in t' +
      'he center of chart<BR>'#13#10'<B>Shift+M</B> - Zoom to fit selection i' +
      'n the bounds of chart<BR>'#13#10'<B>Shift+Ctrl+M</B> - Zoom to fit sel' +
      'ection in the bounds of chart by width<BR>'#13#10'<BR>'#13#10'<B>Ctrl+Z</B> ' +
      '- Undo<BR>'#13#10'<B>Shift+Ctrl+Z</B> - Redo<BR>'#13#10'<BR>'#13#10'<B>Ctrl+1 | 2 ' +
      '| 3 | 4 | 5</B> - Go to the bookmark<BR>'#13#10'Shift+Ctrl+1 | 2 | 3 |' +
      ' 4 | 5 - Set | reset a bookmark<BR>'#13#10'<BR>'#13#10'<B>Ctrl+Back</B> - Go' +
      ' to the previous selection (Undo selection)<BR>'#13#10'<B>Shift+Ctrl+B' +
      'ack</B> - Go to the next selection (Redo selection)<BR>'#13#10'<BR>'#13#10'<' +
      'B>F2</B> - Edit shape text<BR>'#13#10'<B>F3</B> - Zoom to fit all|Zoom' +
      ' 100% - reverse behaviour<BR>'#13#10'<B>Ctrl+F3</B> - Zoom to fit widt' +
      'h of selection<BR>'#13#10'<B>Shift+F3</B> - Zoom to fit height of sele' +
      'ction<BR>'#13#10'<B>F11</B> - Toggle shape nodes tree<BR>'#13#10'<B>F12</B> ' +
      '- Toggle fullscreen mode<BR>'#13#10'<B>ESC</B> - Exit from fullscreen ' +
      'mode | Exit from editing shape text<BR>'#13#10
    VertScrollBarStyles.Step = 11
    WordWrap = False
    Transparent = False
    Flat = False
    GradientStartColor = clBlack
    GradientEndColor = clBlack
    UseCustomScrollBars = False
    Color = clInfoBk
    Align = alClient
  end
end
