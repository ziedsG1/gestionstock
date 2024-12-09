#ifndef STOCK_H
#define STOCK_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QDATE>


class stock
{
private:
    QString Id_prod;
    QString Des;
    int Prix;
    int Qte;
    bool Etat;
    QDate Date_ent;
    QDate Date_exp;

public:
    stock() {}
    stock(QString, QString, int, int, bool, QDate, QDate);

    // Getters
    QString getId_prod() { return Id_prod; }
    QString getDes() { return Des; }
    int getPrix() { return Prix; }
    int getQte() { return Qte; }
    bool getEtat() { return Etat; }
    QDate getDate_ent() { return Date_ent; }
    QDate getDate_exp() { return Date_exp; }

    // SetteRS
    void setId_prod(QString Id_prod) { this->Id_prod = Id_prod; }
    void setDes(QString Des) { this->Des = Des; }
    void setPrix(int Prix) { this->Prix = Prix; }
    void setQte(int Qte) { this->Qte = Qte; }
    void setEtat(bool Etat) { this->Etat = Etat; }
    void setDate_ent(QDate Date_ent) { this->Date_ent = Date_ent; }
    void setDate_exp(QDate Date_exp) { this->Date_exp = Date_exp; }


    bool ajouter();
    QSqlQueryModel *afficher();
  //  bool supprimer(QString id_prod);
    bool supprimer(const QString &id_prod);
    bool modifier();


    bool modifierDescription();
    bool modifierPrix();
    bool modifierEtat();
    bool modifierQuantity();
    bool modifierDateEnt();
    bool modifierDateExp();

};


#endif // STOCK_H
