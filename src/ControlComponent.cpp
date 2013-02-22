
#include "ControlComponent.h"
#include "Icons.h"
#include "MenuTree.h"
#include "MenuTreeAction.h"
#include <algorithm>

////////////////////////////////////////////////////////////
//
// 2ND SLIDER
//
////////////////////////////////////////////////////////////
#include <boost/bind/bind.hpp>

void nop(double)
{
}
class ActionSlider   : public juce::Slider, public juce::SliderListener
{
public:
    typedef boost::function<void (double)> Functor;
private:
	Functor m_f;
	juce::String m_format;
public:
	ActionSlider(juce::String const& name="")
		:juce::Slider(name)
		,m_f(boost::bind(&nop, _1))
	{
		setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
		setSliderStyle (juce::Slider::LinearBar);
		setAlpha(1.f);
		setOpaque(true);
		addListener(this);
	}
	virtual ~ActionSlider(){}
	void resetCallback(){m_f = boost::bind(&nop, _1);}
	void setCallback(ActionSliderCallback const& f){m_f = f;}
	void setLabelFormat(juce::String const& f){m_format = f;}
	void paint(juce::Graphics& g)
	{		
		juce::Slider::paint(g);
		
		juce::Font f = g.getCurrentFont().withHeight(getHeight());
		f.setStyleFlags(juce::Font::plain);
		g.setFont(f);
		g.setColour (juce::Colours::black);
		g.drawFittedText (juce::String::formatted(m_format,getValue()),
							0, 0, getWidth(), getHeight(),
							juce::Justification::centred, 
							1, //1 line
							1.f//no h scale
							);
	}
    virtual void sliderValueChanged (juce::Slider* slider)
	{
		m_f(slider->getValue());
	}
	void setup(juce::String const& label, ActionSliderCallback const& f, double value, double volumeMin, double volumeMax, double step)
	{
		setRange(volumeMin, volumeMax, step);
		setValue(value);
		setLabelFormat(label);
		setCallback(f);
	}
};

SecondaryControlComponent::SecondaryControlComponent()
	:m_buttonsStep(0.)
{
	setOpaque(true);
	m_leftImage = juce::Drawable::createFromImageData (left_svg, left_svgSize);
	m_rightImage = juce::Drawable::createFromImageData (right_svg, right_svgSize);

	m_leftButton = new juce::DrawableButton("m_leftButton", juce::DrawableButton::ImageFitted);
	m_leftButton->setOpaque(false);
	m_leftButton->setImages(m_leftImage);
	m_leftButton->addListener(this);

	m_rightButton = new juce::DrawableButton("m_rightButton", juce::DrawableButton::ImageFitted);
	m_rightButton->setOpaque(false);
	m_rightButton->setImages(m_rightImage);
	m_rightButton->addListener(this);

	m_slider = new ActionSlider("AlternateControlComponentSlider");
	addAndMakeVisible(m_slider);

	addChildComponent(m_leftButton);
	addChildComponent(m_rightButton);

	setOpaque(false);
}
SecondaryControlComponent::~SecondaryControlComponent()
{
}
void SecondaryControlComponent::resized()
{
	bool showButtons = m_buttonsStep>0.;
	int buttonSize = showButtons?getHeight():0;

	int leftButtonSize=0;
	if(m_leftButton->isVisible())
	{
		leftButtonSize=buttonSize;
		m_leftButton->setBounds(0, 0, buttonSize, getHeight());
	}
	int rightButtonSize=0;
	if(m_rightButton->isVisible())
	{
		rightButtonSize=buttonSize;
		m_rightButton->setBounds(getWidth()-buttonSize, 0, buttonSize, getHeight());
	}
	m_slider->setBounds(leftButtonSize, 0, getWidth()-leftButtonSize-rightButtonSize, getHeight());
}
	
void SecondaryControlComponent::paint(juce::Graphics& g)
{
}
void SecondaryControlComponent::show(juce::String const& label, ActionSliderCallback const& f, double value, double volumeMin, double volumeMax, double step, double buttonsStep)
{
	m_buttonsStep = buttonsStep;
	bool showButtons = m_buttonsStep>0.;
	m_leftButton->setVisible(showButtons);
	m_rightButton->setVisible(showButtons);
	m_slider->setup(label, f, value, volumeMin, volumeMax, step);
	resized();
	setVisible(true);
}
	
