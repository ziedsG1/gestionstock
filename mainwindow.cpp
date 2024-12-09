#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stock.h"
#include "connection.h"
#include <QMessageBox>
#include <QSqlQueryModel>

#include <QSqlRecord>
#include <QRegularExpression>

#include <QtCharts/QBarSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QLayout>
#include <QBarSet>
#include <QVBoxLayout>


#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QColor>

#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableView>
#include <QHeaderView>
#include <QPageSize>


#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

#include <QTimer>
#include <QSqlQuery>
#include <QSqlRecord>

QSerialPort *serial = nullptr;

MainWindow::~MainWindow()
{
    if (serial->isOpen()) {
        serial->close();
    }
    delete ui;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort(this);
    startStockQuantityMonitoring();
    //seral
    serial = new QSerialPort(this);
    serial->setPortName("COM3");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened successfully!";
    } else {
        qDebug() << "Failed to open serial port!";

    //    connect(ui->spinBoxQuantity, SIGNAL(valueChanged(int)), this, SLOT(onQuantityChanged(int)));

    }

    // Establishing database connection
    Connection conn;
    if (conn.createconnect()) {
        QMessageBox::information(this, "Connection", "Database connected successfully.");
    } else {
        QMessageBox::critical(this, "Connection", "Database connection failed.");
    }

    connect(ui->afffstat_2, &QPushButton::clicked, this, &MainWindow::on_afffstat_2_clicked);

}


//PDF

