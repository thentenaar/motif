XMSTRING_LIST = xmstring.list

MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = xmstring.list Xm.msg \
             XmStrDefs.ct

AM_CPPFLAGS = -I.. -I$(srcdir)/.. \
              -DXMBINDDIR_FALLBACK=\"@XMBINDDIR_FALLBACK@\" \
              -DINCDIR=\"@INCDIR@\" -DLIBDIR=\"@LIBDIR@\"   \
              -DDATADIR=\"@DATADIR@\" -DRGB_TXT=\"@RGB_TXT@\" \
              $(XFT_CFLAGS) ${X_CFLAGS}
xmdir = $(includedir)/Xm

libXm_la_LDFLAGS = -version-info @ABI_VERSION@
libXm_la_LIBADD  = $(XFT_LIBS) ${X_LIBS}
lib_LTLIBRARIES  = libXm.la

TIP_SRCS = ToolTip.c
TIP_HDRS = ToolTipI.h

EXTRA_HDRS = Ext.h ExtP.h  \
             Column.h ColumnP.h \
             ButtonBox.h ButtonBoxP.h \
             ColorS.h ColorSP.h \
             Picture.h PictureP.h DataF.h DataFP.h DataFSelP.h \
             Ext18List.h Ext18ListP.h MultiList.h MultiListP.h \
             Paned.h PanedP.h FontS.h FontSP.h \
             IconBox.h IconBoxP.h IconButton.h IconButtonP.h xmlist.h \
             Hierarchy.h HierarchyP.h Outline.h OutlineP.h \
             TabBox.h TabBoxP.h TabStack.h TabStackP.h \
             TabList.h DrawUtils.h Tree.h TreeP.h \
             ComboBox2.h ComboBox2P.h DropDown.h DropDownP.h \
             ToolTipT.h ToolTipCT.h

EXTRA_SRCS = XmExtUtil.c \
             Column.c ButtonBox.c ColorS.c Picture.c DataFSel.c DataF.c \
             MultiList.c I18List.c Paned.c FontS.c IconBox.c IconButton.c xmlist.c \
             Hierarchy.c Outline.c TabBox.c TabList.c TabStack.c \
             DrawUtils.c Tree.c DropDown.c \
             XmStrDefs.ht XmStrDefs22.ht XmStrDefs23.ht XmStrDefsI.ht

SLIDE_HDRS = SlideC.h SlideCP.h
SLIDE_SRCS = SlideC.c

if XM_WITH_JPEG
JPEG_SRCS = Jpeg.c
JPEG_HDRS = JpegI.h
AM_CPPFLAGS     += $(JPEG_CFLAGS)
libXm_la_LIBADD += $(JPEG_LIBS)
else
JPEG_SRCS =
JPEG_HDRS =
endif

if XM_WITH_PNG
PNG_SRCS = Png.c
PNG_HDRS = PngI.h
AM_CPPFLAGS += $(PNG_CFLAGS)
libXm_la_LIBADD += $(PNG_LIBS)
else
PNG_SRCS =
PNG_HDRS =
endif

if XM_PRINTING
PRINTS_SRC = PrintS.c
AM_CPPFLAGS += $(XP_CFLAGS)
libXm_la_LIBADD += $(XP_LIBS)
else
PRINTS_SRC =
endif

