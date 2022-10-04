object MacroComposer: TMacroComposer
  Left = 0
  Top = 0
  Caption = 'MacroComposer'
  ClientHeight = 681
  ClientWidth = 773
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 767
    Height = 13
    Align = alTop
    Caption = 'Post-Build CMD Script'
    ExplicitWidth = 102
  end
  object Label2: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 223
    Width = 767
    Height = 13
    Align = alTop
    Caption = 'System macros (Readonly)'
    ExplicitWidth = 128
  end
  object Label3: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 479
    Width = 767
    Height = 13
    Align = alTop
    Caption = 'User macros'
    ExplicitWidth = 59
  end
  object Splitter1: TSplitter
    Left = 0
    Top = 473
    Width = 773
    Height = 3
    Cursor = crVSplit
    Align = alTop
    ExplicitTop = 425
  end
  object Splitter2: TSplitter
    Left = 0
    Top = 217
    Width = 773
    Height = 3
    Cursor = crVSplit
    Align = alTop
    ExplicitLeft = 8
    ExplicitTop = 273
  end
  object LMDEditView1: TLMDEditView
    Left = 0
    Top = 239
    Width = 773
    Height = 234
    Align = alTop
    TabOrder = 0
    Document = EditDocSystem
    TabWidth = 4
    ReadOnlyBgColor = clInfoBk
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    Gutter.Bars = <
      item
        Kind = gbLineNumberBar
      end
      item
        Kind = gbFoldBar
      end>
    CustomKeyBindings = <>
    CompletionSettings.ShowTimeout = 50
    ReadOnly = True
    GutterBarsEmpty = False
  end
  object LMDEditView2: TLMDEditView
    Left = 0
    Top = 495
    Width = 773
    Height = 145
    Align = alClient
    TabOrder = 1
    Document = EditDocUser
    TabWidth = 4
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    Gutter.Bars = <
      item
        Kind = gbLineNumberBar
      end
      item
        Kind = gbFoldBar
      end>
    CustomKeyBindings = <>
    CompletionSettings.ShowTimeout = 50
    GutterBarsEmpty = False
  end
  object TabControl1: TTabControl
    Left = 0
    Top = 19
    Width = 773
    Height = 198
    Align = alTop
    TabOrder = 2
    Tabs.Strings = (
      'Original Script'
      'Evaluated Script (Readonly)')
    TabIndex = 0
    OnChange = TabControl1Change
    object EditView: TLMDEditView
      Left = 4
      Top = 24
      Width = 765
      Height = 170
      Align = alClient
      TabOrder = 0
      Document = EditDocOriginal
      TabWidth = 4
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Courier New'
      Font.Style = []
      Gutter.Bars = <
        item
          Kind = gbLineNumberBar
        end
        item
          Kind = gbFoldBar
        end>
      CustomKeyBindings = <>
      CompletionSettings.ShowTimeout = 50
      CompletionSettings.MinChars = 0
      CompletionSettings.CompletionType = ctCustom
      OnFillCompletionList = EditViewFillCompletionList
      OnCompletionReplace = EditViewCompletionReplace
      OnKeyPressW = EditViewKeyPressW
      GutterBarsEmpty = False
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 640
    Width = 773
    Height = 41
    Align = alBottom
    Caption = 'Panel1'
    ShowCaption = False
    TabOrder = 3
    object Button1: TButton
      AlignWithMargins = True
      Left = 85
      Top = 4
      Width = 75
      Height = 33
      Align = alLeft
      Caption = 'CANCEL'
      ModalResult = 2
      TabOrder = 0
    end
    object Button2: TButton
      AlignWithMargins = True
      Left = 4
      Top = 4
      Width = 75
      Height = 33
      Align = alLeft
      Caption = 'OK'
      ModalResult = 1
      TabOrder = 1
    end
  end
  object EditDocOriginal: TLMDEditDocument
    Lines.Strings = (
      '')
    WordSeparators = '[];,'#39'"-+/\:{}.'
    SyntaxSchemes = <
      item
        Name = 'Lua'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '    <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>                '
          '    '
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Numbers'#39'>'
          '        <Regex token0='#39'number'#39'>'
          '            \b (0x \d+ [lL]? | \d+ (e \d*)? [lLdDfF]? ) \b'
          '        </Regex>'
          '        '
          '        <Regex token0='#39'number'#39'>'
          '            \b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? \d+) \b'
          '        </Regex>'
          '    </Scheme>'
          ''
          
            '    <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39 +
            '  />'
          ''
          '    <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    </Scheme>'
          ''
          
            '    <Scheme name='#39'StringInterpolation'#39' inherit='#39'String'#39' defaultT' +
            'oken='#39'string'#39'>'
          
            '        <Regex token0='#39'escaped'#39' regex='#39'\\[a-z 0 &quot; &apos; ]'#39 +
            ' />'
          '        <Regex token0='#39'escaped'#39' regex='#39'\\ \d{1,3}'#39' />'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'true'#39
          '    >'
          '        <KeywordRegex>\b[a-zA-Z_]\w*\b</KeywordRegex>'
          ''
          '        <Keywords>    '
          '             and       break     do        else      elseif'
          '             end       false     for       function  if'
          '             in        local     nil       not       or'
          
            '             repeat    return    then      true      until     w' +
            'hile            '
          '        </Keywords>'
          ''
          
            '        <RegexBlock innerScheme='#39'StringInterpolation'#39' start_toke' +
            'n0='#39'string'#39' '
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> &quot; | &apos;  </Start>        '
          '            <End> $0 </End>        '
          '        </RegexBlock>'
          ''
          
            '        <RegexBlock innerScheme='#39'String'#39' start_token0='#39'string'#39' p' +
            'riority='#39'100'#39
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> \[ (\=*) \[  </Start>        '
          '            <End> \] $1 \] </End>        '
          '        </RegexBlock>'
          ''
          ''
          
            '        <RegexBlock innerScheme='#39'Comment'#39' start_token0='#39'comment'#39 +
            ' priority='#39'100'#39
          '                    end_token0='#39'comment'#39' >'
          ''
          '            <Start> --\[ (\=*) \[  </Start>        '
          '            <End> \] $1 \] </End>        '
          '        </RegexBlock>'
          ''
          
            '        <Regex innerScheme='#39'Comment'#39' priority='#39'1000'#39' regex='#39'--([' +
            '^ \[ ].*)?$'#39' />'
          '        '
          
            '        <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39 +
            ' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '        <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]  \#  \[ ' +
            ' \( \) &gt; &lt; ]'#39' />'
          '        <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          '        <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> ( kw:local )? kw:function  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '       '
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:if </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:repeat </Start>'
          '            <End> kw:until  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> [ kw:while kw:for ] .+? kw:do  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:do  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'Text'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <TokenAlias name='#39'text'#39' baseToken='#39'default'#39' />'
          '  '
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' keywordsIgnoreCase='#39'false'#39' '
          '          inherit='#39'Text'#39
          '          defaultToken='#39'text'#39'>'
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'XML'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39'>'
          '    <Regex token0='#39'entity'#39'>'
          '        &amp;[a-zA-Z]+;'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> &lt; !--  </Start>        '
          '        <End> --&gt; </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'InsideTag'#39' inherit='#39'BaseSyntax'#39'>'
          
            '    <RegexBlock innerScheme='#39'CDATA'#39' start_token0='#39'cdataTagStart'#39 +
            ' end_token0='#39'cdataTagEnd'#39' >'
          '        <Start> &lt; !\[CDATA  </Start>'
          '        <End> \]\] &gt; </End>        '
          '    </RegexBlock>'
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'WithTags'#39' inherit='#39'Tag'#39' inherit0='#39'BaseSyntax'#39'>'
          '  '
          
            '    <RegexBlock innerScheme='#39'WithTags'#39' start_token0='#39'tagOpen'#39' en' +
            'd_token0='#39'tagClose'#39' >'
          '        <Start> &lt; ([\w:]+) </Start>        '
          '        <End> ( / &gt; ) | ( &lt; / $1 &gt; ) </End>        '
          '    </RegexBlock>  '
          ''
          
            '    <RegexBlock innerScheme='#39'InsideTag'#39' start_token0='#39'tagOpen'#39' e' +
            'nd_token0='#39'tagClose'#39' >'
          '        <Start> &gt;  </Start>        '
          '        <End> (?= &lt; [^ ! ] ) </End>        '
          '    </RegexBlock>  '
          ''
          ''
          '    <SyntaxBlock capture='#39'true'#39'>'
          '        <Start> tagOpen </Start>'
          '        <End> tagClose </End>'
          '    </SyntaxBlock>'
          '    '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' defaultToken='#39'comment'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'XmlDecl'#39' inherit='#39'Tag'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'CDATA'#39' defaultToken='#39'xmlCdata'#39'>'
          '    <SyntaxBlock capture='#39'true'#39'>'
          '        <Start> cdataTagStart </Start>'
          '        <End> cdataTagEnd </End>'
          '    </SyntaxBlock>  '
          '  </Scheme>    '
          '    '
          '  <Scheme name='#39'DTD'#39' defaultToken='#39'comment'#39'>               '
          '  </Scheme>'
          '    '
          '  <Scheme name='#39'AttrStringQuot'#39'>'
          '    <Regex token0='#39'attributeValue'#39'> [^ &quot; ]+</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'AttrStringApos'#39'>'
          '    <Regex token0='#39'attributeValue'#39'> [^ &apos; ]+</Regex>'
          '  </Scheme>'
          '    '
          '  <Scheme name='#39'Tag'#39'>  '
          '  '
          '    <Regex token0='#39'attributeName'#39' priority='#39'10'#39'>'
          '        \b\w+ \s* = \s*        '
          ''
          '    </Regex>'
          ''
          '    <Regex token0='#39'attributeValue'#39'>'
          '        [^ &lt; &gt; &quot; &apos; = \s ]+         '
          '    </Regex>'
          ''
          
            '    <RegexBlock innerScheme='#39'AttrStringQuot'#39' start_token0='#39'attri' +
            'buteValue'#39' '
          '        end_token0='#39'attributeValue'#39' >'
          ''
          '        <Start> &quot;  </Start>        '
          '        <End> &quot; </End>        '
          '    </RegexBlock>'
          ''
          
            '    <RegexBlock innerScheme='#39'AttrStringApos'#39' start_token0='#39'attri' +
            'buteValue'#39' '
          '        end_token0='#39'attributeValue'#39' >'
          ''
          '        <Start> &apos;  </Start>        '
          '        <End> &apos; </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>'
          '  '
          ''
          ''
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39'        '
          '          inherit0='#39'WithTags'#39'      '
          '          keywordsIgnoreCase='#39'true'#39' >'
          '                    '
          ''
          
            '    <RegexBlock innerScheme='#39'DTD'#39' start_token0='#39'dtdTagStartEnd'#39' ' +
            'end_token0='#39'dtdTagStartEnd'#39' >'
          '        <Start> &lt; !\w+  </Start>        '
          '        <End> &gt; </End>        '
          '    </RegexBlock>'
          '                    '
          '    <Regex token0='#39'tagClose'#39'>'
          '        &lt; / [\w:]+ \s* &gt;'
          '    </Regex>'
          ''
          
            '    <RegexBlock innerScheme='#39'XmlDecl'#39' start_token0='#39'xmlDeclStart' +
            #39' end_token0='#39'xmlDeclEnd'#39' >'
          '        <Start> &lt; \? xml  </Start>'
          '        <End> \? &gt; </End>        '
          '    </RegexBlock>'
          ''
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'C'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'>'
          
            '    <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />               ' +
            ' '
          '            '
          '     <SyntaxBlock capture="true">'
          '        <Start> st:/*  </Start>'
          '        <End> st:*/  </End>'
          '     </SyntaxBlock>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />'
          '    <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex token0='#39'badString'#39'> '
          '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ )?'
          '    </Regex>'
          ''
          '    <Regex token0='#39'string'#39' priority='#39'10'#39'> '
          
            '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ ) &ap' +
            'os;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot;'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot; &quot;'
          '    </Regex>'
          ''
          '  </Scheme>'
          '  '
          
            '  <Scheme name='#39'Preprocessor'#39' defaultToken='#39'preprocessor'#39' inheri' +
            't='#39'BaseSyntax'#39' >'
          ''
          '    <IdentRe>\b[a-zA-Z]+\b</IdentRe>'
          ''
          '    <Keywords>'
          '      if ifdef endif else using include define undef pragma'
          '    </Keywords>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BadPreprocessor'#39' defaultToken='#39'badPreprocessor'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          ''
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'Main'#39' inherit='#39'BaseSyntax'#39' inherit0='#39'StringFind'#39 +
            ' '
          '          defaultToken='#39'default'#39'           '
          ''
          '          keywordsIgnoreCase='#39'false'#39
          '  >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          '      asm auto bool break case char const continue'
          '      default do double else enum export'
          '      extern false float for goto if inline int long '
          '      register return short signed sizeof static '
          
            '      switch typedef union unsigned void volatile wchar_t while ' +
            'NULL'
          '      __try __finally __catch'
          '    </Keywords>'
          ''
          ''
          
            '    <Regex innerScheme='#39'Preprocessor'#39' innerContentGroup='#39'1'#39' prio' +
            'rity='#39'10'#39' >'
          '        ^ \s* \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadPreprocessor'#39' innerContentGroup='#39'1'#39'>'
          '        ^ .*? \S .*? \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Preprocessor'#39' '
          '                innerContentGroup='#39'1'#39
          '                priority='#39'100'#39'>'
          '        <Start> ^ \s* \#([a-zA-Z]+) .* \\ \s* $  </Start>'
          '        <End>   (^ .* [^ \\ \s ] \s* $) | (^ \s* $)  </End>'
          '    </RegexBlock>'
          ''
          '    '
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> '
          '            [ preprocessor:if  preprocessor:ifdef  ]'
          '        </Start>'
          ''
          '        <End> [ preprocessor:ifend  preprocessor:endif ] </End>'
          '    </SyntaxBlock>'
          ''
          ''
          '        '
          '    <SyntaxBlock capture="true">'
          '        <Start> id  sym:(  [^ sym:;    sym:}   sym:{   ]*  '
          
            '                sym:) ( sym::   [^ sym:;    sym:}   sym:{   ]+  ' +
            ' )? sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:__try  kw:__fin' +
            'ally   kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  ]  sym:(   [^  sym:;  ' +
            ' sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '             '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'CPP'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'>'
          
            '    <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />               ' +
            ' '
          '            '
          '     <SyntaxBlock capture="true">'
          '        <Start> st:/*  </Start>'
          '        <End> st:*/  </End>'
          '     </SyntaxBlock>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />  '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex token0='#39'badString'#39'> '
          '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ )?'
          '    </Regex>'
          ''
          '    <Regex token0='#39'string'#39' priority='#39'10'#39'> '
          
            '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ ) &ap' +
            'os;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot;'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot; &quot;'
          '    </Regex>'
          '    '
          '  </Scheme>'
          '  '
          
            '  <Scheme name='#39'Preprocessor'#39' defaultToken='#39'preprocessor'#39' inheri' +
            't='#39'BaseSyntax'#39' >'
          ''
          '    <IdentRe>\b[a-zA-Z]+\b</IdentRe>'
          ''
          '    <Keywords>'
          '      if ifdef endif else using include define undef pragma'
          '    </Keywords>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BadPreprocessor'#39' defaultToken='#39'badPreprocessor'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' inherit='#39'BaseSyntax'#39' '
          '          inherit1='#39'StringFind'#39' '
          '          defaultToken='#39'default'#39'           '
          ''
          '          keywordsIgnoreCase='#39'false'#39
          '  >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          
            '      asm auto bool break case catch char class const const_cast' +
            ' continue'
          
            '      default delete do double dynamic_cast else enum explicit e' +
            'xport'
          
            '      extern false float for friend goto if inline int long muta' +
            'ble'
          '      namespace new operator private protected public register'
          
            '      reinterpret_cast return short signed sizeof static static_' +
            'cast struct'
          
            '      switch template this throw true try typedef typeid typenam' +
            'e union'
          
            '      unsigned using virtual void volatile wchar_t while nullptr' +
            '  NULL'
          '      __try __finally'
          '    </Keywords>'
          ''
          ''
          
            '    <Regex innerScheme='#39'Preprocessor'#39' innerContentGroup='#39'1'#39' prio' +
            'rity='#39'10'#39' >'
          '        ^ \s* \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadPreprocessor'#39' innerContentGroup='#39'1'#39'>'
          '        ^ .*? \S .*? \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Preprocessor'#39' '
          '                innerContentGroup='#39'1'#39
          '                priority='#39'100'#39'>'
          '        <Start> ^ \s* \#([a-zA-Z]+) .* \\ \s* $  </Start>'
          '        <End>   (^ .* [^ \\ \s ] \s* $) | (^ \s* $)  </End>'
          '    </RegexBlock>'
          ''
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> '
          '            [ preprocessor:if  preprocessor:ifdef  ]'
          '        </Start>'
          ''
          '        <End> [ preprocessor:ifend  preprocessor:endif ] </End>'
          '    </SyntaxBlock>'
          ''
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> [ kw:class  kw:struct ]'
          '                [^ sym:; ]+ sym:\{ '
          '        </Start>'
          ''
          '        <End> sym:} </End>'
          '    </SyntaxBlock>'
          ''
          '        '
          '    <SyntaxBlock capture="true">'
          '        <Start> id  '
          '                sym:(  '
          '                    [^ sym:;    sym:}   sym:{   ]*  '
          '                sym:) '
          '                ( '
          '                    sym:: '
          '                    [^ sym:;    sym:}   sym:{   ]+   '
          '                )? '
          '                sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:namespace  kw:t' +
            'ry kw:catch  kw:__try  kw:__finally   kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  ]  sym:(   [^  sym:;  ' +
            ' sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '             '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'JavaScript'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '      <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '            <Regex token0='#39'email'#39'>'
          '                [_a-zA-Z\d\-\.]{1,40}'
          '                @ '
          '                ([_ a-z A-Z \d \-]+ '
          '                (\. [_ a-z A-Z \d \-]+ )+ )'
          '            </Regex>                '
          '      </Scheme>'
          ''
          '      <Scheme name='#39'Numbers'#39'>'
          
            '        <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [' +
            'lLdDfF]? ) \b</Regex>'
          
            '        <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\' +
            '-]? \d+) \b</Regex>'
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'commen' +
            't'#39'>'
          
            '        <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />           ' +
            '     '
          '                '
          '         <SyntaxBlock capture="true">'
          '            <Start> st:/*  </Start>'
          '            <End> st:*/  </End>'
          '         </SyntaxBlock>                '
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39 +
            '>'
          
            '        <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />   ' +
            '   '
          '        <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '        <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'ba' +
            'dString'#39'>'
          '      </Scheme>'
          ''
          '      <Scheme name='#39'StringFind'#39'>'
          '        <Regex innerScheme='#39'BadString'#39' > '
          '            (&quot;|&apos;) (.*?\\ \1 )*? .* $ '
          '        </Regex>'
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '            (&quot;|&apos;) (.*?\\ \1 )*? \1'
          '        </Regex>'
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '            (&quot;|&apos;) (.*?\\ \1 )*? .*? [^\\] \1'
          '        </Regex>  '
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '            (&quot;|&apos;) \1'
          '        </Regex>'
          '        '
          '      </Scheme>'
          ''
          '    <Scheme name='#39'RegexFind'#39' defaultToken='#39'defaultEmbed'#39'>'
          '        <Regex token0='#39'escaped'#39' regex='#39'\\/'#39' />            '
          '    </Scheme>'
          '    '
          
            '    <Scheme name='#39'RegexSubst'#39' inherit='#39'RegexFind'#39' defaultToken='#39 +
            'defaultEmbed'#39'>'
          '        <Regex token0='#39'escaped'#39' regex='#39'\$\d'#39' />                '
          '    </Scheme>'
          ''
          '    <Scheme name='#39'DefaultEmpty'#39' defaultToken='#39'default'#39' />'
          '    '
          '    <Scheme name='#39'Main'#39' inherit='#39'StringFind'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'false'#39
          '    >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          '            Boolean'
          '            Array'
          '            join length reverse sort'
          '            NaN '
          '            null '
          '            true false '
          '            Infinity '
          '            undefined '
          '            Break '#9#9'continue '#9#9#9'for  in  if else return  while '
          '            '
          '            function new this var with  arguments '
          '            '
          '            Global escape unescape eval Number '
          '            Object new instanceof  delete typeof void '#9#9#9
          '            throw  try catch finally with'
          '    </Keywords>'
          ''
          ''
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          '    <RegexBlock innerScheme='#39'DefaultEmpty'#39' '
          '                start_token0='#39'symbol'#39
          '                end_token0='#39'symbol'#39' '
          '                priority='#39'100'#39
          '                chainBlock='#39'RegexFindEnd'#39
          '    >'
          ''
          '        <Start> \s* ([ =( ]) \s* (?= / )  </Start>        '
          '        <End> (?= / )  </End>        '
          '    </RegexBlock>'
          '    '
          '    <ChainBlock name='#39'RegexFindEnd'#39' '
          '                innerScheme='#39'RegexFind'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                chainBlock='#39'RegexFindEnd2'#39
          '    >'
          '        <End> / </End>        '
          '    </ChainBlock>'
          ''
          '    <ChainBlock name='#39'RegexFindEnd2'#39' '
          '                innerScheme='#39'RegexFind'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '    >'
          '        <End> /[igmx]* </End>        '
          '    </ChainBlock>'
          '        '
          '    <ChainBlock name='#39'RegexSubstEnd'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                innerScheme='#39'RegexSubst'#39'  >'
          '        <End> /[igmx]* </End>        '
          '    </ChainBlock>'
          '    '
          '    <RegexBlock innerScheme='#39'RegexFind'#39' '
          '                start_token0='#39'regexStart'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                chainBlock='#39'RegexSubstEnd'#39'>'
          ''
          '        <Start> s/  </Start>        '
          '        <End> /[igmx]* </End>        '
          '    </RegexBlock>'
          ''
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> kw:function  id  '
          '                sym:(  '
          '                        [^ sym:;    sym:}   sym:{   ]*  '
          '                sym:) '
          '                sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:try kw:catch  k' +
            'w:finally  kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  kw:with ]  sym:(   [^ ' +
            ' sym:;   sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '            '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'INI'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'tagOpen'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLd' +
            'DfF]? ) \b</Regex>'
          
            '    <Regex token0='#39'tagOpen'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]?' +
            ' \d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> ^\#  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> ^;  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Url'#39' end_token0='#39'url'#39' >'
          '        <Start> ^\[  </Start>        '
          '        <End> \]$ </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>    '
          ''
          
            '  <Scheme name='#39'String'#39' inherit='#39'BaseSyntax'#39' defaultToken='#39'strin' +
            'g'#39'>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot; c?'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot; c?'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot;  &quot; c?'
          '    </Regex>    '
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'Comment'#39' defaultToken='#39'comment'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Url'#39' defaultToken='#39'url'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39' inherit0='#39'StringFind'#39
          '          keywordsIgnoreCase='#39'true'#39' >'
          '  </Scheme>'
          '  '
          '</SchemeList>')
      end
      item
        Name = 'DFM'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          ''
          '    <Scheme name='#39'Numbers'#39'>'
          '        <Regex token0='#39'number'#39'>'
          '            \b (0x \d+ [lL]? | \d+ (e \d*)? [lLdDfF]? ) \b'
          '        </Regex>'
          '        '
          '        <Regex token0='#39'number'#39'>'
          '            \b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? \d+) \b'
          '        </Regex>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'String'#39' defaultToken='#39'string'#39'>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Binary'#39' defaultToken='#39'comment'#39'>        '
          '        <SyntaxBlock capture="true">'
          '            <Start> commentStart  </Start>'
          '            <End> commentEnd  </End>'
          '        </SyntaxBlock>        '
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'true'#39
          '    >'
          '        <KeywordRegex>\b[a-zA-Z_]\w*\b</KeywordRegex>'
          ''
          '        <Keywords>    '
          '            object  end item'
          '        </Keywords>'
          ''
          '        <RegexBlock innerScheme='#39'String'#39' start_token0='#39'string'#39' '
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> &apos;  </Start>        '
          '            <End> &apos; </End>        '
          '        </RegexBlock>'
          ''
          
            '        <RegexBlock innerScheme='#39'Binary'#39' start_token0='#39'commentSt' +
            'art'#39' '
          '                    end_token0='#39'commentEnd'#39' >'
          '            <Start> \{  </Start>        '
          '            <End> \} </End>        '
          '        </RegexBlock>'
          ''
          
            '        <Regex token0='#39'symbol'#39' regex='#39'[   \]  \#  \[  \( \) &gt;' +
            ' &lt; ]'#39' />'
          '        <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          '        '
          '        <SyntaxBlock capture="true">'
          '            <Start> [ kw:item kw:object ] </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> sym:&lt;  </Start>'
          '            <End> sym:&gt;  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> sym:(  </Start>'
          '            <End> sym:)  </End>'
          '        </SyntaxBlock>'
          ''
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'Batch'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'  ' +
            '/>'
          '  '
          '  <Scheme name='#39'Variable'#39' inherit='#39'Text'#39' defaultToken='#39'url'#39'  >'
          '     <Regex token0='#39'url'#39'>\%[^\%]*\%</Regex>   '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Label'#39' inherit='#39'Text'#39' defaultToken='#39'tagOpen'#39'  >'
          '     <Regex token0='#39'tagOpen'#39'>\:[a-zA-Z_][\w_]*\b</Regex>   '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Macros'#39' inherit='#39'Text'#39' defaultToken='#39'url'#39'  >'
          '     <Regex token0='#39'url'#39'>\$\([a-zA-Z_][\w_]*\)</Regex>   '
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'escaped'#39'>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot; c?'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot; c?'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot;  &quot; c?'
          '    </Regex>    '
          '  </Scheme>'
          ''
          ''
          ''
          
            '  <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' inherit0='#39'StringFind'#39' in' +
            'herit1='#39'Variable'#39
          '  inherit2='#39'Label'#39' inherit3='#39'Macros'#39
          '          defaultToken='#39'default'#39'           '
          ''
          '          allIgnoreCase='#39'true'#39
          '  >'
          
            '    <KeywordRegex moreWordChars='#39'.'#39'>\b[a-zA-Z_][\w_]*\b</Keyword' +
            'Regex>'
          ''
          '    <Keywords>'
          '            rem set if else exist errorlevel'
          '            for in do break call copy chcp cd'
          '            chdir choice cls country ctty date del'
          '            erase dir echo exit goto loadfix loadhigh'
          '            mkdir md move path pause prompt rename ren'
          '            rmdir rd shift time type ver verify vol'
          '            com con lpt nul defined not errorlevel'
          '            cmdextversion'
          '            '
          '    </Keywords>'
          ''
          ''
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'Rem.*$'#39' />'
          '    <Regex innerScheme='#39'Comment'#39' regex="\:\:.*$ " />'
          ''
          '    <Regex token0='#39'keyword'#39' regex="^\s*\b[a-zA-Z_][\w_]*\b" />'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[    \]    \[  \( \) &gt; &lt;' +
            '  &amp; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-=+^;,]'#39' />'
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '     <SyntaxBlock capture="true" priority='#39'0'#39'  >'
          '        <Start> sym:(  </Start>'
          '        <End> sym:)  </End>'
          '    </SyntaxBlock>'
          '    '
          '  </Scheme>'
          '</SchemeList>')
      end>
    ColorSchemes = <
      item
        Name = 'default'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          '<Attributes>'
          #9'<Token name="default" textColor="#0"/>'
          #9'<Token name="defaultEmbed" backgroundColor="#ECE9D8"/>'
          #9'<Token name="docStart" fontStyle="bold"/>'
          #9'<Token name="docEnd" fontStyle="bold"/>'
          #9'<Token name="docBlockStart" fontStyle="bold"/>'
          #9'<Token name="docBlockEnd" fontStyle="bold"/>'
          #9'<Token name="docInstruction" fontStyle="bold"/>'
          #9'<Token name="docDefault" backgroundColor="#ECE9D8"/>'
          #9'<Token name="comment" textColor="#008000"/>'
          #9'<Token name="commentStart" textColor="#008000"/>'
          #9'<Token name="commentEnd" textColor="#008000"/>'
          #9'<Token name="keyword" textColor="#0F0FFF" fontStyle="bold"/>'
          #9'<Token name="privateSymbol" textColor="Navy"/>'
          #9'<Token name="email" fontStyle="bold"/>'
          
            #9'<Token name="hairy" textColor="#000080" fontStyle="bold, italic' +
            ', underline, strikeOut"/>'
          #9'<Token name="number" textColor="Blue"/>'
          #9'<Token name="symbol" textColor="Blue"/>'
          #9'<Token name="string" textColor="Blue"/>'
          #9'<Token name="blockStart" fontStyle="bold"/>'
          #9'<Token name="blockEnd" fontStyle="bold"/>'
          #9'<Token name="badString" textColor="Red"/>'
          #9'<Token name="preprocessor" textColor="#008080"/>'
          #9'<Token name="badPreprocessor" textColor="red"/>'
          #9'<Token name="escaped" textColor="Navy"/>'
          
            #9'<Token name="quotedVariable" textColor="Navy" fontStyle="bold"/' +
            '>'
          #9'<Token name="regexStart" textColor="Navy" fontStyle="bold"/>'
          
            #9'<Token name="regexEnd" textColor="Navy" fontStyle="bold"/><!--h' +
            'tml/xml tokens-->'
          #9'<Token name="tagOpen" textColor="#FF8000"/>'
          #9'<Token name="tagClose" textColor="Blue"/>'
          
            #9'<Token name="dtdTagStartEnd" textColor="Purple" fontStyle="bold' +
            '"/>'
          #9'<Token name="url" textColor="Red"/>'
          #9'<Token name="fpi" textColor="Red" fontStyle="bold"/>'
          #9'<Token name="entity" textColor="#FF4500"/>'
          #9'<Token name="attributeName" textColor="Red"/>'
          
            #9'<Token name="attributeValue" textColor="#8000FF" fontStyle="bol' +
            'd"/>'
          #9'<Token name="badAttributeValue" textColor="Red"/>'
          #9'<Token name="cssRule" textColor="#583485" fontStyle="bold"/>'
          #9'<Token name="cssRelSymbol" fontStyle="bold"/>'
          #9'<Token name="cssUrlHref" fontStyle="underline"/>'
          #9'<Token name="cssUrlAttr"/>'
          #9'<Token name="cssAttrListBG" textColor="Blue"/>'
          
            #9'<Token name="cdataTagStart" textColor="Purple" fontStyle="bold"' +
            '/>'
          #9'<Token name="cdataTagEnd" textColor="Purple" fontStyle="bold"/>'
          
            #9'<Token name="xmlDeclStart" backgroundColor="Yellow" textColor="' +
            'Red" fontStyle="bold"/>'
          
            #9'<Token name="xmlDeclEnd" backgroundColor="Yellow" textColor="Re' +
            'd" fontStyle="bold"/>'
          '</Attributes>')
      end
      item
        Name = 'Lua'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          '<Attributes><!--html/xml tokens-->'
          #9'<Token name="comment" textColor="Green" fontStyle="bold"/>'
          #9'<Token name="string" textColor="Gray"/>'
          #9'<Token name="keyword" textColor="Blue" fontStyle="bold"/>'
          #9'<Token name="number" textColor="#FF8000"/>'
          '</Attributes>')
      end>
    ActiveSyntaxScheme = 'Batch'
    ActiveColorScheme = 'default'
    OnDocChanged = EditDocOriginalDocChanged
    Left = 224
    Top = 120
  end
  object EditDocEvaluated: TLMDEditDocument
    Lines.Strings = (
      '')
    WordSeparators = '[];,'#39'"-+/\:{}.'
    SyntaxSchemes = <
      item
        Name = 'Lua'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '    <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>                '
          '    '
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Numbers'#39'>'
          '        <Regex token0='#39'number'#39'>'
          '            \b (0x \d+ [lL]? | \d+ (e \d*)? [lLdDfF]? ) \b'
          '        </Regex>'
          '        '
          '        <Regex token0='#39'number'#39'>'
          '            \b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? \d+) \b'
          '        </Regex>'
          '    </Scheme>'
          ''
          
            '    <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39 +
            '  />'
          ''
          '    <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    </Scheme>'
          ''
          
            '    <Scheme name='#39'StringInterpolation'#39' inherit='#39'String'#39' defaultT' +
            'oken='#39'string'#39'>'
          
            '        <Regex token0='#39'escaped'#39' regex='#39'\\[a-z 0 &quot; &apos; ]'#39 +
            ' />'
          '        <Regex token0='#39'escaped'#39' regex='#39'\\ \d{1,3}'#39' />'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'true'#39
          '    >'
          '        <KeywordRegex>\b[a-zA-Z_]\w*\b</KeywordRegex>'
          ''
          '        <Keywords>    '
          '             and       break     do        else      elseif'
          '             end       false     for       function  if'
          '             in        local     nil       not       or'
          
            '             repeat    return    then      true      until     w' +
            'hile            '
          '        </Keywords>'
          ''
          
            '        <RegexBlock innerScheme='#39'StringInterpolation'#39' start_toke' +
            'n0='#39'string'#39' '
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> &quot; | &apos;  </Start>        '
          '            <End> $0 </End>        '
          '        </RegexBlock>'
          ''
          
            '        <RegexBlock innerScheme='#39'String'#39' start_token0='#39'string'#39' p' +
            'riority='#39'100'#39
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> \[ (\=*) \[  </Start>        '
          '            <End> \] $1 \] </End>        '
          '        </RegexBlock>'
          ''
          ''
          
            '        <RegexBlock innerScheme='#39'Comment'#39' start_token0='#39'comment'#39 +
            ' priority='#39'100'#39
          '                    end_token0='#39'comment'#39' >'
          ''
          '            <Start> --\[ (\=*) \[  </Start>        '
          '            <End> \] $1 \] </End>        '
          '        </RegexBlock>'
          ''
          
            '        <Regex innerScheme='#39'Comment'#39' priority='#39'1000'#39' regex='#39'--([' +
            '^ \[ ].*)?$'#39' />'
          '        '
          
            '        <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39 +
            ' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '        <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]  \#  \[ ' +
            ' \( \) &gt; &lt; ]'#39' />'
          '        <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          '        <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> ( kw:local )? kw:function  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '       '
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:if </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:repeat </Start>'
          '            <End> kw:until  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> [ kw:while kw:for ] .+? kw:do  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:do  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'Text'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <TokenAlias name='#39'text'#39' baseToken='#39'default'#39' />'
          '  '
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' keywordsIgnoreCase='#39'false'#39' '
          '          inherit='#39'Text'#39
          '          defaultToken='#39'text'#39'>'
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'XML'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39'>'
          '    <Regex token0='#39'entity'#39'>'
          '        &amp;[a-zA-Z]+;'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> &lt; !--  </Start>        '
          '        <End> --&gt; </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'InsideTag'#39' inherit='#39'BaseSyntax'#39'>'
          
            '    <RegexBlock innerScheme='#39'CDATA'#39' start_token0='#39'cdataTagStart'#39 +
            ' end_token0='#39'cdataTagEnd'#39' >'
          '        <Start> &lt; !\[CDATA  </Start>'
          '        <End> \]\] &gt; </End>        '
          '    </RegexBlock>'
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'WithTags'#39' inherit='#39'Tag'#39' inherit0='#39'BaseSyntax'#39'>'
          '  '
          
            '    <RegexBlock innerScheme='#39'WithTags'#39' start_token0='#39'tagOpen'#39' en' +
            'd_token0='#39'tagClose'#39' >'
          '        <Start> &lt; ([\w:]+) </Start>        '
          '        <End> ( / &gt; ) | ( &lt; / $1 &gt; ) </End>        '
          '    </RegexBlock>  '
          ''
          
            '    <RegexBlock innerScheme='#39'InsideTag'#39' start_token0='#39'tagOpen'#39' e' +
            'nd_token0='#39'tagClose'#39' >'
          '        <Start> &gt;  </Start>        '
          '        <End> (?= &lt; [^ ! ] ) </End>        '
          '    </RegexBlock>  '
          ''
          ''
          '    <SyntaxBlock capture='#39'true'#39'>'
          '        <Start> tagOpen </Start>'
          '        <End> tagClose </End>'
          '    </SyntaxBlock>'
          '    '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' defaultToken='#39'comment'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'XmlDecl'#39' inherit='#39'Tag'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'CDATA'#39' defaultToken='#39'xmlCdata'#39'>'
          '    <SyntaxBlock capture='#39'true'#39'>'
          '        <Start> cdataTagStart </Start>'
          '        <End> cdataTagEnd </End>'
          '    </SyntaxBlock>  '
          '  </Scheme>    '
          '    '
          '  <Scheme name='#39'DTD'#39' defaultToken='#39'comment'#39'>               '
          '  </Scheme>'
          '    '
          '  <Scheme name='#39'AttrStringQuot'#39'>'
          '    <Regex token0='#39'attributeValue'#39'> [^ &quot; ]+</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'AttrStringApos'#39'>'
          '    <Regex token0='#39'attributeValue'#39'> [^ &apos; ]+</Regex>'
          '  </Scheme>'
          '    '
          '  <Scheme name='#39'Tag'#39'>  '
          '  '
          '    <Regex token0='#39'attributeName'#39' priority='#39'10'#39'>'
          '        \b\w+ \s* = \s*        '
          ''
          '    </Regex>'
          ''
          '    <Regex token0='#39'attributeValue'#39'>'
          '        [^ &lt; &gt; &quot; &apos; = \s ]+         '
          '    </Regex>'
          ''
          
            '    <RegexBlock innerScheme='#39'AttrStringQuot'#39' start_token0='#39'attri' +
            'buteValue'#39' '
          '        end_token0='#39'attributeValue'#39' >'
          ''
          '        <Start> &quot;  </Start>        '
          '        <End> &quot; </End>        '
          '    </RegexBlock>'
          ''
          
            '    <RegexBlock innerScheme='#39'AttrStringApos'#39' start_token0='#39'attri' +
            'buteValue'#39' '
          '        end_token0='#39'attributeValue'#39' >'
          ''
          '        <Start> &apos;  </Start>        '
          '        <End> &apos; </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>'
          '  '
          ''
          ''
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39'        '
          '          inherit0='#39'WithTags'#39'      '
          '          keywordsIgnoreCase='#39'true'#39' >'
          '                    '
          ''
          
            '    <RegexBlock innerScheme='#39'DTD'#39' start_token0='#39'dtdTagStartEnd'#39' ' +
            'end_token0='#39'dtdTagStartEnd'#39' >'
          '        <Start> &lt; !\w+  </Start>        '
          '        <End> &gt; </End>        '
          '    </RegexBlock>'
          '                    '
          '    <Regex token0='#39'tagClose'#39'>'
          '        &lt; / [\w:]+ \s* &gt;'
          '    </Regex>'
          ''
          
            '    <RegexBlock innerScheme='#39'XmlDecl'#39' start_token0='#39'xmlDeclStart' +
            #39' end_token0='#39'xmlDeclEnd'#39' >'
          '        <Start> &lt; \? xml  </Start>'
          '        <End> \? &gt; </End>        '
          '    </RegexBlock>'
          ''
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'C'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'>'
          
            '    <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />               ' +
            ' '
          '            '
          '     <SyntaxBlock capture="true">'
          '        <Start> st:/*  </Start>'
          '        <End> st:*/  </End>'
          '     </SyntaxBlock>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />'
          '    <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex token0='#39'badString'#39'> '
          '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ )?'
          '    </Regex>'
          ''
          '    <Regex token0='#39'string'#39' priority='#39'10'#39'> '
          
            '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ ) &ap' +
            'os;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot;'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot; &quot;'
          '    </Regex>'
          ''
          '  </Scheme>'
          '  '
          
            '  <Scheme name='#39'Preprocessor'#39' defaultToken='#39'preprocessor'#39' inheri' +
            't='#39'BaseSyntax'#39' >'
          ''
          '    <IdentRe>\b[a-zA-Z]+\b</IdentRe>'
          ''
          '    <Keywords>'
          '      if ifdef endif else using include define undef pragma'
          '    </Keywords>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BadPreprocessor'#39' defaultToken='#39'badPreprocessor'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          ''
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'Main'#39' inherit='#39'BaseSyntax'#39' inherit0='#39'StringFind'#39 +
            ' '
          '          defaultToken='#39'default'#39'           '
          ''
          '          keywordsIgnoreCase='#39'false'#39
          '  >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          '      asm auto bool break case char const continue'
          '      default do double else enum export'
          '      extern false float for goto if inline int long '
          '      register return short signed sizeof static '
          
            '      switch typedef union unsigned void volatile wchar_t while ' +
            'NULL'
          '      __try __finally __catch'
          '    </Keywords>'
          ''
          ''
          
            '    <Regex innerScheme='#39'Preprocessor'#39' innerContentGroup='#39'1'#39' prio' +
            'rity='#39'10'#39' >'
          '        ^ \s* \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadPreprocessor'#39' innerContentGroup='#39'1'#39'>'
          '        ^ .*? \S .*? \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Preprocessor'#39' '
          '                innerContentGroup='#39'1'#39
          '                priority='#39'100'#39'>'
          '        <Start> ^ \s* \#([a-zA-Z]+) .* \\ \s* $  </Start>'
          '        <End>   (^ .* [^ \\ \s ] \s* $) | (^ \s* $)  </End>'
          '    </RegexBlock>'
          ''
          '    '
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> '
          '            [ preprocessor:if  preprocessor:ifdef  ]'
          '        </Start>'
          ''
          '        <End> [ preprocessor:ifend  preprocessor:endif ] </End>'
          '    </SyntaxBlock>'
          ''
          ''
          '        '
          '    <SyntaxBlock capture="true">'
          '        <Start> id  sym:(  [^ sym:;    sym:}   sym:{   ]*  '
          
            '                sym:) ( sym::   [^ sym:;    sym:}   sym:{   ]+  ' +
            ' )? sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:__try  kw:__fin' +
            'ally   kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  ]  sym:(   [^  sym:;  ' +
            ' sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '             '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'CPP'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'>'
          
            '    <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />               ' +
            ' '
          '            '
          '     <SyntaxBlock capture="true">'
          '        <Start> st:/*  </Start>'
          '        <End> st:*/  </End>'
          '     </SyntaxBlock>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />  '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex token0='#39'badString'#39'> '
          '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ )?'
          '    </Regex>'
          ''
          '    <Regex token0='#39'string'#39' priority='#39'10'#39'> '
          
            '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ ) &ap' +
            'os;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot;'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot; &quot;'
          '    </Regex>'
          '    '
          '  </Scheme>'
          '  '
          
            '  <Scheme name='#39'Preprocessor'#39' defaultToken='#39'preprocessor'#39' inheri' +
            't='#39'BaseSyntax'#39' >'
          ''
          '    <IdentRe>\b[a-zA-Z]+\b</IdentRe>'
          ''
          '    <Keywords>'
          '      if ifdef endif else using include define undef pragma'
          '    </Keywords>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BadPreprocessor'#39' defaultToken='#39'badPreprocessor'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' inherit='#39'BaseSyntax'#39' '
          '          inherit1='#39'StringFind'#39' '
          '          defaultToken='#39'default'#39'           '
          ''
          '          keywordsIgnoreCase='#39'false'#39
          '  >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          
            '      asm auto bool break case catch char class const const_cast' +
            ' continue'
          
            '      default delete do double dynamic_cast else enum explicit e' +
            'xport'
          
            '      extern false float for friend goto if inline int long muta' +
            'ble'
          '      namespace new operator private protected public register'
          
            '      reinterpret_cast return short signed sizeof static static_' +
            'cast struct'
          
            '      switch template this throw true try typedef typeid typenam' +
            'e union'
          
            '      unsigned using virtual void volatile wchar_t while nullptr' +
            '  NULL'
          '      __try __finally'
          '    </Keywords>'
          ''
          ''
          
            '    <Regex innerScheme='#39'Preprocessor'#39' innerContentGroup='#39'1'#39' prio' +
            'rity='#39'10'#39' >'
          '        ^ \s* \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadPreprocessor'#39' innerContentGroup='#39'1'#39'>'
          '        ^ .*? \S .*? \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Preprocessor'#39' '
          '                innerContentGroup='#39'1'#39
          '                priority='#39'100'#39'>'
          '        <Start> ^ \s* \#([a-zA-Z]+) .* \\ \s* $  </Start>'
          '        <End>   (^ .* [^ \\ \s ] \s* $) | (^ \s* $)  </End>'
          '    </RegexBlock>'
          ''
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> '
          '            [ preprocessor:if  preprocessor:ifdef  ]'
          '        </Start>'
          ''
          '        <End> [ preprocessor:ifend  preprocessor:endif ] </End>'
          '    </SyntaxBlock>'
          ''
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> [ kw:class  kw:struct ]'
          '                [^ sym:; ]+ sym:\{ '
          '        </Start>'
          ''
          '        <End> sym:} </End>'
          '    </SyntaxBlock>'
          ''
          '        '
          '    <SyntaxBlock capture="true">'
          '        <Start> id  '
          '                sym:(  '
          '                    [^ sym:;    sym:}   sym:{   ]*  '
          '                sym:) '
          '                ( '
          '                    sym:: '
          '                    [^ sym:;    sym:}   sym:{   ]+   '
          '                )? '
          '                sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:namespace  kw:t' +
            'ry kw:catch  kw:__try  kw:__finally   kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  ]  sym:(   [^  sym:;  ' +
            ' sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '             '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'JavaScript'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '      <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '            <Regex token0='#39'email'#39'>'
          '                [_a-zA-Z\d\-\.]{1,40}'
          '                @ '
          '                ([_ a-z A-Z \d \-]+ '
          '                (\. [_ a-z A-Z \d \-]+ )+ )'
          '            </Regex>                '
          '      </Scheme>'
          ''
          '      <Scheme name='#39'Numbers'#39'>'
          
            '        <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [' +
            'lLdDfF]? ) \b</Regex>'
          
            '        <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\' +
            '-]? \d+) \b</Regex>'
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'commen' +
            't'#39'>'
          
            '        <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />           ' +
            '     '
          '                '
          '         <SyntaxBlock capture="true">'
          '            <Start> st:/*  </Start>'
          '            <End> st:*/  </End>'
          '         </SyntaxBlock>                '
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39 +
            '>'
          
            '        <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />   ' +
            '   '
          '        <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '        <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'ba' +
            'dString'#39'>'
          '      </Scheme>'
          ''
          '      <Scheme name='#39'StringFind'#39'>'
          '        <Regex innerScheme='#39'BadString'#39' > '
          '            (&quot;|&apos;) (.*?\\ \1 )*? .* $ '
          '        </Regex>'
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '            (&quot;|&apos;) (.*?\\ \1 )*? \1'
          '        </Regex>'
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '            (&quot;|&apos;) (.*?\\ \1 )*? .*? [^\\] \1'
          '        </Regex>  '
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '            (&quot;|&apos;) \1'
          '        </Regex>'
          '        '
          '      </Scheme>'
          ''
          '    <Scheme name='#39'RegexFind'#39' defaultToken='#39'defaultEmbed'#39'>'
          '        <Regex token0='#39'escaped'#39' regex='#39'\\/'#39' />            '
          '    </Scheme>'
          '    '
          
            '    <Scheme name='#39'RegexSubst'#39' inherit='#39'RegexFind'#39' defaultToken='#39 +
            'defaultEmbed'#39'>'
          '        <Regex token0='#39'escaped'#39' regex='#39'\$\d'#39' />                '
          '    </Scheme>'
          ''
          '    <Scheme name='#39'DefaultEmpty'#39' defaultToken='#39'default'#39' />'
          '    '
          '    <Scheme name='#39'Main'#39' inherit='#39'StringFind'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'false'#39
          '    >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          '            Boolean'
          '            Array'
          '            join length reverse sort'
          '            NaN '
          '            null '
          '            true false '
          '            Infinity '
          '            undefined '
          '            Break '#9#9'continue '#9#9#9'for  in  if else return  while '
          '            '
          '            function new this var with  arguments '
          '            '
          '            Global escape unescape eval Number '
          '            Object new instanceof  delete typeof void '#9#9#9
          '            throw  try catch finally with'
          '    </Keywords>'
          ''
          ''
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          '    <RegexBlock innerScheme='#39'DefaultEmpty'#39' '
          '                start_token0='#39'symbol'#39
          '                end_token0='#39'symbol'#39' '
          '                priority='#39'100'#39
          '                chainBlock='#39'RegexFindEnd'#39
          '    >'
          ''
          '        <Start> \s* ([ =( ]) \s* (?= / )  </Start>        '
          '        <End> (?= / )  </End>        '
          '    </RegexBlock>'
          '    '
          '    <ChainBlock name='#39'RegexFindEnd'#39' '
          '                innerScheme='#39'RegexFind'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                chainBlock='#39'RegexFindEnd2'#39
          '    >'
          '        <End> / </End>        '
          '    </ChainBlock>'
          ''
          '    <ChainBlock name='#39'RegexFindEnd2'#39' '
          '                innerScheme='#39'RegexFind'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '    >'
          '        <End> /[igmx]* </End>        '
          '    </ChainBlock>'
          '        '
          '    <ChainBlock name='#39'RegexSubstEnd'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                innerScheme='#39'RegexSubst'#39'  >'
          '        <End> /[igmx]* </End>        '
          '    </ChainBlock>'
          '    '
          '    <RegexBlock innerScheme='#39'RegexFind'#39' '
          '                start_token0='#39'regexStart'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                chainBlock='#39'RegexSubstEnd'#39'>'
          ''
          '        <Start> s/  </Start>        '
          '        <End> /[igmx]* </End>        '
          '    </RegexBlock>'
          ''
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> kw:function  id  '
          '                sym:(  '
          '                        [^ sym:;    sym:}   sym:{   ]*  '
          '                sym:) '
          '                sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:try kw:catch  k' +
            'w:finally  kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  kw:with ]  sym:(   [^ ' +
            ' sym:;   sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '            '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'INI'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'tagOpen'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLd' +
            'DfF]? ) \b</Regex>'
          
            '    <Regex token0='#39'tagOpen'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]?' +
            ' \d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> ^\#  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> ^;  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Url'#39' end_token0='#39'url'#39' >'
          '        <Start> ^\[  </Start>        '
          '        <End> \]$ </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>    '
          ''
          
            '  <Scheme name='#39'String'#39' inherit='#39'BaseSyntax'#39' defaultToken='#39'strin' +
            'g'#39'>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot; c?'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot; c?'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot;  &quot; c?'
          '    </Regex>    '
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'Comment'#39' defaultToken='#39'comment'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Url'#39' defaultToken='#39'url'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39' inherit0='#39'StringFind'#39
          '          keywordsIgnoreCase='#39'true'#39' >'
          '  </Scheme>'
          '  '
          '</SchemeList>')
      end
      item
        Name = 'DFM'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          ''
          '    <Scheme name='#39'Numbers'#39'>'
          '        <Regex token0='#39'number'#39'>'
          '            \b (0x \d+ [lL]? | \d+ (e \d*)? [lLdDfF]? ) \b'
          '        </Regex>'
          '        '
          '        <Regex token0='#39'number'#39'>'
          '            \b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? \d+) \b'
          '        </Regex>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'String'#39' defaultToken='#39'string'#39'>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Binary'#39' defaultToken='#39'comment'#39'>        '
          '        <SyntaxBlock capture="true">'
          '            <Start> commentStart  </Start>'
          '            <End> commentEnd  </End>'
          '        </SyntaxBlock>        '
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'true'#39
          '    >'
          '        <KeywordRegex>\b[a-zA-Z_]\w*\b</KeywordRegex>'
          ''
          '        <Keywords>    '
          '            object  end item'
          '        </Keywords>'
          ''
          '        <RegexBlock innerScheme='#39'String'#39' start_token0='#39'string'#39' '
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> &apos;  </Start>        '
          '            <End> &apos; </End>        '
          '        </RegexBlock>'
          ''
          
            '        <RegexBlock innerScheme='#39'Binary'#39' start_token0='#39'commentSt' +
            'art'#39' '
          '                    end_token0='#39'commentEnd'#39' >'
          '            <Start> \{  </Start>        '
          '            <End> \} </End>        '
          '        </RegexBlock>'
          ''
          
            '        <Regex token0='#39'symbol'#39' regex='#39'[   \]  \#  \[  \( \) &gt;' +
            ' &lt; ]'#39' />'
          '        <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          '        '
          '        <SyntaxBlock capture="true">'
          '            <Start> [ kw:item kw:object ] </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> sym:&lt;  </Start>'
          '            <End> sym:&gt;  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> sym:(  </Start>'
          '            <End> sym:)  </End>'
          '        </SyntaxBlock>'
          ''
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'Batch'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'  ' +
            '/>'
          '  '
          '  <Scheme name='#39'Variable'#39' inherit='#39'Text'#39' defaultToken='#39'url'#39'  >'
          '     <Regex token0='#39'url'#39'>\%[^\%]*\%</Regex>   '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Label'#39' inherit='#39'Text'#39' defaultToken='#39'tagOpen'#39'  >'
          '     <Regex token0='#39'tagOpen'#39'>\:[a-zA-Z_][\w_]*\b</Regex>   '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Macros'#39' inherit='#39'Text'#39' defaultToken='#39'url'#39'  >'
          '     <Regex token0='#39'url'#39'>\$\([a-zA-Z_][\w_]*\)</Regex>   '
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'escaped'#39'>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot; c?'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot; c?'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot;  &quot; c?'
          '    </Regex>    '
          '  </Scheme>'
          ''
          ''
          ''
          
            '  <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' inherit0='#39'StringFind'#39' in' +
            'herit1='#39'Variable'#39
          '  inherit2='#39'Label'#39' inherit3='#39'Macros'#39
          '          defaultToken='#39'default'#39'           '
          ''
          '          allIgnoreCase='#39'true'#39
          '  >'
          
            '    <KeywordRegex moreWordChars='#39'.'#39'>\b[a-zA-Z_][\w_]*\b</Keyword' +
            'Regex>'
          ''
          '    <Keywords>'
          '            rem set if else exist errorlevel'
          '            for in do break call copy chcp cd'
          '            chdir choice cls country ctty date del'
          '            erase dir echo exit goto loadfix loadhigh'
          '            mkdir md move path pause prompt rename ren'
          '            rmdir rd shift time type ver verify vol'
          '            com con lpt nul defined not errorlevel'
          '            cmdextversion'
          '            '
          '    </Keywords>'
          ''
          ''
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'Rem.*$'#39' />'
          '    <Regex innerScheme='#39'Comment'#39' regex="\:\:.*$ " />'
          ''
          '    <Regex token0='#39'keyword'#39' regex="^\s*\b[a-zA-Z_][\w_]*\b" />'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[    \]    \[  \( \) &gt; &lt;' +
            '  &amp; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-=+^;,]'#39' />'
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '     <SyntaxBlock capture="true" priority='#39'0'#39'  >'
          '        <Start> sym:(  </Start>'
          '        <End> sym:)  </End>'
          '    </SyntaxBlock>'
          '    '
          '  </Scheme>'
          '</SchemeList>')
      end>
    ColorSchemes = <
      item
        Name = 'default'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          '<Attributes>'
          #9'<Token name="default" textColor="#0"/>'
          #9'<Token name="defaultEmbed" backgroundColor="#ECE9D8"/>'
          #9'<Token name="docStart" fontStyle="bold"/>'
          #9'<Token name="docEnd" fontStyle="bold"/>'
          #9'<Token name="docBlockStart" fontStyle="bold"/>'
          #9'<Token name="docBlockEnd" fontStyle="bold"/>'
          #9'<Token name="docInstruction" fontStyle="bold"/>'
          #9'<Token name="docDefault" backgroundColor="#ECE9D8"/>'
          #9'<Token name="comment" textColor="#008000"/>'
          #9'<Token name="commentStart" textColor="#008000"/>'
          #9'<Token name="commentEnd" textColor="#008000"/>'
          #9'<Token name="keyword" textColor="Blue" fontStyle="bold"/>'
          #9'<Token name="privateSymbol" textColor="Navy"/>'
          #9'<Token name="email" fontStyle="bold"/>'
          
            #9'<Token name="hairy" textColor="#000080" fontStyle="bold, italic' +
            ', underline, strikeOut"/>'
          #9'<Token name="number" textColor="Blue"/>'
          #9'<Token name="symbol" textColor="Blue"/>'
          #9'<Token name="string" textColor="Blue"/>'
          #9'<Token name="blockStart" fontStyle="bold"/>'
          #9'<Token name="blockEnd" fontStyle="bold"/>'
          #9'<Token name="badString" textColor="Red"/>'
          #9'<Token name="preprocessor" textColor="#008080"/>'
          #9'<Token name="badPreprocessor" textColor="red"/>'
          #9'<Token name="escaped" textColor="Navy"/>'
          
            #9'<Token name="quotedVariable" textColor="Navy" fontStyle="bold"/' +
            '>'
          #9'<Token name="regexStart" textColor="Navy" fontStyle="bold"/>'
          
            #9'<Token name="regexEnd" textColor="Navy" fontStyle="bold"/><!--h' +
            'tml/xml tokens-->'
          #9'<Token name="tagOpen" textColor="#FF8000"/>'
          #9'<Token name="tagClose" textColor="Blue"/>'
          
            #9'<Token name="dtdTagStartEnd" textColor="Purple" fontStyle="bold' +
            '"/>'
          #9'<Token name="url" textColor="Red"/>'
          #9'<Token name="fpi" textColor="Red" fontStyle="bold"/>'
          #9'<Token name="entity" textColor="#FF4500"/>'
          #9'<Token name="attributeName" textColor="Red"/>'
          
            #9'<Token name="attributeValue" textColor="#8000FF" fontStyle="bol' +
            'd"/>'
          #9'<Token name="badAttributeValue" textColor="Red"/>'
          #9'<Token name="cssRule" textColor="#583485" fontStyle="bold"/>'
          #9'<Token name="cssRelSymbol" fontStyle="bold"/>'
          #9'<Token name="cssUrlHref" fontStyle="underline"/>'
          #9'<Token name="cssUrlAttr"/>'
          #9'<Token name="cssAttrListBG" textColor="Blue"/>'
          
            #9'<Token name="cdataTagStart" textColor="Purple" fontStyle="bold"' +
            '/>'
          #9'<Token name="cdataTagEnd" textColor="Purple" fontStyle="bold"/>'
          
            #9'<Token name="xmlDeclStart" backgroundColor="Yellow" textColor="' +
            'Red" fontStyle="bold"/>'
          
            #9'<Token name="xmlDeclEnd" backgroundColor="Yellow" textColor="Re' +
            'd" fontStyle="bold"/>'
          '</Attributes>')
      end
      item
        Name = 'Lua'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          '<Attributes><!--html/xml tokens-->'
          #9'<Token name="comment" textColor="Green" fontStyle="bold"/>'
          #9'<Token name="string" textColor="Gray"/>'
          #9'<Token name="keyword" textColor="Blue" fontStyle="bold"/>'
          #9'<Token name="number" textColor="#FF8000"/>'
          '</Attributes>')
      end>
    ActiveSyntaxScheme = 'Batch'
    ActiveColorScheme = 'default'
    Left = 328
    Top = 120
  end
  object EditDocSystem: TLMDEditDocument
    Lines.Strings = (
      '$(aasdf)=asdf')
    WordSeparators = '[]();,'#39'"-+/\:{}.'
    SyntaxSchemes = <
      item
        Name = 'Lua'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '    <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>                '
          '    '
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Numbers'#39'>'
          '        <Regex token0='#39'number'#39'>'
          '            \b (0x \d+ [lL]? | \d+ (e \d*)? [lLdDfF]? ) \b'
          '        </Regex>'
          '        '
          '        <Regex token0='#39'number'#39'>'
          '            \b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? \d+) \b'
          '        </Regex>'
          '    </Scheme>'
          ''
          
            '    <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39 +
            '  />'
          ''
          '    <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    </Scheme>'
          ''
          
            '    <Scheme name='#39'StringInterpolation'#39' inherit='#39'String'#39' defaultT' +
            'oken='#39'string'#39'>'
          
            '        <Regex token0='#39'escaped'#39' regex='#39'\\[a-z 0 &quot; &apos; ]'#39 +
            ' />'
          '        <Regex token0='#39'escaped'#39' regex='#39'\\ \d{1,3}'#39' />'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'true'#39
          '    >'
          '        <KeywordRegex>\b[a-zA-Z_]\w*\b</KeywordRegex>'
          ''
          '        <Keywords>    '
          '             and       break     do        else      elseif'
          '             end       false     for       function  if'
          '             in        local     nil       not       or'
          
            '             repeat    return    then      true      until     w' +
            'hile            '
          '        </Keywords>'
          ''
          
            '        <RegexBlock innerScheme='#39'StringInterpolation'#39' start_toke' +
            'n0='#39'string'#39' '
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> &quot; | &apos;  </Start>        '
          '            <End> $0 </End>        '
          '        </RegexBlock>'
          ''
          
            '        <RegexBlock innerScheme='#39'String'#39' start_token0='#39'string'#39' p' +
            'riority='#39'100'#39
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> \[ (\=*) \[  </Start>        '
          '            <End> \] $1 \] </End>        '
          '        </RegexBlock>'
          ''
          ''
          
            '        <RegexBlock innerScheme='#39'Comment'#39' start_token0='#39'comment'#39 +
            ' priority='#39'100'#39
          '                    end_token0='#39'comment'#39' >'
          ''
          '            <Start> --\[ (\=*) \[  </Start>        '
          '            <End> \] $1 \] </End>        '
          '        </RegexBlock>'
          ''
          
            '        <Regex innerScheme='#39'Comment'#39' priority='#39'1000'#39' regex='#39'--([' +
            '^ \[ ].*)?$'#39' />'
          '        '
          
            '        <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39 +
            ' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '        <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]  \#  \[ ' +
            ' \( \) &gt; &lt; ]'#39' />'
          '        <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          '        <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> ( kw:local )? kw:function  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '       '
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:if </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:repeat </Start>'
          '            <End> kw:until  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> [ kw:while kw:for ] .+? kw:do  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:do  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'Text'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <TokenAlias name='#39'text'#39' baseToken='#39'default'#39' />'
          '  '
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' keywordsIgnoreCase='#39'false'#39' '
          '          inherit='#39'Text'#39
          '          defaultToken='#39'text'#39'>'
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'XML'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39'>'
          '    <Regex token0='#39'entity'#39'>'
          '        &amp;[a-zA-Z]+;'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> &lt; !--  </Start>        '
          '        <End> --&gt; </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'InsideTag'#39' inherit='#39'BaseSyntax'#39'>'
          
            '    <RegexBlock innerScheme='#39'CDATA'#39' start_token0='#39'cdataTagStart'#39 +
            ' end_token0='#39'cdataTagEnd'#39' >'
          '        <Start> &lt; !\[CDATA  </Start>'
          '        <End> \]\] &gt; </End>        '
          '    </RegexBlock>'
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'WithTags'#39' inherit='#39'Tag'#39' inherit0='#39'BaseSyntax'#39'>'
          '  '
          
            '    <RegexBlock innerScheme='#39'WithTags'#39' start_token0='#39'tagOpen'#39' en' +
            'd_token0='#39'tagClose'#39' >'
          '        <Start> &lt; ([\w:]+) </Start>        '
          '        <End> ( / &gt; ) | ( &lt; / $1 &gt; ) </End>        '
          '    </RegexBlock>  '
          ''
          
            '    <RegexBlock innerScheme='#39'InsideTag'#39' start_token0='#39'tagOpen'#39' e' +
            'nd_token0='#39'tagClose'#39' >'
          '        <Start> &gt;  </Start>        '
          '        <End> (?= &lt; [^ ! ] ) </End>        '
          '    </RegexBlock>  '
          ''
          ''
          '    <SyntaxBlock capture='#39'true'#39'>'
          '        <Start> tagOpen </Start>'
          '        <End> tagClose </End>'
          '    </SyntaxBlock>'
          '    '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' defaultToken='#39'comment'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'XmlDecl'#39' inherit='#39'Tag'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'CDATA'#39' defaultToken='#39'xmlCdata'#39'>'
          '    <SyntaxBlock capture='#39'true'#39'>'
          '        <Start> cdataTagStart </Start>'
          '        <End> cdataTagEnd </End>'
          '    </SyntaxBlock>  '
          '  </Scheme>    '
          '    '
          '  <Scheme name='#39'DTD'#39' defaultToken='#39'comment'#39'>               '
          '  </Scheme>'
          '    '
          '  <Scheme name='#39'AttrStringQuot'#39'>'
          '    <Regex token0='#39'attributeValue'#39'> [^ &quot; ]+</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'AttrStringApos'#39'>'
          '    <Regex token0='#39'attributeValue'#39'> [^ &apos; ]+</Regex>'
          '  </Scheme>'
          '    '
          '  <Scheme name='#39'Tag'#39'>  '
          '  '
          '    <Regex token0='#39'attributeName'#39' priority='#39'10'#39'>'
          '        \b\w+ \s* = \s*        '
          ''
          '    </Regex>'
          ''
          '    <Regex token0='#39'attributeValue'#39'>'
          '        [^ &lt; &gt; &quot; &apos; = \s ]+         '
          '    </Regex>'
          ''
          
            '    <RegexBlock innerScheme='#39'AttrStringQuot'#39' start_token0='#39'attri' +
            'buteValue'#39' '
          '        end_token0='#39'attributeValue'#39' >'
          ''
          '        <Start> &quot;  </Start>        '
          '        <End> &quot; </End>        '
          '    </RegexBlock>'
          ''
          
            '    <RegexBlock innerScheme='#39'AttrStringApos'#39' start_token0='#39'attri' +
            'buteValue'#39' '
          '        end_token0='#39'attributeValue'#39' >'
          ''
          '        <Start> &apos;  </Start>        '
          '        <End> &apos; </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>'
          '  '
          ''
          ''
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39'        '
          '          inherit0='#39'WithTags'#39'      '
          '          keywordsIgnoreCase='#39'true'#39' >'
          '                    '
          ''
          
            '    <RegexBlock innerScheme='#39'DTD'#39' start_token0='#39'dtdTagStartEnd'#39' ' +
            'end_token0='#39'dtdTagStartEnd'#39' >'
          '        <Start> &lt; !\w+  </Start>        '
          '        <End> &gt; </End>        '
          '    </RegexBlock>'
          '                    '
          '    <Regex token0='#39'tagClose'#39'>'
          '        &lt; / [\w:]+ \s* &gt;'
          '    </Regex>'
          ''
          
            '    <RegexBlock innerScheme='#39'XmlDecl'#39' start_token0='#39'xmlDeclStart' +
            #39' end_token0='#39'xmlDeclEnd'#39' >'
          '        <Start> &lt; \? xml  </Start>'
          '        <End> \? &gt; </End>        '
          '    </RegexBlock>'
          ''
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'C'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'>'
          
            '    <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />               ' +
            ' '
          '            '
          '     <SyntaxBlock capture="true">'
          '        <Start> st:/*  </Start>'
          '        <End> st:*/  </End>'
          '     </SyntaxBlock>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />'
          '    <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex token0='#39'badString'#39'> '
          '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ )?'
          '    </Regex>'
          ''
          '    <Regex token0='#39'string'#39' priority='#39'10'#39'> '
          
            '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ ) &ap' +
            'os;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot;'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot; &quot;'
          '    </Regex>'
          ''
          '  </Scheme>'
          '  '
          
            '  <Scheme name='#39'Preprocessor'#39' defaultToken='#39'preprocessor'#39' inheri' +
            't='#39'BaseSyntax'#39' >'
          ''
          '    <IdentRe>\b[a-zA-Z]+\b</IdentRe>'
          ''
          '    <Keywords>'
          '      if ifdef endif else using include define undef pragma'
          '    </Keywords>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BadPreprocessor'#39' defaultToken='#39'badPreprocessor'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          ''
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'Main'#39' inherit='#39'BaseSyntax'#39' inherit0='#39'StringFind'#39 +
            ' '
          '          defaultToken='#39'default'#39'           '
          ''
          '          keywordsIgnoreCase='#39'false'#39
          '  >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          '      asm auto bool break case char const continue'
          '      default do double else enum export'
          '      extern false float for goto if inline int long '
          '      register return short signed sizeof static '
          
            '      switch typedef union unsigned void volatile wchar_t while ' +
            'NULL'
          '      __try __finally __catch'
          '    </Keywords>'
          ''
          ''
          
            '    <Regex innerScheme='#39'Preprocessor'#39' innerContentGroup='#39'1'#39' prio' +
            'rity='#39'10'#39' >'
          '        ^ \s* \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadPreprocessor'#39' innerContentGroup='#39'1'#39'>'
          '        ^ .*? \S .*? \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Preprocessor'#39' '
          '                innerContentGroup='#39'1'#39
          '                priority='#39'100'#39'>'
          '        <Start> ^ \s* \#([a-zA-Z]+) .* \\ \s* $  </Start>'
          '        <End>   (^ .* [^ \\ \s ] \s* $) | (^ \s* $)  </End>'
          '    </RegexBlock>'
          ''
          '    '
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> '
          '            [ preprocessor:if  preprocessor:ifdef  ]'
          '        </Start>'
          ''
          '        <End> [ preprocessor:ifend  preprocessor:endif ] </End>'
          '    </SyntaxBlock>'
          ''
          ''
          '        '
          '    <SyntaxBlock capture="true">'
          '        <Start> id  sym:(  [^ sym:;    sym:}   sym:{   ]*  '
          
            '                sym:) ( sym::   [^ sym:;    sym:}   sym:{   ]+  ' +
            ' )? sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:__try  kw:__fin' +
            'ally   kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  ]  sym:(   [^  sym:;  ' +
            ' sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '             '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'CPP'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'>'
          
            '    <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />               ' +
            ' '
          '            '
          '     <SyntaxBlock capture="true">'
          '        <Start> st:/*  </Start>'
          '        <End> st:*/  </End>'
          '     </SyntaxBlock>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />  '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex token0='#39'badString'#39'> '
          '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ )?'
          '    </Regex>'
          ''
          '    <Regex token0='#39'string'#39' priority='#39'10'#39'> '
          
            '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ ) &ap' +
            'os;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot;'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot; &quot;'
          '    </Regex>'
          '    '
          '  </Scheme>'
          '  '
          
            '  <Scheme name='#39'Preprocessor'#39' defaultToken='#39'preprocessor'#39' inheri' +
            't='#39'BaseSyntax'#39' >'
          ''
          '    <IdentRe>\b[a-zA-Z]+\b</IdentRe>'
          ''
          '    <Keywords>'
          '      if ifdef endif else using include define undef pragma'
          '    </Keywords>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BadPreprocessor'#39' defaultToken='#39'badPreprocessor'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' inherit='#39'BaseSyntax'#39' '
          '          inherit1='#39'StringFind'#39' '
          '          defaultToken='#39'default'#39'           '
          ''
          '          keywordsIgnoreCase='#39'false'#39
          '  >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          
            '      asm auto bool break case catch char class const const_cast' +
            ' continue'
          
            '      default delete do double dynamic_cast else enum explicit e' +
            'xport'
          
            '      extern false float for friend goto if inline int long muta' +
            'ble'
          '      namespace new operator private protected public register'
          
            '      reinterpret_cast return short signed sizeof static static_' +
            'cast struct'
          
            '      switch template this throw true try typedef typeid typenam' +
            'e union'
          
            '      unsigned using virtual void volatile wchar_t while nullptr' +
            '  NULL'
          '      __try __finally'
          '    </Keywords>'
          ''
          ''
          
            '    <Regex innerScheme='#39'Preprocessor'#39' innerContentGroup='#39'1'#39' prio' +
            'rity='#39'10'#39' >'
          '        ^ \s* \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadPreprocessor'#39' innerContentGroup='#39'1'#39'>'
          '        ^ .*? \S .*? \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Preprocessor'#39' '
          '                innerContentGroup='#39'1'#39
          '                priority='#39'100'#39'>'
          '        <Start> ^ \s* \#([a-zA-Z]+) .* \\ \s* $  </Start>'
          '        <End>   (^ .* [^ \\ \s ] \s* $) | (^ \s* $)  </End>'
          '    </RegexBlock>'
          ''
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> '
          '            [ preprocessor:if  preprocessor:ifdef  ]'
          '        </Start>'
          ''
          '        <End> [ preprocessor:ifend  preprocessor:endif ] </End>'
          '    </SyntaxBlock>'
          ''
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> [ kw:class  kw:struct ]'
          '                [^ sym:; ]+ sym:\{ '
          '        </Start>'
          ''
          '        <End> sym:} </End>'
          '    </SyntaxBlock>'
          ''
          '        '
          '    <SyntaxBlock capture="true">'
          '        <Start> id  '
          '                sym:(  '
          '                    [^ sym:;    sym:}   sym:{   ]*  '
          '                sym:) '
          '                ( '
          '                    sym:: '
          '                    [^ sym:;    sym:}   sym:{   ]+   '
          '                )? '
          '                sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:namespace  kw:t' +
            'ry kw:catch  kw:__try  kw:__finally   kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  ]  sym:(   [^  sym:;  ' +
            ' sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '             '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'JavaScript'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '      <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '            <Regex token0='#39'email'#39'>'
          '                [_a-zA-Z\d\-\.]{1,40}'
          '                @ '
          '                ([_ a-z A-Z \d \-]+ '
          '                (\. [_ a-z A-Z \d \-]+ )+ )'
          '            </Regex>                '
          '      </Scheme>'
          ''
          '      <Scheme name='#39'Numbers'#39'>'
          
            '        <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [' +
            'lLdDfF]? ) \b</Regex>'
          
            '        <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\' +
            '-]? \d+) \b</Regex>'
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'commen' +
            't'#39'>'
          
            '        <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />           ' +
            '     '
          '                '
          '         <SyntaxBlock capture="true">'
          '            <Start> st:/*  </Start>'
          '            <End> st:*/  </End>'
          '         </SyntaxBlock>                '
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39 +
            '>'
          
            '        <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />   ' +
            '   '
          '        <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '        <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'ba' +
            'dString'#39'>'
          '      </Scheme>'
          ''
          '      <Scheme name='#39'StringFind'#39'>'
          '        <Regex innerScheme='#39'BadString'#39' > '
          '            (&quot;|&apos;) (.*?\\ \1 )*? .* $ '
          '        </Regex>'
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '            (&quot;|&apos;) (.*?\\ \1 )*? \1'
          '        </Regex>'
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '            (&quot;|&apos;) (.*?\\ \1 )*? .*? [^\\] \1'
          '        </Regex>  '
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '            (&quot;|&apos;) \1'
          '        </Regex>'
          '        '
          '      </Scheme>'
          ''
          '    <Scheme name='#39'RegexFind'#39' defaultToken='#39'defaultEmbed'#39'>'
          '        <Regex token0='#39'escaped'#39' regex='#39'\\/'#39' />            '
          '    </Scheme>'
          '    '
          
            '    <Scheme name='#39'RegexSubst'#39' inherit='#39'RegexFind'#39' defaultToken='#39 +
            'defaultEmbed'#39'>'
          '        <Regex token0='#39'escaped'#39' regex='#39'\$\d'#39' />                '
          '    </Scheme>'
          ''
          '    <Scheme name='#39'DefaultEmpty'#39' defaultToken='#39'default'#39' />'
          '    '
          '    <Scheme name='#39'Main'#39' inherit='#39'StringFind'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'false'#39
          '    >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          '            Boolean'
          '            Array'
          '            join length reverse sort'
          '            NaN '
          '            null '
          '            true false '
          '            Infinity '
          '            undefined '
          '            Break '#9#9'continue '#9#9#9'for  in  if else return  while '
          '            '
          '            function new this var with  arguments '
          '            '
          '            Global escape unescape eval Number '
          '            Object new instanceof  delete typeof void '#9#9#9
          '            throw  try catch finally with'
          '    </Keywords>'
          ''
          ''
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          '    <RegexBlock innerScheme='#39'DefaultEmpty'#39' '
          '                start_token0='#39'symbol'#39
          '                end_token0='#39'symbol'#39' '
          '                priority='#39'100'#39
          '                chainBlock='#39'RegexFindEnd'#39
          '    >'
          ''
          '        <Start> \s* ([ =( ]) \s* (?= / )  </Start>        '
          '        <End> (?= / )  </End>        '
          '    </RegexBlock>'
          '    '
          '    <ChainBlock name='#39'RegexFindEnd'#39' '
          '                innerScheme='#39'RegexFind'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                chainBlock='#39'RegexFindEnd2'#39
          '    >'
          '        <End> / </End>        '
          '    </ChainBlock>'
          ''
          '    <ChainBlock name='#39'RegexFindEnd2'#39' '
          '                innerScheme='#39'RegexFind'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '    >'
          '        <End> /[igmx]* </End>        '
          '    </ChainBlock>'
          '        '
          '    <ChainBlock name='#39'RegexSubstEnd'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                innerScheme='#39'RegexSubst'#39'  >'
          '        <End> /[igmx]* </End>        '
          '    </ChainBlock>'
          '    '
          '    <RegexBlock innerScheme='#39'RegexFind'#39' '
          '                start_token0='#39'regexStart'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                chainBlock='#39'RegexSubstEnd'#39'>'
          ''
          '        <Start> s/  </Start>        '
          '        <End> /[igmx]* </End>        '
          '    </RegexBlock>'
          ''
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> kw:function  id  '
          '                sym:(  '
          '                        [^ sym:;    sym:}   sym:{   ]*  '
          '                sym:) '
          '                sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:try kw:catch  k' +
            'w:finally  kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  kw:with ]  sym:(   [^ ' +
            ' sym:;   sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '            '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'INI'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'tagOpen'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLd' +
            'DfF]? ) \b</Regex>'
          
            '    <Regex token0='#39'tagOpen'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]?' +
            ' \d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> ^\#  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> ^;  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Url'#39' end_token0='#39'url'#39' >'
          '        <Start> ^\[  </Start>        '
          '        <End> \]$ </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>    '
          ''
          
            '  <Scheme name='#39'String'#39' inherit='#39'BaseSyntax'#39' defaultToken='#39'strin' +
            'g'#39'>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot; c?'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot; c?'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot;  &quot; c?'
          '    </Regex>    '
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'Comment'#39' defaultToken='#39'comment'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Url'#39' defaultToken='#39'url'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39' inherit0='#39'StringFind'#39
          '          keywordsIgnoreCase='#39'true'#39' >'
          ''
          '          <Regex token0='#39'symbol'#39' regex='#39'[=]'#39' />'
          '          '
          '          <Regex token0='#39'keyword'#39' regex="^\$\([\w_]*\)" />'
          '          '
          '  </Scheme>'
          '  '
          '</SchemeList>')
      end
      item
        Name = 'DFM'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          ''
          '    <Scheme name='#39'Numbers'#39'>'
          '        <Regex token0='#39'number'#39'>'
          '            \b (0x \d+ [lL]? | \d+ (e \d*)? [lLdDfF]? ) \b'
          '        </Regex>'
          '        '
          '        <Regex token0='#39'number'#39'>'
          '            \b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? \d+) \b'
          '        </Regex>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'String'#39' defaultToken='#39'string'#39'>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Binary'#39' defaultToken='#39'comment'#39'>        '
          '        <SyntaxBlock capture="true">'
          '            <Start> commentStart  </Start>'
          '            <End> commentEnd  </End>'
          '        </SyntaxBlock>        '
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'true'#39
          '    >'
          '        <KeywordRegex>\b[a-zA-Z_]\w*\b</KeywordRegex>'
          ''
          '        <Keywords>    '
          '            object  end item'
          '        </Keywords>'
          ''
          '        <RegexBlock innerScheme='#39'String'#39' start_token0='#39'string'#39' '
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> &apos;  </Start>        '
          '            <End> &apos; </End>        '
          '        </RegexBlock>'
          ''
          
            '        <RegexBlock innerScheme='#39'Binary'#39' start_token0='#39'commentSt' +
            'art'#39' '
          '                    end_token0='#39'commentEnd'#39' >'
          '            <Start> \{  </Start>        '
          '            <End> \} </End>        '
          '        </RegexBlock>'
          ''
          
            '        <Regex token0='#39'symbol'#39' regex='#39'[   \]  \#  \[  \( \) &gt;' +
            ' &lt; ]'#39' />'
          '        <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          '        '
          '        <SyntaxBlock capture="true">'
          '            <Start> [ kw:item kw:object ] </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> sym:&lt;  </Start>'
          '            <End> sym:&gt;  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> sym:(  </Start>'
          '            <End> sym:)  </End>'
          '        </SyntaxBlock>'
          ''
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'Batch'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'  ' +
            '/>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'escaped'#39'>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot; c?'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot; c?'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot;  &quot; c?'
          '    </Regex>    '
          '  </Scheme>'
          ''
          ''
          ''
          '  <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' inherit0='#39'StringFind'#39
          '          defaultToken='#39'default'#39'           '
          ''
          '          allIgnoreCase='#39'true'#39
          '  >'
          
            '    <KeywordRegex moreWordChars='#39'.'#39'>\b[a-zA-Z_][\w_]*\b</Keyword' +
            'Regex>'
          ''
          '    <Keywords>'
          '            rem set if else exist errorlevel'
          '            for in do break call copy chcp cd'
          '            chdir choice cls country ctty date del'
          '            erase dir echo exit goto loadfix loadhigh'
          '            mkdir md move path pause prompt rename ren'
          '            rmdir rd shift time type ver verify vol'
          '            com con lpt nul defined not errorlevel'
          '            cmdextversion'
          '            '
          '    </Keywords>'
          ''
          ''
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'Rem.*$'#39' />'
          '    <Regex innerScheme='#39'Comment'#39' regex="\:\:.*$ " />'
          ''
          '    <Regex token0='#39'keyword'#39' regex="^\s*\b[a-zA-Z_][\w_]*\b" />'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[    \]    \[  \( \) &gt; &lt;' +
            '  &amp; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-=+^;,]'#39' />'
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '     <SyntaxBlock capture="true" priority='#39'0'#39'  >'
          '        <Start> sym:(  </Start>'
          '        <End> sym:)  </End>'
          '    </SyntaxBlock>'
          '    '
          '  </Scheme>'
          '</SchemeList>')
      end>
    ColorSchemes = <
      item
        Name = 'default'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          '<Attributes>'
          #9'<Token name="default" textColor="#0"/>'
          #9'<Token name="defaultEmbed" backgroundColor="#ECE9D8"/>'
          #9'<Token name="docStart" fontStyle="bold"/>'
          #9'<Token name="docEnd" fontStyle="bold"/>'
          #9'<Token name="docBlockStart" fontStyle="bold"/>'
          #9'<Token name="docBlockEnd" fontStyle="bold"/>'
          #9'<Token name="docInstruction" fontStyle="bold"/>'
          #9'<Token name="docDefault" backgroundColor="#ECE9D8"/>'
          #9'<Token name="comment" textColor="#008000"/>'
          #9'<Token name="commentStart" textColor="#008000"/>'
          #9'<Token name="commentEnd" textColor="#008000"/>'
          #9'<Token name="keyword" textColor="Blue" fontStyle="bold"/>'
          #9'<Token name="privateSymbol" textColor="Navy"/>'
          #9'<Token name="email" fontStyle="bold"/>'
          
            #9'<Token name="hairy" textColor="#000080" fontStyle="bold, italic' +
            ', underline, strikeOut"/>'
          #9'<Token name="number" textColor="Blue"/>'
          #9'<Token name="symbol" textColor="Maroon" fontStyle="bold"/>'
          #9'<Token name="string" textColor="Blue"/>'
          #9'<Token name="blockStart" fontStyle="bold"/>'
          #9'<Token name="blockEnd" fontStyle="bold"/>'
          #9'<Token name="badString" textColor="Red"/>'
          #9'<Token name="preprocessor" textColor="#008080"/>'
          #9'<Token name="badPreprocessor" textColor="red"/>'
          #9'<Token name="escaped" textColor="Navy"/>'
          
            #9'<Token name="quotedVariable" textColor="Navy" fontStyle="bold"/' +
            '>'
          #9'<Token name="regexStart" textColor="Navy" fontStyle="bold"/>'
          
            #9'<Token name="regexEnd" textColor="Navy" fontStyle="bold"/><!--h' +
            'tml/xml tokens-->'
          #9'<Token name="tagOpen" textColor="Blue"/>'
          #9'<Token name="tagClose" textColor="Blue"/>'
          
            #9'<Token name="dtdTagStartEnd" textColor="Purple" fontStyle="bold' +
            '"/>'
          #9'<Token name="url" textColor="Red"/>'
          #9'<Token name="fpi" textColor="Red" fontStyle="bold"/>'
          #9'<Token name="entity" textColor="#FF4500"/>'
          #9'<Token name="attributeName" textColor="Red"/>'
          
            #9'<Token name="attributeValue" textColor="#8000FF" fontStyle="bol' +
            'd"/>'
          #9'<Token name="badAttributeValue" textColor="Red"/>'
          #9'<Token name="cssRule" textColor="#583485" fontStyle="bold"/>'
          #9'<Token name="cssRelSymbol" fontStyle="bold"/>'
          #9'<Token name="cssUrlHref" fontStyle="underline"/>'
          #9'<Token name="cssUrlAttr"/>'
          #9'<Token name="cssAttrListBG" textColor="Blue"/>'
          
            #9'<Token name="cdataTagStart" textColor="Purple" fontStyle="bold"' +
            '/>'
          #9'<Token name="cdataTagEnd" textColor="Purple" fontStyle="bold"/>'
          
            #9'<Token name="xmlDeclStart" backgroundColor="Yellow" textColor="' +
            'Red" fontStyle="bold"/>'
          
            #9'<Token name="xmlDeclEnd" backgroundColor="Yellow" textColor="Re' +
            'd" fontStyle="bold"/>'
          '</Attributes>')
      end>
    ActiveSyntaxScheme = 'INI'
    ActiveColorScheme = 'default'
    Left = 232
    Top = 288
  end
  object EditDocUser: TLMDEditDocument
    Lines.Strings = (
      '$(aasdf)=asdf')
    WordSeparators = '[]();,'#39'"-+/\:{}.'
    SyntaxSchemes = <
      item
        Name = 'Lua'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '    <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>                '
          '    '
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Numbers'#39'>'
          '        <Regex token0='#39'number'#39'>'
          '            \b (0x \d+ [lL]? | \d+ (e \d*)? [lLdDfF]? ) \b'
          '        </Regex>'
          '        '
          '        <Regex token0='#39'number'#39'>'
          '            \b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? \d+) \b'
          '        </Regex>'
          '    </Scheme>'
          ''
          
            '    <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39 +
            '  />'
          ''
          '    <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    </Scheme>'
          ''
          
            '    <Scheme name='#39'StringInterpolation'#39' inherit='#39'String'#39' defaultT' +
            'oken='#39'string'#39'>'
          
            '        <Regex token0='#39'escaped'#39' regex='#39'\\[a-z 0 &quot; &apos; ]'#39 +
            ' />'
          '        <Regex token0='#39'escaped'#39' regex='#39'\\ \d{1,3}'#39' />'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'true'#39
          '    >'
          '        <KeywordRegex>\b[a-zA-Z_]\w*\b</KeywordRegex>'
          ''
          '        <Keywords>    '
          '             and       break     do        else      elseif'
          '             end       false     for       function  if'
          '             in        local     nil       not       or'
          
            '             repeat    return    then      true      until     w' +
            'hile            '
          '        </Keywords>'
          ''
          
            '        <RegexBlock innerScheme='#39'StringInterpolation'#39' start_toke' +
            'n0='#39'string'#39' '
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> &quot; | &apos;  </Start>        '
          '            <End> $0 </End>        '
          '        </RegexBlock>'
          ''
          
            '        <RegexBlock innerScheme='#39'String'#39' start_token0='#39'string'#39' p' +
            'riority='#39'100'#39
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> \[ (\=*) \[  </Start>        '
          '            <End> \] $1 \] </End>        '
          '        </RegexBlock>'
          ''
          ''
          
            '        <RegexBlock innerScheme='#39'Comment'#39' start_token0='#39'comment'#39 +
            ' priority='#39'100'#39
          '                    end_token0='#39'comment'#39' >'
          ''
          '            <Start> --\[ (\=*) \[  </Start>        '
          '            <End> \] $1 \] </End>        '
          '        </RegexBlock>'
          ''
          
            '        <Regex innerScheme='#39'Comment'#39' priority='#39'1000'#39' regex='#39'--([' +
            '^ \[ ].*)?$'#39' />'
          '        '
          
            '        <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39 +
            ' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '        <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]  \#  \[ ' +
            ' \( \) &gt; &lt; ]'#39' />'
          '        <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          '        <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> ( kw:local )? kw:function  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '       '
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:if </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:repeat </Start>'
          '            <End> kw:until  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> [ kw:while kw:for ] .+? kw:do  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> kw:do  </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'Text'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <TokenAlias name='#39'text'#39' baseToken='#39'default'#39' />'
          '  '
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' keywordsIgnoreCase='#39'false'#39' '
          '          inherit='#39'Text'#39
          '          defaultToken='#39'text'#39'>'
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'XML'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39'>'
          '    <Regex token0='#39'entity'#39'>'
          '        &amp;[a-zA-Z]+;'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> &lt; !--  </Start>        '
          '        <End> --&gt; </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'InsideTag'#39' inherit='#39'BaseSyntax'#39'>'
          
            '    <RegexBlock innerScheme='#39'CDATA'#39' start_token0='#39'cdataTagStart'#39 +
            ' end_token0='#39'cdataTagEnd'#39' >'
          '        <Start> &lt; !\[CDATA  </Start>'
          '        <End> \]\] &gt; </End>        '
          '    </RegexBlock>'
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'WithTags'#39' inherit='#39'Tag'#39' inherit0='#39'BaseSyntax'#39'>'
          '  '
          
            '    <RegexBlock innerScheme='#39'WithTags'#39' start_token0='#39'tagOpen'#39' en' +
            'd_token0='#39'tagClose'#39' >'
          '        <Start> &lt; ([\w:]+) </Start>        '
          '        <End> ( / &gt; ) | ( &lt; / $1 &gt; ) </End>        '
          '    </RegexBlock>  '
          ''
          
            '    <RegexBlock innerScheme='#39'InsideTag'#39' start_token0='#39'tagOpen'#39' e' +
            'nd_token0='#39'tagClose'#39' >'
          '        <Start> &gt;  </Start>        '
          '        <End> (?= &lt; [^ ! ] ) </End>        '
          '    </RegexBlock>  '
          ''
          ''
          '    <SyntaxBlock capture='#39'true'#39'>'
          '        <Start> tagOpen </Start>'
          '        <End> tagClose </End>'
          '    </SyntaxBlock>'
          '    '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' defaultToken='#39'comment'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'XmlDecl'#39' inherit='#39'Tag'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'CDATA'#39' defaultToken='#39'xmlCdata'#39'>'
          '    <SyntaxBlock capture='#39'true'#39'>'
          '        <Start> cdataTagStart </Start>'
          '        <End> cdataTagEnd </End>'
          '    </SyntaxBlock>  '
          '  </Scheme>    '
          '    '
          '  <Scheme name='#39'DTD'#39' defaultToken='#39'comment'#39'>               '
          '  </Scheme>'
          '    '
          '  <Scheme name='#39'AttrStringQuot'#39'>'
          '    <Regex token0='#39'attributeValue'#39'> [^ &quot; ]+</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'AttrStringApos'#39'>'
          '    <Regex token0='#39'attributeValue'#39'> [^ &apos; ]+</Regex>'
          '  </Scheme>'
          '    '
          '  <Scheme name='#39'Tag'#39'>  '
          '  '
          '    <Regex token0='#39'attributeName'#39' priority='#39'10'#39'>'
          '        \b\w+ \s* = \s*        '
          ''
          '    </Regex>'
          ''
          '    <Regex token0='#39'attributeValue'#39'>'
          '        [^ &lt; &gt; &quot; &apos; = \s ]+         '
          '    </Regex>'
          ''
          
            '    <RegexBlock innerScheme='#39'AttrStringQuot'#39' start_token0='#39'attri' +
            'buteValue'#39' '
          '        end_token0='#39'attributeValue'#39' >'
          ''
          '        <Start> &quot;  </Start>        '
          '        <End> &quot; </End>        '
          '    </RegexBlock>'
          ''
          
            '    <RegexBlock innerScheme='#39'AttrStringApos'#39' start_token0='#39'attri' +
            'buteValue'#39' '
          '        end_token0='#39'attributeValue'#39' >'
          ''
          '        <Start> &apos;  </Start>        '
          '        <End> &apos; </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>'
          '  '
          ''
          ''
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39'        '
          '          inherit0='#39'WithTags'#39'      '
          '          keywordsIgnoreCase='#39'true'#39' >'
          '                    '
          ''
          
            '    <RegexBlock innerScheme='#39'DTD'#39' start_token0='#39'dtdTagStartEnd'#39' ' +
            'end_token0='#39'dtdTagStartEnd'#39' >'
          '        <Start> &lt; !\w+  </Start>        '
          '        <End> &gt; </End>        '
          '    </RegexBlock>'
          '                    '
          '    <Regex token0='#39'tagClose'#39'>'
          '        &lt; / [\w:]+ \s* &gt;'
          '    </Regex>'
          ''
          
            '    <RegexBlock innerScheme='#39'XmlDecl'#39' start_token0='#39'xmlDeclStart' +
            #39' end_token0='#39'xmlDeclEnd'#39' >'
          '        <Start> &lt; \? xml  </Start>'
          '        <End> \? &gt; </End>        '
          '    </RegexBlock>'
          ''
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'C'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'>'
          
            '    <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />               ' +
            ' '
          '            '
          '     <SyntaxBlock capture="true">'
          '        <Start> st:/*  </Start>'
          '        <End> st:*/  </End>'
          '     </SyntaxBlock>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />'
          '    <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex token0='#39'badString'#39'> '
          '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ )?'
          '    </Regex>'
          ''
          '    <Regex token0='#39'string'#39' priority='#39'10'#39'> '
          
            '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ ) &ap' +
            'os;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot;'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot; &quot;'
          '    </Regex>'
          ''
          '  </Scheme>'
          '  '
          
            '  <Scheme name='#39'Preprocessor'#39' defaultToken='#39'preprocessor'#39' inheri' +
            't='#39'BaseSyntax'#39' >'
          ''
          '    <IdentRe>\b[a-zA-Z]+\b</IdentRe>'
          ''
          '    <Keywords>'
          '      if ifdef endif else using include define undef pragma'
          '    </Keywords>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BadPreprocessor'#39' defaultToken='#39'badPreprocessor'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          ''
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'Main'#39' inherit='#39'BaseSyntax'#39' inherit0='#39'StringFind'#39 +
            ' '
          '          defaultToken='#39'default'#39'           '
          ''
          '          keywordsIgnoreCase='#39'false'#39
          '  >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          '      asm auto bool break case char const continue'
          '      default do double else enum export'
          '      extern false float for goto if inline int long '
          '      register return short signed sizeof static '
          
            '      switch typedef union unsigned void volatile wchar_t while ' +
            'NULL'
          '      __try __finally __catch'
          '    </Keywords>'
          ''
          ''
          
            '    <Regex innerScheme='#39'Preprocessor'#39' innerContentGroup='#39'1'#39' prio' +
            'rity='#39'10'#39' >'
          '        ^ \s* \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadPreprocessor'#39' innerContentGroup='#39'1'#39'>'
          '        ^ .*? \S .*? \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Preprocessor'#39' '
          '                innerContentGroup='#39'1'#39
          '                priority='#39'100'#39'>'
          '        <Start> ^ \s* \#([a-zA-Z]+) .* \\ \s* $  </Start>'
          '        <End>   (^ .* [^ \\ \s ] \s* $) | (^ \s* $)  </End>'
          '    </RegexBlock>'
          ''
          '    '
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> '
          '            [ preprocessor:if  preprocessor:ifdef  ]'
          '        </Start>'
          ''
          '        <End> [ preprocessor:ifend  preprocessor:endif ] </End>'
          '    </SyntaxBlock>'
          ''
          ''
          '        '
          '    <SyntaxBlock capture="true">'
          '        <Start> id  sym:(  [^ sym:;    sym:}   sym:{   ]*  '
          
            '                sym:) ( sym::   [^ sym:;    sym:}   sym:{   ]+  ' +
            ' )? sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:__try  kw:__fin' +
            'ally   kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  ]  sym:(   [^  sym:;  ' +
            ' sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '             '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'CPP'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'>'
          
            '    <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />               ' +
            ' '
          '            '
          '     <SyntaxBlock capture="true">'
          '        <Start> st:/*  </Start>'
          '        <End> st:*/  </End>'
          '     </SyntaxBlock>                '
          '  </Scheme>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39'>'
          '    <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />  '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '    <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex token0='#39'badString'#39'> '
          '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ )?'
          '    </Regex>'
          ''
          '    <Regex token0='#39'string'#39' priority='#39'10'#39'> '
          
            '        &apos; ( [^\\] | \\[a-z &apos;] | \\0x[a-fA-F0-9]+ ) &ap' +
            'os;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot;'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot;'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot; &quot;'
          '    </Regex>'
          '    '
          '  </Scheme>'
          '  '
          
            '  <Scheme name='#39'Preprocessor'#39' defaultToken='#39'preprocessor'#39' inheri' +
            't='#39'BaseSyntax'#39' >'
          ''
          '    <IdentRe>\b[a-zA-Z]+\b</IdentRe>'
          ''
          '    <Keywords>'
          '      if ifdef endif else using include define undef pragma'
          '    </Keywords>'
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BadPreprocessor'#39' defaultToken='#39'badPreprocessor'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          ''
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' inherit='#39'BaseSyntax'#39' '
          '          inherit1='#39'StringFind'#39' '
          '          defaultToken='#39'default'#39'           '
          ''
          '          keywordsIgnoreCase='#39'false'#39
          '  >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          
            '      asm auto bool break case catch char class const const_cast' +
            ' continue'
          
            '      default delete do double dynamic_cast else enum explicit e' +
            'xport'
          
            '      extern false float for friend goto if inline int long muta' +
            'ble'
          '      namespace new operator private protected public register'
          
            '      reinterpret_cast return short signed sizeof static static_' +
            'cast struct'
          
            '      switch template this throw true try typedef typeid typenam' +
            'e union'
          
            '      unsigned using virtual void volatile wchar_t while nullptr' +
            '  NULL'
          '      __try __finally'
          '    </Keywords>'
          ''
          ''
          
            '    <Regex innerScheme='#39'Preprocessor'#39' innerContentGroup='#39'1'#39' prio' +
            'rity='#39'10'#39' >'
          '        ^ \s* \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'BadPreprocessor'#39' innerContentGroup='#39'1'#39'>'
          '        ^ .*? \S .*? \# ([a-zA-Z]+) .* $'
          '    </Regex>'
          ''
          '    <RegexBlock innerScheme='#39'Preprocessor'#39' '
          '                innerContentGroup='#39'1'#39
          '                priority='#39'100'#39'>'
          '        <Start> ^ \s* \#([a-zA-Z]+) .* \\ \s* $  </Start>'
          '        <End>   (^ .* [^ \\ \s ] \s* $) | (^ \s* $)  </End>'
          '    </RegexBlock>'
          ''
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> '
          '            [ preprocessor:if  preprocessor:ifdef  ]'
          '        </Start>'
          ''
          '        <End> [ preprocessor:ifend  preprocessor:endif ] </End>'
          '    </SyntaxBlock>'
          ''
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> [ kw:class  kw:struct ]'
          '                [^ sym:; ]+ sym:\{ '
          '        </Start>'
          ''
          '        <End> sym:} </End>'
          '    </SyntaxBlock>'
          ''
          '        '
          '    <SyntaxBlock capture="true">'
          '        <Start> id  '
          '                sym:(  '
          '                    [^ sym:;    sym:}   sym:{   ]*  '
          '                sym:) '
          '                ( '
          '                    sym:: '
          '                    [^ sym:;    sym:}   sym:{   ]+   '
          '                )? '
          '                sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:namespace  kw:t' +
            'ry kw:catch  kw:__try  kw:__finally   kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  ]  sym:(   [^  sym:;  ' +
            ' sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '             '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '  </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'JavaScript'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '      <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '            <Regex token0='#39'email'#39'>'
          '                [_a-zA-Z\d\-\.]{1,40}'
          '                @ '
          '                ([_ a-z A-Z \d \-]+ '
          '                (\. [_ a-z A-Z \d \-]+ )+ )'
          '            </Regex>                '
          '      </Scheme>'
          ''
          '      <Scheme name='#39'Numbers'#39'>'
          
            '        <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [' +
            'lLdDfF]? ) \b</Regex>'
          
            '        <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\' +
            '-]? \d+) \b</Regex>'
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'commen' +
            't'#39'>'
          
            '        <Regex token0='#39'st'#39' regex='#39' \/\* | \*\/   '#39' />           ' +
            '     '
          '                '
          '         <SyntaxBlock capture="true">'
          '            <Start> st:/*  </Start>'
          '            <End> st:*/  </End>'
          '         </SyntaxBlock>                '
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'string'#39 +
            '>'
          
            '        <Regex token0='#39'escaped'#39' priority='#39'10'#39' regex='#39'\\\\'#39' />   ' +
            '   '
          '        <Regex token0='#39'escaped'#39' regex='#39'\\[a-z &quot; ]'#39' />    '
          '        <Regex token0='#39'escaped'#39' regex='#39'\\0x[a-fA-F0-9]+'#39' />'
          '      </Scheme>'
          ''
          
            '      <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'ba' +
            'dString'#39'>'
          '      </Scheme>'
          ''
          '      <Scheme name='#39'StringFind'#39'>'
          '        <Regex innerScheme='#39'BadString'#39' > '
          '            (&quot;|&apos;) (.*?\\ \1 )*? .* $ '
          '        </Regex>'
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '            (&quot;|&apos;) (.*?\\ \1 )*? \1'
          '        </Regex>'
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '            (&quot;|&apos;) (.*?\\ \1 )*? .*? [^\\] \1'
          '        </Regex>  '
          ''
          '        <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '            (&quot;|&apos;) \1'
          '        </Regex>'
          '        '
          '      </Scheme>'
          ''
          '    <Scheme name='#39'RegexFind'#39' defaultToken='#39'defaultEmbed'#39'>'
          '        <Regex token0='#39'escaped'#39' regex='#39'\\/'#39' />            '
          '    </Scheme>'
          '    '
          
            '    <Scheme name='#39'RegexSubst'#39' inherit='#39'RegexFind'#39' defaultToken='#39 +
            'defaultEmbed'#39'>'
          '        <Regex token0='#39'escaped'#39' regex='#39'\$\d'#39' />                '
          '    </Scheme>'
          ''
          '    <Scheme name='#39'DefaultEmpty'#39' defaultToken='#39'default'#39' />'
          '    '
          '    <Scheme name='#39'Main'#39' inherit='#39'StringFind'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'false'#39
          '    >'
          '    <KeywordRegex>\b[a-zA-Z_][\w_]*\b</KeywordRegex>'
          ''
          '    <Keywords>'
          '            Boolean'
          '            Array'
          '            join length reverse sort'
          '            NaN '
          '            null '
          '            true false '
          '            Infinity '
          '            undefined '
          '            Break '#9#9'continue '#9#9#9'for  in  if else return  while '
          '            '
          '            function new this var with  arguments '
          '            '
          '            Global escape unescape eval Number '
          '            Object new instanceof  delete typeof void '#9#9#9
          '            throw  try catch finally with'
          '    </Keywords>'
          ''
          ''
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'//.*$'#39' />'
          ''
          ''
          '    <RegexBlock start='#39'\/\*'#39' end='#39'\*\/'#39' end_token0='#39'comment'#39' '
          '                innerScheme='#39'Comment'#39'/>'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[   \}   \{    \]    \[  \( \)' +
            ' &gt; &lt; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          ''
          '    <RegexBlock innerScheme='#39'DefaultEmpty'#39' '
          '                start_token0='#39'symbol'#39
          '                end_token0='#39'symbol'#39' '
          '                priority='#39'100'#39
          '                chainBlock='#39'RegexFindEnd'#39
          '    >'
          ''
          '        <Start> \s* ([ =( ]) \s* (?= / )  </Start>        '
          '        <End> (?= / )  </End>        '
          '    </RegexBlock>'
          '    '
          '    <ChainBlock name='#39'RegexFindEnd'#39' '
          '                innerScheme='#39'RegexFind'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                chainBlock='#39'RegexFindEnd2'#39
          '    >'
          '        <End> / </End>        '
          '    </ChainBlock>'
          ''
          '    <ChainBlock name='#39'RegexFindEnd2'#39' '
          '                innerScheme='#39'RegexFind'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '    >'
          '        <End> /[igmx]* </End>        '
          '    </ChainBlock>'
          '        '
          '    <ChainBlock name='#39'RegexSubstEnd'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                innerScheme='#39'RegexSubst'#39'  >'
          '        <End> /[igmx]* </End>        '
          '    </ChainBlock>'
          '    '
          '    <RegexBlock innerScheme='#39'RegexFind'#39' '
          '                start_token0='#39'regexStart'#39' '
          '                end_token0='#39'regexEnd'#39' '
          '                chainBlock='#39'RegexSubstEnd'#39'>'
          ''
          '        <Start> s/  </Start>        '
          '        <End> /[igmx]* </End>        '
          '    </RegexBlock>'
          ''
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '    <SyntaxBlock capture="true">'
          '        <Start> kw:function  id  '
          '                sym:(  '
          '                        [^ sym:;    sym:}   sym:{   ]*  '
          '                sym:) '
          '                sym:{'
          '        </Start>'
          ''
          '        <End> sym:\}  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="true" priority='#39'10'#39'>'
          '        <Start>                 '
          
            '            [ kw:while  kw:do kw:if  kw:else  kw:try kw:catch  k' +
            'w:finally  kw:switch ]'
          '                                '
          '            [^ sym:;  sym:}  ]*?    sym:\{'
          '        </Start>'
          ''
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          '        '
          ''
          
            '    <SyntaxBlock capture="true" priority='#39'100'#39'  startGroupUsed='#39 +
            '1'#39'>'
          '        <Start>'
          
            '                    (  [ kw:while  kw:if  kw:with ]  sym:(   [^ ' +
            ' sym:;   sym:}    sym:{   ]+ sym:)   )    '
          ''
          '                    [^ sym:{  sym:}  sym:; ]+ sym:;'
          ''
          '            '
          '        </Start>'
          ''
          '        <End> sym:;  </End>'
          '    </SyntaxBlock>'
          ''
          '    <SyntaxBlock capture="false" priority='#39'0'#39'  >'
          '        <Start> sym:{  </Start>'
          '        <End> sym:}  </End>'
          '    </SyntaxBlock>'
          ''
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'INI'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ' '
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'tagOpen'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLd' +
            'DfF]? ) \b</Regex>'
          
            '    <Regex token0='#39'tagOpen'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]?' +
            ' \d+) \b</Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'BaseSyntax'#39' inherit='#39'Numbers'#39'>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> ^\#  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Comment'#39' end_token0='#39'comment'#39' >'
          '        <Start> ^;  </Start>        '
          '        <End> $ </End>        '
          '    </RegexBlock>'
          ''
          '    <RegexBlock innerScheme='#39'Url'#39' end_token0='#39'url'#39' >'
          '        <Start> ^\[  </Start>        '
          '        <End> \]$ </End>        '
          '    </RegexBlock>'
          ''
          '  </Scheme>    '
          ''
          
            '  <Scheme name='#39'String'#39' inherit='#39'BaseSyntax'#39' defaultToken='#39'strin' +
            'g'#39'>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot; c?'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot; c?'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot;  &quot; c?'
          '    </Regex>    '
          '  </Scheme>'
          '  '
          '  <Scheme name='#39'Comment'#39' defaultToken='#39'comment'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Url'#39' defaultToken='#39'url'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Main'#39' defaultToken='#39'default'#39
          '          inherit='#39'BaseSyntax'#39' inherit0='#39'StringFind'#39
          '          keywordsIgnoreCase='#39'true'#39' >'
          ''
          '          <Regex token0='#39'symbol'#39' regex='#39'[=]'#39' />'
          '          '
          '          <Regex token0='#39'keyword'#39' regex="^\$\([\w_]*\)" />'
          '          '
          '  </Scheme>'
          '  '
          '</SchemeList>')
      end
      item
        Name = 'DFM'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          ''
          '    <Scheme name='#39'Numbers'#39'>'
          '        <Regex token0='#39'number'#39'>'
          '            \b (0x \d+ [lL]? | \d+ (e \d*)? [lLdDfF]? ) \b'
          '        </Regex>'
          '        '
          '        <Regex token0='#39'number'#39'>'
          '            \b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? \d+) \b'
          '        </Regex>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'String'#39' defaultToken='#39'string'#39'>'
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Binary'#39' defaultToken='#39'comment'#39'>        '
          '        <SyntaxBlock capture="true">'
          '            <Start> commentStart  </Start>'
          '            <End> commentEnd  </End>'
          '        </SyntaxBlock>        '
          '    </Scheme>'
          ''
          '    <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' '
          '            defaultToken='#39'default'#39'           '
          ''
          '            keywordsIgnoreCase='#39'true'#39
          '    >'
          '        <KeywordRegex>\b[a-zA-Z_]\w*\b</KeywordRegex>'
          ''
          '        <Keywords>    '
          '            object  end item'
          '        </Keywords>'
          ''
          '        <RegexBlock innerScheme='#39'String'#39' start_token0='#39'string'#39' '
          '                    end_token0='#39'string'#39' >'
          ''
          '            <Start> &apos;  </Start>        '
          '            <End> &apos; </End>        '
          '        </RegexBlock>'
          ''
          
            '        <RegexBlock innerScheme='#39'Binary'#39' start_token0='#39'commentSt' +
            'art'#39' '
          '                    end_token0='#39'commentEnd'#39' >'
          '            <Start> \{  </Start>        '
          '            <End> \} </End>        '
          '        </RegexBlock>'
          ''
          
            '        <Regex token0='#39'symbol'#39' regex='#39'[   \]  \#  \[  \( \) &gt;' +
            ' &lt; ]'#39' />'
          '        <Regex token0='#39'symbol'#39' regex='#39'[-:?\~=+!^;,]'#39' />'
          '        '
          '        <SyntaxBlock capture="true">'
          '            <Start> [ kw:item kw:object ] </Start>'
          '            <End> kw:end  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> sym:&lt;  </Start>'
          '            <End> sym:&gt;  </End>'
          '        </SyntaxBlock>'
          ''
          '        <SyntaxBlock capture="true">'
          '            <Start> sym:(  </Start>'
          '            <End> sym:)  </End>'
          '        </SyntaxBlock>'
          ''
          '    </Scheme>'
          '</SchemeList>')
      end
      item
        Name = 'Batch'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          ''
          '<SchemeList root='#39'Main'#39'>'
          ''
          '  <Scheme name='#39'Text'#39' inherit='#39'Numbers'#39'>'
          '        <Regex token0='#39'email'#39'>'
          '            [_a-zA-Z\d\-\.]{1,40}'
          '            @ '
          '            ([_ a-z A-Z \d \-]+ '
          '            (\. [_ a-z A-Z \d \-]+ )+ )'
          '        </Regex>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'Numbers'#39'>'
          
            '    <Regex token0='#39'number'#39'>\b (0x \d+ [lL]? | \d+ (e \d*)? [lLdD' +
            'fF]? ) \b</Regex>'
          
            '    <Regex token0='#39'number'#39'>\b [+\-]? \d+ (\. \d+)? ([eE] [+\-]? ' +
            '\d+) \b</Regex>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'Comment'#39' inherit='#39'Text'#39' defaultToken='#39'comment'#39'  ' +
            '/>'
          ''
          '  <Scheme name='#39'String'#39' inherit='#39'Text'#39' defaultToken='#39'escaped'#39'>'
          '  </Scheme>'
          ''
          
            '  <Scheme name='#39'BadString'#39' inherit='#39'String'#39' defaultToken='#39'badStr' +
            'ing'#39'>'
          '  </Scheme>'
          ''
          '  <Scheme name='#39'StringFind'#39'>'
          '    <Regex innerScheme='#39'BadString'#39' > '
          '        &quot; (.*?\\ &quot; )*? .* $ '
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'10'#39'> '
          '        &quot; (.*?\\ &quot; )*? &quot; c?'
          '    </Regex>'
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'20'#39'> '
          '        &quot; (.*?\\ &quot; )*? .*? [^\\] &quot; c?'
          '    </Regex>  '
          ''
          '    <Regex innerScheme='#39'String'#39' priority='#39'30'#39'> '
          '        &quot;  &quot; c?'
          '    </Regex>    '
          '  </Scheme>'
          ''
          ''
          ''
          '  <Scheme name='#39'Main'#39' inherit='#39'Numbers'#39' inherit0='#39'StringFind'#39
          '          defaultToken='#39'default'#39'           '
          ''
          '          allIgnoreCase='#39'true'#39
          '  >'
          
            '    <KeywordRegex moreWordChars='#39'.'#39'>\b[a-zA-Z_][\w_]*\b</Keyword' +
            'Regex>'
          ''
          '    <Keywords>'
          '            rem set if else exist errorlevel'
          '            for in do break call copy chcp cd'
          '            chdir choice cls country ctty date del'
          '            erase dir echo exit goto loadfix loadhigh'
          '            mkdir md move path pause prompt rename ren'
          '            rmdir rd shift time type ver verify vol'
          '            com con lpt nul defined not errorlevel'
          '            cmdextversion'
          '            '
          '    </Keywords>'
          ''
          ''
          '    <Regex innerScheme='#39'Comment'#39' regex='#39'Rem.*$'#39' />'
          '    <Regex innerScheme='#39'Comment'#39' regex="\:\:.*$ " />'
          ''
          '    <Regex token0='#39'keyword'#39' regex="^\s*\b[a-zA-Z_][\w_]*\b" />'
          ''
          
            '    <Regex token0='#39'symbol'#39' regex='#39'[    \]    \[  \( \) &gt; &lt;' +
            '  &amp; ]'#39' />'
          '    <Regex token0='#39'symbol'#39' regex='#39'[-=+^;,]'#39' />'
          ''
          '    <SkipSyntaxToken token='#39'comment'#39' />'
          ''
          '     <SyntaxBlock capture="true" priority='#39'0'#39'  >'
          '        <Start> sym:(  </Start>'
          '        <End> sym:)  </End>'
          '    </SyntaxBlock>'
          '    '
          '  </Scheme>'
          '</SchemeList>')
      end>
    ColorSchemes = <
      item
        Name = 'default'
        Scheme.Strings = (
          '<?xml version="1.0" encoding="UTF-8"?>'
          '<Attributes>'
          #9'<Token name="default" textColor="#0"/>'
          #9'<Token name="defaultEmbed" backgroundColor="#ECE9D8"/>'
          #9'<Token name="docStart" fontStyle="bold"/>'
          #9'<Token name="docEnd" fontStyle="bold"/>'
          #9'<Token name="docBlockStart" fontStyle="bold"/>'
          #9'<Token name="docBlockEnd" fontStyle="bold"/>'
          #9'<Token name="docInstruction" fontStyle="bold"/>'
          #9'<Token name="docDefault" backgroundColor="#ECE9D8"/>'
          #9'<Token name="comment" textColor="#008000"/>'
          #9'<Token name="commentStart" textColor="#008000"/>'
          #9'<Token name="commentEnd" textColor="#008000"/>'
          #9'<Token name="keyword" textColor="Blue" fontStyle="bold"/>'
          #9'<Token name="privateSymbol" textColor="Navy"/>'
          #9'<Token name="email" fontStyle="bold"/>'
          
            #9'<Token name="hairy" textColor="#000080" fontStyle="bold, italic' +
            ', underline, strikeOut"/>'
          #9'<Token name="number" textColor="Blue"/>'
          #9'<Token name="symbol" textColor="Maroon" fontStyle="bold"/>'
          #9'<Token name="string" textColor="Blue"/>'
          #9'<Token name="blockStart" fontStyle="bold"/>'
          #9'<Token name="blockEnd" fontStyle="bold"/>'
          #9'<Token name="badString" textColor="Red"/>'
          #9'<Token name="preprocessor" textColor="#008080"/>'
          #9'<Token name="badPreprocessor" textColor="red"/>'
          #9'<Token name="escaped" textColor="Navy"/>'
          
            #9'<Token name="quotedVariable" textColor="Navy" fontStyle="bold"/' +
            '>'
          #9'<Token name="regexStart" textColor="Navy" fontStyle="bold"/>'
          
            #9'<Token name="regexEnd" textColor="Navy" fontStyle="bold"/><!--h' +
            'tml/xml tokens-->'
          #9'<Token name="tagOpen" textColor="Blue"/>'
          #9'<Token name="tagClose" textColor="Blue"/>'
          
            #9'<Token name="dtdTagStartEnd" textColor="Purple" fontStyle="bold' +
            '"/>'
          #9'<Token name="url" textColor="Red"/>'
          #9'<Token name="fpi" textColor="Red" fontStyle="bold"/>'
          #9'<Token name="entity" textColor="#FF4500"/>'
          #9'<Token name="attributeName" textColor="Red"/>'
          
            #9'<Token name="attributeValue" textColor="#8000FF" fontStyle="bol' +
            'd"/>'
          #9'<Token name="badAttributeValue" textColor="Red"/>'
          #9'<Token name="cssRule" textColor="#583485" fontStyle="bold"/>'
          #9'<Token name="cssRelSymbol" fontStyle="bold"/>'
          #9'<Token name="cssUrlHref" fontStyle="underline"/>'
          #9'<Token name="cssUrlAttr"/>'
          #9'<Token name="cssAttrListBG" textColor="Blue"/>'
          
            #9'<Token name="cdataTagStart" textColor="Purple" fontStyle="bold"' +
            '/>'
          #9'<Token name="cdataTagEnd" textColor="Purple" fontStyle="bold"/>'
          
            #9'<Token name="xmlDeclStart" backgroundColor="Yellow" textColor="' +
            'Red" fontStyle="bold"/>'
          
            #9'<Token name="xmlDeclEnd" backgroundColor="Yellow" textColor="Re' +
            'd" fontStyle="bold"/>'
          '</Attributes>')
      end>
    ActiveSyntaxScheme = 'INI'
    ActiveColorScheme = 'default'
    OnDocChanged = EditDocOriginalDocChanged
    Left = 240
    Top = 520
  end
end