void MainWindow::on_pdfB_R_clicked() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save PDF", "", "PDF Files (*.pdf)");
    if (filePath.isEmpty()) {
        return;  // User canceled the save dialog
    }

    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize::A4); // A4 page size
    writer.setResolution(300);
    writer.setPageMargins(QMargins(10, 10, 10, 10));  // Set page margins

    QPainter painter(&writer);
    if (!painter.isActive()) {
        QMessageBox::critical(this, "Error", "Failed to open PDF for writing.");
        return;
    }

    int yOffset = 60;  // Increased Y offset to provide more space between the headline and the table

    QFont headlineFont = painter.font();
    headlineFont.setBold(true);
    headlineFont.setPointSize(18);  // Set font size for the headline
    painter.setFont(headlineFont);
    painter.setPen(QColor(0, 0, 255));  // Set the text color to blue (RGB value for blue)

    int headlineWidth = painter.fontMetrics().horizontalAdvance("Stock Management Reports");
    int headlineX = (writer.width() - headlineWidth) / 2;  // Center it on the page
    painter.drawText(headlineX, yOffset, "Stock Management Reports");
    yOffset += 60;  // Increase Y offset after the headline

    QFont titleFont = painter.font();
    titleFont.setBold(true);
    titleFont.setPointSize(16);  // Set font size for the title
    painter.setFont(titleFont);
    painter.setPen(QColor(0, 0, 0));  // Set the text color to black for the title
    painter.drawText(50, yOffset, "II) Stock Items Table");
    yOffset += 50;  // Increase Y offset after the title

    QTableView *tableView = ui->affichage;  // Assuming 'affichage' is the QTableView containing the data
    QAbstractItemModel *model = tableView->model();  // Get the model for the table

    if (!model) {
        QMessageBox::critical(this, "Error", "No data to export.");
        return;
    }

    QFont tableFont = painter.font();
    tableFont.setPointSize(10);  // Set font size for the table data
    painter.setFont(tableFont);
    painter.setPen(QColor(0, 0, 0));  // Set the text color to black for the table

    int columnCount = model->columnCount();
    QVector<int> columnWidths(columnCount, 100);  // Default column width for each column
    for (int column = 0; column < columnCount; ++column) {
        int maxWidth = 0;
        for (int row = 0; row < model->rowCount(); ++row) {
            QString cellData = model->data(model->index(row, column)).toString();
            int cellWidth = painter.fontMetrics().horizontalAdvance(cellData);
            maxWidth = qMax(maxWidth, cellWidth);
        }
        columnWidths[column] = maxWidth + 20;  // Add some padding for better readability
    }

    int xPos = 50;
    int headerHeight = 25;  // Increased header height
    for (int column = 0; column < columnCount; ++column) {
        QString header = model->headerData(column, Qt::Horizontal).toString();
        painter.drawText(xPos, yOffset, header);
        xPos += columnWidths[column];  // Move to the next column position
    }
    yOffset += headerHeight;  // Move down after drawing headers

    int rowHeight = 30;  // Increased row height to fit content better
    bool alternateRow = false;  // To alternate row colors

    for (int row = 0; row < model->rowCount(); ++row) {
        xPos = 50;  // Reset xPos for each row

        if (alternateRow) {
            painter.fillRect(50, yOffset - rowHeight, writer.width() - 20, rowHeight, QColor(230, 230, 230));  // Light grey
        }

        for (int column = 0; column < columnCount; ++column) {
            QString cellData = model->data(model->index(row, column)).toString();

            if (column == 0) {  // Example: First column is left-aligned
                painter.drawText(xPos, yOffset, cellData);
            } else if (column == 1 || column == 2) {  // Example: Middle columns are center-aligned
                painter.drawText(xPos + (columnWidths[column] / 2) - (painter.fontMetrics().horizontalAdvance(cellData) / 2), yOffset, cellData);
            } else {  // Example: Other columns are right-aligned
                painter.drawText(xPos + columnWidths[column] - painter.fontMetrics().horizontalAdvance(cellData), yOffset, cellData);
            }

            xPos += columnWidths[column];  // Move to the next column position
        }

        yOffset += rowHeight;  // Move down for the next row

        alternateRow = !alternateRow;

        if (yOffset > writer.height() - 40) {
            painter.end();
            writer.newPage();
            painter.begin(&writer);
            yOffset = 40;  // Reset the Y offset for the new page

            painter.setFont(headlineFont);
            painter.setPen(QColor(0, 0, 255));
            painter.drawText(headlineX, yOffset, "Stock Management Reports");
            yOffset += 60;
            painter.setFont(titleFont);
            painter.setPen(QColor(0, 0, 0));
            painter.drawText(50, yOffset, "II) Stock Items Table");
            yOffset += 50;

            // Redraw the headers
            xPos = 50;
            for (int column = 0; column < columnCount; ++column) {
                QString header = model->headerData(column, Qt::Horizontal).toString();
                painter.drawText(xPos, yOffset, header);
                xPos += columnWidths[column];
            }
            yOffset += headerHeight;
        }
    }

    painter.setPen(QPen(QColor(0, 0, 0), 2));  // Set border width to 2 for thicker lines
    xPos = 50;
    int totalWidth = 0;
    for (int column = 0; column < columnCount; ++column) {
        totalWidth += columnWidths[column];
    }

    // Draw the outer border of the table
    painter.drawRect(50, 40, totalWidth, yOffset - 40);  // Outer rectangle around the table

    // Draw inner borders between columns
    xPos = 50;
    for (int column = 0; column < columnCount; ++column) {
        painter.drawLine(xPos, 40, xPos, yOffset);  // Vertical lines between columns
        xPos += columnWidths[column];
    }

    // Draw horizontal lines between rows
    int rowTop = 40 + headerHeight;
    for (int row = 0; row < model->rowCount(); ++row) {
        painter.drawLine(50, rowTop, 50 + totalWidth, rowTop);  // Horizontal line for each row
        rowTop += rowHeight;
    }

    // 13. Finish the PDF writing process
    painter.end();

    // 14. Notify the user that the PDF has been saved successfully
    QMessageBox::information(this, "PDF Export", "Table data has been successfully exported to PDF!");
}


