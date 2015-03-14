#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Setup database
    ConecToDb(db_, "tareas");

    db_.exec("CREATE TABLE IF NOT EXISTS tareas ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
              "name TEXT,"
              "descripcion TEXT,"
              "date TEXT,"
              "done INTEGER,"
              "id_categ INTEGER"
              ");");

    db_.exec("CREATE TABLE IF NOT EXISTS categorias ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
              "name TEXT,"
              "descripcion TEXT"
              ");");

    db_.exec("CREATE TABLE IF NOT EXISTS etiquetas ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
              "name TEXT"
              ");");

    db_.exec("CREATE TABLE IF NOT EXISTS tareas_etiq ("
              "id_tarea INTEGER,"
              "id_etiq INTEGER"
              ");");

    connect(ui->actionNuevaTarea, SIGNAL(triggered()), this, SLOT(onAddTarea()), Qt::QueuedConnection);
    connect(ui->tblTareas, SIGNAL(cellChanged(int,int)), this, SLOT(onTareasCellChanged(int,int)), Qt::QueuedConnection);
    connect(ui->comboCategoria, SIGNAL(currentIndexChanged(int)), this, SLOT(onLoadTareas()), Qt::QueuedConnection);
    connect(ui->actionNuevaCateg, SIGNAL(triggered()), this, SLOT(onAddCategoria()), Qt::QueuedConnection);
    connect(ui->tblCateg, SIGNAL(cellChanged(int,int)), this, SLOT(onCategoriasCellChanged(int,int)), Qt::QueuedConnection);
    connect(ui->actionNuevaEtiq, SIGNAL(triggered()), this, SLOT(onAddEtiqueta()), Qt::QueuedConnection);
    connect(ui->tblEtiq, SIGNAL(cellChanged(int,int)), this, SLOT(onEtiquetasCellChanged(int,int)), Qt::QueuedConnection);

    //Obtenemos las categorias
    QSqlQuery q = db_.exec("SELECT * FROM categorias;");
    ui->comboCategoria->addItem("Todas", 0);
    while (q.next()) {
        //Añadimos la categoria al combo y como userData su ID
        int id = GetField(q,"id").toInt();
        ui->comboCategoria->addItem(GetField(q,"name").toString(), id);

        //Añadimos la categoria a la tabla de categorias
        int rowNumber = ui->tblCateg->rowCount();
        ui->tblCateg->insertRow(rowNumber);
        QTableWidgetItem* item = new QTableWidgetItem(GetField(q, "name").toString());
        item->setData(Qt::UserRole, id);
        ui->tblCateg->setItem(rowNumber, 0, item);
    }
    //Activamos el sorting en la tabla de categorias
    ui->tblCateg->setSortingEnabled(true);
    ui->comboCategoria->setCurrentIndex(0);

    //Obtenemos las etiquetas
    q = db_.exec("SELECT * FROM etiquetas;");
    ui->comboEtiqueta->addItem("Todas", 0);
    while (q.next()) {
        //Añadimos la etiqueta al combo y como userData su ID
        int id = GetField(q,"id").toInt();
        ui->comboEtiqueta->addItem(GetField(q,"name").toString(), id);

        //Añadimos la categoria a la tabla de categorias
        int rowNumber = ui->tblEtiq->rowCount();
        ui->tblEtiq->insertRow(rowNumber);
        QTableWidgetItem* item = new QTableWidgetItem(GetField(q, "name").toString());
        item->setData(Qt::UserRole, id);
        ui->tblEtiq->setItem(rowNumber, 0, item);
    }
    //Activamos el sorting en la tabla de etiquetas
    ui->tblEtiq->setSortingEnabled(true);
    ui->comboEtiqueta->setCurrentIndex(0);

    cargarTareas();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddTarea()
{

    ui->tblTareas->insertRow(ui->tblTareas->rowCount());
    QTableWidgetItem* item = new QTableWidgetItem("");
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item->setCheckState(Qt::Unchecked);
    ui->tblTareas->setItem(ui->tblTareas->rowCount()-1, 2, item);

    ui->tblTareas->setItem(ui->tblTareas->rowCount()-1, 0, new QTableWidgetItem(""));
    ui->tblTareas->setItem(ui->tblTareas->rowCount()-1, 1, new QTableWidgetItem(""));

}

void MainWindow::onAddCategoria()
{

    ui->tblCateg->insertRow(ui->tblCateg->rowCount());
    ui->tblCateg->setItem(ui->tblCateg->rowCount()-1, 0, new QTableWidgetItem(""));

}

void MainWindow::onAddEtiqueta()
{

    ui->tblEtiq->insertRow(ui->tblEtiq->rowCount());
    ui->tblEtiq->setItem(ui->tblEtiq->rowCount()-1, 0, new QTableWidgetItem(""));

}