noinst_HEADERS = BaseClassI.h \
BitmapsI.h \
BulletinBI.h \
CacheI.h \
CallbackI.h \
CareVisualTI.h \
CascadeBGI.h \
CascadeBI.h \
ClipWindTI.h \
CntrbmI.h \
ColorI.h \
ColorObjI.h \
CommandI.h \
CutPasteI.h \
DestI.h \
DisplayI.h \
DragBSI.h \
DragCI.h \
DragICCI.h \
DragIconI.h \
DragOverSI.h \
DragUnderI.h \
DrawI.h \
DrawingAI.h \
DropSMgrI.h \
ExtObjectI.h \
GMUtilsI.h \
GadgetI.h \
GadgetUtiI.h \
GeoUtilsI.h \
HashI.h \
IconGI.h \
ImageCachI.h \
LabelGI.h \
LabelI.h \
ManagerI.h \
MapEventsI.h \
MenuProcI.h \
MenuShellI.h \
MenuStateI.h \
MenuUtilI.h \
MessageBI.h \
MessagesI.h \
nanosvg.h \
nanosvgrast.h \
PixConvI.h \
PrimitiveI.h \
PrintSI.h \
ProtocolsI.h \
RCHookI.h \
RCLayoutI.h \
RCMenuI.h \
ReadImageI.h \
RegionI.h \
RepTypeI.h \
ResConverI.h \
ResEncodI.h \
ResIndI.h \
RowColumnI.h \
ScreenI.h \
ScrollFramTI.h \
ScrolledWI.h \
SelectioBI.h \
SeparatoGI.h \
SvgI.h \
SyntheticI.h \
TearOffI.h \
TextDIconI.h \
TextFI.h \
TextFSelI.h \
TextI.h \
TextInI.h \
TextOutI.h \
TextSelI.h \
TextStrSoI.h \
ToggleBGI.h \
TraitI.h \
TransferI.h \
TravActI.h \
TraversalI.h \
UniqueEvnI.h \
VaSimpleI.h \
VendorSEI.h \
VendorSI.h \
VirtKeysI.h \
XmI.h \
XmImI.h \
XmMsgI.h \
XmRenderTI.h \
XmStrDefsI.h \
XmStringI.h \
XmTabListI.h \
XmosI.h \
XpmI.h \
$(JPEG_HDRS) \
$(PNG_HDRS) \
$(TIP_HDRS)

xm_HEADERS = ArrowB.h      ArrowBG.h       ArrowBGP.h      ArrowBP.h \
        BulletinB.h     BulletinBP.h    CacheP.h        CascadeB.h \
        CascadeBG.h     CascadeBGP.h    CascadeBP.h     Command.h \
        CommandP.h      CutPaste.h      DialogS.h \
        DialogSP.h      DialogSEP.h     DragC.h         DragCP.h \
        DragDrop.h      DragIcon.h      DragIconP.h     DragOverS.h \
        DragOverSP.h    DropTransP.h    DropSMgr.h      DropSMgrP.h \
        DropTrans.h     DrawingA.h      DrawingAP.h     DrawnB.h \
        DrawnBP.h       FileSB.h        FileSBP.h       Form.h \
        FormP.h         Frame.h         FrameP.h        Label.h \
        LabelG.h        LabelGP.h       LabelP.h        List.h \
        ListP.h         MainW.h         MainWP.h        MenuShell.h \
        MenuShellP.h    MessageB.h      MessageBP.h     PanedW.h \
        PanedWP.h       PushB.h         PushBG.h        PushBGP.h \
        PushBP.h        RCLayoutP.h     RowColumn.h     RowColumnP.h \
        SashP.h         Scale.h         ScaleP.h        ScrollBar.h \
        ScrollBarP.h    ScrolledW.h     ScrolledWP.h    SelectioB.h \
        SelectioBP.h    SeparatoG.h     SeparatoGP.h    Separator.h \
        SeparatorP.h    TextF.h         TextFP.h        TextFSelP.h \
        TextSelP.h      Text.h          TextInP.h       TextOutP.h \
        TextP.h         TextStrSoP.h    ToggleB.h       ToggleBG.h \
        ToggleBGP.h     ToggleBP.h      XmP.h \
        BaseClassP.h    ManagerP.h      PrimitiveP.h    GadgetP.h \
        XmStrDefs.h     XmStrDefs22.h   XmStrDefs23.h \
        VaSimpleP.h     VirtKeys.h      VirtKeysP.h \
        ExtObjectP.h    Display.h       DisplayP.h      Screen.h \
        ScreenP.h       ShellEP.h       VendorS.h       VendorSP.h \
        VendorSEP.h     DesktopP.h      AtomMgr.h \
        Protocols.h     ProtocolsP.h    MwmUtil.h       TransltnsP.h \
        RepType.h       XmosP.h         Xmos_r.h       DrawP.h   \
        Xmpoll.h        TearOffP.h \
        TearOffBP.h     MenuUtilP.h     MenuT.h         XmAll.h \
        TraitP.h        SpinB.h         SpinBP.h        SSpinB.h \
        SSpinBP.h \
        Container.h     ContainerP.h    IconG.h         IconGP.h \
        Notebook.h      NotebookP.h     ContItemT.h     ContainerT.h \
        RCMenuP.h       ClipWindowP.h   TransferT.h \
        Transfer.h      AccTextT.h      NavigatorT.h    IconFileP.h\
        ScrollFrameT.h  GrabShell.h     GrabShellP.h    ComboBox.h \
        ComboBoxP.h     XmIm.h          IconFile.h      AccColorT.h\
        ActivatableT.h  JoinSideT.h     SpecRenderT.h \
        CareVisualT.h   DialogSavvyT.h  TransferP.h     TakesDefT.h \
        ColorP.h        Primitive.h     Manager.h \
        Gadget.h        LayoutT.h       IconH.h         IconHP.h \
        UnitTypeT.h     TxtPropCv.h     MenuProcP.h     MenuStateP.h\
        ColorObjP.h     TravConT.h      UnhighlightT.h  PointInT.h \
        Print.h         PrintSP.h       XpmP.h		Xmfuncs.h \
        deprecated.h obsolete.h version.h ${SLIDE_HDRS} ${EXTRA_HDRS}

