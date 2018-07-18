#include "FlawPicker.h"
#include <QtWidgets/QApplication>
#pragma comment(lib,"Shell32.lib")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Q_INIT_RESOURCE(FlawPicker);
	FlawPicker w;
	w.show();
	
	return a.exec();
}
