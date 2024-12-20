#include <QtWidgets/QApplication>
#include "MainWindow.hpp"


struct Spy : QObject {
    bool eventFilter(QObject* obj, QEvent* ev) override {
        if (ev->type()==QEvent::GraphicsSceneMouseDoubleClick ||
            ev->type()==QEvent::GraphicsSceneContextMenu ||
            ev->type()==QEvent::MouseButtonDblClick) {
            qDebug() << "dbl/context on" << obj << obj->metaObject()->className() << ev->type();
        }
        return false;
    }
};
int main(int argc, char** argv){
    QApplication app(argc, argv);
    Spy spy; qApp->installEventFilter(&spy);

    auto w = new MainWindow();   // ★ 放到堆上
    w->show();
    //MainWindow w; w.show();
    return app.exec();
}
