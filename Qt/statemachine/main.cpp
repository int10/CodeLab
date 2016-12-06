#include <QtGui/QApplication>
#include "mainwindow.h"



#include <QApplication>
#include <QStateMachine>
#include <QPushButton>
#include <QSignalTransition>
#include <QPropertyAnimation>

int main(int argc,char **argv)
{
	QApplication app(argc,argv);
	QWidget *w=new QWidget;
	w->resize(240,320);
	QPushButton *button = new QPushButton("hicjiajia",w);
	QStateMachine *machine = new QStateMachine;     //新建状态机
	QState *state1 = new QState(machine);     //状态1
	state1->assignProperty(button,"geometry",QRect(0,0,80,30));    //绑定button的geomertry属性
	machine->setInitialState(state1);     //state1设为初始化状态
	QState *state2 = new QState(machine);
	state2->assignProperty(button,"geometry",QRect(w->width()-30,w->height()-80,30,80));
	QFont font = QFont("Airl", 12);
	state2->assignProperty(button,"font",font);     //状态2字体


	QPropertyAnimation *ani=new QPropertyAnimation(button,"geometry");
	ani->setDuration(2000);
	ani->setEasingCurve(QEasingCurve::OutBounce);   //动画效果—弹跳

	QSignalTransition *transition1=state1->addTransition(button,SIGNAL(clicked()),state2);      //动画触发信号
	transition1->addAnimation(ani);
	QSignalTransition *transition2=state2->addTransition(button,SIGNAL(clicked()),state1);
	transition2->addAnimation(ani);
	machine->start();     //开启状态机

	w->show();
	return app.exec();
}