nodist_xm_HEADERS = Xm.h

SRCS1 = XmStrDefs.c ArrowB.c        ArrowBG.c       BulletinB.c     CallbackI.c \
        CascadeB.c      CascadeBG.c     Command.c       CutPaste.c \
        Dest.c          DialogS.c       DialogSE.c      DragBS.c \
        DragC.c         DragOverS.c     DragICC.c       DragIcon.c \
        DragUnder.c     DrawingA.c      DrawnB.c        DropSMgr.c \
        DropSMgrI.c     DropTrans.c     ExtObject.c     FileSB.c \
        Form.c          Frame.c         Gadget.c        GadgetUtil.c \
        GeoUtils.c      ImageCache.c    Label.c         LabelG.c \
        List.c          MainW.c         MapEvents.c     MenuShell.c \
        MenuUtil.c      MessageB.c      PanedW.c        PushB.c \
        PushBG.c        ReadImage.c     Region.c        ResConvert.c \
        ResInd.c        RowColumn.c     Sash.c          Scale.c \
        ScrollBar.c     ScrolledW.c     SelectioB.c     SeparatoG.c \
        Separator.c     Simple.c        Svg.c           Text.c \
        TextIn.c        TextOut.c       TextStrSo.c     TextSel.c \
        ToggleB.c       ToggleBG.c      TrackLoc.c      UniqueEvnt.c \
        VaSimple.c      VirtKeys.c      XmString.c      ResEncod.c\
        TextF.c         TextFSel.c      AtomMgr.c \
        Cache.c         Protocols.c     Primitive.c     Manager.c \
        Traversal.c     TravAct.c       TraversalI.c    BaseClass.c

SRCS2 = Desktop.c       Messages.c      Transltns.c     RepType.c \
        Xmos.c          Draw.c          TearOff.c       TearOffB.c \
        XmIm.c          GMUtils.c       Display.c       Screen.c \
        ShellE.c        VendorS.c       VendorSE.c      RCMenu.c  \
        RCLayout.c      GetPixData.c    RCPopup.c       Trait.c  \
        Synthetic.c     XmFontList.c    ClipWindow.c    Transfer.c \
        Hash.c          ScrollFrameT.c  \
        XmTabList.c     Resolve.c       XmStringFunc.c \
        XmRenderT.c     PixConv.c       Color.c         Xm.c \
        CareVisualT.c   Direction.c     XmStringObso.c \
        IsMwmRun.c      MenuProc.c      MenuState.c     TextFind.c \
        TextFunc.c      ColorObj.c      IconFile.c      RCHook.c \
        GetRes.c        BBUtil.c        ChColor.c       ColObjFunc.c \
        DrTog.c         DrArrow.c       DrPoly.c        DrHiDash.c \
        ValTime.c       FocusAct.c      CvtUnit.c       ScalTics.c \
        ScroVis.c       WidGet.c        $(PRINTS_SRC)

