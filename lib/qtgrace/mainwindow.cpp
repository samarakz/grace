#include <QPainter>
#include <QFont>

#include "mainwindow.h"
#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>
#include <QTextStream>
#include <QBitmap>
extern "C" {
#include <files.h>
#include <bitmaps.h>
}

MainWindow::MainWindow(GraceApp *gapp, QMainWindow *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    setWindowIcon(QPixmap(gapp_icon_xpm));
    setToolBarIcons();

    readSettings();
    //restoreState();

    setCurrentFile("");

    this->gapp = gapp;
    gapp->gui->inwin = TRUE; // TODO: reimplement startup_gui(gapp) function here

    canvasWidget = ui.widget;
    canvasWidget->qtdrawgraph(gapp->gp);
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (maybeSave()) {
    writeSettings();
//    restoreState();
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::setToolBarIcons()
{
    ui.actionRedraw->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), redraw_bits, QImage::Format_MonoLSB)));

    ui.actionZoom->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), zoom_bits, QImage::Format_MonoLSB)));
    ui.actionZoomX->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), zoom_x_bits, QImage::Format_MonoLSB)));
    ui.actionZoomY->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), zoom_y_bits, QImage::Format_MonoLSB)));

    ui.actionAutoScale->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), auto_bits, QImage::Format_MonoLSB)));
    ui.actionAutoScaleX->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), auto_x_bits, QImage::Format_MonoLSB)));
    ui.actionAutoScaleY->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), auto_y_bits, QImage::Format_MonoLSB)));
    ui.actionAutoTick->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), auto_tick_bits, QImage::Format_MonoLSB)));

    ui.actionScrollLeft->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), left_bits, QImage::Format_MonoLSB)));
    ui.actionScrollRight->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), right_bits, QImage::Format_MonoLSB)));
    ui.actionScrollUp->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), up_bits, QImage::Format_MonoLSB)));
    ui.actionScrollDown->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), down_bits, QImage::Format_MonoLSB)));

    ui.actionZoomIn->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), expand_bits, QImage::Format_MonoLSB)));
    ui.actionZoomOut->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), shrink_bits, QImage::Format_MonoLSB)));

    ui.actionAddText->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), atext_bits, QImage::Format_MonoLSB)));

    ui.actionExit->setIcon(QIcon(QBitmap::fromData(QSize(16, 16), exit_bits, QImage::Format_MonoLSB)));
}

void MainWindow::on_actionExit_triggered()
{
  close();
}

void MainWindow::on_actionOpen_triggered()
{
  if (maybeSave()) {
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
      loadFile(fileName);
  }
}

void MainWindow::page_zoom_inout(GraceApp *gapp, int inout)
{
    if (!gui_is_page_free(gapp->gui)) {
        if (inout > 0) {
            gapp->gui->zoom *= ZOOM_STEP;
        } else
        if (inout < 0) {
            gapp->gui->zoom /= ZOOM_STEP;
        } else {
            gapp->gui->zoom = 1.0;
        }
        //xdrawgraph(gapp->gp);
	canvasWidget->qtdrawgraph(gapp->gp);
        //set_left_footer(NULL);
    }
}

void MainWindow::on_actionSmaller_triggered()
{
    page_zoom_inout(this->gapp, -1);
}

void MainWindow::on_actionLarger_triggered()
{
    page_zoom_inout(this->gapp, +1);
}

void MainWindow::on_actionOriginalSize_triggered()
{
    page_zoom_inout(this->gapp, 0);
}

void MainWindow::on_actionRedraw_triggered()
{
    canvasWidget->qtdrawgraph(gapp->gp);
}

void MainWindow::readSettings()
{
  QSettings settings("Grace Project", "Grace");
  QPoint pos = settings.value("pos", QPoint(0, 0)).toPoint();
  QSize size = settings.value("size", QSize(800, 600)).toSize();
  resize(size);
  move(pos);
}

void MainWindow::writeSettings()
{
  QSettings settings("Grace Project", "Grace");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
  //if (textEdit->document()->isModified()) {
  if (false) {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Grace"),
	tr("The document has been modified.\n"
	  "Do you want to save your changes?"),
	QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
      //return save();
      return true;
    else if (ret == QMessageBox::Cancel)
      return false;
  }
  return true;
}

void MainWindow::loadFile(const QString &fileName)
{
/*  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("Grace"),
	tr("Cannot read file %1:\n%2.")
	.arg(fileName)
	.arg(file.errorString()));
    return;
  }*/

    QByteArray bytes = fileName.toAscii();
    char *filename = bytes.data();

    if (load_project(gapp, filename) == RETURN_SUCCESS) {
	QApplication::setOverrideCursor(Qt::WaitCursor);
	canvasWidget->qtdrawgraph(gapp->gp);
	//canvasWidget->update_all();
	QApplication::restoreOverrideCursor();
	setCurrentFile(fileName);
	statusBar()->showMessage(tr("File loaded"), 2000);
    } else {
	statusBar()->showMessage(tr("File failed to load"), 2000);
    }

  //canvasWidget->draw(fileName);

  //QTextStream in(&file);
  //textEdit->setPlainText(in.readAll());

}

void MainWindow::setCurrentFile(const QString &fileName)
{
  curFile = fileName;
  //textEdit->document()->setModified(false);
  setWindowModified(false);

  QString shownName;
  if (curFile.isEmpty())
    shownName = "untitled.txt";
  else
    shownName = strippedName(curFile);

  setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Grace")));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}
