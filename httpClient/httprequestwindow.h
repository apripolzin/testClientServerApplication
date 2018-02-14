#ifndef HTTPREQUESTWINDOW_H
#define HTTPREQUESTWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QComboBox>
#include <QProgressBar>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QPointer>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QSplitter>

class HttpRequestWindow : public QWidget
{
    Q_OBJECT
private:
    explicit HttpRequestWindow(QWidget *parent = 0);
    ~HttpRequestWindow();

private slots:
    void slotStartPing(bool start);

private slots:
    void slotReplyFinished(QNetworkReply *reply);
    void newRequest();
    void slotTimerTimeout();


private:
    QPointer<QComboBox> cbUrl;
    QPointer<QPushButton> pbStartPing;
    QPointer<QTextEdit> teResult;
    QPointer<QTextEdit> teLog;
    QPointer<QNetworkReply> reply;
    QPointer<QDoubleSpinBox> dsbPingInterval;
    QPointer<QPushButton> pbCleanLog;
    QPointer<QComboBox> cb_method;
    QPointer<QPushButton> pbSendRequest;
    QPointer<QTimer> timer;
    QPointer<QSplitter> splitter;
    QPointer<QNetworkAccessManager> nam;
    QPointer<QLineEdit> logPath;
    QPointer<QDoubleSpinBox> logLength;

private slots:
    void slotDownloadInProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    static HttpRequestWindow *p_instance;

public:
    void logMessage(QString mes);

public:
    static HttpRequestWindow *getInsatance();

private:
    bool requestInProgress;

};

#endif // HTTPREQUESTWINDOW_H
