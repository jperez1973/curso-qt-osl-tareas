#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "dbconnection.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onAddTarea();
    void onAddCategoria();
    void onAddEtiqueta();
    void onTareasCellChanged(int row, int column);
    void onCategoriasCellChanged(int row, int column);
    void onEtiquetasCellChanged(int row, int column);
    void onLoadTareas();

private:
    Ui::MainWindow *ui;
    QSqlDatabase db_;
    void cargarTareas(void);
};

#endif // MAINWINDOW_H
