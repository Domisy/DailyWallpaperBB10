// Default empty project template
#include "DailyWallpaper.hpp"
#include "WebImageView.h"

#include <QObject>
#include <QIODevice>
#include <QDir>
#include <QFile>
#include <QVariant>
#include <bb/data/JsonDataAccess>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/XmlDataModel>
#include <bb/cascades/GroupDataModel>
#include <bb/platform/HomeScreen>
#include <bb/system/SystemToast>
#include <bb/system/SystemDialog>

using namespace bb::data;
using namespace bb::cascades;
using namespace bb::system;

QString imageInfo;
QString bingUrl = "http://www.bing.com/";

DailyWallpaper::DailyWallpaper(bb::cascades::Application *app) :
		QObject(app) {

	qmlRegisterType<WebImageView>("org.labsquare", 1, 0, "WebImageView");
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	// Expose this class to QML so that we can call its functions from there
	qml->setContextProperty("app", this);

	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	mFile = new QFile("data/bingWallpaper.jpg");
	bingImage = root->findChild<WebImageView*>("bingImageComponent");
	activity = root->findChild<ActivityIndicator*>("indicator");
	imageUrl = "";
	mNetworkAccessManager = new QNetworkAccessManager(this);

	bool result = connect(mNetworkAccessManager,
			SIGNAL(finished(QNetworkReply*)), this,
			SLOT(requestFinished(QNetworkReply*)));

	// Displays a warning message if there's an issue connecting the signal
	// and slot. This is a good practice with signals and slots as it can
	// be easier to mistype a slot or signal definition
	Q_ASSERT (result);
	Q_UNUSED(result);

	initiateRequest();
	// set created root object as a scene
	app->setScene(root);
}

void DailyWallpaper::initiateRequest() {
	//bingImage = qobject_cast<WebImageView*>(imageView);
	activity->start();
	QNetworkRequest request = QNetworkRequest();
	request.setUrl(QUrl(bingUrl));
	mNetworkAccessManager->get(request);
}

void DailyWallpaper::requestFinished(QNetworkReply* reply) {
	QString replyRedirect = reply->attribute(
			QNetworkRequest::RedirectionTargetAttribute).toString();
	if (replyRedirect != "") {
		bingUrl = replyRedirect;
		initiateRequest();
	} else {
		if (reply->error() == QNetworkReply::NoError) {
			const QByteArray response(reply->readAll());

			const char* cString = response.constData();
			QString responseString = QString::fromUtf8(cString);
			qDebug() << "Net String" << responseString;
			searchWebpage(responseString);
		} else {
			qDebug() << "\n Problem with the network";
			qDebug() << "\n" << reply->errorString();
		}
	}
	reply->deleteLater();
}

void DailyWallpaper::searchWebpage(QString stringdata) {
	//Grab image URL
	int indexBegin = stringdata.indexOf("g_img={url:'", 0) + 12;
	int indexEnd = stringdata.indexOf(".jpg", indexBegin) + 4;
	imageUrl = "http://www.bing.com"
			+ stringdata.mid(indexBegin, indexEnd - indexBegin);
	//Grab image info
	indexBegin = stringdata.indexOf("class=\"hpcCopyInfo\"", 0) + 41;
	indexEnd = stringdata.indexOf("</span>", indexBegin) - 4;
	QString imageInfoRaw = stringdata.mid(indexBegin, indexEnd - indexBegin);
	grabImageInfo(imageInfoRaw);
	//imageInfo = imageInfoRaw;

	bingImage->setUrl(QUrl(imageUrl));
	activity->stop();
	//infoToast->setProperty("infoString", imageInfo);
}

void DailyWallpaper::grabImageInfo(QString raw) {
	QString decodedInfo = raw;
	int start = 0;
	for (int i = 0; i < raw.count("&#"); i++) { //occurs as many times as there are html entities found in the string.
		start = raw.indexOf("&#", start);
		if (start != -1) {
			int end = raw.indexOf(";", start);
			if (end - start <= 6) {
				QString intAscii = (raw.mid(start + 2, end - start - 2));
				QString stringAscii = QChar(intAscii.toInt());
				decodedInfo = decodedInfo.replace("&#" + intAscii + ";",
						stringAscii);
			}
			start++; //make sure it looks for the next occurrence starting from after the previous one
		}
	}
	imageInfo = decodedInfo;
	//QString stringAscii = QChar(intAscii);
	//QString stringAscii = QString::fromUtf8((const char*)intAscii,-1);
	//QString *stringAscii = new QString(QChar::fromAscii((char) intAscii));
	//stringAscii.fromAscii(intAscii);
	//imageInfo = raw.replace(start, end-start, stringAscii);
}

void DailyWallpaper::setImageWallpaper(QString imageUrl) {
	QNetworkRequest request = QNetworkRequest();
	request.setUrl(QUrl(imageUrl));

	QNetworkAccessManager *networkAccessManagerImage =
			new QNetworkAccessManager(this);
	bool res = connect(networkAccessManagerImage,
			SIGNAL(finished(QNetworkReply*)), this,
			SLOT(imageRequestFinished(QNetworkReply*)));

	Q_ASSERT (res);

	// Indicate that the variable res isn't used in the rest of the app, to prevent
	// a compiler warning
	Q_UNUSED(res);

	// Sends the request
	networkAccessManagerImage->get(request);

}

void DailyWallpaper::imageRequestFinished(QNetworkReply* reply) {

	if (reply->error() == QNetworkReply::NoError) {
		//QString workingDir = QDir::currentPath();
		//QFile imgFile("file://accounts/1000/shared/photos/bingWallpaper.jpg");

		if (!mFile->open(QIODevice::ReadWrite)) {
			qDebug() << "\n Failed to open file";
			return;
		} else {
			mFile->write(reply->readAll());
			mFile->flush();
			mFile->close();
			bb::platform::HomeScreen homeScreen;
			bool result = homeScreen.setWallpaper(
					QUrl("data/bingWallpaper.jpg"));
			SystemToast *wallpaperSuccess = new SystemToast(this);
			if (result) {
				wallpaperSuccess->setBody(
						"Image successfully set as wallpaper!");
				wallpaperSuccess->show();
			} else {
				wallpaperSuccess->setBody(
						"Image could not be set as wallpaper, please try again!");
				wallpaperSuccess->show();
			}
		}

	} else {
		qDebug() << "\n Problem with the network";
		qDebug() << "\n" << reply->errorString();
	}
	reply->deleteLater();
}

void DailyWallpaper::showImageInfoToast() {
	SystemDialog *toast = new SystemDialog("OK", this);
	toast->setBody(imageInfo);
	toast->setTitle("Image Info");
	toast->show();
}

