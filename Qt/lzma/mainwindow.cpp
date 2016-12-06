#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "lzmalib/LzmaLib.h"
#include <QDebug>
#include <QFile>
#include "lzma.h"
#include "packer.h"

unsigned char SourceBuf[100*1024*1024];
unsigned char CompressedBuf[100*1024*1024*2];
unsigned char UnCompressedBuf[100*1024*1024];
unsigned char outProps[5];
size_t outPropsSize = 5;

QByteArray filebuf;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
#if 0
	size_t destlen;
	QString output;
	size_t srcLen;

	QFile srcFile("aa.txt");
	qDebug()<<"open file "<<srcFile.open(QIODevice::ReadOnly);
	filebuf = srcFile.readAll();
	qint64 size = srcFile.size();
	srcFile.close();
	qDebug()<<"file size = "<<size;
	qDebug()<<"LzmaCompress result = "<<Lzma::Compress(CompressedBuf, &destlen, (const quint8 *)filebuf.data(), (size_t)size);
	qDebug()<<"destlen="<<destlen;

	QFile compressedfile("bb.7z");
	qDebug()<<"open 7z write file"<<compressedfile.open(QIODevice::WriteOnly);
	compressedfile.write((const char *)CompressedBuf,destlen);
	compressedfile.close();

	QFile testfilein("bb.7z");
	testfilein.open(QIODevice::ReadOnly);
	filebuf = testfilein.readAll();
	srcLen = testfilein.size();
	testfilein.close();

	size_t uncompSize;

	QFile testfileout("bb.txt");
	testfileout.open(QIODevice::WriteOnly);

	qDebug()<<"LzmaUncompress result="<<Lzma::UnCompress(UnCompressedBuf, &uncompSize, (const unsigned char *)filebuf.data(), &srcLen);
	qDebug()<<"uncompSize="<<uncompSize;
	testfileout.write((const char *)UnCompressedBuf,uncompSize);

#endif
	init();



}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::init()
{
	Packer pack;
	pack.PackPath("E:/dirTest","E:/setup.exe");
	pack.UnPackFile("E:/output","E:/setup.exe");


}