void MainWindow::on_pdfB_clicked() {
    // 1. Choose the save location and filename for the PDF
    QString filePath = QFileDialog::getSaveFileName(this, "Save PDF", "", "PDF Files (*.pdf)");
    if (filePath.isEmpty()) {
        return;  // User canceled the save dialog
    }

    // 2. Create a PDF writer object
    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize::A4); // A4 page size
    writer.setResolution(300);
    writer.setPageMargins(QMargins(10, 10, 10, 10));  // Set page margins

    // 3. Create a QPainter to draw the PDF content
    QPainter painter(&writer);
    if (!painter.isActive()) {
        QMessageBox::critical(this, "Error", "Failed to open PDF for writing.");
        return;
    }

    // 4. Prepare a Y offset for page layout
    int yOffset = 20;  // Starting Y position for the first content block

    // 5. Draw the headline "Stock Management Reports" in blue, centered at the top of the page
    QFont headlineFont = painter.font();
    headlineFont.setBold(true);
    headlineFont.setPointSize(18);  // Set font size for the headline
    painter.setFont(headlineFont);
    painter.setPen(QColor(0, 0, 255));  // Set the text color to blue (RGB value for blue)

    // Calculate the X position to center the headline
    int headlineWidth = painter.fontMetrics().horizontalAdvance("Stock Management Reports");
    int headlineX = (writer.width() - headlineWidth) / 2;  // Center it on the page
    painter.drawText(headlineX, yOffset, "Stock Management Reports");
    yOffset += 60;  // Move the Y offset down more after the headline

    // 6. Draw the title text "I) Statics based on ETAT" below the headline
    QFont titleFont = painter.font();
    titleFont.setBold(true);
    titleFont.setPointSize(16);  // Set font size for the title
    painter.setFont(titleFont);
    painter.setPen(QColor(0, 0, 0));  // Set the text color to black for the title
    painter.drawText(50, yOffset, "I) Statics based on ETAT");
    yOffset += 40;  // Move the Y offset down more after the title

    // 7. Capture the content of the QGraphicsView (grafetat) into a QPixmap
    QGraphicsView *grafetatView = ui->grafetat;  // Assuming 'grafetat' is the QGraphicsView containing the chart
    QPixmap pixmap(grafetatView->size());  // Create a pixmap with the same size as the view
    QPainter pixmapPainter(&pixmap);  // Create a QPainter to render into the QPixmap
    grafetatView->render(&pixmapPainter);  // Render the content of the QGraphicsView into the pixmap

    // 8. Scale the pixmap to make it larger before drawing it to the PDF
    int scaleFactor = 3;  // Scale factor to make the chart 3x bigger
    QPixmap scaledPixmap = pixmap.scaled(pixmap.width() * scaleFactor, pixmap.height() * scaleFactor, Qt::KeepAspectRatio);

    // 9. Draw the scaled pixmap (chart content) onto the PDF
    painter.drawPixmap(50, yOffset, scaledPixmap);  // Draw the pixmap onto the PDF at coordinates (50, yOffset)

    // Move the Y offset down after the chart
    yOffset += scaledPixmap.height() + 10;  // Add some space after the chart

    // 10. End the PDF writing process
    painter.end();

    // 11. Notify the user that the PDF was generated successfully
    QMessageBox::information(this, "Success", "PDF generated successfully!");
}

//
void MainWindow::on_afffstat_2_clicked() {
    qDebug() << "Button clicked, preparing pie chart...";  // Debugging line to confirm button click

    // Prepare the SQL query to count the number of 'good' and 'bad' products by 'etat' (1 for 'good', 0 for 'bad')
    QSqlQuery query;
    query.prepare("SELECT etat, COUNT(*) FROM stock WHERE etat IN (0, 1) GROUP BY etat");

    if (query.exec()) {
        int goodCount = 0;
        int badCount = 0;

        // Process the query result
        while (query.next()) {
            int etat = query.value(0).toInt();  // Get the etat (0 for 'bad' or 1 for 'good')
            int count = query.value(1).toInt(); // Get the count of products in that state

            // Assign counts to good or bad based on the etat value
            if (etat == 1) {  // 1 is "good"
                goodCount = count;
            } else if (etat == 0) {  // 0 is "bad"
                badCount = count;
            }
        }

        // If there are no products in either category, handle that gracefully
        if (goodCount == 0 && badCount == 0) {
            QMessageBox::information(this, "No Data", "No 'good' or 'bad' products found.");
            return;
        }

        // Create a pie chart series
        QPieSeries *series = new QPieSeries();

        // Calculate the total count of products
        int totalCount = goodCount + badCount;

        // Add slices for 'good' and 'bad'
        QPieSlice *goodSlice = series->append("Good", goodCount);
        QPieSlice *badSlice = series->append("Bad", badCount);

        // Set the labels manually to show the count and the percentage
        double goodPercentage = (totalCount > 0) ? (static_cast<double>(goodCount) / totalCount) * 100 : 0;
        double badPercentage = (totalCount > 0) ? (static_cast<double>(badCount) / totalCount) * 100 : 0;

        goodSlice->setLabel(QString("Good: %1 (%2%)").arg(goodCount).arg(goodPercentage, 0, 'f', 2)); // Show count and percentage
        badSlice->setLabel(QString("Bad: %1 (%2%)").arg(badCount).arg(badPercentage, 0, 'f', 2));   // Show count and percentage

        // Set colors for slices
        goodSlice->setBrush(QBrush(Qt::green));   // Green for 'Good'
        badSlice->setBrush(QBrush(Qt::blue));     // Blue for 'Bad'

        // Create the chart and add the pie series to it
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Product Status: Good vs Bad");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        // Create the chart view for displaying the chart
        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        // Find the QGraphicsView named 'grafetat' from your UI
        QGraphicsView *grafetatView = ui->grafetat;  // Assuming grafetat is the name of the QGraphicsView

        // Resize the chart view to fit the size of grafetatView
        chartView->setGeometry(grafetatView->geometry());  // Set the same geometry as grafetatView
        chartView->setMinimumSize(grafetatView->size());   // Ensure the chart view fits inside

        // Create a new QGraphicsScene to hold the chart view
        QGraphicsScene *scene = new QGraphicsScene(this);
        scene->addWidget(chartView);  // Add the chart view to the scene

        // Set the scene to the QGraphicsView (grafetat)
        grafetatView->setScene(scene);
        grafetatView->setRenderHint(QPainter::Antialiasing); // Ensure smooth rendering of the chart

        // Optional: Hide the background grid of the QGraphicsView (grafetat)
        grafetatView->setBackgroundBrush(QBrush(Qt::transparent));
    } else {
        // Handle query failure
        QMessageBox::critical(this, "Query Error", "Failed to fetch statistics: " + query.lastError().text());
    }
}

