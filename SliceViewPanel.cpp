#include "ui_SliceViewPanel.h"
#include "SliceViewPanel.h"

#include <QCursor>
#include <QBitmap>
#include <QToolButton>
#include <QDebug>

#include <QStackedLayout>
#include <QMenu>

SliceViewPanel::SliceViewPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SliceViewPanel)
{
  ui->setupUi(this);

  // Initialize
  //m_GlobalUI = NULL;
  //m_SliceModel = NULL;

  // Create my own renderers
  //m_SnakeModeRenderer = SnakeModeRenderer::New();
  //m_DecorationRenderer = SliceWindowDecorationRenderer::New();

  QString menuStyle = "font-size: 12px;";
  //qDebug() << "ui->setupUi(this)";
  // Create the popup menus for the polygon mode
  //m_MenuPolyInactive = new QMenu(ui->imPolygon);
  /*m_MenuPolyInactive->setStyleSheet(menuStyle);
  m_MenuPolyInactive->addAction(ui->actionPaste);


  //m_MenuPolyDrawing = new QMenu(ui->imPolygon);
  m_MenuPolyDrawing->setStyleSheet(menuStyle);
  m_MenuPolyDrawing->addAction(ui->actionComplete);
  m_MenuPolyDrawing->addAction(ui->actionCompleteAndAccept);
  m_MenuPolyDrawing->addAction(ui->actionUndo);
  m_MenuPolyDrawing->addAction(ui->actionClearDrawing);

  m_MenuPolyEditing = new QMenu(ui->slice);
  m_MenuPolyEditing->setStyleSheet(menuStyle);
  m_MenuPolyEditing->addAction(ui->actionAccept);
  m_MenuPolyEditing->addAction(ui->actionDeleteSelected);
  m_MenuPolyEditing->addAction(ui->actionSplitSelected);
  m_MenuPolyEditing->addAction(ui->actionClearPolygon);*/


  // Connect the actions to the toolbar buttons (sucks to do this by hand)
  ui->btnAcceptPolygon->setDefaultAction(ui->actionAccept);
  ui->btnPastePolygon->setDefaultAction(ui->actionPaste);
  ui->btnClearDrawing->setDefaultAction(ui->actionClearDrawing);
  ui->btnCloseLoop->setDefaultAction(ui->actionComplete);
  ui->btnDeleteNodes->setDefaultAction(ui->actionDeleteSelected);
  ui->btnDeletePolygon->setDefaultAction(ui->actionClearPolygon);
  ui->btnSplitNodes->setDefaultAction(ui->actionSplitSelected);
  ui->btnUndoLast->setDefaultAction(ui->actionUndo);

  ui->btnAnnotationAcceptLine->setDefaultAction(ui->actionAnnotationAcceptLine);
  ui->btnAnnotationClearLine->setDefaultAction(ui->actionAnnotationClearLine);
  ui->btnAnnotationSelectAll->setDefaultAction(ui->actionAnnotationSelectAll);
  ui->btnAnnotationEdit->setDefaultAction(ui->actionAnnotationEdit);
  ui->btnAnnotationDeleteSelected->setDefaultAction(ui->actionAnnotationDelete);
  ui->btnAnnotationNext->setDefaultAction(ui->actionAnnotationNext);
  ui->btnAnnotationPrevious->setDefaultAction(ui->actionAnnotationPrevious);

  this->addAction(ui->actionZoom_In);
  this->addAction(ui->actionZoom_Out);

  // Connect the context menu signal from polygon mode to this widget
  //connect(ui->imPolygon, SIGNAL(contextMenuRequested()), SLOT(onContextMenu()));

  // Arrange the interaction modes into a tree structure. The first child of
  // every interaction mode is an empty QWidget. The tree is used to allow
  // events to fall through from one interaction mode to another

  // TODO: can we think of a more dynamic system for doing this? Essentially,
  // there is a pipeline through which events pass through, which is
  // reconfigured when the mode changes.


  // Add all the individual interactors to the main panel using a stacked
  // layout. This assures that all of the interactors have the same size as
  // the sliceView. The actual propagation of events is handled by the
  // event filter logic.
  /*
  QStackedLayout *loMain = new QStackedLayout();
  loMain->setContentsMargins(0,0,0,0);
  loMain->addWidget(ui->imCrosshairs);
  loMain->addWidget(ui->imZoomPan);
  loMain->addWidget(ui->imThumbnail);
  loMain->addWidget(ui->imPolygon);
  loMain->addWidget(ui->imSnakeROI);
  loMain->addWidget(ui->imPaintbrush);
  delete ui->sliceView->layout();
  ui->sliceView->setLayout(loMain);
  */

  // Configure the initial event chain
  m_CurrentEventFilter = NULL;
  //ConfigureEventChain(ui->imCrosshairs);

  // Also lay out the pages
  QStackedLayout *loPages = new QStackedLayout();
  loPages->addWidget(ui->pageDefault);
  loPages->addWidget(ui->pagePolygonDraw);
  loPages->addWidget(ui->pagePolygonEdit);
  loPages->addWidget(ui->pagePolygonInactive);
  loPages->addWidget(ui->pageAnnotateLineActive);
  loPages->addWidget(ui->pageAnnotateSelect);
  delete ui->toolbar->layout();
  ui->toolbar->setLayout(loPages);

  // Send wheel events from Crosshairs mode to the slider
  //ui->imCrosshairs->SetWheelEventTargetWidget(ui->inSlicePosition);

  // Set page size on the slice position widget
  ui->inSlicePosition->setPageStep(5);

  // Set up the drawing cursor
  QBitmap bmBitmap(":/new/Resources/crosshair_cursor_bitmap.png");
  QBitmap bmMask(":/new/Resources/crosshair_cursor_mask.png");
  m_DrawingCrosshairCursor = new QCursor(bmBitmap, bmMask, 7, 7);

  // Configure the context tool button
  /*m_ContextToolButton = new QToolButton(ui->sliceView);
  m_ContextToolButton->setIcon(QIcon(":/root/context_gray_10.png"));
  m_ContextToolButton->setVisible(false);
  m_ContextToolButton->setAutoRaise(true);
  m_ContextToolButton->setIconSize(QSize(10,10));
  m_ContextToolButton->setMinimumSize(QSize(16,16));
  m_ContextToolButton->setMaximumSize(QSize(16,16));
  m_ContextToolButton->setPopupMode(QToolButton::InstantPopup);
  m_ContextToolButton->setStyleSheet("QToolButton::menu-indicator { image: none; }");*/
}

