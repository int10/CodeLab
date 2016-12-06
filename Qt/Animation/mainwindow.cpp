#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPoint>
#include <QPixmap>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	connect(this,SIGNAL(startAnimation()),this,SLOT(downloadAnimation()));
}

MainWindow::~MainWindow()
{
	delete ui;
}


QPoint MainWindow::GetMousePoint()
{
	QPoint WindowPoint =this->pos();
	QPoint point =QCursor::pos();
	QPoint MousePoint;
	MousePoint= point-WindowPoint;
	return MousePoint;
}
void MainWindow::downloadAnimation()
{
	QPixmap icon;
	icon.load(":/images/main/down_animation.png");
	QLabel* label=new QLabel(this);
	label->setPixmap(icon.scaled(32,32,Qt::KeepAspectRatio));
	label->show();

	QPoint first,end;
	first = QPoint(ui->btnDownload->pos().x()+ui->btnDownload->rect().width(),ui->btnDownload->pos().y());
	end = QPoint(ui->btnDownload->pos().x()+100,ui->btnDownload->pos().y()+50);
	QPropertyAnimation *animation = new QPropertyAnimation(label,"geometry");
	animation->setDuration(1000);
	animation->setStartValue(QRect(GetMousePoint().x()-50,GetMousePoint().y()-30,0,0));
	animation->setEndValue(QRect(first,QSize( 50,50)));
	animation->setEasingCurve(QEasingCurve::OutQuart);

	QPropertyAnimation *animation1 = new QPropertyAnimation(label,"geometry");
	animation1->setDuration(300);
	animation1->setStartValue(QRect(first,QSize( 50,50)));
	animation1->setEndValue(QRect(end,QSize(0,0)));
	animation1->setEasingCurve(QEasingCurve::InCirc);

   QSequentialAnimationGroup  *group=new QSequentialAnimationGroup(this);
	group->addAnimation(animation);
	group->addAnimation(animation1);
	group->start();


}

void MainWindow::on_btnStart_clicked()
{
	emit startAnimation();
}
