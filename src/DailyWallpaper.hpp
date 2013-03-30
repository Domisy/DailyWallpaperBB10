// Default empty project template
#ifndef DailyWallpaper_HPP_
#define DailyWallpaper_HPP_
#include "WebImageView.h"

#include <QObject>
#include <QFile>
#include <bb/platform/HomeScreen>
#include <bb/cascades/ActivityIndicator>
#include <bb/cascades/ListView>


using namespace bb::cascades;

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class DailyWallpaper : public QObject
{
    Q_OBJECT
public:
    DailyWallpaper(bb::cascades::Application *app);
    Q_INVOKABLE void initiateRequest();
    Q_INVOKABLE void setImageWallpaper(QString imageUrl);
    Q_INVOKABLE void showImageInfoToast();
    QString imageUrl;
    virtual ~DailyWallpaper() {}

private slots:
    void requestFinished(QNetworkReply* reply);
    void imageRequestFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager *mNetworkAccessManager;
    void searchWebpage(QString stringdata);
    void grabImageInfo(QString raw);
    QFile *mFile;
    QString imageInfo;
    ActivityIndicator *activity;
    WebImageView *bingImage;
};


#endif /* DailyWallpaper_HPP_ */