//servo

void MainWindow::startStockQuantityMonitoring() {
    // Set up a QTimer to check the stock quantity periodically
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::checkStockQuantity);
    timer->start(1000); // Check every 1 second
}

void MainWindow::checkStockQuantity() {
    // Get the current stock quantity from the database (replace with actual query)
    QString id_prod = ui->Idprod->text();
    QSqlQuery query;
    query.prepare("SELECT qte FROM stock WHERE id_prod = :id_prod");
    query.bindValue(":id_prod", id_prod);
    if (query.exec() && query.next()) {
        int newQuantity = query.value(0).toInt();

        if (newQuantity != currentQuantity) {
            currentQuantity = newQuantity;
            moveServoBasedOnQuantity(newQuantity); // Move the servo based on the quantity
        }
    } else {
        qDebug() << "Failed to query stock quantity";
    }
}

void MainWindow::moveServoBasedOnQuantity(int qte) {
    // Send the quantity to Arduino via serial port to control the servo
    if (serial && serial->isOpen()) {
        QByteArray data;
        data.append(QString::number(qte).toUtf8());  // Convert QString to QByteArray before appending
        serial->write(data);
        qDebug() << "Sending quantity to Arduino: " << qte;  // Debug output to ensure the value is being sent
    } else {
        qDebug() << "Serial port not open";
    }
}



//
void MainWindow::on_ModifierB_clicked() {

    QString id_prod = ui->modidprod->text();


    if (id_prod.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un ID de produit valide.");
        return;
    }


    QString field = ui->boxmodif->currentText();

    QString newValue = ui->mod->text();

    if (newValue.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer une nouvelle valeur.");
        return;
    }

    QSqlQuery query;

    QString updateQuery = QString("UPDATE stock SET %1 = :newValue WHERE id_prod = :id_prod").arg(field);

    query.prepare(updateQuery);
    query.bindValue(":newValue", newValue);
    query.bindValue(":id_prod", id_prod);

    if (query.exec()) {
        QMessageBox::information(this, "Succès", "Produit modifié avec succès.");
    } else {
        QMessageBox::critical(this, "Erreur", "La modification a échoué. Veuillez vérifier l'ID et le champ.");
        qDebug() << "Erreur SQL : " << query.lastError().text();
    }
}

void MainWindow::on_ajouterB_clicked() {

    QString id_prod = ui->Idprod->text();
    QString description = ui->des->text();
    int prix = ui->prix->text().toInt();
    int qte = ui->qte->text().toInt();
    QString etatText = ui->etat->text().toLower();
    bool etat = (etatText == "good");

    QDate date_ent = ui->dateEdit_ent->date();
    QDate date_exp = ui->dateEdit_exp->date();


    stock newStock(id_prod, description, prix, qte, etat, date_ent, date_exp);


    if (newStock.ajouter()) {
        QMessageBox::information(this, "Success", "Stock added successfully.");
        // ui->affichage->setModel(newStock.afficher()); // Refresh the view with new data
    } else {
        QMessageBox::critical(this, "Failure", "Failed to add stock.");
    }
}

