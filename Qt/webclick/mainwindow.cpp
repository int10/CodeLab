#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QNetworkProxyFactory::setUseSystemConfiguration(true);
//	QNetworkProxy proxy;
//	proxy.setType(QNetworkProxy::HttpProxy);
//	proxy.setHostName("127.0.0.1");
//	proxy.setPort(16823);
//	QNetworkProxy::setApplicationProxy(proxy);

//	QNetworkProxy::setApplicationProxy(QNetworkProxy::applicationProxy());

	QUrl url;
	url.setUrl("https://emui.huawei.com/en/plugin.php?id=unlock&mod=detail#_=_");
//	url.setUrl("https://hwid5.vmall.com/casserver/jsp/facebook/authorize.jsp?submit=true&loginUrl=https%3A%2F%2Fhwid5.vmall.com%2Foauth2%2Fportal%2Fcloud_login.jsp&service=http%3A%2F%2Femui.huawei.com%2Fen%2Fplugin.php%3Fid%3Dhwlogin%3Avalidate%26ru%3Dhttp%253A%252F%252Femui.huawei.com%252Fen%252Fplugin.php%253Fid%253Dunlock%2526mod%253Ddetail&loginChannel=22000000&reqClientType=2023&deviceID=&adUrl=http%3A%2F%2Femui.huawei.com%2Fen%2Fsource%2Fplugin%2Fhwlogin%2Flogin.php&InviterUserID=&Inviter=&viewType=0&lang=en-us");
//	url.setUrl("https://emui.huawei.com/en/member.php?mod=logging&action=login");
	connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(SlotWebViewFinished(bool)));
//	connect(ui->webView->page()->mainFrame(), SIGNAL(pageChanged()), this, SLOT(SlotFrameChange()));


	ui->webView->load(url);

	this->showMaximized();

}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::SlotWebViewFinished(bool)
{

	QWebFrame *frame = ui->webView->page()->mainFrame();
	eWebFrameType ft;
	QString title = frame->title();

	if(title == "Reformatting Lock - EMUI")
	{
		//如果页面上有Submit，我就认为现在是在解锁的页面。。
		if(frame->findAllElements("input[value=Submit]").count()){
			ft = E_WEB_FRAME_UNLOCK_DETAIN;
		}else{
			ft = E_WEB_FRAME_LICENSE;
		}
	}
	else if(title == "Login_Huawei ID")
	{
		ft = E_WEB_FRAME_HUAWEI_LOGIN;
	}
	else if(title == "登录 Facebook | Facebook")
	{
		ft = E_WEB_FRAME_FACEBOOK_LOGIN;
	}
	else if(title == "Redirecting...")
	{
		ft = E_WEB_FRAME_REDIRECTING;
	}
	else
	{
		ft = E_WEB_FRAME_UNKNOWN;
	}

	switch(ft)
	{
	case E_WEB_FRAME_UNKNOWN:
		on_btnSave_clicked();
		qDebug()<<"出事了！！！";
		break;
	case E_WEB_FRAME_LICENSE:
		on_btnNext_clicked();
		break;
	case E_WEB_FRAME_HUAWEI_LOGIN:
		on_btnFacebook_clicked();
		break;
	case E_WEB_FRAME_FACEBOOK_LOGIN:
		on_btnInput_clicked();
		on_btnLogin_clicked();
		break;
	case E_WEB_FRAME_UNLOCK_DETAIN:
	{
		on_btnSmartphone_clicked();
		on_btnSubmit_clicked();
		ShowVerifyGif();
		break;
	}
	case E_WEB_FRAME_REDIRECTING:
		break;
	default:
		break;
	}

	qDebug()<<frame->title()<<frame->frameName()<<ft;

}

void MainWindow::ShowVerifyGif()
{
	QWebFrame *frame = ui->webView->page()->mainFrame();
	QWebElement el = frame->findFirstElement("img[id=verify_img1]");

	QImage image(el.geometry().width(), el.geometry().height(), QImage::Format_ARGB32);

	QPainter painter(&image);
	el.render(&painter);

	QPixmap pixmapToShow = QPixmap::fromImage( image.scaled(image.size()*2, Qt::KeepAspectRatio) );
	ui->lbVerGif->setPixmap(pixmapToShow);
	painter.end();

}


void MainWindow::on_btnSubmit_clicked()
{
	QWebFrame *frame = ui->webView->page()->mainFrame();

	//设置sn。。
	frame->evaluateJavaScript("JQ('#sn').val('QLXBBBA5A2106604');");
	//设置imei。。
	frame->evaluateJavaScript("JQ('#imei').val('A000005906A99C');");
	//设置id。。
	frame->evaluateJavaScript("JQ('#checkPassword').val('72033410');");

	//调用submit的脚本。。
//	frame->evaluateJavaScript("javascript:applyUnlock();");

}

void MainWindow::on_btnSave_clicked()
{
	QWebFrame *frame = ui->webView->page()->mainFrame();
	qDebug()<<frame->toHtml();
}



void MainWindow::on_btnGetKey_clicked()
{
	QWebFrame *frame = ui->webView->page()->mainFrame();


//	<input class="ulf-txt ulf-vc" name="veri_field" id="veri_field" autocomplete="off">
	QString veri = ui->leVerCode->text();

	frame->evaluateJavaScript("JQ('#veri_field').val('" + veri + "');");


	//调用submit的脚本。。
	frame->evaluateJavaScript("javascript:applyUnlock();");

	QWebElement key = frame->findFirstElement("div[id=]checkMsg");

	qDebug()<<key.toPlainText();
	qDebug()<<key.toInnerXml();
	qDebug()<<key.toOuterXml();
}

