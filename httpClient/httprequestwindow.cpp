#include "httprequestwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QApplication>
#include <QLabel>


HttpRequestWindow* HttpRequestWindow::p_instance = 0;

HttpRequestWindow::HttpRequestWindow(QWidget *parent)
    : QWidget(parent), requestInProgress(false)
{
    QStringList Items = QStringList() << "http://localhost";

    QFile urls("urls.txt");
    if (urls.open(QFile::ReadOnly | QIODevice::Text))
    {
        while (!urls.atEnd()) {
            QByteArray line(urls.readLine());
            Items.append(QString(line).simplified());
        }
        urls.close();
    }

    cbUrl = new QComboBox;
    cbUrl->setEditable(true);
    cbUrl->addItems(Items);

    pbStartPing = new QPushButton("Ping enable");
    pbStartPing->setCheckable(true);
    pbCleanLog = new QPushButton("Clean log");

    teResult = new QTextEdit;
    teLog = new QTextEdit;
    dsbPingInterval = new QDoubleSpinBox;
    dsbPingInterval->setDecimals(0);
    dsbPingInterval->setMaximum(10000.0);
    dsbPingInterval->setValue(1000.0);

    cb_method = new QComboBox;
    cb_method->addItems(QStringList() << "GET" << "POST");
    cb_method->setCurrentIndex(1);

    logPath = new QLineEdit;
    logPath->setText("/var/log/syslog");

    logLength = new QDoubleSpinBox;
    logLength->setDecimals(0);
    logLength->setMaximum(1000);
    logLength->setMinimum(0);
    logLength->setValue(4.0);

    pbSendRequest = new QPushButton("Send");

    splitter = new QSplitter(Qt::Vertical);
    splitter->addWidget(teResult);
    splitter->addWidget(teLog);

    QGridLayout *grdLayout = new QGridLayout;
    grdLayout->addWidget(new QLabel("Ping interval,msec:"), 0,0);
    grdLayout->addWidget(dsbPingInterval, 0,1);
    grdLayout->addWidget(new QLabel("Method:"), 1,0);
    grdLayout->addWidget(cb_method, 1,1);
    grdLayout->addWidget(new QLabel("Log path:"), 2,0);
    grdLayout->addWidget(logPath, 2,1);
    grdLayout->addWidget(new QLabel("Log length:"), 3,0);
    grdLayout->addWidget(logLength, 3,1);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(cbUrl);
    mainLayout->addLayout(grdLayout);
    mainLayout->addWidget(pbStartPing);
    mainLayout->addWidget(pbSendRequest);
    mainLayout->addWidget(splitter);
    mainLayout->addWidget(pbCleanLog);

    this->setLayout(mainLayout);

    nam = new QNetworkAccessManager(this);
    timer = new QTimer;

    connect(timer, &QTimer::timeout, this, &HttpRequestWindow::slotTimerTimeout);
    connect(pbCleanLog, &QPushButton::clicked, teLog, &QTextEdit::clear);
    connect(pbSendRequest, &QPushButton::clicked, this, &HttpRequestWindow::newRequest);
    connect(pbStartPing, &QPushButton::clicked, this, &HttpRequestWindow::slotStartPing);
    connect(nam, &QNetworkAccessManager::finished, this, &HttpRequestWindow::slotReplyFinished);

    connect(dsbPingInterval,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [=](double new_val){
        //qDebug() << "value changed" << new_val;
        timer->setInterval(new_val);
    });
}

HttpRequestWindow::~HttpRequestWindow()
{
}

void HttpRequestWindow::slotStartPing(bool start)
{
    if (start)
        timer->start(dsbPingInterval->value());
    else
        timer->stop();
}

void HttpRequestWindow::slotReplyFinished(QNetworkReply *reply)
{
    requestInProgress = false;

    if (reply->error()){
        teResult->setText(reply->errorString());
        qDebug() << "reply finished with" << reply->errorString();
    }
    else{
        QByteArray reply_data = reply->readAll();
        qDebug() << "reply finished";
        teResult->setText(reply_data);
    }
    reply->deleteLater();
}

void HttpRequestWindow::slotDownloadInProgress(qint64 bytesReceived, qint64 bytesTotal)
{
}

void HttpRequestWindow::newRequest()
{
    qDebug() << "new request";
    QNetworkReply* networkReply = NULL;
    if (cb_method->currentText() == "POST")
    {

        QString xmldata = "<?xml version=\"1.0\"?>"
                          "<request_data>"
                          "<path>%1</path>"
                          "<num_lines>%2</num_lines>"
                          "</request_data>";
        xmldata = xmldata.arg(logPath->text(), QString::number(logLength->value()));

        QNetworkRequest request(QUrl(cbUrl->currentText()));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        networkReply = nam->post(request, xmldata.toLocal8Bit());
    }
    else {
        nam->get(QNetworkRequest(QUrl(cbUrl->currentText())));
    }

    if (!networkReply)
        return;
    requestInProgress = true;

    connect(networkReply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
          [=](QNetworkReply::NetworkError code){
        qDebug() << "network error code:" << code;
    });
}

void HttpRequestWindow::slotTimerTimeout()
{
    if (!requestInProgress)
        newRequest();
}

void HttpRequestWindow::logMessage(QString mes)
{
    teLog->append(mes);
}

HttpRequestWindow *HttpRequestWindow::getInsatance()
{
    if (!p_instance)
        p_instance = new HttpRequestWindow;
    return p_instance;
}

