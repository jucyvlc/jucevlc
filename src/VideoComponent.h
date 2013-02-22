
#ifndef VIDEO_COMPONENT
#define VIDEO_COMPONENT

#include "juce.h"
#include "VLCWrapper.h"
#include "ControlComponent.h"
#include "MenuTree.h"
#include "AppProportionnalComponent.h"
#include <modules\vf_concurrent\vf_concurrent.h>
#include <sstream>
#include "LookNFeel.h"


#define BUFFER_DISPLAY
#undef BUFFER_DISPLAY

class VideoComponent   : public juce::Component , public juce::KeyListener, 
	
#ifdef BUFFER_DISPLAY
	DisplayCallback, 
#else
	juce::ComponentListener,
#endif
	juce::Slider::Listener, juce::Button::Listener, EventCallBack, InputCallBack, MouseInputCallBack
{
#ifdef BUFFER_DISPLAY
	juce::ScopedPointer<juce::Image> img;
	juce::ScopedPointer<juce::Image::BitmapData> ptr;
#else
    juce::ScopedPointer<juce::Component> vlcNativePopupComponent;
#endif
    juce::ScopedPointer<ControlComponent> controlComponent;
    juce::ScopedPointer<MenuTree> tree;
    juce::CriticalSection imgCriticalSection;
	juce::ScopedPointer<VLCWrapper> vlc;
	bool sliderUpdating;
	bool videoUpdating;
    juce::ScopedPointer<juce::Drawable> itemImage;
    juce::ScopedPointer<juce::Drawable> folderImage;
    juce::ScopedPointer<juce::Drawable> folderShortcutImage;
    juce::ScopedPointer<juce::Drawable> audioImage;
    juce::ScopedPointer<juce::Drawable> displayImage;
    juce::ScopedPointer<juce::Drawable> subtitlesImage;
    juce::ScopedPointer<juce::Drawable> exitImage;
	LnF lnf;
	juce::ComponentDragger dragger;
    juce::ScopedPointer<juce::ResizableBorderComponent> resizableBorder;
    juce::ComponentBoundsConstrainer defaultConstrainer;
	bool browsingFiles;
	bool mousehookset;
	juce::int64 lastMouseMoveMovieTime;
	juce::PropertiesFile m_settings;
	juce::StringArray m_shortcuts;
    juce::ScopedPointer<vf::GuiCallQueue> invokeLater;

public:
    VideoComponent();
    virtual ~VideoComponent();
	

	void play(char* path);
	void play();
	void pause();
	void stop();
	bool isFullScreen() const;
	void setFullScreen(bool fs = true);
	void switchFullScreen();
	void switchPlayPause();
	void showVolumeSlider();
	void showPlaybackSpeedSlider();
	void showZoomSlider();

	juce::Drawable const* getItemImage() const { return itemImage; };
	juce::Drawable const* getFolderImage() const { return folderImage; };
	juce::Drawable const* getFolderShortcutImage() const { return folderShortcutImage; };
	juce::Drawable const* getAudioImage() const { return audioImage; };
	juce::Drawable const* getDisplayImage() const { return displayImage; };
	juce::Drawable const* getSubtitlesImage() const { return subtitlesImage; };
	juce::Drawable const* getExitImage() const { return exitImage; };
	
#ifdef BUFFER_DISPLAY
	//VLC DiaplListener
	void *vlcLock(void **p_pixels);
	void vlcUnlock(void *id, void *const *p_pixels);
	void vlcDisplay(void *id);
#else
    void componentMovedOrResized(Component& component,bool wasMoved, bool wasResized);
    void componentVisibilityChanged(Component& component);
#endif

	/////////////// MenuTree
	void onMenuListFiles(MenuTreeItem& item, AbstractFileAction* fileMethod);
	void onMenuOpenFiles(MenuTreeItem& item, AbstractFileAction* fileMethod);
	void onMenuListFavorites(MenuTreeItem& item, AbstractFileAction* fileMethod);
	
	void onMenuAddFavorite (MenuTreeItem& item, juce::String path);
	void onMenuRemoveFavorite (MenuTreeItem& item, juce::String path);
	void writeFavorites();
    void onMenuOpen (MenuTreeItem& item, juce::File const& file);
    void onMenuOpenSubtitle (MenuTreeItem& item, juce::File const& file);
    void onMenuOpenPlaylist (MenuTreeItem& item, juce::File const& file);
	
	void onMenuSubtitleSelect(MenuTreeItem& item, int i);
    void onMenuSubtitleMenu (MenuTreeItem& item);
    void onMenuCrop (MenuTreeItem& item, double ratio);
    void onMenuCropSlider (MenuTreeItem& item);
    void onMenuRate (MenuTreeItem& item, double rate);
    void onMenuRateSlider (MenuTreeItem& item);
    void onMenuSetAspectRatio(MenuTreeItem& item, juce::String ratio);
    void onMenuShiftAudio(MenuTreeItem& item, double s);
    void onMenuShiftAudioSlider(MenuTreeItem& item);
    void onMenuShiftSubtitles(MenuTreeItem& item, double s);
    void onMenuShiftSubtitlesSlider(MenuTreeItem& item);
    void onMenuAudioVolume(MenuTreeItem& item, double volume);
    void onMenuAudioVolumeSlider (MenuTreeItem& item);
	
    void onMenuFullscreen(MenuTreeItem& item, bool fs);
	
	void onMenuExit(MenuTreeItem& item);
	void onMenuSoundOptions(MenuTreeItem& item);
	void onMenuRatio(MenuTreeItem& item);
	void onMenuVideoOptions(MenuTreeItem& item);
	void onMenuRoot(MenuTreeItem& item);
	/////////////// VLC EvtListener
	virtual void vlcTimeChanged();
	virtual void vlcPaused();
	virtual void vlcStarted();
	virtual void vlcStopped();
	virtual void vlcPopupCallback(bool show);
	virtual void vlcFullScreenControlCallback();
	virtual void vlcMouseMove(int x, int y, int button);
	virtual void vlcMouseClick(int x, int y, int button);
	
	void startedSynchronous();
	void stoppedSynchronous();
	

	/////////////// GUI CALLBACKS
    void paint (juce::Graphics& g);
	
    virtual void resized();
	void updateTimeAndSlider();

    virtual void sliderValueChanged (juce::Slider* slider);
    virtual void buttonClicked (juce::Button* button);
	void userTriedToCloseWindow();
	bool keyPressed (const juce::KeyPress& key,
								juce::Component* originatingComponent);
    void mouseDown (const juce::MouseEvent& e);
	void mouseDrag (const juce::MouseEvent& e);
    void mouseMove (const juce::MouseEvent& event);
	
	//void minimisationStateChanged (bool isNowMinimised){if(!isNowMinimised)resized();}
    //void broughtToFront(){resized();}
private:
	
	void setBrowsingFiles(bool newBrowsingFiles = true);
	void initFromSettings();

};

#endif //VIDEO_COMPONENT