void MainWindow::on_btnNext_clicked()
{

	QWebFrame *frame = ui->webView->page()->mainFrame();

	QWebElement checkbox, next;
	checkbox = frame->findFirstElement("input[id=ireaded]");
	next = frame->findFirstElement("input[id=next]");

	checkbox.setAttribute("checked", "true");
	next.removeAttribute("disabled");
	next.evaluateJavaScript("this.click()");
}

void MainWindow::on_btnInput_clicked()
{
	QWebFrame *frame = ui->webView->page()->mainFrame();

	QWebElement account, password;


	frame->evaluateJavaScript("document.getElementById('email').value = 'movaxbx@163.com';");
	frame->evaluateJavaScript("document.getElementById('pass').value = 'zeng5571559';");

//	frame->evaluateJavaScript("JQ('#pass').val('zeng5571559');");
//	frame->evaluateJavaScript("JQ('#email').val('movaxbx@163.com');");


//	password = frame->findFirstElement("input[id=pass]");
//	account = frame->findFirstElement("input[id=email]");

//	account.setAttribute("value", "movaxbx@163.com");
//	password.setAttribute("value", "zeng5571559");
}

void MainWindow::on_btnFacebook_clicked()
{
//	<a href="javascript:void(0);" tourl="/facebook/authorize.jsp"></a>
	QWebFrame *frame = ui->webView->page()->mainFrame();
//	QWebElement btn = frame->findFirstElement("a[tourl=/facebook/authorize.jsp]");
	QWebElement btn;

	QWebElementCollection ec = frame->findAllElements("a");
	foreach(QWebElement ele, ec)
	{
		if(ele.attribute("href") == "javascript:void(0);" && ele.attribute("tourl") == "/facebook/authorize.jsp")
		{
			btn = ele;
			break;
		}
	}

	btn.evaluateJavaScript("location.href='https://hwid5.vmall.com/casserver/jsp/facebook/authorize.jsp?submit=true&loginUrl=https%3A%2F%2Fhwid5.vmall.com%2Foauth2%2Fportal%2Fcloud_login.jsp&service=http%3A%2F%2Femui.huawei.com%2Fen%2Fplugin.php%3Fid%3Dhwlogin%3Avalidate%26ru%3Dhttp%253A%252F%252Femui.huawei.com%252Fen%252Fplugin.php%253Fid%253Dunlock%2526mod%253Ddetail&loginChannel=22000000&reqClientType=2023&deviceID=&adUrl=http%3A%2F%2Femui.huawei.com%2Fen%2Fsource%2Fplugin%2Fhwlogin%2Flogin.php&InviterUserID=&Inviter=&viewType=0&lang=en-us'");
}

void MainWindow::on_btnLogin_clicked()
{
	QWebFrame *frame = ui->webView->page()->mainFrame();
	QWebElement btn = frame->findFirstElement("input[id=u_0_2]");

	frame->evaluateJavaScript("document.getElementById('u_0_2').click();");

//	btn.evaluateJavaScript("this.click()");
}

void MainWindow::on_btnSmartphone_clicked()
{

	QWebFrame *frame = ui->webView->page()->mainFrame();
	QWebElement smartphone, model;

	QWebElementCollection ec = frame->findAllElements("select");
	QWebElementCollection childen;

	foreach(QWebElement ele, ec)
	{
		if(ele.attribute("id") == "checkType")
		{
			smartphone = ele;
			break;
		}
	}
//	smartphone.evaluateJavaScript("this.click()");
	smartphone.setAttribute("value", "MOBILE");
	childen = smartphone.findAll("option");
	foreach(QWebElement child, childen)
	{
		child.removeAttribute("selected");
		if(child.attribute("value") == "MOBILE")
		{
			child.setAttribute("selected", "selected");
		}
	}
//	smartphone.evaluateJavaScript("this.click()");
//	smartphone.evaluateJavaScript("this.change()");
	smartphone.evaluateJavaScript("this.onchange()");
//	smartphone.evaluateJavaScript("this.selectProduct()");


	QEventLoop loop;
	QTimer timer;
	timer.singleShot(1000, &loop, SLOT(quit()));
	loop.exec();


	ec = frame->findAllElements("select");
	foreach(QWebElement ele, ec)
	{
		if(ele.attribute("id") == "productName")
		{
			model = ele;
			break;
		}

	}
//	model.evaluateJavaScript("this.click()");
	childen = model.findAll("option");


	foreach(QWebElement child, childen)
	{
		qDebug()<<child.attribute("value");
	}

	foreach(QWebElement child, childen)
	{
		child.removeAttribute("selected");
		if(child.attribute("value") == "TAG-AL00")
		{
			child.setAttribute("selected", "selected");
		}
	}
	model.evaluateJavaScript("this.onchange()");
}



void MainWindow::on_btnRefresh_clicked()
{
	QWebFrame *frame = ui->webView->page()->mainFrame();
	QWebElement el = frame->findFirstElement("img[id=verify_img1]");

	el.evaluateJavaScript("this.onclick()");

	for(;;)
	{
		QEventLoop loop;
		QTimer timer;
		timer.singleShot(100, &loop, SLOT(quit()));
		loop.exec();
		bool end = el.evaluateJavaScript("this.complete").toBool();
		if(end) break;
	}

	ShowVerifyGif();
}


