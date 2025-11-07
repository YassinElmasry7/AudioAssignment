#pragma once 
#include <JuceHeader.h>

class PlayerAudio : public juce::AudioSource
{
public:
	PlayerAudio();
	~PlayerAudio();

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
	void releaseResources();
	bool loadFile(const juce::File& file);
	void stop();
	void start();
	void setGain(float gain);
	void setPosition(double pos);
	double getCurrentPosition();
	double getPosition() const;
	double getLengthInSeconds() const;
	double getLength() const;
	void toggleMute();
	void setSpeed(double speed);
	bool Muted = false;
	float previous = 0.5f;
	void setLooping(bool shouldLoop);
	void setLoopPoints(double startTime, double endTime);
	void clearLoopPoints();
	bool isSegmentLooping() const;

private:
	juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;
	std::unique_ptr<juce::ResamplingAudioSource> resampleSource;
	double loopStartTime = 0.0;
	double loopEndTime = 0.0;
	bool segmentLooping = false;
};