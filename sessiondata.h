#ifndef SESSIONDATA_H
#define SESSIONDATA_H

    #include <QVector>
    //added x QJson
    #include <QJsonObject>
    #include <QJsonArray>
    #include <QJsonDocument>
    #include <QFile>

    enum class TipoRegistro
    {
        SP,
        NC,
        NL,
        L
    };

    struct Configuracion
    {
        double recorridoTotal = 0.0;
        double intervalo = 0.0;
        TipoRegistro tipoRegistro = TipoRegistro::SP;
        int pulsosEncoder = 0;
        double longitudArco = 0.0;
    };

    struct Estado
    {
        double recorridoActual = 0.0;
        int filaActual = 0;
        bool ensayoIniciado = false;
        bool ensayoPausado = false;
        bool motorActivo = false;
        //long encoderActual = 0;

    };

    struct FilaMedicion
    {
        double posicion = 0;
        double corriente = 0;
        double sp = 0;
        double vnc = 0;
        double vnl = 0;
        double rnc = 0;
        double rnl = 0;

    };

    struct SessionData
    {
        int version = 1;
        Configuracion config;
        Estado estado;
        QVector<FilaMedicion> tabla;

        //add:
        QJsonObject toJson() const;
        //bool fromJson(const QJsonObject&);
        static SessionData fromJson(const QJsonObject &root);
        void clear();
        bool isEmpty() const;
        //

    };
#endif // SESSIONDATA_H
