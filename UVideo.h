#ifndef UVIDEO_H
#define UVIDEO_H

#include <urbi/uobject.hh>

class DataManager;

class UVideo :  public QObject, public urbi::UObject
{
    Q_OBJECT

public:
    UVideo(const std::string& s);
    ~UVideo();

    void setDataStorage(DataManager *pdm);
    void subscribeImage(bool b);

private:
    int init();

    // image
    void setImageSrc(urbi::UVar& image);
    void onImage(urbi::UVar& img);

private:
    // A reference to Data Storage
    DataManager *pDataStorage;

    urbi::UVar *imageSrc;

public:
    static UVideo *gpUVideo;
};

//extern UVideo *gpUVideo;

#endif // #ifndef UVIDEO_H