void MainWindow::onTareasCellChanged(int row, int column)
{
    int checked = (ui->tblTareas->item(row, 2)->checkState() == Qt::Checked);

    QSqlQuery query;

    if (ui->tblTareas->item(row, 0)->data(Qt::UserRole).isNull()) {
        query = db_.exec("INSERT INTO tareas (name, descripcion, date, done, id_categ) "
                 "VALUES ("+QString("'%1','%2','%3','%4','%5');" )\
                 .arg(ui->tblTareas->item(row, 0)->text())\
                 .arg(ui->txtTareaDescr->toPlainText())\
                 .arg(ui->tblTareas->item(row, 1)->text())\
                 .arg(checked)\
                 .arg(ui->comboCategoria->currentData().toInt()));
        ui->tblTareas->item(row, 0)->setData(Qt::UserRole, query.lastInsertId());
    } else {
        query = db_.exec("UPDATE tareas "
                 "SET "+QString("name='%1',descripcion='%2',date='%3',done='%4',id_categ='%5' " )\
                 .arg(ui->tblTareas->item(row, 0)->text())\
                 .arg(ui->txtTareaDescr->toPlainText())\
                 .arg(ui->tblTareas->item(row, 1)->text())\
                 .arg(checked)\
                 .arg(ui->comboCategoria->currentData().toInt()) +
                 "WHERE id = " + ui->tblTareas->item(row, 0)->data(Qt::UserRole).toString() + ";");
    }
}

void MainWindow::onCategoriasCellChanged(int row, int column)
{
    qDebug() << "***************";
    QSqlQuery query;
    if (ui->tblCateg->item(row, 0)->data(Qt::UserRole).isNull()) {
        QString name = ui->tblCateg->item(row, 0)->text();
        query = db_.exec("INSERT INTO categorias (name, descripcion) VALUES ("+QString("'%1','%2');").arg(name).
                         arg(ui->txtCategDescr->toPlainText()));
        int lid = query.lastInsertId().toInt();
        ui->tblCateg->item(row, 0)->setData(Qt::UserRole, lid);
        ui->comboCategoria->addItem(name, lid);
    } else {
        query = db_.exec("UPDATE categorias "
                 "SET "+QString("name='%1',descripcion='%2' ")
                 .arg(ui->tblCateg->item(row, 0)->text())
                 .arg(ui->txtCategDescr->toPlainText())+
                 "WHERE id = " + ui->tblCateg->item(row, 0)->data(Qt::UserRole).toString() + ";");
    }
}

void MainWindow::onEtiquetasCellChanged(int row, int column)
{
    QSqlQuery query;
    if (ui->tblEtiq->item(row, 0)->data(Qt::UserRole).isNull()) {
        query = db_.exec("INSERT INTO etiquetas (name) VALUES ("+QString("'%1');").arg(ui->tblEtiq->item(row, 0)->text()));
        ui->tblEtiq->item(row, 0)->setData(Qt::UserRole, query.lastInsertId());
    } else {
        query = db_.exec("UPDATE etiquetas SET "+QString("name='%1' ")
                 .arg(ui->tblEtiq->item(row, 0)->text())+
                 "WHERE id = " + ui->tblEtiq->item(row, 0)->data(Qt::UserRole).toString() + ";");
    }
}

void MainWindow::onLoadTareas()
{

    while (ui->tblTareas->rowCount())
        ui->tblTareas->removeRow(0);

    //Obtenemos las tareas
    QSqlQuery q = db_.exec("SELECT * "
                 "FROM tareas "
                 "WHERE id_categ = " + ui->comboCategoria->currentData().toString());

    while (q.next()) {
        //Añadimos la tarea a la tabla de categorias
        int rowNumber = ui->tblTareas->rowCount();
        int id = GetField(q, "id").toInt();
        ui->tblTareas->insertRow(rowNumber);

        QTableWidgetItem* item = new QTableWidgetItem(GetField(q, "name").toString());
        item->setData(Qt::UserRole, id);
        ui->tblTareas->setItem(rowNumber, 0, item);

        ui->tblTareas->setItem(rowNumber, 1, new QTableWidgetItem(GetField(q, "date").toString()));

        item = new QTableWidgetItem("");
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if (GetField(q, "done").toInt())
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        ui->tblTareas->setItem(rowNumber, 2, item);
    }
    //Activamos el sorting en la tabla de categorias
    ui->tblTareas->setSortingEnabled(true);

}

void MainWindow::cargarTareas(void)
{

    int row = 0;
    QSqlQuery q = db_.exec("SELECT * FROM tareas;");
    while (q.next())
    {
        ui->tblTareas->insertRow(row);

        QTableWidgetItem * name = new QTableWidgetItem(GetField(q, "name").toString());
        name->setData(Qt::UserRole, GetField(q, "id").toInt());

        QTableWidgetItem * done = new QTableWidgetItem("");
        done->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        done->setCheckState(GetField(q, "done").toInt() ? Qt::Checked : Qt::Unchecked);

        ui->tblTareas->setItem(row, 0, name);
        ui->tblTareas->setItem(row, 1, new QTableWidgetItem(GetField(q, "date").toString()));
        ui->tblTareas->setItem(row, 2, done);

        row++;
    }
    ui->tblTareas->setSortingEnabled(true);


}