void MainWindow::on_supprimerB_clicked() {
    QString id_prod = ui->suppidstock->text();

    if (id_prod.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a product ID.");
        return;
    }

    stock stockItem;

    if (stockItem.supprimer(id_prod)) {
        QMessageBox::information(this, "Success", "Stock item deleted successfully.");
        ui->affichage->setModel(stockItem.afficher()); // Refresh the table view
    } else {
        QMessageBox::critical(this, "Failure", "Failed to delete stock item.");
    }
}

void MainWindow::on_rechercheB_clicked() {
    QString id_prod = ui->rechidprod->text();  // Retrieve the product ID from the input field

    QSqlQuery query;

    // If the product ID is empty, fetch all records from the stock table
    if (id_prod.isEmpty()) {
        query.prepare("SELECT * FROM stock");  // Query to fetch all records from the stock table
    } else {
        // Otherwise, search for the specific product based on the provided ID
        id_prod = '%'+id_prod+'%';
        query.prepare("SELECT * FROM stock WHERE id_prod like :id_prod");
        query.bindValue(":id_prod", id_prod);
    }

    // Execute the query
    if (query.exec()) {
        QSqlQueryModel* model = new QSqlQueryModel();
        model->setQuery(query);

        // Check if any records are returned
        if (model->rowCount() > 0) {
            // Set the headers for the table view
            model->setHeaderData(0, Qt::Horizontal, "Product ID");
            model->setHeaderData(1, Qt::Horizontal, "Description");
            model->setHeaderData(2, Qt::Horizontal, "Price");
            model->setHeaderData(3, Qt::Horizontal, "Quantity");
            model->setHeaderData(4, Qt::Horizontal, "State");
            model->setHeaderData(5, Qt::Horizontal, "Entry Date");
            model->setHeaderData(6, Qt::Horizontal, "Expiration Date");

            // Set the model for the table view to display the data
            ui->affichage->setModel(nullptr);  // Reset the current model
            ui->affichage->setModel(model);    // Set the new model with the results
        } else {
            // If no records were found
            QMessageBox::information(this, "Search Result", "No stock found with the given product ID.");
        }
    } else {
        // If the query execution failed
        QMessageBox::critical(this, "Query Error", "Failed to execute the search query: " + query.lastError().text());
    }
}

//tri

void MainWindow::on_trib_clicked() {
    // Get the sorting criteria from the comboBoxTri (either quantité, date_ent, or prix)
    QString sortBy = ui->comboBoxtri->currentText(); // Get selected sorting column from the combo box
    qDebug() << "ComboBox Selection: " << sortBy;

    // Prepare the sorting column for the query
    QString orderBy;
    if (sortBy == "Qte") {
        orderBy = "Qte";  // Sort by 'quantité'
    } else if (sortBy == "Date_ent") {
        orderBy = "Date_ent";  // Sort by 'date_ent'
    } else if (sortBy == "Prix") {
        orderBy = "Prix";  // Sort by 'prix'
    } else {
        // If no valid sort option is selected, show an error
        QMessageBox::critical(this, "Sorting Error", "Invalid sorting criteria selected.");
        return;
    }

    // Prepare the SQL query to fetch all records, sorted by the selected field
    QString queryStr = QString(R"(
        SELECT * FROM stock
        ORDER BY %1 ASC
    )").arg(orderBy);

    QSqlQuery query;
    query.prepare(queryStr);

    // Execute the query to get the records
    if (query.exec()) {
        QSqlQueryModel* model = new QSqlQueryModel();
        model->setQuery(query);

        // Check if any records are returned
        if (model->rowCount() > 0) {
            // Set the headers for the table view
            model->setHeaderData(0, Qt::Horizontal, "Product ID");
            model->setHeaderData(1, Qt::Horizontal, "Description");
            model->setHeaderData(2, Qt::Horizontal, "Prix");
            model->setHeaderData(3, Qt::Horizontal, "Quantité");
            model->setHeaderData(4, Qt::Horizontal, "State");
            model->setHeaderData(5, Qt::Horizontal, "Entry Date");
            model->setHeaderData(6, Qt::Horizontal, "Expiration Date");

            // Set the model to the table view (afftri)
            ui->afftri->setModel(model);  // Assuming afftri is the QTableView
        } else {
            QMessageBox::information(this, "No Results", "No matching records found.");
        }
    } else {
        QMessageBox::critical(this, "Query Error", "Failed to execute the query: " + query.lastError().text());
    }
}
//database