SliceViewPanel::~SliceViewPanel()
{
  delete ui;
  delete m_DrawingCrosshairCursor;
}


void SliceViewPanel::on_inSlicePosition_valueChanged(int value)
{
  // Update the text output
  int pos = ui->inSlicePosition->value();
  int lim = ui->inSlicePosition->maximum();
  ui->lblSliceInfo->setText(QString("%1 of %2").arg(pos+1).arg(lim+1));
}


// TODO: implement semi-transparent rendering on widgets on top of the
// OpenGL scene using code from
// http://www.qtcentre.org/wiki/index.php?title=Accelerate_your_Widgets_with_OpenGL
void SliceViewPanel::enterEvent(QEvent *)
{
  /*
  ui->mainToolbar->show();
  ui->sidebar->show();*/

}

void SliceViewPanel::leaveEvent(QEvent *)
{
  /*
  ui->mainToolbar->hide();
  ui->sidebar->hide();*/



}

void SliceViewPanel::SetActiveMode(QWidget *mode, bool clearChildren)
{
  // If the widget does not have a stacked layout, do nothing, we've reached
  // the end of the recursion
  QStackedLayout *loParent =
      dynamic_cast<QStackedLayout *>(mode->parentWidget()->layout());

  if(loParent)
    {
    // Set the mode as the current widget in the parent
    loParent->setCurrentWidget(mode);

    // Make sure the parent widget is also the current widget
    SetActiveMode(mode->parentWidget(), false);

    // Make sure no children are selected
    if(clearChildren)
      {
      // If the selected mode has child modes, make sure none is selected
      QStackedLayout *lo = dynamic_cast<QStackedLayout *>(mode->layout());
      if(lo)
        lo->setCurrentIndex(0);
      }
    }
}

