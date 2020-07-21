#include "mediator.h"
#include "reader.h"

#include <writer.h>
#include <message-queue_impl.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlPropertyMap>
#include <QQmlContext>
#include <QTimer>

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app {argc, argv};

    mq::MessageQueue<mq::MessageType> queue {30000, 10, 90};

    Reader reader {queue, std::clog};
    reader.run();

    mq::Writer writer1 {queue};
    mq::Writer writer2 {queue};

    writer1.run();
    writer2.run();

    // Test graphical application that demonstrates the state of the message queue.
    QQmlPropertyMap ownerData {};
    ownerData.insert("fullnessQueue", QVariant::fromValue(0));
    ownerData.insert("fixedSizeQueue", QVariant::fromValue(0));
    ownerData.setProperty("fixedSizeQueue", QVariant::fromValue(queue.fixedSize()));
    Mediator mediator {queue};
    qmlRegisterUncreatableType<Mediator>("Mediator", 1, 0, "Mediator", "");

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&](){
        const auto fullness {static_cast<double>(queue.percentFullnessQueue())};
        ownerData.setProperty("fullnessQueue", QVariant::fromValue(fullness));
    });
    timer.start(10);

    QQmlApplicationEngine engine {};
    engine.rootContext()->setContextProperty("owner", &ownerData);
    const QUrl url {QStringLiteral("qrc:/main.qml")};
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    engine.rootObjects().first()->setProperty("mediator",
                                              QVariant::fromValue(qobject_cast<QObject *>(&mediator)));;
    return app.exec();
}
