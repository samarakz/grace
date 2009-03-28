#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "canvaswidget.h"
extern "C" {
#include <graceapp.h>
}

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
  MainWindow(GraceApp *gapp, QMainWindow *parent = 0);
  ~MainWindow();

  void scroll_pix(int dx, int dy);
  CanvasWidget *canvasWidget;

  Ui::MainWindow ui;

protected:
  void closeEvent(QCloseEvent*);

private slots:
  void on_actionExit_triggered();
  void on_actionNew_triggered();
  void on_actionOpen_triggered();
  void on_actionSave_triggered();
  void on_actionSaveAs_triggered();
  void on_actionRevertToSaved_triggered();

  void on_actionZoom_triggered();
  void on_actionZoomX_triggered();
  void on_actionZoomY_triggered();

  void autoscale_proc(int type);
  void on_actionAutoScale_triggered();
  void on_actionAutoScaleX_triggered();
  void on_actionAutoScaleY_triggered();
  void on_actionAutoTick_triggered();

  void page_zoom_inout(int inout);
  void on_actionSmaller_triggered();
  void on_actionLarger_triggered();
  void on_actionOriginalSize_triggered();
  void on_actionRedraw_triggered();

  void graph_scroll_proc(int type);
  void on_actionScrollLeft_triggered();
  void on_actionScrollRight_triggered();
  void on_actionScrollUp_triggered();
  void on_actionScrollDown_triggered();

  void graph_zoom_proc(int type);
  void on_actionZoomIn_triggered();
  void on_actionZoomOut_triggered();

  void on_actionAddText_triggered();

  void on_actionPreferences_triggered();

private:
  GraceApp *gapp;

  void setToolBarIcons();
  void sync_canvas_size(GraceApp *gapp);

  QString curFile;
};

#endif /* __MAINWINDOW_H_ */