OBSO_SRCS = Obso1_2.c ObsoStr.c ObsoXme.c Obso2_0.c

XPM_SRCS = \
        XpmAttrib.c    XpmCrIFrDat.c  XpmInfo.c      XpmWrFFrDat.c  Xpmmisc.c \
        XpmCrBufFrI.c  XpmCrIFrP.c    XpmRdFToBuf.c  XpmWrFFrI.c    Xpmparse.c\
        XpmCrBufFrP.c  XpmCrPFrBuf.c  XpmRdFToDat.c  XpmWrFFrP.c    Xpmrgb.c \
        XpmCrDatFrI.c  XpmCrPFrDat.c  XpmRdFToI.c    Xpmcreate.c    Xpmscan.c \
	XpmCrDatFrP.c  XpmCrPFrI.c    XpmRdFToP.c    Xpmdata.c \
        XpmCrIFrBuf.c  XpmImage.c     XpmWrFFrBuf.c  Xpmhashtab.c

NEW_WID_SRCS =         IconH.c        Container.c     IconG.c  \
        Notebook.c     ComboBox.c     GrabShell.c     SpinB.c  \
        SSpinB.c       TxtPropCv.c    TranslNew.c

libXm_la_SOURCES = $(SRCS1) $(SRCS2) $(OBSO_SRCS) $(XPM_SRCS) $(NEW_WID_SRCS) $(SLIDE_SRCS) \
        $(TIP_SRCS) $(EXTRA_SRCS) $(JPEG_SRCS) $(PNG_SRCS)

EXTRA_libXm_la_SOURCES = $(SLIDE_SRCS) $(TIP_SRCS)

STRING_FILES = XmStrDefs.h XmStrDefs22.h XmStrDefs23.h XmStrDefsI.h XmStrDefs.c

BUILT_SOURCES = $(STRING_FILES) $(CAT_FILES)

$(top_builddir)/config/util/makestrs:
	( cd $(top_builddir)/config/util && $(MAKE) $(AM_MAKEFLAGS) makestrs )

XmStrDefs.h XmStrDefs22.h XmStrDefs23.h XmStrDefsI.h: XmStrDefs.c

XmStrDefs.c: Makefile $(XMSTRING_LIST) XmStrDefs.ht XmStrDefsI.ht XmStrDefs.ct $(top_builddir)/config/util/makestrs
	test -f $(XMSTRING_LIST) || $(LN_S) $(srcdir)/$(XMSTRING_LIST) $(XMSTRING_LIST)
	test -f XmStrDefs.ht || $(LN_S) $(srcdir)/XmStrDefs.ht XmStrDefs.ht
	test -f XmStrDefs22.ht || $(LN_S) $(srcdir)/XmStrDefs22.ht XmStrDefs22.ht
	test -f XmStrDefs23.ht || $(LN_S) $(srcdir)/XmStrDefs23.ht XmStrDefs23.ht
	test -f XmStrDefsI.ht || $(LN_S) $(srcdir)/XmStrDefsI.ht XmStrDefsI.ht
	test -f XmStrDefs.ct || $(LN_S) $(srcdir)/XmStrDefs.ct XmStrDefs.ct
	$(top_builddir)/config/util/makestrs -f $(XMSTRING_LIST) >XmStrDefs.c

if XM_MSGCAT
CAT_FILES = Xm.cat XmMsgCatI.h

$(CAT_FILES): Xm.msg
	$(mkinstalldirs) $(top_builddir)/localized/C/msg
	$(top_builddir)/localized/util/mkcatdefs XmMsgCatI.h $(srcdir)/Xm.msg >$(top_builddir)/localized/C/msg/Xm.msg
	gencat Xm.cat $(top_builddir)/localized/C/msg/Xm.msg
endif