/*
void SliceViewPanel::OnToolbarModeChange()
{
  // Get the renderer and configure its overlays
  GenericSliceRenderer *ren = (GenericSliceRenderer *) ui->sliceView->GetRenderer();

  // Configure the renderers
  GenericSliceRenderer::RendererDelegateList &ovTiled = ren->GetTiledOverlays();
  GenericSliceRenderer::RendererDelegateList &ovGlobal = ren->GetGlobalOverlays();

  // Append the overlays in the right order
  ovTiled.clear();
  ovTiled.push_back(m_SnakeModeRenderer);
  ovTiled.push_back(ui->imCrosshairs->GetRenderer());
  ovTiled.push_back(ui->imAnnotation->GetRenderer());
  ovTiled.push_back(ui->imPolygon->GetRenderer());

  ovGlobal.clear();
  ovGlobal.push_back(m_DecorationRenderer);

  switch(m_GlobalUI->GetGlobalState()->GetToolbarMode())
    {
    case POLYGON_DRAWING_MODE:
      ConfigureEventChain(ui->imPolygon);
      break;
    case PAINTBRUSH_MODE:
      ConfigureEventChain(ui->imPaintbrush);
      ovTiled.push_back(ui->imPaintbrush->GetRenderer());
      break;
    case ANNOTATION_MODE:
      ConfigureEventChain(ui->imAnnotation);
      ovTiled.push_back(ui->imAnnotation->GetRenderer());
      break;
    case REGISTRATION_MODE:
      ConfigureEventChain(ui->imRegistration);
      ovTiled.push_back(ui->imRegistration->GetRenderer());
      break;
    case ROI_MODE:
      ConfigureEventChain(ui->imSnakeROI);
      ovTiled.push_back(ui->imSnakeROI->GetRenderer());
      break;
    case CROSSHAIRS_MODE:
      ConfigureEventChain(ui->imCrosshairs);
      break;
    case NAVIGATION_MODE:
      ConfigureEventChain(ui->imZoomPan);
      break;
    }

  // Need to change to the appropriate page
  QStackedLayout *loPages =
      static_cast<QStackedLayout *>(ui->toolbar->layout());
  if(m_GlobalUI->GetGlobalState()->GetToolbarMode() == POLYGON_DRAWING_MODE)
    {
    switch(m_GlobalUI->GetPolygonDrawingModel(m_Index)->GetState())
      {
      case PolygonDrawingModel::DRAWING_STATE:
        loPages->setCurrentWidget(ui->pagePolygonDraw); break;
      case PolygonDrawingModel::EDITING_STATE:
        loPages->setCurrentWidget(ui->pagePolygonEdit); break;
      case PolygonDrawingModel::INACTIVE_STATE:
        loPages->setCurrentWidget(ui->pagePolygonInactive); break;
      }
    }
  else if(m_GlobalUI->GetGlobalState()->GetToolbarMode() == ANNOTATION_MODE)
    {
    AnnotationModel *am = m_GlobalUI->GetAnnotationModel(m_Index);
    if(am->GetFlagDrawingLine())
      loPages->setCurrentWidget(ui->pageAnnotateLineActive);
    else if(am->GetAnnotationMode() == ANNOTATION_SELECT)
      loPages->setCurrentWidget(ui->pageAnnotateSelect);
    else
      loPages->setCurrentWidget(ui->pageDefault);
    }
  else
    {
    loPages->setCurrentWidget(ui->pageDefault);
    }
}
*/

void SliceViewPanel::on_btnZoomToFit_clicked()
{
  //m_GlobalUI->GetSliceCoordinator()->ResetViewToFitInOneWindow(m_Index);

}

void SliceViewPanel::onContextMenu()
{
  /*if(m_GlobalUI->GetGlobalState()->GetToolbarMode() == POLYGON_DRAWING_MODE)
    {
    QMenu *menu = NULL;
    switch(m_GlobalUI->GetPolygonDrawingModel(m_Index)->GetState())
      {
      case PolygonDrawingModel::DRAWING_STATE:
        menu = m_MenuPolyDrawing; break;
      case PolygonDrawingModel::EDITING_STATE:
        menu = m_MenuPolyEditing; break;
      case PolygonDrawingModel::INACTIVE_STATE:
        menu = m_MenuPolyInactive; break;
      }

    if(menu)
      {
      menu->popup(QCursor::pos());
      }
    }*/
}

void SliceViewPanel::SetMouseMotionTracking(bool enable)
{
  ui->sliceView->setMouseTracking(enable);
  // TODO: in the future, consider using a better cursor for polygon drawing operations
  /*
  if(enable)
    this->setCursor(*m_DrawingCrosshairCursor);
  else
    this->setCursor(QCursor(Qt::ArrowCursor));
    */
}

void SliceViewPanel::on_btnExpand_clicked()
{
  // Get the layout applied when the button is pressed
  /*DisplayLayoutModel *dlm = m_GlobalUI->GetDisplayLayoutModel();
  DisplayLayoutModel::ViewPanelLayout layout =
      dlm->GetViewPanelExpandButtonActionModel(m_Index)->GetValue();

  // Apply this layout
  dlm->GetViewPanelLayoutModel()->SetValue(layout);*/
}

