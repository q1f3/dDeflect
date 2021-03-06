#ifndef SOURCECODEDESCRIPTION_H
#define SOURCECODEDESCRIPTION_H

#include <QObject>
#include <QList>
#include <QString>

class SourceCodeDescription : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
public:
    /**
     * @brief Obsługiwane systemy operacyjne.
     */
    enum class SystemType {
        Linux = 0,
        Windows
    };

    /**
     * @brief Konstruktor.
     * @param parent rodzic.
     */
    explicit SourceCodeDescription(QObject *parent = 0) :
        QObject(parent) {}

    /**
     * @brief Metoda kopiująca zawartość
     * @param new_scd Nowy obiekt
     */
    void copy(SourceCodeDescription *new_scd);

    /**
     * @brief Nazwa metody.
     */
    QString m_name;

    /**
     * @brief Ścieżka do pliku z zawartością metody.
     */
    QString path;

    /**
     * @brief Opis metody.
     */
    QString description;

    /**
     * @brief Nagłówki, których potrzebuje metoda.
     */
    QList<QString> headers;

    /**
     * @brief Identyfikator systemu.
     */
    SystemType sys_type;

    /**
     * @brief Metoda odpowiada za wczytanie obiektu JSON do aktualnej instancji klasy.
     * @param json obiekt JSON.
     * @return True, jeżeli wczytanie się powiodło, False w innych przypadkach.
     */
    bool read(const QJsonObject & json);


    QString name() const;
signals:
    void nameChanged();
public slots:

};

#endif // SOURCECODEDESCRIPTION_H