void SecondaryControlComponent::buttonClicked (juce::Button* button)
{
	double value = m_slider->getValue();
	double min = m_slider->getMinimum();
	double max = m_slider->getMaximum();
	double step = m_slider->getInterval();

	if( button == m_leftButton.get() )
	{
		m_slider->setRange(min-m_buttonsStep, max-m_buttonsStep, step);
		m_slider->setValue(value-m_buttonsStep);
	}
	else if( button == m_rightButton.get() )
	{
		m_slider->setRange(min+m_buttonsStep, max+m_buttonsStep, step);
		m_slider->setValue(value+m_buttonsStep);
	}
}
////////////////////////////////////////////////////////////
//
// CONTROL COMPONENT
//
////////////////////////////////////////////////////////////
ControlComponent::ControlComponent()
{
	m_slider = new juce::Slider("media time");
	m_slider->setRange(0, 1000);
	m_slider->setSliderStyle (juce::Slider::LinearBar);
	m_slider->setAlpha(1.f);
	m_slider->setOpaque(true);
    m_slider->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

	
	
    m_playImage = juce::Drawable::createFromImageData (play_svg, play_svgSize);
    m_pauseImage = juce::Drawable::createFromImageData (pause_svg, pause_svgSize);
    m_stopImage = juce::Drawable::createFromImageData (stop_svg, stop_svgSize);

    m_playPauseButton = new juce::DrawableButton("playPause", juce::DrawableButton::ImageFitted);
	m_playPauseButton->setOpaque(false);
	m_playPauseButton->setImages(m_playImage);
    m_stopButton = new juce::DrawableButton("stop", juce::DrawableButton::ImageFitted);
	m_stopButton->setOpaque(false);
	m_stopButton->setImages(m_stopImage);
	

	

	m_alternateControlComponent = new SecondaryControlComponent();
	
	addAndMakeVisible(m_slider);
    addAndMakeVisible(m_playPauseButton);
    addAndMakeVisible(m_stopButton);
    addChildComponent(m_alternateControlComponent);

	
	setOpaque(false);
}
ControlComponent::~ControlComponent()
{
	m_slider = nullptr;
	m_playPauseButton = nullptr;
	m_stopButton = nullptr;
	m_playImage = nullptr;
	m_pauseImage = nullptr;
	m_stopImage = nullptr;
}
void ControlComponent::resized()
{
	int w =  getWidth();
	int h =  getHeight();

	
	int hMargin = 0.025*w;
	int buttonWidth = 0.03*w;
	int sliderHeight = 0.3*h;
	m_slider->setBounds (hMargin, h-sliderHeight-buttonWidth, w-2*hMargin, sliderHeight);

	m_playPauseButton->setBounds (hMargin, h-buttonWidth, buttonWidth, buttonWidth);
	m_stopButton->setBounds (hMargin+buttonWidth, h-buttonWidth, buttonWidth, buttonWidth);

	m_alternateControlComponent->setBounds (hMargin+10*buttonWidth, h-buttonWidth+hMargin/4, w -20*buttonWidth -2* hMargin, buttonWidth-hMargin/2);
}
void ControlComponent::paint(juce::Graphics& g)
{
	int w =  getWidth();
	int h =  getHeight();

	
	int hMargin = 0.025*w;
	int buttonWidth = 0.03*w;
	int sliderHeight = 0.3*h;
	int roundness = hMargin/4;
	
	///////////////// CONTROL ZONE:	
	g.setGradientFill (juce::ColourGradient (juce::Colours::darkgrey.withAlpha(0.5f),
										w/2, h-sliderHeight-buttonWidth-hMargin/2,
										juce::Colours::black,
										w/2, h,
										false));
	g.fillRoundedRectangle(hMargin/2,  h-sliderHeight-buttonWidth-hMargin/2, w-hMargin, sliderHeight+buttonWidth+hMargin/2, roundness);

	g.setGradientFill (juce::ColourGradient (juce::Colours::lightgrey.withAlpha(0.5f),
										w/2, h-sliderHeight-buttonWidth-hMargin/2,
										juce::Colours::black,
										w/2, h-hMargin/2,
										false));
	g.drawRoundedRectangle(hMargin/2,  h-sliderHeight-buttonWidth-hMargin/2, w-hMargin, sliderHeight+buttonWidth+hMargin/2, roundness,2.f);
	
	///////////////// TIME:
	juce::Font f = g.getCurrentFont().withHeight(getFontHeight());
	f.setTypefaceName("Times New Roman");//"Forgotten Futurist Shadow");
	f.setStyleFlags(juce::Font::plain);
	g.setFont(f);

	g.setColour (juce::Colours::white);

	

	g.drawFittedText (timeString,
						hMargin+2*buttonWidth, h-buttonWidth, w-2*hMargin-2*buttonWidth, buttonWidth,
						juce::Justification::topRight, 
						1, //1 line
						1.f//no h scale
						);
}

void ControlComponent::setTime(juce::int64 time, juce::int64 len)
{
	int h = (int)(time/(1000*60*60) );
	int m = (int)(time/(1000*60) - 60*h );
	int s = (int)(time/(1000) - 60*m - 60*60*h );
	
	int dh = (int)(len/(1000*60*60) );
	int dm = (int)(len/(1000*60) - 60*dh );
	int ds = (int)(len/(1000) - 60*dm - 60*60*dh );
	
	timeString = juce::String::formatted("%02d:%02d:%02d/%02d:%02d:%02d", h, m, s, dh, dm, ds);
}

void ControlComponent::showPausedControls()
{
	m_playPauseButton->setImages(m_playImage);

	setVisible(true);
}
void ControlComponent::showPlayingControls()
{
	m_playPauseButton->setImages(m_pauseImage);
	
	setVisible(true);
}
void ControlComponent::hidePlayingControls()
{
	setVisible(false);
}