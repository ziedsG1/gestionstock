#include "stock.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QDebug>



stock::stock(QString Id_prod, QString Des, int Prix, int Qte, bool Etat, QDate Date_ent, QDate Date_exp) {
    this->Id_prod = Id_prod;
    this->Des = Des;
    this->Prix = Prix;
    this->Qte = Qte;
    this->Etat = Etat;
    this->Date_ent = Date_ent;
    this->Date_exp = Date_exp;
}


bool stock::ajouter() {
    QSqlQuery query;
    query.prepare("INSERT INTO STOCK (ID_PROD, DESIGNATION, PRIX, QTE, ETAT, DATE_ENT, DATE_EXP) "
                  "VALUES (:Id_prod, :Des, :Prix, :Qte, :Etat, :Date_ent, :Date_exp)");

    query.bindValue(":Id_prod", Id_prod);
    query.bindValue(":Des", Des);
    query.bindValue(":Prix", Prix);
    query.bindValue(":Qte", Qte);
    query.bindValue(":Etat", Etat);
    query.bindValue(":Date_ent", Date_ent);
    query.bindValue(":Date_exp", Date_exp);

    if (query.exec()) {
        return true;
    } else {
        qDebug() << "Error inserting stock:" << query.lastError().text();
        return false;
    }
}


/*QSqlQueryModel* stock::afficher() {
    QSqlQueryModel *model = new QSqlQueryModel;
    QSqlQuery query;
    query.prepare("SELECT * FROM STOCK");

    if (query.exec()) {
        model->setQuery(query);
    } else {
        qDebug() << "Error fetching stock:" << query.lastError().text();
    }

    return model;
}*/

QSqlQueryModel* stock::afficher() {
    // Create an SQL query to fetch all the stock items
    QSqlQuery query("SELECT * FROM stock");

    // Create a model and set the query to the model
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(query);

    // Optionally set headers for your table
    model->setHeaderData(0, Qt::Horizontal, "Product ID");
    model->setHeaderData(1, Qt::Horizontal, "Description");
    model->setHeaderData(2, Qt::Horizontal, "Price");
    model->setHeaderData(3, Qt::Horizontal, "Quantity");
    model->setHeaderData(4, Qt::Horizontal, "State");
    model->setHeaderData(5, Qt::Horizontal, "Entry Date");
    model->setHeaderData(6, Qt::Horizontal, "Expiration Date");

    return model;
}




bool stock::supprimer(const QString &id_prod) {
    // Create an SQL query to delete the stock item by id_prod
    QSqlQuery query;
    query.prepare("DELETE FROM stock WHERE id_prod = :id_prod");
    query.bindValue(":id_prod", id_prod);

    // Execute the query and check if the deletion was successful
    if (query.exec()) {
        return true; // Successfully deleted
    } else {
        qDebug() << "Failed to delete stock item: " << query.lastError().text();
        return false; // Failed to delete
    }
}