void SliceViewPanel::UpdateExpandViewButton()
{
  // Get the layout applied when the button is pressed
  /*DisplayLayoutModel *dlm = m_GlobalUI->GetDisplayLayoutModel();
  DisplayLayoutModel::ViewPanelLayout layout =
      dlm->GetViewPanelExpandButtonActionModel(m_Index)->GetValue();

  // Set the appropriate icon
  static const char* iconNames[] =
    { "fourviews", "axial", "coronal", "sagittal", "3d" };

  QString iconFile = QString(":/root/dl_%1.png").arg(iconNames[layout]);
  ui->btnExpand->setIcon(QIcon(iconFile));

  // Set the tooltip
  if(layout == DisplayLayoutModel::VIEW_ALL)
    {
    ui->btnExpand->setToolTip("Restore the four-panel display configuration");
    }
  else
    {
    ui->btnExpand->setToolTip("Expand this view to occupy the entire window");
    }

  // Also expand the tile/cascade button
  LayerLayout ll = dlm->GetSliceViewLayerLayoutModel()->GetValue();
  if(ll == LAYOUT_TILED)
    {
    ui->btnToggleLayout->setIcon(QIcon(":/root/layout_thumb_16.png"));
    }
  else if(ll == LAYOUT_STACKED)
    {
    ui->btnToggleLayout->setIcon(QIcon(":/root/layout_tile_16.png"));
    }*/
}


void SliceViewPanel::on_btnScreenshot_clicked()
{
  /*MainImageWindow *parent = findParentWidget<MainImageWindow>(this);
  parent->ExportScreenshot(m_Index);*/
}

void SliceViewPanel::on_btnToggleLayout_clicked()
{
  //m_GlobalUI->GetDisplayLayoutModel()->ToggleSliceViewLayerLayout();
}

void SliceViewPanel::on_actionZoom_In_triggered()
{
  // Zoom in
  //m_GlobalUI->GetSliceCoordinator()->ZoomInOrOutInOneWindow(m_Index, 1.1);
}

void SliceViewPanel::on_actionZoom_Out_triggered()
{
  // Zoom out
  //m_GlobalUI->GetSliceCoordinator()->ZoomInOrOutInOneWindow(m_Index, 1.0 / 1.1);
}

/*void SliceViewPanel::OnHoveredLayerChange(const EventBucket &eb)
{
  // Determine the position where to draw the button
  const SliceViewportLayout::SubViewport *vp = m_SliceModel->GetHoveredViewport();
  if(vp)
    {
    // Set up the location of the button
    int vppr = m_SliceModel->GetSizeReporter()->GetViewportPixelRatio();
    int x = (vp->pos[0] + vp->size[0]) / vppr - m_ContextToolButton->width();
    int y = (ui->sliceView->height() - 1) - (vp->pos[1] + vp->size[1]) / vppr;
    m_ContextToolButton->setVisible(true);
    m_ContextToolButton->move(x, y);

    // Set up the context menu on the button
    MainImageWindow *winmain = findParentWidget<MainImageWindow>(this);
    LayerInspectorDialog *inspector = winmain->GetLayerInspector();

    // Get the corresponding context menu
    QMenu *menu = inspector->GetLayerContextMenu(
                    m_SliceModel->GetDriver()->GetCurrentImageData()->FindLayer(
                      m_SliceModel->GetHoveredImageLayerId(), false, ALL_ROLES));

    // Show the menu
    m_ContextToolButton->setMenu(menu);
    m_ContextToolButton->setDown(false);
    }
  else
    {
    m_ContextToolButton->setVisible(false);
    m_ContextToolButton->setMenu(NULL);
    m_ContextToolButton->setDown(false);
    }
}*/







void SliceViewPanel::on_actionAnnotationAcceptLine_triggered()
{
  //m_GlobalUI->GetAnnotationModel(m_Index)->AcceptLine();
}

void SliceViewPanel::on_actionAnnotationClearLine_triggered()
{
  //m_GlobalUI->GetAnnotationModel(m_Index)->CancelLine();
}

void SliceViewPanel::on_actionAnnotationSelectAll_triggered()
{
  //m_GlobalUI->GetAnnotationModel(m_Index)->SelectAllOnSlice();
}

void SliceViewPanel::on_actionAnnotationDelete_triggered()
{
  //m_GlobalUI->GetAnnotationModel(m_Index)->DeleteSelectedOnSlice();
}

void SliceViewPanel::on_actionAnnotationNext_triggered()
{
  //m_GlobalUI->GetAnnotationModel(m_Index)->GoToNextAnnotation();
}

void SliceViewPanel::on_actionAnnotationPrevious_triggered()
{
  //m_GlobalUI->GetAnnotationModel(m_Index)->GoToPreviousAnnotation();
}

void SliceViewPanel::on_actionAnnotationEdit_triggered()
{
  // Show the annotation editor
  /*AnnotationEditDialog *dialog = new AnnotationEditDialog(this);
  dialog->SetModel(m_GlobalUI->GetAnnotationModel(m_Index));
  dialog->show();
  dialog->activateWindow();
  dialog->raise();*/
}

QVTKOpenGLWidget * SliceViewPanel::getVTKView(){
    return this->ui->slice;
}
